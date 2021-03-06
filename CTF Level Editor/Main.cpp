#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "LevelEditor.h"

#include <vector>
#include <tr1/memory>
#include <iostream>

using std::vector;
using std::tr1::shared_ptr;
using std::cin;
using std::cout;
using std::endl;

int main() {

    sf::RenderWindow window(sf::VideoMode(1024, 768), "Level Editor");

    LevelEditor levelEditor(window);

    sf::Event event;

    sf::Clock updateTimer;

    while(window.isOpen()) {

        while(window.pollEvent(event)) {

            if(event.type == sf::Event::Closed) {

                window.close();
            }

            if(event.type == sf::Event::Resized) {

                sf::View view;
                view.setSize(event.size.width, event.size.height);
                view.setCenter(event.size.width / 2, event.size.height / 2);

                cout << "resize : " << event.size.width << " " << event.size.height << endl;

                window.setView(view);
            }

            levelEditor.handleEvents(event, window);
        }

        sf::Time delta = updateTimer.restart();
        levelEditor.update(delta.asSeconds(), window);

        window.clear();

        levelEditor.draw(window);

        window.display();
    }

    return 0;
}
