#include "threepp/threepp.hpp"
#include <iostream>

using namespace threepp;

struct FlappyKeyListener : KeyListener {
    bool &is_flapping;
    explicit FlappyKeyListener(bool &flap_flag) : is_flapping(flap_flag) {}

    void onKeyPressed(KeyEvent evt) override {
        if (evt.key == Key::SPACE) {
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
    auto texture = texture_loader.load("C:/dev/Flappy Bird/Textures/Flappy Bird.png");
    auto geometry = PlaneGeometry::create(1, 1);
    auto material = MeshBasicMaterial::create();
    material->map = texture;
    material->transparent = true;
    auto square = Mesh::create(geometry, material);
    scene.add(square);

    float gravity = -0.001f;
    float velocity = 0.0f;
    float jump_force = 0.03f;
    bool is_flapping = false;

    FlappyKeyListener key_listener(is_flapping);
    canvas.addKeyListener(key_listener);

    canvas.animate([&]() {
        if (is_flapping) {
            velocity = jump_force;
            is_flapping = false;
        }

        velocity += gravity;
        square->position.y += velocity;

        if (square->position.y < -1.5f) {
            square->position.y = -1.5f;
            velocity = 0;
        } else if (square->position.y > 1.5f) {
            square->position.y = 1.5f;
            velocity = 0;
        }

        renderer.render(scene, camera);
    });

    return 0;
}
