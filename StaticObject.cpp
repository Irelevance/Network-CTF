#include "StaticObject.h"
#include "math.h"
#include "UserPlayer.h"
#include "Collision.h"

#include <iostream>

using std::cout;
using std::endl;

StaticObject::StaticObject(const sf::Vector2f& position):
    collisionBox(sf::Vector2f(20, 20))
    {
        collisionBox.setPosition(position);

        //setup the size and other information about the static object
        setupCollisionBox();
    }

StaticObject::~StaticObject() {

}

const sf::FloatRect StaticObject::getCollisionBox() const {

    return collisionBox.getGlobalBounds();
}

const sf::Vector2f StaticObject::getPosition() const {

    return collisionBox.getPosition();
}

bool StaticObject::contains(const sf::Vector2f& point) const {

    return collisionBox.getGlobalBounds().contains(point);
}

void StaticObject::handleCollision(UserPlayer& collidingPlayer) {

    sf::Vector2f movementOffset = calculateCollisionOffset(collidingPlayer.getDestinationBox(), collisionBox.getGlobalBounds());

    collidingPlayer.move(movementOffset);
}

void StaticObject::draw(sf::RenderWindow& window) {

    window.draw(collisionBox);
}

void StaticObject::setupCollisionBox() {

    collisionBox.setSize(sf::Vector2f(50, 50));
}
