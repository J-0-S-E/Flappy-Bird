#include "threepp/threepp.hpp"
#include <iostream>
#include <thread>    // For std::this_thread::sleep_for
#include <vector>    // For å holde flere rør
#include <random>    // For tilfeldige avstander

using namespace threepp;

struct FlappyKeyListener : KeyListener {
    bool &is_flapping;
    bool &game_started;
    bool &input_disabled;

    explicit FlappyKeyListener(bool &flap_flag, bool &start_flag, bool &input_flag)
        : is_flapping(flap_flag), game_started(start_flag), input_disabled(input_flag) {}

    void onKeyPressed(KeyEvent evt) override {
        if (evt.key == Key::SPACE && !input_disabled) {  // Bare hopp hvis input ikke er deaktivert
            if (!game_started) {
                game_started = true;  // Start spillet ved første trykk
            }
            is_flapping = true;
        }
    }
};

int main() {
    Canvas canvas("Flappy Bird");
    GLRenderer renderer(canvas.size());

    PerspectiveCamera camera(60, canvas.aspect(), 0.1f, 1000.0f);
    camera.position.set(0, 0, 2);
    camera.lookAt(Vector3(0, 0, 0));

    Scene scene;
    scene.background = Color::lightskyblue;

    TextureLoader texture_loader;
    auto bird_texture = texture_loader.load("C:/dev/Flappy Bird/Textures/Flappy Bird.png");
    auto pipe_texture = texture_loader.load("C:/dev/Flappy Bird/Textures/Pipe.png");

    auto bird_geometry = PlaneGeometry::create(0.8, 0.8);
    auto bird_material = MeshBasicMaterial::create();
    bird_material->map = bird_texture;
    bird_material->transparent = true;
    auto bird = Mesh::create(bird_geometry, bird_material);
    scene.add(bird);

    struct PipePair {
        std::shared_ptr<Mesh> top_pipe;
        std::shared_ptr<Mesh> bottom_pipe;
        float gap_y;     // Vertikal posisjon til gapet mellom rørene
        bool scored = false; // For å sjekke om poeng allerede er gitt for dette paret
    };

    std::vector<PipePair> pipes;
    float pipe_speed = -0.01f; // Hastigheten til rørene som beveger seg mot venstre

    float pipe_start_x = 2.5f;     // Startposisjon til rørene utenfor høyre kant
    float pipe_remove_x = -2.5f;   // Fjern rørene når de går utenfor venstre kant

    auto create_pipe_pair = [&]() {
        auto pipe_geometry = PlaneGeometry::create(0.8f, 3.0f);
        auto pipe_material = MeshBasicMaterial::create();
        pipe_material->map = pipe_texture;
        pipe_material->transparent = true;

        // Lag topp- og bunnrør
        auto top_pipe = Mesh::create(pipe_geometry, pipe_material);
        auto bottom_pipe = Mesh::create(pipe_geometry, pipe_material);

        // Tilfeldig posisjon for gapet
        float gap_y = ((rand() % 100) / 100.0f) * 1.0f - 0.5f;  // -0.5 til +0.5

        // Tilfeldig gap mellom topp- og bunnrør
        float pipe_gap = 3.5f + ((rand() % 150) / 400.0f);  // Tilfeldig gap mellom 3.0 og 5.0

        // Sett startposisjon for topp- og bunnrør til høyre for skjermen
        top_pipe->position.set(pipe_start_x, gap_y + pipe_gap / 2.0f, 0);
        bottom_pipe->position.set(pipe_start_x, gap_y - pipe_gap / 2.0f, 0);

        // Roter topp-pipen 180 grader (pi radianer)
        top_pipe->rotation.z = math::PI;

        // Legg rørene til i scenen
        scene.add(*top_pipe);
        scene.add(*bottom_pipe);

        pipes.push_back({top_pipe, bottom_pipe, gap_y});
    };

    float gravity = -0.001f;
    float velocity = 0.0f;
    float jump_force = 0.03f;
    bool is_flapping = false;
    bool game_started = false;
    bool input_disabled = false;
    float rotation_angle = 0.0f;
    const float max_rotation_angle = -90.0f;
    const float flap_rotation = 30.0f;

    FlappyKeyListener key_listener(is_flapping, game_started, input_disabled);
    canvas.addKeyListener(key_listener);

    auto reset_game = [&]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Vent ett sekund før tilbakestilling
        game_started = false;
        input_disabled = false;
        velocity = 0.0f;
        rotation_angle = 0.0f;
        bird->position.set(0, 0, 0);  // Flytt fuglen til midten
        bird->rotation.z = 0;

        // Fjern alle gamle rør og generer et nytt sett
        for (auto &pipe_pair : pipes) {
            scene.remove(*pipe_pair.top_pipe);
            scene.remove(*pipe_pair.bottom_pipe);
        }
        pipes.clear();
        create_pipe_pair();
    };

    create_pipe_pair();

    canvas.animate([&]() {
        if (game_started) {
            if (is_flapping) {
                velocity = jump_force;
                is_flapping = false;
                rotation_angle = flap_rotation;
            }

            velocity += gravity;
            bird->position.y += velocity;

            // Sjekk om fuglen treffer taket
            if (bird->position.y >= 1.4f) {
                bird->position.y = 1.4f;
                velocity = 0;
                input_disabled = true;
            }

            // Sjekk om fuglen treffer bakken
            if (bird->position.y < -1.5f) {
                bird->position.y = -1.5f;
                velocity = 0;
                input_disabled = true;
                reset_game();
                return;
            }

            // Roter fuglen gradvis mot maksimal nedovervinkel
            if (rotation_angle > max_rotation_angle) {
                rotation_angle -= 90.0f * (1.0f / 100.0f);  // Ca. 1 sekund til full rotasjon nedover
                if (rotation_angle < max_rotation_angle) {
                    rotation_angle = max_rotation_angle;
                }
            }
            bird->rotation.z = math::DEG2RAD * rotation_angle;

            // Flytt rørene mot venstre og sjekk kollisjoner
            for (auto &pipe_pair : pipes) {
                pipe_pair.top_pipe->position.x += pipe_speed;
                pipe_pair.bottom_pipe->position.x += pipe_speed;

                // Kollisjonssjekk mellom fuglen og rørene
                if (bird->position.x > pipe_pair.top_pipe->position.x - 0.4f &&
                    bird->position.x < pipe_pair.top_pipe->position.x + 0.4f) {
                    if (bird->position.y > pipe_pair.top_pipe->position.y - 1.5f ||
                        bird->position.y < pipe_pair.bottom_pipe->position.y + 1.5f) {
                        input_disabled = true;
                        reset_game();
                        return;
                    }
                }
            }

            // Fjern rør som har gått ut av skjermen og generer nye
            if (!pipes.empty() && pipes.front().top_pipe->position.x < pipe_remove_x) {
                scene.remove(*pipes.front().top_pipe);
                scene.remove(*pipes.front().bottom_pipe);
                pipes.erase(pipes.begin());
                create_pipe_pair();
            }

        }

        renderer.render(scene, camera);
    });

    return 0;
}
