#include "threepp/threepp.hpp"  // threepp library
#include "threepp/audio/Audio.hpp"
#include <string>

using namespace std;
using namespace threepp;

// Texture paths
const string bird_open = "C:/dev/Flappy Bird/Textures/Colibri open.png";
const string bird_closed = "C:/dev/Flappy Bird/Textures/Colibri closed.png";
string bird_texture = bird_open;

// Update texture based on timer
void update_texture(int timer) {
    bird_texture = (timer % 3 == 0) ? bird_open : bird_closed;
}

// Key Listener for bird movement (WASD controls)
struct ColibriKeyListener : KeyListener {
    bool &move_up, &move_down, &move_left, &move_right;

    ColibriKeyListener(bool &up, bool &down, bool &left, bool &right)
        : move_up(up), move_down(down), move_left(left), move_right(right) {}

    void onKeyPressed(KeyEvent evt) override {
        switch (evt.key) {
            case Key::W: move_up = true; break;
            case Key::S: move_down = true; break;
            case Key::A: move_left = true; break;
            case Key::D: move_right = true; break;
            default: break;
        }
    }

    void onKeyReleased(KeyEvent evt) override {
        switch (evt.key) {
            case Key::W: move_up = false; break;
            case Key::S: move_down = false; break;
            case Key::A: move_left = false; break;
            case Key::D: move_right = false; break;
            default: break;
        }
    }
};

int main() {
    // Create window and renderer
    Canvas canvas("Colibri Flight");
    GLRenderer renderer(canvas.size());

    // Camera setup
    PerspectiveCamera camera(75, canvas.aspect(), 0.1f, 1000.0f);
    camera.position.set(0, 0, 5);

    // Scene setup with sky background
    Scene scene;
    scene.background = Color::skyblue;

    // Add audio
    AudioListener listener;
    PositionalAudio audio(listener, "C:/dev/Flappy Bird/Audio/8-Bit Odyssey.mp3");
    audio.setLooping(true);
    audio.setVolume(0.1f);
    audio.play();

    // Load textures
    TextureLoader texture_loader;
    auto current_texture = texture_loader.load(bird_texture);
    auto upper_pipe_texture = texture_loader.load("C:/dev/Flappy Bird/Textures/Upper_pipe.png");
    auto lower_pipe_texture = texture_loader.load("C:/dev/Flappy Bird/Textures/Lower_pipe.png");

    // Create bird
    auto bird_geometry = PlaneGeometry::create(2.0f, 2.0f);
    auto bird_material = MeshBasicMaterial::create();
    bird_material->map = current_texture;
    bird_material->transparent = true;

    // Create pipes
    auto pipe_geometry = PlaneGeometry::create(2.0f, 4.0f);
    auto upper_pipe_material = MeshBasicMaterial::create();
    auto lower_pipe_material = MeshBasicMaterial::create();
    upper_pipe_material->map = upper_pipe_texture;
    lower_pipe_material->map = lower_pipe_texture;
    upper_pipe_material->transparent = true;
    lower_pipe_material->transparent = true;

    auto bird = Mesh::create(bird_geometry, bird_material);
    auto upper_pipe = Mesh::create(pipe_geometry, upper_pipe_material);
    auto lower_pipe = Mesh::create(pipe_geometry, lower_pipe_material);

    // Set initial positions
    bird->position.set(0, 0, 0);
    upper_pipe->position.set(2.0f, 3.f, 0);
    lower_pipe->position.set(2.0f, -3.f, 0);

    // Add objects to scene
    scene.add(bird);
    scene.add(upper_pipe);
    scene.add(lower_pipe);

    // Bird movement variables
    bool move_up = false, move_down = false, move_left = false, move_right = false;
    const float speed = 0.1f;

    // Add key listener
    ColibriKeyListener key_listener(move_up, move_down, move_left, move_right);
    canvas.addKeyListener(key_listener);

    int timer = 0;
    string last_texture = bird_texture;

    // Animation loop
    canvas.animate([&]() {
        update_texture(timer++);

        // Update texture if changed
        if (bird_texture != last_texture) {
            current_texture = texture_loader.load(bird_texture);
            bird_material->map = current_texture;
            last_texture = bird_texture;
        }

        // Move bird based on key input
        if (move_up) bird->position.y += speed;
        if (move_down) bird->position.y -= speed;
        if (move_left) bird->position.x -= speed;
        if (move_right) bird->position.x += speed;

        // Stop bird from leaving scene
        float scene_top = 3.5f;
        float scene_side = 7.0f;
        if (bird->position.y > scene_top)
            bird->position.y = scene_top;
        if (bird->position.y < -scene_top)
            bird->position.y = -scene_top;
        if (bird->position.x > scene_side)
            bird->position.x = scene_side;
        if (bird->position.x < -scene_side)
            bird->position.x = -scene_side;

        // Collision detection and reset
        Box3 bird_box;
        bird_box.setFromObject(*bird.get());
        Box3 upper_pipe_box;
        upper_pipe_box.setFromObject(*upper_pipe.get());
        Box3 lower_pipe_box;
        lower_pipe_box.setFromObject(*lower_pipe.get());

        if (bird_box.intersectsBox(upper_pipe_box) || bird_box.intersectsBox(lower_pipe_box)) {
            bird->position.set(0, 0, 0);  // Reset position
        }

        // Render scene
        renderer.render(scene, camera);
    });

    return 0;
}
