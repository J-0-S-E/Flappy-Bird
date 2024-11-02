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
    camera.position.set(3, 3, 3);
    camera.lookAt(Vector3(0, 0, 0));

    Scene scene;
    scene.background = Color::blue;

    // Opprett firkant
    auto geometry = BoxGeometry::create(1,1,1);
    auto material = MeshBasicMaterial::create();
    material->color = Color::aliceblue;
    auto square = Mesh::create(geometry,material);
    scene.add(square);


    canvas.animate([&]() {
        renderer.render(scene, camera);
    });

    return 0;
}