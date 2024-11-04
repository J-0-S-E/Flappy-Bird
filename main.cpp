#include "threepp/threepp.hpp"
#include <iostream>
#include <thread>  // For std::this_thread::sleep_for

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
    auto bird = texture_loader.load("C:/dev/Flappy Bird/Textures/Flappy Bird.png");
    auto pipe = texture_loader.load("C:/dev/Flappy Bird/Textures/Pipe.png");
    auto geometry = PlaneGeometry::create(1, 1);
    auto material = MeshBasicMaterial::create();
    material->map = bird;
    material->transparent = true;
    auto square = Mesh::create(geometry, material);
    scene.add(square);

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
        square->position.set(0, 0, 0);  // Flytt fuglen til midten
        square->rotation.z = 0;
    };

    canvas.animate([&]() {
        if (game_started) {
            if (is_flapping) {
                velocity = jump_force;
                is_flapping = false;
                rotation_angle = flap_rotation;
            }

            velocity += gravity;
            square->position.y += velocity;

            // Sjekk om fuglen treffer taket
            if (square->position.y >= 1.4f) {
                square->position.y = 1.4f;
                velocity = 0;         // Stopp oppoverbevegelsen
                input_disabled = true; // Deaktiver input midlertidig
            }

            // Sjekk om fuglen treffer bakken
            if (square->position.y < -1.5f) {
                square->position.y = -1.5f;
                velocity = 0;
                input_disabled = true; // Deaktiver input
                reset_game();           // Start tilbakestillingssekvensen
                return;
            }

            // Roter fuglen gradvis mot maksimal nedovervinkel
            if (rotation_angle > max_rotation_angle) {
                rotation_angle -= 90.0f * (1.0f / 60.0f);  // Ca. 1 sekund til full rotasjon nedover
                if (rotation_angle < max_rotation_angle) {
                    rotation_angle = max_rotation_angle;
                }
            }

            square->rotation.z = math::DEG2RAD * rotation_angle;
        }

        renderer.render(scene, camera);
    });

    return 0;
}
