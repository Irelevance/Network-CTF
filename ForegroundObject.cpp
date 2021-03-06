#include "ForegroundObject.h"
#include "math.h"

#include <iostream>

using std::cout;
using std::endl;
using std::string;

ForegroundObject::ForegroundObject(const string& imagePath) :
    StaticObject(sf::Vector2f(0, 0)),
    foregroundTexture(),
    texturePath(imagePath),
    hasTexture(false),
    hidingPlayer(false)
    {
        hasTexture = foregroundTexture.loadFromFile("images/" + imagePath);

        collisionBox.setTexture(&foregroundTexture);
        collisionBox.setSize(sf::Vector2f(foregroundTexture.getSize().x, foregroundTexture.getSize().y));

        collisionBox.setOrigin(calculateCenter(collisionBox.getGlobalBounds()));
    }

ForegroundObject::ForegroundObject(const sf::Vector2f& centerPosition) :
    StaticObject(centerPosition),
    foregroundTexture(),
    texturePath(),
    hasTexture(false),
    hidingPlayer(false)
    {

    }

bool ForegroundObject::hasImage() const {

    return hasTexture;
}

const string& ForegroundObject::getImageName() const {

    return texturePath;
}

void ForegroundObject::setPosition(const sf::Vector2f& centerPosition) {

    collisionBox.setPosition(centerPosition);
}

void ForegroundObject::setHidingPlayer(const bool& playerHidden) {

    hidingPlayer = playerHidden;

    //make the object see through so player can see whats behind it when he is hiding
    if(hidingPlayer) {

    makeSeethrough();

    } else {

     makeSolid();
    }
}

void ForegroundObject::makeSeethrough() {

    collisionBox.setFillColor(sf::Color(255, 255, 255, 150));
}

void ForegroundObject::makeSolid() {

    collisionBox.setFillColor(sf::Color(255, 255, 255, 255));
}
