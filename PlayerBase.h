#ifndef PLAYERBASE_H_INCLUDED
#define PLAYERBASE_H_INCLUDED

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "Healthbar.h"
#include "FlagManager.h"
#include "Gun.h"
#include "soundSettings.h"

#include <vector>
#include <map>
#include <tr1/memory>
#include <string>

class Gun;
class Bullet;
class Flag;

class PlayerBase {

    protected:

        enum DrawingState {

            STANDING,
            WALKING,
            DYING,
            SPAWNING
        };

        //texture for the indicator that is drawn when player is holding a flag
        //load the texture for both flag indicators so they only have to be loaded once
        sf::Texture redIndicatorTexture;
        sf::Texture blueIndicatorTexture;

        sf::Sprite flagIndicator;

        //texture to use when player is healing
        sf::Texture regenTexture;
        sf::Sprite regenSprite;

        //clips for each frame of teh regeneration animation
        std::vector<sf::IntRect> regenClips;

        unsigned regenFrame;

        sf::Clock regenAnimationTimer;
        sf::Time regenAnimationTime;

        bool playRegenAnimation;

        //id of this player, supplied by the server
        int playerId;

        //id of the team the player is in
        unsigned short teamId;

        //players hit box for drawing, movement, and collision
        //current hitbox is the current position of the player which will interpolate towards the destination box
        //previous hitbox is the past position of the player which interpolates towards the destination box
        //past and destination are used for interpolation, past is the beginning point of interpolation
        sf::RectangleShape pastHitBox;
        sf::RectangleShape currentHitBox;
        sf::RectangleShape destinationHitBox;

        HealthBar health;

        //image for players sprite
        sf::Texture playerTexture;

        sf::Sprite playerSprite;

        unsigned frame;

        sf::Clock animationTimer;

        sf::Time animationTime;

        //connect the player's team to an animation clip
        //key is player team, animation clips for that teams color
        std::map<unsigned short, std::vector<sf::IntRect> > clips;

        bool canPlayDeathAnimation;
        bool playSpawnAnimation;

        //texture, clip, and sprites for the spawn and death animations
        sf::Texture spawnTexture;

        sf::Sprite spawnSprite;

        std::vector<sf::IntRect> spawnClips;
        std::vector<sf::IntRect> deathClips;

        //rotation of the player in degrees, interolates like the hitbox
        float pastRotation;
        float currentRotation;
        float destinationRotation;

        std::tr1::shared_ptr<Gun> gun;

        //flag the player is holding
        std::tr1::weak_ptr<Flag> flagBeingHeld;

        //timer to time how long its been since the last time data about this player was received from either the server or client
        sf::Clock dataReceiveTimer;

        //maximum amount of time this player should go without receiving data
        sf::Time maxNoData;

        //timer to respawn player after he is dead
        sf::Clock respawnTimer;
        sf::Time respawnDelay;

        std::string playerName;

        unsigned short kills;
        unsigned short deaths;
        unsigned short flagCaptures;
        unsigned short flagReturns;

        //maximum distance the player can interpolate to, this is the squared distnace
        const float maxInterpolationDist;

        //current floor of the player, can only interact with others in the same floor
        unsigned short currentFloor;

        bool pickingUpGun;

        sf::SoundBuffer respawnBuffer;
        sf::Sound respawnSound;

        sf::SoundBuffer getFlagBuffer;
        sf::Sound getFlagSound;

        sf::SoundBuffer scoreBuffer;
        sf::Sound scoreSound;

        bool shouldPlaySounds;

        void setupClips();

        //sets position without interpolation
        void setPosition(const sf::Vector2f& position);

        void updateHitboxRotation();

        //update teh position of the healthbar, tracks players current hitbox
        void updateHealthPosition();

        //update the position of the player's sprite, places it at the players hitbox position
        void updateSpritePosition();

        void die();

        //overload in inherited class to draw differnt part of guns
        virtual void drawGun(sf::RenderWindow& window, const unsigned& drawingFloor) = 0;

        virtual DrawingState getDrawingState() = 0;

    public:

        PlayerBase();

        virtual ~PlayerBase();

        std::vector<std::tr1::shared_ptr<Bullet> > getBullets();

        int getId();
        void setId(const int& id);

        unsigned short getTeam() const;
        void setTeam(const unsigned short& team);

        const float getRotation() const;

        virtual void updateGun(const float& delta);

        //make player interpolate from current position to the given position
        //called by several functions related to the player class and interpolationg player class
        //you can also choose to not reset the data timer if you are interpolating wihtout using server data
        void setInterpolationPosition(const sf::Vector2f& position, bool resetData = true);

        //interpolates from current position to given position
        //instead of jumping to the last destination postiion and interpolating from there
        void setInterpolationPositionV2(const sf::Vector2f& position);

        //interpolate from previous position to the destination position
        ///time passed is what fraction of time has passed from the physics update to the next physics update, not the actual time passed
        virtual void interpolate(const float& deltaFraction);
        void stopInterpolation();

        void animate();

        virtual void draw(sf::RenderWindow& window, const unsigned& drawingFloor = 0);

        //drawing components to show on minimap
        void drawMinimap(sf::RenderWindow& window);

        const sf::Vector2f& getDestinationPosition() const;

        const sf::Vector2f& getCurrentPosition() const;

        //just returns the player's current position box, usually jsut used to indicate his properties
        sf::FloatRect getCurrentHitbox() const;

        sf::FloatRect getCollisionBox() const;

        sf::FloatRect getDestinationBox() const;

        void setPlayRegenAnimation(bool shouldPlay) {

            //regeneration animation plays if player is in the regeneration zone and he has health to regen
            playRegenAnimation = shouldPlay && !health.isFull() && isAlive();
        }

        void move(const sf::Vector2f& offset);

        int getHealth() const;

        bool isAlive() const;
        bool shouldRespawn() const;

        void respawn(const sf::Vector2f& spawnPosition);

        //uses sethealth
        void getHit(int damage);

        //set helath to given value, if its less than current health then run damage animation
        //if its more than current health, run regeneration animation
        void setHealth(int value);

        bool isHoldingFlag() const;

        void holdFlag(std::tr1::shared_ptr<Flag> flagToHold);

        void dropFlag();

        //drop the flag at the given position
        void dropFlag(const sf::Vector2f& position);

        void playScoreSound() {

            if(GLO_PLAY_SOUNDS) {

                scoreSound.play();
            }
        }

        void setPlaySounds(bool play) {

            shouldPlaySounds = play;
            gun->setPlaySounds(play);
        }

        bool checkPlaySounds() {

            return shouldPlaySounds;
        }

        void setFloor(const unsigned& destinationFloor);
        const unsigned getFloor() const;

        bool timedOut();
        void resetDataTimer();

        std::tr1::shared_ptr<Gun> getGun();
        void setGun(std::tr1::shared_ptr<Gun> newGun);

        void resetStats();
        void resetGun();

        bool canPickUpGun() {

            return pickingUpGun;
        }

        void setPickUpGun(bool toPick) {

            pickingUpGun = toPick;
        }

        const std::string& getName() const;
        const unsigned short& getKills() const;
        const unsigned short& getDeaths() const;
        const unsigned short& getFlagCaptures() const;
        const unsigned short& getFlagReturns() const;

        void setName(const std::string& name);
        void setKills(const unsigned short& amount);
        void setDeaths(const unsigned short& amount);
        void setCaptures(const unsigned short& amount);
        void setReturns(const unsigned short& amount);
};

#endif // PLAYERBASE_H_INCLUDED
