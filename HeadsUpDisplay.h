#ifndef HEADSUPDISPLAY_H_INCLUDED
#define HEADSUPDISPLAY_H_INCLUDED

#include "Minimap.h"
#include "StatDisplay.h"
#include "ScoreDisplay.h"

#include <string>

//the player's weapon display, health, minimap, and the team scores
class HeadsUpDisplay {

    private:

        Minimap minimap;
        StatDisplay statDisplay;
        ScoreDisplay scoreDisplay;

    public:

        HeadsUpDisplay(const sf::Vector2u& screenSize);

        void handleScreenResize(const sf::Vector2u& screenSize) {

            minimap.handleScreenResize(screenSize);
            statDisplay.handleScreenResize(screenSize);
        }

        StatDisplay& getStatDisplay() {

            return statDisplay;
        }

        Minimap& getMinimap() {

            return minimap;
        }

        ScoreDisplay& getScoreDisplay() {

            return scoreDisplay;
        }

        void draw(sf::RenderWindow& window) {

            statDisplay.draw(window);
            minimap.drawBorder(window);
            scoreDisplay.draw(window);
        }
};

#endif // HEADSUPDISPLAY_H_INCLUDED
