#ifndef SNIPER_H_INCLUDED
#define SNIPER_H_INCLUDED

#include "Gun.h"

//sniper fires gun upon releasing the key, while holding down the fire button the accuracy just increases

class Sniper : public Gun {

    private:

        //holding down key so increase accuracy
        bool holdingFireButton;

        //current accuracy modifier, decreases while holding down fire button
        float currentAccuracyModifier;

        //absolute value of the rate at which accuracy modifer changes, in degrees/second
        float accuracyChangeRate;

        //sniper draws a grey triangle like thing to indicate line of sight, the narrorwer the triangle, the better your accuracy
        //use vertex array to show the accuracy range of the sniper
        sf::VertexArray accuracyRange;

        static sf::SoundBuffer fireBuffer;
        static sf::SoundBuffer reloadBuffer;

        //calculate the points of the triangle on the accuracy range based on rotations
        void setupAccuracyRange();
        void setupAccuracyPoints();
        void setupAccuracyColour();

        void drawAccuracyRange(sf::RenderWindow& window);

        bool canIncreaseAccuracy();

        void setupClips();

    protected:

        virtual void updateLineOfSight(const sf::Vector2f& origin);
        virtual const float getAccuracyModifier() const;

        virtual void handleFireSound() {

            if(GLO_PLAY_SOUNDS) {

                fireSound.play();
            }
        }

    public:

        Sniper();
        virtual ~Sniper(){}

        virtual void handleButtonPress();
        virtual void handleButtonRelease();

        virtual bool isAiming() {

            return canIncreaseAccuracy();
        }

        virtual void updateGunfire(const sf::Time& delta);

        virtual void drawSight(sf::RenderWindow& window);

        virtual void setPlaySounds(bool shouldPlay) {

            shouldPlaySounds = true;
        }

        virtual GunTypes getGunType();
};

#endif // SNIPER_H_INCLUDED
