#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include "SFML/Graphics.hpp"

#include <string>

//some constants to define when the player closed the input
extern std::string exitedFunction;

//maximum length of the text in pixels is given so that the returned text can fit in the given text box
std::string receiveInput(sf::RenderWindow& window, std::string message, const sf::Font& font, const float& maxTextLength, const float& scale = 0.6);

#endif // INPUT_H_INCLUDED
