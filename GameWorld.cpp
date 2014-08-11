#include "GameWorld.h"
#include "Block.h"
#include "LevelManager.h"
#include "ForegroundObject.h"
#include "FlagManager.h"
#include "TeamManager.h"
#include "PlayerBase.h"
#include "Portal.h"

using std::vector;
using std::tr1::shared_ptr;
using std::string;

GameWorld::GameWorld():
    floors(),
    flagManager()
    {
        //create new floors
        floors[OVERGROUND_FLOOR] = shared_ptr<GameFloor>(new GameFloor());
        floors[UNDERGROUND_FLOOR] = shared_ptr<GameFloor>(new GameFloor());
    }

vector<shared_ptr<Block> >& GameWorld::getBlocks(const unsigned& floor) {

    return floors[floor]->blocks;
}

vector<shared_ptr<ForegroundObject> >& GameWorld::getForeground(const unsigned& floor) {

    return floors[floor]->foregroundObjects;
}

shared_ptr<FlagManager> GameWorld::getFlagManager() {

    return flagManager;
}

vector<shared_ptr<Portal> >& GameWorld::getPortals(const unsigned& floor) {

    return floors[floor]->portals;
}

bool GameWorld::load(string levelName) {

    clearWorld();

    flagManager.reset(new FlagManager(sf::Vector2f(0, 0), sf::Vector2f(500, 800)));
    floors[OVERGROUND_FLOOR]->portals.push_back(shared_ptr<Portal>(new Portal(sf::Vector2f(-200, 200), sf::Vector2f(600, 800), UNDERGROUND_FLOOR)));

    return loadLevel(levelName, floors[OVERGROUND_FLOOR]->blocks, floors[OVERGROUND_FLOOR]->foregroundObjects);
}

void GameWorld::clearWorld() {

    //clear all floors
    for(auto floor : floors) {

        floor.second->blocks.clear();
        floor.second->foregroundObjects.clear();
        floor.second->portals.clear();
    }

    flagManager.reset();
}

void GameWorld::updateFlagPosition(PlayerBase& player) {

    if(player.isHoldingFlag()) {

        flagManager->setFlagPosition(player.getCurrentPosition(), getOpposingTeam(player.getTeam()));
    }
}

void GameWorld::drawBackground(sf::RenderWindow& window, const unsigned& floor) {

    for(auto& block : floors[floor]->blocks) {

        block->draw(window);
    }

    for(auto& portal : floors[floor]->portals) {

        portal->draw(window);
    }

    flagManager->draw(window);
}

void GameWorld::drawForeground(sf::RenderWindow& window, const unsigned& floor) {

    for(auto& object : floors[floor]->foregroundObjects) {

        object->draw(window);
    }
}
