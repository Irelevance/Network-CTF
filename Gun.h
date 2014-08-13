#ifndef GUN_H_INCLUDED
#define GUN_H_INCLUDED

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"

#include <tr1/memory>
#include <vector>

class LineSegment;
class Bullet;

//gun for player, creates a ray bullet when fired
//when idle gun will show direction player is aiming
class Gun {

    protected:

        //line used to show direction of player's aim
        ///the position of the first point of the line of sight is also the position of the gun
        ///and the origin of all bullets
        sf::VertexArray lineOfSight;

        //texture for the line of sight to give it graphics
        sf::Texture lineTexture;

        std::vector<std::tr1::shared_ptr<Bullet> > bullets;

        //maximum distance the gun can shoot in order to draw the line of sight and determine bullet's start and end points
        //that way players can't shoot accross the map
        const float MAX_DISTANCE_FIRED;

        //angle of player's rotation in degrees
        //this is the angle in standard position
        float rotation;

        //range of values to add or subtract to the gun's rotation while firing
        //in order to change the trajectory of the bullet to have the guns firing with different accuracies
        //range is from 0 to accuracy modifier
        //all in degrees
        ///always positive
        float accuracyModifier;

        //container containing the rotation of the gun when the gun was fired
        //these rotations can be sent to the server in order to replicate the gunfire on the server
        //should be cleared when data is sent to server
        std::vector<float> queuedRotations;

        //flag to determine if the gun is being fired
        bool fired;

        //timer to keep track of how long it has been since the gun was last fired
        //time is accumulated through the delta time sent by update loop
        sf::Time timeSinceFired;

        sf::Time fireDelay;

        //current floor
        unsigned floor;

        int bulletDamage;

        //calculate the end point of the bullet once fired
        sf::Vector2f calculateEndPoint(const sf::Vector2f& beginPoint, const float& angle) const;

        //set the new position of the line of sight using the player's origin and rotation
        virtual void updateLineOfSight(const sf::Vector2f& origin);

        //creates a bullet using the guns current state
        void createBullet(const sf::Vector2f& bulletBegin, const sf::Vector2f& bulletEnd, const unsigned& floor);

        //get the accuracy modifier to use when firing the gun
        //different guns might use different accuracy modifier than the default one
        virtual const float getAccuracyModifier() const;

    public:

        Gun(const int& damage = 10, const float& maxDist = 2000.f, const sf::Time& fireDelay = sf::milliseconds(200), const float& accuracyMod = 5);
        virtual ~Gun(){}

        //handle a player's mouselick when he tries to fire a gun
        //have this function in order to have guns that handle button presses differently
        virtual void handleButtonPress();
        virtual void handleButtonRelease();

        //determines if it is time to fire again and fires
        virtual void updateGunfire(const sf::Time& delta);

        void updateBullets(const sf::Time& delta);

        void updateRotation(const sf::Vector2f& playerPosition, const float& playerRotation);

        void drawAll(sf::RenderWindow& window);

        //draw gun's line of sight
        virtual void drawSight(sf::RenderWindow& window);

        //draw the bullets that have already checked for collision with other entities
        void drawBullets(sf::RenderWindow& window);

        std::vector<std::tr1::shared_ptr<Bullet> > getBullets();
        const std::vector<float>& getQueuedRotations() const;

        void clearRotations();

        void setFloor(const unsigned& floor);

        //returns the angle the gun shot the bullet at because guns could have different accuracies, this can be used to determine where the gun was shot
        float fire();

        //fire the gun at the given angle, does not change angle of gun
        void fire(const float& angle);

        //fire a gun and place the bullet at the given location
        void fire(const sf::Vector2f& bulletBegin, const sf::Vector2f& bulletEnd, const unsigned& floor);

};

#endif // GUN_H_INCLUDED
