#include "Gun.h"
#include "LineSegment.h"
#include "Bullet.h"
#include <cmath>
#include "math.h"
#include <iostream>

using std::cos;
using std::sin;
using std::tr1::shared_ptr;
using std::vector;

Gun::Gun() :
    lineOfSight(sf::Lines, 2),
    lineTexture(),
    bullets(),
    MAX_DISTANCE_FIRED(2000.f),
    rotation(0.0)
    {
        //set a default location for the line of sight
        updateLineOfSight(sf::Vector2f(0, 0), 0.0);
        lineOfSight[0].texCoords = sf::Vector2f(0, 0);
        lineOfSight[1].texCoords = sf::Vector2f(100, 0);

        lineTexture.loadFromFile("texture.png");
    }

void Gun::updateRotation(const sf::Vector2f& playerPosition, const float& playerRotation) {

    updateLineOfSight(playerPosition, playerRotation);
}

void Gun::draw(sf::RenderWindow& window) {

    ///window.draw(lineOfSight, &lineTexture);

    for(auto bullet : bullets) {

        bullet->draw(window);
    }
}

vector<shared_ptr<Bullet> > Gun::getBullets() {

    return bullets;
}

float Gun::fire() {

    createBullet(lineOfSight[0].position, lineOfSight[1].position);

    //this gun just fires in the same place as its rotation
    return rotation;
}

void Gun::fire(const float& angle) {

    //calculate the end point of the bullet on the given angle
    sf::Vector2f endPoint = calculateEndPoint(lineOfSight[0].position, angle);

    //fire with new angle
    createBullet(lineOfSight[0].position, endPoint);
}

void Gun::fire(const sf::Vector2f& bulletBegin, const sf::Vector2f& bulletEnd) {

    createBullet(bulletBegin, bulletEnd);
}

void Gun::updateBullets(const sf::Time& delta) {

    //update all bullets and remove the ones that are dead
    for(unsigned i = 0; i < bullets.size();) {

        bullets[i]->updateElapsedTime(delta);
        bullets[i]->disableCollision();

        //if a bullet can't be drawn it's dead and should be deleted
        if(!bullets[i]->canDraw()) {

            bullets.erase(bullets.begin() + i);
            continue;
        }

        i++;
    }
}

void Gun::updateLineOfSight(const sf::Vector2f& origin, const float& newRotation) {

    rotation = newRotation;
    lineOfSight[0].position = origin;

    //calculate the end point of the line of sight
    sf::Vector2f endPoint = calculateEndPoint(origin, rotation);

    lineOfSight[1].position = endPoint;
}

sf::Vector2f Gun::calculateEndPoint(const sf::Vector2f& beginPoint, const float& angle) const {

    //using the angle you can figure out the x and y components of the
    //triangle formed form the origin to the end point because you know the
    //lenght of the hypotenuse
    float xComponent = cos(degreesToRadians(angle)) * MAX_DISTANCE_FIRED;

    //in games y is positive towards the bottom and negative towards the top
    //so inverse the sign of the y component because the math calculations assumes
    //y is positive towards the top and negative towards the bottom
    float yComponent = -sin(degreesToRadians(angle)) * MAX_DISTANCE_FIRED;

    //find final position by addition the components to the initial position
    //because the x and y components are already negative or positive depending on which
    //way the player is aiming in order to reach that position
    sf::Vector2f endPoint(beginPoint.x + xComponent, beginPoint.y + yComponent);

    return endPoint;
}

void Gun::createBullet(const sf::Vector2f& bulletBegin, const sf::Vector2f& bulletEnd) {

    //create a line for the bullet
    shared_ptr<LineSegment> bulletLine = createLine(bulletBegin, bulletEnd,
                                                    "", sf::Vector2f(0, 0), sf::Vector2f(0, 0));
    shared_ptr<Bullet> bullet(new Bullet(bulletLine));

    bullets.push_back(bullet);
}