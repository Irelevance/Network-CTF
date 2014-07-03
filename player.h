#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"

#include <vector>
#include <tr1/memory>

class Gun;
class Bullet;

class Player {

    //define anything that needs to be public
    //if defined in the private section then it can't be accessed so define things up here that way both the private and public functions can access
    public:

        //the different types of actions the player can do
        //in order to save the command information and exchange with the server
        enum Action {

            PRESS_LEFT = 0,
            PRESS_RIGHT,
            PRESS_UP,
            PRESS_DOWN,

            RELEASE_LEFT,
            RELEASE_RIGHT,
            RELEASE_UP,
            RELEASE_DOWN
        };

        //information about what the player inputted and when he inputted it
        //used to create a queue of commands which are only confirmed once the server sends an update saying the command has been confirmed
        struct Input {

            Action action;

            sf::Uint32 inputId;
        };

        //player's state, used to save the player's position and other information in order to receive updates from the server
        struct State {

            sf::Vector2f position;
            float rotation;
        };

    private:

        //use invalid inputs when you search for an input that doesn't exist
        const sf::Uint32 INVALID_INPUT_ID;

        //id of this player, supplied by the server
        int playerId;

        //players hit box for drawing, movement, and collision
        //current hitbox is the current position of the player which will interpolate towards the destination box
        //previous hitbox is the past position of the player which interpolates towards the destination box
        //past and destination are used for interpolation, past is the beginning point of interpolation
        sf::RectangleShape pastHitBox;
        sf::RectangleShape currentHitBox;
        sf::RectangleShape destinationHitBox;

        sf::Vector2f velocities;

        //rotation of the player in degrees, interolates like the hitbox
        float pastRotation;
        float currentRotation;
        float destinationRotation;


        std::tr1::shared_ptr<Gun> gun;

        //a buffer of all inputs the player has used
        //every timestep (one iteration in the game loop) add all the events the player is activating into the buffer, don't process the events right away
        //every time step just process the next event in the queue
        //whenever the server sends an update, find the input that the server confirmed and delete all inputs prior to that one and reprocess the events from that point forward
        std::vector<Input> inputBuffer;

        //a list of all the events that need to be sent to the server for confirmation this loop iteration
        std::vector<Input> inputsToSend;

        //list of all shots player has fired that haven't been sent to server, however the only data that needs to be saved is the rotation of the player
        //when the shot was fired so its just a vector of floats
        std::vector<float> queuedGunshotRotations;

        //the id of the input that should be processed
        sf::Uint32 currentInputId;

        //each input's ide is 1 more than the last so keep track of what id we're on so you can actually give input's ids
        sf::Uint32 nextNewInputId;

        const float getHorizontalVelocity();
        const float getVerticalVelocity();

        //find the next input to process if there is any, if there is none then return an invalid input
        //advances the current input id up 1 if an input is found
        Input getInputToProcess();

        void processInput(const Input& inputToProcess);

        //insert the given input into the list of inputs to execute and the list of inputs to send to the server for processing
        //the inputs have to be brand new, meaning created right before this function is called
        void placeIntoQueue(Input inputToQueue);

        //remove the input with the given id from all queues, the id is not the position in the vector, it is Input::inputId
        void removeConfirmedInputs(const sf::Uint32& lastConfirmedInputId);

        Input createInput(const Action& playerAction);

        void updateHitboxRotation();

        //fire the gun and place rotations into queue
        void fireGun();

    public:

        Player();

        //handle any input the player triggered and save them to send to the server
        void handleEvent(sf::Event& event);

        //update the player's state according to the server's data and replay all events from the last input confirmed by the server
        void handleServerUpdate(const State& stateUpdate, const sf::Uint32& lastConfirmedInputId);

        const std::vector<Input>& getInputsToSend() const;

        const std::vector<float>& getGunshotsToSend() const;

        std::vector<std::tr1::shared_ptr<Bullet> > getBullets();

        void clearGunshotQueue();

        //clear all the inputs in queue to be sent to the server
        //should be called after you send the inputs to the server so player knows to empty it's containers so it doesn't send old input
        void clearInputsToSend();

        int getId();
        void setId(const int& id);

        const float getRotation() const;

        void setRotation(const float& newRotation);

        //sets a rotation for hte player to interpolate towards
        void setInterpolationRotation(const float& newRotation);

        //fire the gun at the given angle
        void fireGun(const float& angle);

        //fire a gun and place the bullet at the given location
        void fireGun(const sf::Vector2f& bulletBegin, const sf::Vector2f& bulletEnd);

        //given the client input simple execute it
        void handleClientInput(Input& clientInput);

        void update(const float& delta, const sf::Vector2f& screenSize);

        void updateGun(const float& delta);

        //update the player's rotation using the mouse position, not the same as set rotation which jsut sets the rotation of the player
        //uses set rotation
        void updateRotation(const sf::Vector2f& mousePosition);

        //update without interpolating
        void forceUpdate(const float& delta, const sf::Vector2f& screenSize);

        //make player interpolate from current position to the given position
        void setInterpolationPosition(const sf::Vector2f& position);

        //interpolate from previous position to the destination position
        ///time passed is what fraction of time has passed from the physics update to the next physics update, not the actual time passed
        void interpolate(const float& deltaFraction);
        void draw(sf::RenderWindow& window);

        const sf::Vector2f& getPosition() const;

        //just returns the player's current position box, usually jsut used to indicate his properties
        sf::FloatRect getCollisionRect() const;
};

#endif // PLAYER_H_INCLUDED