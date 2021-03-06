#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"

#include <tr1/memory>

class LineSegment;

//bullets are rays but they need to keep track of how long they're live
//bullets can only damage things the moment they're fired, the actual bullet body is seen as a pellet
//after a bullet is fired and the collision detection occurs the pellet is interpolated from the starting point to the end to give the illusion of a bullet moving
class Bullet {

    private:

        //line of the bullet
        std::tr1::shared_ptr<LineSegment> line;

        //because bullets look odd when they are drawn as a line, have this bullet image travel from the origin of the bullet to the end
        //to make it look like a bullet was fired
        sf::RectangleShape bulletTracer;

        //total distance tracer has traveled, if tracer travels more than the distance from the bullet's start to end points, stop drawing the tracer
        sf::Vector2f tracerTravelDistance;

        //how long the bullet should live for AFTER IT HAS COLLIDED
        const sf::Time LIFE_TIME;

        //how much time has passed since bullet was first fired
        //increased every update loop by delta time
        sf::Time elapsedTime;

        //the speed of the tracer should be such that it can cover the initial distance of the bullet within it's lifetime
        //that is, the length of the line segment of the bullet before it collides with anything / life time of the bullet = velocity
        sf::Vector2f tracerVelocity;

        bool canCollide;

        const int damage;

        //floor the bullet was fired on
        unsigned floor;

        //the tracer needs to stop drawing once it reaches near the end point
        bool shouldDrawTracer();
        void moveTracer(const sf::Time& delta);

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
