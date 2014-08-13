#include "Bullet.h"
#include "LineSegment.h"

using std::tr1::shared_ptr;

Bullet::Bullet(std::tr1::shared_ptr<LineSegment> bulletLine, const unsigned& originFloor, const int& bulletDamage) :
    line(bulletLine),
    LIFE_TIME(sf::milliseconds(160)),
    elapsedTime(sf::seconds(0)),
    canCollide(true),
    damage(bulletDamage),
    floor(originFloor)
    {

    }

void Bullet::updateElapsedTime(const sf::Time& deltaTime) {

    elapsedTime += deltaTime;
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

    line->draw(window);
}

void Bullet::setEndPoint(const sf::Vector2f endPoint) {

    line->setEndPoint(endPoint);
}

shared_ptr<LineSegment> Bullet::getLine() {

    return line;
}
