#ifndef GAMEMANAGER_H_INCLUDED
#define GAMEMANAGER_H_INCLUDED

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "GameWorld.h"
#include "HeadsUpDisplay.h"
#include "Floors.h"

#include <vector>
#include <tr1/memory>

class Block;
class ForegroundObject;
class FlagManager;
class StatDisplay;
class ScoreDisplay;

//base game manager class for client and server to derive from
//the internal functions are virtual because the functions have the same outer appearence but the inner workings are different
class GameManager {

    private:

        //timer for calculating delta time for physics
        sf::Clock timer;

        //until enough time has passed a physics update will not occure and the time will just accumulate
        sf::Time accumulator;

        //optimal timestep (amount of time passed) for physics simulation to run smoothly
        sf::Time optimalTimeStep;

        GameWorld world;

        HeadsUpDisplay headsUpDisplay;

        //view that uses the current window size, in order to draw UI and stuff onto the screen
        sf::View currentWindow;

        //handling input function runs the input loop, all other input functions have to work assuming its inside the input loop
        void handleInputs(sf::Event& event, sf::RenderWindow& window);

        //handles inputs that must be handled regardless of the game state
        void handleCommonInputs(sf::Event& event, sf::RenderWindow& window);

        //updates all objects in the world, updates in two seperate parts: objects that require a delta time and objects that don't
        void updateWorld(sf::RenderWindow& window);

        void drawWorld(sf::RenderWindow& window);

        void setupCurrentWindow(sf::RenderWindow& window);

    protected:

        //amount of points required for a team to win the game
        short unsigned pointsToWinGame;

        //when the player exits the game loop to return to lobby or something
        bool exitGameLoop;

        //do anything that needs to be done before the game starts running
        //so do all the set up
        //make it virtual so derived classes can modify it to do different things
        virtual void setup(sf::RenderWindow& window);

        //handle events related to the window, such as resizing
        virtual void handleWindowEvents(sf::Event& event, sf::RenderWindow& window);

        //handle input from other components, different for each derived class
        virtual void handleComponentInputs(sf::Event& event, sf::RenderWindow& window) = 0;

        virtual void handleStateEvents(sf::RenderWindow& window) {

            //empty for now
        }

        virtual void updateComponents(sf::RenderWindow& window) = 0;

        //updates objects that behave according to delta time
        virtual void updateTimeComponents(const float& delta, sf::RenderWindow& window) = 0;

        //handle everything that needs to be called after the update loop is completed
        virtual void handlePostUpdate(sf::RenderWindow& window) = 0;

        //draw all components
        virtual void drawComponents(sf::RenderWindow& window) = 0;

        virtual void drawForeground(sf::RenderWindow& window) {

        }

        //all UI that are drawn are drawn independantly from the camera so they don't need to be moved along with the camera inorder to say on screen
        //this also means that if you need to interact with the UI you need to reset the view to the default view in order to be able to click on them
        virtual void drawUI(sf::RenderWindow& window);
        virtual void drawMinimap(sf::RenderWindow& window);

        //handle collision for all objects
        virtual void handleCollisions() = 0;

        //get the current floor to draw
        virtual const unsigned getFloor() const = 0;

        std::vector<std::tr1::shared_ptr<Block> >& getBlocks(const unsigned& floor = OVERGROUND_FLOOR);
        std::vector<std::tr1::shared_ptr<ForegroundObject> >& getForeground(const unsigned& floor = OVERGROUND_FLOOR);
        std::vector<std::tr1::shared_ptr<Portal> >& getPortals(const unsigned& floor = OVERGROUND_FLOOR);
        std::vector<std::tr1::shared_ptr<GunGiver> >& getGunGivers(const unsigned& floor = OVERGROUND_FLOOR);

        std::tr1::shared_ptr<FlagManager> getFlagManager();

        HeadsUpDisplay& getHeadsUpDisplay() {

            return headsUpDisplay;
        }

        GameWorld& getGameWorld();

        StatDisplay& getStatDisplay() {

            return headsUpDisplay.getStatDisplay();
        }

        ScoreDisplay& getScoreDisplay() {

            return headsUpDisplay.getScoreDisplay();
        }

        //calculate what fraction of time has passed since the last physics update to the next physics update
        const float calculateDeltaFraction();

    public:

        GameManager();

        //run the entire game, manages all input, updates and drawings
        void runGame(sf::RenderWindow& window);
};

#endif // GAMEMANAGER_H_INCLUDED
