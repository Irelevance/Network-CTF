#ifndef PREDRAWNBUTTON_H_INCLUDED
#define PREDRAWNBUTTON_H_INCLUDED

#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"

#include <string>

///same as button except the button text and texture are predrawn onto the image file
class PredrawnButton {

    private:

        sf::Texture texture;
        sf::Sprite sprite;

        //covers the button with a dark overlay when user is hovering mouse over the button
        sf::RectangleShape cover;

        bool isMouseTouching;

        //resizing cover also resets the origin
        void resizeCover();
        void repositionCover();

        //resizes and repositions the cover
        void resetCover() {

            resizeCover();
            repositionCover();
        }

    public:

        PredrawnButton(const std::string& imagePath);

        void setPosition(const sf::Vector2f& position);

        //if mouse is touching button it also sets isMouseTouching to true that way it can draw the cover
        bool checkMouseTouching(const sf::Vector2f& mousePosition);

        void draw(sf::RenderWindow& window);
};

#endif // PREDRAWNBUTTON_H_INCLUDED