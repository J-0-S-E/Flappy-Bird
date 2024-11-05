#include "threepp/threepp.hpp"  // Dette inkluderer threepp-biblioteket
#include "threepp/audio/Audio.hpp"
#include <string>

using namespace std;

using namespace threepp;  // Gjør at vi kan bruke funksjonene i threepp direkte

string bird_open =  "C:/dev/Flappy Bird/Textures/Colibri open.png";
string bird_closed = "C:/dev/Flappy Bird/Textures/Colibri closed.png";

string bird_texture = bird_open;

void update_texture(int timer) {
    if (timer % 2 = 0) {
        bird_texture = bird_open;
    } else {
        bird_texture = bird_closed;
    }
}

// Klasse for å håndtere tastetrykk (WASD for å styre kolibrien)
struct colibri_key_listener : KeyListener {
    bool &move_up, &move_down, &move_left, &move_right;

    colibri_key_listener(bool &up, bool &down, bool &left, bool &right)
        : move_up(up), move_down(down), move_left(left), move_right(right) {}

    // Når en tast trykkes, oppdateres bevegelsesvariablene
    void onKeyPressed(KeyEvent evt) override {
        switch (evt.key) {
            case Key::W:
                move_up = true;
                break;
            case Key::S:
                move_down = true;
                break;
            case Key::A:
                move_left = true;
                break;
            case Key::D:
                move_right = true;
                break;
        }
    }

    // Når en tast slippes, stopper vi bevegelsen
    void onKeyReleased(KeyEvent evt) override {
        switch (evt.key) {
            case Key::W:
                move_up = false;
                break;
            case Key::S:
                move_down = false;
                break;
            case Key::A:
                move_left = false;
                break;
            case Key::D:
                move_right = false;
                break;
        }
    }
};

int main() {
    int timer = 0;

    while (true) {
        update_texture(timer);
        timer++;

    }
    // Opprett et vindu
    Canvas canvas("Colibri Flight");
    GLRenderer renderer(canvas.size());

    // Kameraoppsett
    PerspectiveCamera camera(75, canvas.aspect(), 0.1f, 1000.0f);
    camera.position.set(0, 0, 5);

    // Sceneoppsett med en blå bakgrunn (representerer himmelen)
    Scene scene;
    scene.background = Color::skyblue;

    AudioListener listener;
    PositionalAudio audio(listener, "C:/dev/Flappy Bird/Audio/8-Bit Odyssey.mp3");
    audio.setLooping(true);
    audio.play();

    // Last inn kolibri-bildet (Flappy Bird.png)
    TextureLoader texture_loader;
    auto bird_texture = texture_loader.load("C:/dev/Flappy Bird/Textures/Flappy Bird.png");

    // Opprett kolibrien som et flatt plan med tekstur
    auto bird_geometry = PlaneGeometry::create(2.0f, 2.0f);
    auto bird_material = MeshBasicMaterial::create();
    bird_material->map = bird_texture;
    bird_material->transparent = true;
    auto bird = Mesh::create(bird_geometry, bird_material);
    bird->position.set(0, 0, 0);  // Start posisjon midt på skjermen
    scene.add(bird);

    // Bevegelsesvariabler for kolibrien
    bool move_up = false, move_down = false, move_left = false, move_right = false;
    float speed = 0.05f; // Hastigheten til kolibrien

    // Tastelytter for å kontrollere kolibrien med WASD
    colibri_key_listener key_listener(move_up, move_down, move_left, move_right);
    canvas.addKeyListener(key_listener);

    // Animasjonsløkke som kjører kontinuerlig
    canvas.animate([&]() {
        // Oppdater kolibriens posisjon basert på tastetrykk
        if (move_up) bird->position.y += speed;
        if (move_down) bird->position.y -= speed;
        if (move_left) bird->position.x -= speed;
        if (move_right) bird->position.x += speed;

        // Renderer scenen
        renderer.render(scene, camera);
    });

    return 0;
}
