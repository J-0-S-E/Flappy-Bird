#include "threepp/threepp.hpp"  // threepp library
#include "threepp/audio/Audio.hpp"
#include <string>
#include <random>
#include <cmath>
#include <memory>

using namespace std;
using namespace threepp;

// Texture paths for the bird's animation
const string bird_open = "C:/dev/Flappy Bird/Textures/Colibri open.png";
const string bird_closed = "C:/dev/Flappy Bird/Textures/Colibri closed.png";
string bird_texture = bird_open;

// Function to alternate bird texture based on timer
void update_texture(int timer) {
    bird_texture = (timer % 3 == 0) ? bird_open : bird_closed;
}

// Key listener for bird movement (WASD controls)
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

// Function to randomly select a flower texture path
string get_random_flower_texture() {
    vector<string> flower_textures = {
        "C:/dev/Flappy Bird/Textures/Flower1.png",
        "C:/dev/Flappy Bird/Textures/Flower2.png",
        "C:/dev/Flappy Bird/Textures/Flower3.png"
    };

    static random_device rd;  // Seed
    static mt19937 gen(rd()); // Random number generator
    uniform_int_distribution<> dis(0, flower_textures.size() - 1);

    return flower_textures[dis(gen)];
}

int main() {

    int timer = 0;

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
    auto current_texture = texture_loader.load(bird_texture);  // Bird initial texture

    // Bird setup
    auto bird_geometry = PlaneGeometry::create(2.0f, 2.0f);
    auto bird_material = MeshBasicMaterial::create();
    bird_material->map = current_texture;
    bird_material->transparent = true;
    auto bird = Mesh::create(bird_geometry, bird_material);
    bird->position.set(0, 0, 0);

    // Select random textures for the top and bottom flowers
    string random_flower_texture_top = get_random_flower_texture();
    string random_flower_texture_bottom = get_random_flower_texture();

    auto flower_top_texture = texture_loader.load(random_flower_texture_top);
    auto flower_bottom_texture = texture_loader.load(random_flower_texture_bottom);

    // Create separate materials for each flower
    auto flower_top_material = MeshBasicMaterial::create();
    flower_top_material->map = flower_top_texture;
    flower_top_material->transparent = true;

    auto flower_bottom_material = MeshBasicMaterial::create();
    flower_bottom_material->map = flower_bottom_texture;
    flower_bottom_material->transparent = true;

    // Create two flower meshes (one for top, one for bottom) with independent textures
    auto flower_geometry = PlaneGeometry::create(2.0f, 3.0f);
    auto flower_top = Mesh::create(flower_geometry, flower_top_material);
    auto flower_bottom = Mesh::create(flower_geometry, flower_bottom_material);

    // Position the flowers
    float pos_x = 5.0f;

    // Set initial positions
    flower_top->position.set(pos_x, 3.0f, 0);
    flower_top->rotation.z = M_PI;
    flower_bottom->position.set(pos_x, -3.0f, 0);

    // Add objects to the scene
    scene.add(bird);
    scene.add(flower_top);
    scene.add(flower_bottom);

    // Bird movement variables
    bool move_up = false, move_down = false, move_left = false, move_right = false;
    const float speed = 0.1f;

    // Add key listener for bird movement
    ColibriKeyListener key_listener(move_up, move_down, move_left, move_right);
    canvas.addKeyListener(key_listener);

    string last_texture = bird_texture;
    float speed_x = 0.05f;
    // Animation loop
    canvas.animate([&]() {
        update_texture(timer++);

        if (timer % 1 == 0)
            pos_x -= speed_x;

        if (timer % 10 == 0)
            speed_x += 0.0001f;

        // Apply updated position to flowers
        flower_top->position.x = pos_x;
        flower_bottom->position.x = pos_x;

        // Update bird texture if it has changed
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

        // Prevent bird from leaving scene bounds
        float scene_top = 3.5f;
        float scene_side = 7.0f;
        bird->position.y = clamp(bird->position.y, -scene_top, scene_top);
        bird->position.x = clamp(bird->position.x, -scene_side, scene_side);

        // Collision detection and reset position if needed
        Box3 bird_box;
        bird_box.setFromObject(*bird.get());
        Box3 flower_top_box;
        flower_top_box.setFromObject(*flower_top.get());
        flower_top_box.setFromCenterAndSize(flower_top->position, Vector3(1.0f, 1.5f, 1.0f));  // Smaller hitbox size

        Box3 flower_bottom_box;
        flower_bottom_box.setFromObject(*flower_bottom.get());
        flower_bottom_box.setFromCenterAndSize(flower_bottom->position, Vector3(1.0f, 1.5f, 1.0f));  // Smaller hitbox size

        if (bird_box.intersectsBox(flower_top_box) || bird_box.intersectsBox(flower_bottom_box)) {
            bird->position.set(0, 0, 0);  // Reset bird position
        }

        // Render scene
        renderer.render(scene, camera);
    });

    return 0;
}
