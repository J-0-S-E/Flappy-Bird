#include "threepp/threepp.hpp"  // Dette inkluderer threepp-biblioteket
#include <iostream>  // Dette trengs for utskrift (f.eks. debugging)

using namespace threepp;  // Gjør at vi kan bruke funksjonene i threepp direkte

// Her lager vi en enkel klasse for å lytte til tastetrykk
struct BirdKeyListener : KeyListener {

    // Variabler for å lagre om vi skal bevege oss opp, ned, til venstre, eller høyre
    bool &move_up, &move_down, &move_left, &move_right;

    // Konstruktør for å sette opp tastene
    BirdKeyListener(bool &up, bool &down, bool &left, bool &right)
        : move_up(up), move_down(down), move_left(left), move_right(right) {}

    // Når en tast trykkes ned, oppdaterer vi bevegelsesvariablene
    void on_key_pressed(KeyEvent evt) override {
        if (evt.key == Key::W) move_up = true;       // W for å flytte opp
        if (evt.key == Key::S) move_down = true;     // S for å flytte ned
        if (evt.key == Key::A) move_left = true;     // A for å flytte til venstre
        if (evt.key == Key::D) move_right = true;    // D for å flytte til høyre
    }

    // Når tasten slippes, stopper vi bevegelsen
    void on_key_released(KeyEvent evt) override {
        if (evt.key == Key::W) move_up = false;
        if (evt.key == Key::S) move_down = false;
        if (evt.key == Key::A) move_left = false;
        if (evt.key == Key::D) move_right = false;
    }
};

int main() {
    // Oppretter et vindu som vi kan vise grafikken i
    Canvas canvas("Hoverbird");

    // Renderer som brukes til å vise grafikken på skjermen
    GLRenderer renderer(canvas.size());

    // Oppsett for kamera - hvordan vi "ser" scenen
    PerspectiveCamera camera(75, canvas.aspect(), 0.1f, 1000.0f);
    camera.position.set(0, 0, 5);  // Plasserer kamera litt tilbake for å se hele scenen

    // Oppretter scenen med en bakgrunnsfarge (blå himmel)
    Scene scene;
    scene.background = Color::skyblue;

    // Last inn fugleteksturen (bildet vi bruker som "fugl")
    TextureLoader texture_loader;
    auto bird_texture = texture_loader.load("C:/dev/Flappy Bird/Textures/Flappy Bird.png");

    // Lager fuglen som et enkelt, flatt objekt med tekstur
    auto bird_geometry = PlaneGeometry::create(0.5f, 0.5f);  // Fuglens størrelse
    auto bird_material = MeshBasicMaterial::create();
    bird_material->map = bird_texture;  // Setter fugleteksturen
    auto bird = Mesh::create(bird_geometry, bird_material);
    bird->position.set(0, 0, 0);  // Plasserer fuglen midt på skjermen
    scene.add(bird);  // Legger fuglen til i scenen

    // Variabler for å spore bevegelse (true/false om en retning er aktiv)
    bool move_up = false, move_down = false, move_left = false, move_right = false;
    float speed = 0.05f; // Hastigheten til fuglen

    // Tastelytter som oppdaterer bevegelsesvariablene når vi trykker på tastene
    BirdKeyListener key_listener(move_up, move_down, move_left, move_right);
    canvas.addKeyListener(key_listener);

    // Animasjonsløkke - koden her kjører hele tiden mens spillet er åpent
    canvas.animate([&]() {
        // Flytter fuglen basert på om vi trykker på en av tastene
        if (move_up) bird->position.y += speed;
        if (move_down) bird->position.y -= speed;
        if (move_left) bird->position.x -= speed;
        if (move_right) bird->position.x += speed;

        // Renderer scenen (oppdaterer skjermen)
        renderer.render(scene, camera);
    });

    return 0;
}
