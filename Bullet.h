#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"

#include <tr1/memory>

class LineSegment;

//bullets are rays but they need to keep track of how long they're live
//bullets can only damage things the moment they're fired, afterwards they remain in order for the players to see a line indicating where they fired
class Bullet {

    private:

        //line of the bullet
        std::tr1::shared_ptr<LineSegment> line;

        //because bullets look odd when they are drawn as a line, have this bullet image travel from the origin of the bullet to the end
        //to make it look like a bullet was fired
        sf::RectangleShape bulletTracer;

        //how long the bullet should live for AFTER IT HAS COLLIDED
        const sf::Time LIFE_TIME;

        //how much time has passed since bullet was first fired
        //increased every update loop by delta time
        sf::Time elapsedTime;

        bool canCollide;

        const int damage;

        //floor the bullet was fired on
        unsigned floor;

        void interpolateTracer();

    public:

        Bullet(std::tr1::shared_ptr<LineSegment> bulletLine, const unsigned& originFloor, const int& bulletDamage);

        void updateElapsedTime(const sf::Time& deltaTime);

        //a live bullet is just a bullet that can be drawn, if it's dead it should be deleted
        bool canDraw();

        //checks if a bullet should be used to handle collision
        bool checkCanCollide();

        //disabling a bullet means to stop it from being able to damage or collide with other entities
        void disableCollision();

        //amount of damage this bullet should do
        int getDamage();

        const unsigned getFloor() const;

        void draw(sf::RenderWindow& window);

        void setEndPoint(const sf::Vector2f endPoint);

        std::tr1::shared_ptr<LineSegment> getLine();
};

#endif // BULLET_H_INCLUDED
