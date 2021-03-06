#ifndef PORTAL_H_INCLUDED
#define PORTAL_H_INCLUDED

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "StaticObject.h"
#include "Floors.h"

class UserPlayer;

//when a player collides with a portal they are taken to a different part of the map or a different floor entirely
//used to have caves and other stuff
class Portal : public StaticObject {

    private:

        //location to teleport to once user hits the portal
        //also servers as a drawing position
        sf::RectangleShape teleportPosition;

        //floor the portal takes the player to
        unsigned destinationFloor;

    public:

        Portal(const sf::Vector2f& position, const sf::Vector2f& size);

        virtual ~Portal(){/*empty destructor*/}

        void setTeleportPosition(const sf::Vector2f& position, const sf::Vector2f& size = sf::Vector2f(20, 20), const unsigned destinationFloor = UNDERGROUND_FLOOR);
        void setDestinationFloor(const unsigned& floor);

        const sf::Vector2f& getTeleportPosition() const;
        const unsigned getDestinationFloor() const;

        bool contains(const sf::Vector2f& point) const;

        void draw(sf::RenderWindow& window, unsigned floor);
        void drawDestination(sf::RenderWindow& window, unsigned floor);
};

#endif // PORTAL_H_INCLUDED
