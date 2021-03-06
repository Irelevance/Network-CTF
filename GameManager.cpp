#include "GameManager.h"
#include "LevelManager.h"
#include "Minimap.h"

#include <iostream>

using std::cout;
using std::endl;
using std::vector;
using std::tr1::shared_ptr;

GameManager::GameManager() :
    timer(),
    accumulator(),
    optimalTimeStep(sf::milliseconds(30)),
    world(),
    headsUpDisplay(sf::Vector2u(1024, 768)),
    currentWindow(),
    pointsToWinGame(1),
    exitGameLoop(false)
    {
        world.load("level/untitled");
        headsUpDisplay.getMinimap().setLevelSize(sf::Vector2f(3968, 1472));
    }

void GameManager::runGame(sf::RenderWindow& window) {

    //setup is virtual so this will be a polymorphic call depending on what class calls it
    //first setup for the base game manager, then setup for the derived classes
    GameManager::setup(window);
    this->setup(window);

    sf::Event event;

    while(window.isOpen() && !exitGameLoop) {

        handleInputs(event, window);

        updateWorld(window);

        drawWorld(window);
    }

    //reset exit game loop that way if the game starts again it wont exit immediately
    exitGameLoop = false;
}

void GameManager::handleInputs(sf::Event& event, sf::RenderWindow& window) {

    while(window.pollEvent(event)) {

        handleCommonInputs(event, window);

        GameManager::handleWindowEvents(event, window);

        this->handleWindowEvents(event, window);

        this->handleComponentInputs(event, window);
    }

    this->handleStateEvents(window);
}

void GameManager::handleCommonInputs(sf::Event& event, sf::RenderWindow& window) {

    if(event.type == sf::Event::Closed) {

        window.close();
    }
}

void GameManager::updateWorld(sf::RenderWindow& window) {

    this->updateComponents(window);

    //calculate the time passed since the last frame
    accumulator += timer.restart();

    while(accumulator > optimalTimeStep) {

        float delta = optimalTimeStep.asSeconds();

        this->updateTimeComponents(delta, window);

        //after updating objects handle collision between objects
        this->handleCollisions();

        accumulator -= optimalTimeStep;
    }

    this->handlePostUpdate(window);
}

void GameManager::drawWorld(sf::RenderWindow& window) {

    window.clear();

    world.drawBackground(window, getFloor());

    this->drawComponents(window);

    world.drawForeground(window, getFloor());

    this->drawForeground(window);

    GameManager::drawUI(window);

    window.display();
}

void GameManager::setupCurrentWindow(sf::RenderWindow& window) {

    currentWindow.setSize(window.getSize().x, window.getSize().y);
    currentWindow.setCenter(window.getSize().x / 2, window.getSize().y / 2);
}

void GameManager::setup(sf::RenderWindow& window) {

    headsUpDisplay.handleScreenResize(window.getSize());

    setupCurrentWindow(window);
}

void GameManager::handleWindowEvents(sf::Event& event, sf::RenderWindow& window) {

    if(event.type == sf::Event::Resized) {

        headsUpDisplay.handleScreenResize(sf::Vector2u(event.size.width, event.size.height));

        setupCurrentWindow(window);
    }
}

void GameManager::drawUI(sf::RenderWindow& window) {

    //get the previous view before UI is drawn that way it can be reapplied later
    sf::View previousView = window.getView();

    //all UI is drawn on the defualt window because they don't move along with the camera and always stay on the same spot on the screen
    window.setView(currentWindow);

    headsUpDisplay.draw(window);

    ///GameManager::drawMinimap(window);

    //derived class UI
    this->drawUI(window);

    //re-apply old view
    window.setView(previousView);
}

void GameManager::drawMinimap(sf::RenderWindow& window) {

    //save the view of the window before the minimap is drawn that way the view can be restored and any other items being drawn wont be drawn onto the minimap
    sf::View previousView = window.getView();

    //set the minimaps view
    headsUpDisplay.getMinimap().applyMinimap(window);

    ///on minimap always draw overground
    world.drawBackground(window, OVERGROUND_FLOOR);

    //draw the derived class's minimap components
    this->drawMinimap(window);

    world.drawForeground(window, OVERGROUND_FLOOR);

    //re-apply old view
    window.setView(previousView);
}

vector<shared_ptr<Block> >& GameManager::getBlocks(const unsigned& floor) {

    return world.getBlocks(floor);
}

vector<shared_ptr<ForegroundObject> >& GameManager::getForeground(const unsigned& floor) {

    return world.getForeground(floor);
}

vector<shared_ptr<Portal> >& GameManager::getPortals(const unsigned& floor) {

    return world.getPortals(floor);
}

vector<shared_ptr<GunGiver> >& GameManager::getGunGivers(const unsigned& floor) {

    return world.getGunGivers(floor);
}

shared_ptr<FlagManager> GameManager::getFlagManager() {

    return world.getFlagManager();
}

GameWorld& GameManager::getGameWorld() {

    return world;
}

const float GameManager::calculateDeltaFraction() {

    return accumulator.asSeconds() / optimalTimeStep.asSeconds();
}
