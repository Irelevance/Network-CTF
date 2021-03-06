#ifndef SHOTGUN_H_INCLUDED
#define SHOTGUN_H_INCLUDED

#include "Gun.h"

class Shotgun : public Gun {

    private:

        //number of bullets shotgun should fire
        //no matter the damage done by the shotgun bullets if you're hit by all of them
        //you should get killed
        const int bulletsPerShot;

        //each magazine of a shotgun fires multiple bullets
        //however when a single bullet is fired the ammo count goes down
        //because the server handles each shot individully
        //so keep track of how many bullets were fired and only reduce the ammo count
        //if the given number of bullets were fired
        //amount of bullets a shotgun needs to fire in order to reduce current magazine count is equal to bulletspershot
        int bulletsFired;

        static sf::SoundBuffer fireBuffer;
        static sf::SoundBuffer reloadBuffer;

        sf::Clock soundTimer;
        sf::Time soundDelay;

        void setupClips();

    protected:

        virtual void useAmmo();

        virtual void handleFireSound() {

            if(soundTimer.getElapsedTime() > soundDelay && GLO_PLAY_SOUNDS) {

                fireSound.play();
                soundTimer.restart();
            }
        }

    public:

        Shotgun();
        virtual ~Shotgun(){}

        virtual void updateGunfire(const sf::Time& delta);

        virtual GunTypes getGunType();
};

#endif // SHOTGUN_H_INCLUDED
