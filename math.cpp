#include "math.h"
#include <cmath>
#include <cstdlib>

using std::abs;
using std::sin;
using std::cos;

const float PI = 3.14159265;

float calculateReferenceAngle(const float& angleStandardPosition) {

    //reference angle dependent on quadrant
    if(angleStandardPosition <= 90) {

        return angleStandardPosition;
    }

    if(90 > angleStandardPosition && angleStandardPosition <= 180) {

        return 180 - angleStandardPosition;
    }

    if(180 > angleStandardPosition && angleStandardPosition <= 270) {

        return angleStandardPosition - 180;
    }

    //last case is quandrant 4
    return 360 - angleStandardPosition;
}

float calculateAngle(const sf::Vector2f& beginPoint, const sf::Vector2f& endPoint) {

    //figure out reference angle by using the triangle that forms between the begin and end points
    //because we use tangent you have to make sure the denominator (x value) is not 0
    float vertical = endPoint.y - beginPoint.y;

    //in game world y is positive at the bottom and negative at the top, but the math needs to have y positive at the top and negative at the bottom so inverse the sign
    vertical *= -1;

    float horizontal = endPoint.x - beginPoint.x;

    if(horizontal == 0) {

        //check if the angle is 90 or 270 because when horizontal is 0 it's either pointing straight up or straight down
        //end point is above beginning so hes pointing straight up
        if(endPoint.y <= beginPoint.y) {

            return 90.f;
        }

        //pointing straight down
        return 270.f;
    }

    //angle is calculated in radians to covnert to degree before returning
    float angle = atan2(vertical, horizontal);

    return radiansToDegrees(angle);
}

float degreesToRadians(const float& angleInDegrees) {

    return angleInDegrees * PI / 180.f;
}

float radiansToDegrees(const float& angleInRadians) {

    return angleInRadians * 180.f / PI;
}

float distanceToPoint(const sf::Vector2f& beginPoint, const sf::Vector2f& endPoint) {

    //distance is a^2 + b^2 because this is te same as a triangle
    float horizontal = endPoint.x - beginPoint.x;
    float vertical = endPoint.y - beginPoint.y;

    return horizontal * horizontal + vertical * vertical;
}

const sf::Vector2f& rotate(const sf::Vector2f& vec, const float& rotation) {

    float rot = degreesToRadians(rotation);

    sf::Vector2f newVec(0, 0);

    newVec.x = vec.x * (cos(rot)) + vec.y * (-sin(rot));
    newVec.y = vec.x * (sin(rot)) + vec.y * (cos(rot));

    return newVec;
}

sf::Vector2f calculateCenter(const sf::FloatRect& rect) {

    sf::Vector2f center;
    center.x = rect.left + rect.width / 2;
    center.y = rect.top + rect.height / 2;

    return center;
}

sf::Vector2f calculateCenter(const sf::IntRect& rect) {

    sf::FloatRect rect1(rect.left, rect.top, rect.width, rect.height);

    return calculateCenter(rect1);
}

int getRand(int max, int min) {

    //if max == 0 you will divide by 0 and it will crash the program so if max is 0 then return 0
    if(max == 0) {

        return max;
    }

    ///see header as to why you are subtracting
    //make sure subtracted is a positive number otherwise the range [max, min] is changed
    int subtracted = min - 1 > 0 ? min - 1 : 0;

    max -= subtracted;
    min -= subtracted;

    return (rand() % max + min) + subtracted;
}

bool compareFloats(const float& float1, const float& float2, const float differenceThreshold) {

    return abs(float1 - float2) < differenceThreshold;
}

bool compareFloatRects(const sf::FloatRect& rect1, const sf::FloatRect& rect2) {

    bool samePositions = compareFloats(rect1.left, rect2.left) && compareFloats(rect1.top, rect2.top);
    bool sameSizes = compareFloats(rect1.width, rect2.width) && compareFloats(rect1.height, rect2.height);

    return samePositions && sameSizes;
}
