#include <threepp/threepp.hpp>
#include <iostream>
#include <thread>

using namespace threepp;
using namespace std;
int main()
{
    // Opprett Threepp-vindu
    Canvas canvas;
    GLRenderer renderer{canvas.size()};

    // Opprett kamera
    PerspectiveCamera camera(60, canvas.aspect(), 0.1f, 1000.0f);
    camera.position.set(0, 0, 2);
    camera.lookAt(Vector3(0, 0, 0));

    Scene scene;
    scene.background = Color::lightskyblue;

    // Last inn PNG
    TextureLoader texture_loader;
    auto texture = texture_loader.load("C:/dev/Flappy Bird/Textures/Flappy Bird.png");
    auto geometry = PlaneGeometry::create(1,1);
    auto material = MeshBasicMaterial::create();
    material->map = texture;
    material->transparent = true;
    auto square = Mesh::create(geometry,material);
    scene.add(square);

    float gravity = -0.001f;
    float velocity = 0.02f; // Fuglens hastighet
    bool is_flapping = false;

    canvas.animate([&]() {
        if (square->position.y > 1.5f || square->position.y < -1.5f) {
            velocity = -velocity;
        }

        square->position.y += velocity;

        renderer.render(scene, camera);
    });

    return 0;
}

