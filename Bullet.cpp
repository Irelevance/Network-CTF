#include "Bullet.h"
#include "LineSegment.h"
#include "math.h"
#include "Lerp.h"

using std::tr1::shared_ptr;

Bullet::Bullet(std::tr1::shared_ptr<LineSegment> bulletLine, const unsigned& originFloor, const int& bulletDamage) :
    line(bulletLine),
    bulletTracer(sf::Vector2f(25, 2)),
    LIFE_TIME(sf::milliseconds(200)),
    elapsedTime(sf::seconds(0)),
    canCollide(true),
    damage(bulletDamage),
    floor(originFloor)
    {
        bulletTracer.setPosition(bulletLine->getStartPoint());
        bulletTracer.setOrigin(calculateCenter(bulletTracer.getLocalBounds()));
        bulletTracer.setRotation(-calculateAngle(bulletLine->getStartPoint(), bulletLine->getEndPoint()));
        bulletTracer.setFillColor(sf::Color::Yellow);
    }

void Bullet::updateElapsedTime(const sf::Time& deltaTime) {

    elapsedTime += deltaTime;

    //bullet's lifetime is only valid if it has collided already
    //if it hasn't collided then the time shouldn't move
    if(checkCanCollide()) {

        elapsedTime = sf::milliseconds(0);
        return;
    }

    interpolateTracer();
}

bool Bullet::canDraw() {

    return elapsedTime < LIFE_TIME;
}

bool Bullet::checkCanCollide() {

    return canCollide;
}

void Bullet::disableCollision() {

    canCollide = false;
}

int Bullet::getDamage() {

    return damage;
}

const unsigned Bullet::getFloor() const {

    return floor;
}

void Bullet::draw(sf::RenderWindow& window) {

    window.draw(bulletTracer);
}

void Bullet::setEndPoint(const sf::Vector2f endPoint) {

    line->setEndPoint(endPoint);
}

shared_ptr<LineSegment> Bullet::getLine() {

    return line;
}

void Bullet::interpolateTracer() {

    //move the tracer closer to the end point of the bullet
    const float xTransition = interpolate(line->getStartPoint().x, line->getEndPoint().x, elapsedTime.asSeconds() / LIFE_TIME.asSeconds());
    const float yTransition = interpolate(line->getStartPoint().y, line->getEndPoint().y, elapsedTime.asSeconds() / LIFE_TIME.asSeconds());

    bulletTracer.setPosition(line->getStartPoint().x + xTransition, line->getStartPoint().y + yTransition);
}
