#include <SFML/Graphics.hpp>
using namespace sf;

int main() {
    RenderWindow window(sf::VideoMode(800, 600), "Collapse Chase");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }


        window.clear(Color(0xADD8E6));
        window.display();
    }
    return 0;
}
