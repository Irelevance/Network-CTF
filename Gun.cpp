#include "Gun.h"
#include "LineSegment.h"
#include "Bullet.h"
#include <cmath>
#include "math.h"
#include "Floors.h"
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
    rotation(0.0),
    accuracyModifier(0, 0),
    queuedRotations(),
    fired(false),
    timeSinceFired(sf::seconds(0)),
    fireDelay(sf::milliseconds(200)),
    floor(OVERGROUND_FLOOR)
    {
        //set a default location for the line of sight
        updateLineOfSight(sf::Vector2f(0, 0), 0.0);
        lineOfSight[0].texCoords = sf::Vector2f(0, 0);
        lineOfSight[1].texCoords = sf::Vector2f(100, 0);

        lineTexture.loadFromFile("texture.png");
    }

void Gun::handleButtonPress() {

    //being firing and set the time since last fired equal to the fire delay that way gun fires immediately
    //so spamming the fire button is faster than holding it if user presses fast enough
    fired = true;
    timeSinceFired = fireDelay;
}

void Gun::handleButtonRelease() {

    fired = false;
}

void Gun::updateGunfire(const sf::Time& delta) {

    timeSinceFired += delta;

    //if gun if firing and the delay is over shoot a bullet
    if(fired && timeSinceFired > fireDelay) {

        fire();
        timeSinceFired = sf::seconds(0);
    }
}

void Gun::updateBullets(const sf::Time& delta) {

    //update all bullets and remove the ones that are dead
    for(unsigned i = 0; i < bullets.size();) {

        bullets[i]->updateElapsedTime(delta);

        //if a bullet can't be drawn it's dead and should be deleted
        if(!bullets[i]->canDraw()) {

            bullets.erase(bullets.begin() + i);
            continue;
        }

        i++;
    }
}

void Gun::updateRotation(const sf::Vector2f& playerPosition, const float& playerRotation) {

    updateLineOfSight(playerPosition, playerRotation);
}

void Gun::drawAll(sf::RenderWindow& window) {

    drawSight(window);
    drawBullets(window);
}

void Gun::drawSight(sf::RenderWindow& window) {

    window.draw(lineOfSight, &lineTexture);
}

void Gun::drawBullets(sf::RenderWindow& window) {

    for(auto& bullet : bullets) {

        //only draw the bullet if it has checked for collision
        //if it hasn't hcecked for collision then it is a brand new bullet, and new bullets may look as if they are going through all other entities
        //when infact they check for collision the next update and onlly are cut off
        if(!bullet->checkCanCollide()) {

            bullet->draw(window);
        }
    }
}

vector<shared_ptr<Bullet> > Gun::getBullets() {

    return bullets;
}

const vector<float>& Gun::getQueuedRotations() const {

    return queuedRotations;
}

void Gun::clearRotations() {

    queuedRotations.clear();
}

void Gun::setFloor(const unsigned& newFloor) {

    floor = newFloor;
}

float Gun::fire() {

    //angle at which gun was fired
    float gunfireAngle = rotation;

    //modify the rotation by randomly adding and subtracting the accuracy modifier in order to randomly change range
    //because the range is always positive you have to subtract and add a value manually
    gunfireAngle -= getRand(accuracyModifier.y, accuracyModifier.x);
    gunfireAngle += getRand(accuracyModifier.y, accuracyModifier.x);

    sf::Vector2f bulletEndPoint = calculateEndPoint(lineOfSight[0].position, gunfireAngle);

    createBullet(lineOfSight[0].position, bulletEndPoint, floor);

    //now queue the rotation so data can be sent to server later
    queuedRotations.push_back(gunfireAngle);

    return gunfireAngle;
}

void Gun::fire(const float& angle) {

    //calculate the end point of the bullet on the given angle
    sf::Vector2f endPoint = calculateEndPoint(lineOfSight[0].position, angle);

    //fire with new angle
    createBullet(lineOfSight[0].position, endPoint, floor);
}

void Gun::fire(const sf::Vector2f& bulletBegin, const sf::Vector2f& bulletEnd, const unsigned& bulletFloor) {

    createBullet(bulletBegin, bulletEnd, bulletFloor);

    //a bullet created using start and end points are bullets sent from the server
    //meaning thye have already checked for collision
    //so indcate the bullet has checked for collision, this bullet is the last bullet added to the container
    bullets[bullets.size() - 1]->disableCollision();
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

void Gun::createBullet(const sf::Vector2f& bulletBegin, const sf::Vector2f& bulletEnd, const unsigned& bulletFloor) {

    //create a line for the bullet
    shared_ptr<LineSegment> bulletLine = createLine(bulletBegin, bulletEnd,
                                                    "", sf::Vector2f(0, 0), sf::Vector2f(0, 0));
    shared_ptr<Bullet> bullet(new Bullet(bulletLine, bulletFloor));

    bullets.push_back(bullet);
}
