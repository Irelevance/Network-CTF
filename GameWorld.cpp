#include "GameWorld.h"
#include "Block.h"
#include "LevelManager.h"
#include "ForegroundObject.h"
#include "FlagManager.h"
#include "TeamManager.h"
#include "PlayerBase.h"
#include "Portal.h"
#include "math.h"
#include "GunGiver.h"

#include <cmath>
#include <iostream>
#include <algorithm>

using std::vector;
using std::tr1::shared_ptr;
using std::string;
using std::cout;
using std::endl;
using std::abs;
using std::min;
using std::max;
using std::unique;

GameWorld::GameWorld():
    SECTIONS_IN_FLOOR(8),
    floors(),
    flagManager(),
    teamASpawn(0, 0, 0, 0),
    teamBSpawn(0, 0, 0, 0)
    {
        flagManager.reset(new FlagManager(sf::Vector2f(0, 0), sf::Vector2f(0, 0)));
        //create new floors
        floors[OVERGROUND_FLOOR] = shared_ptr<Floor>(new Floor());
        floors[UNDERGROUND_FLOOR] = shared_ptr<Floor>(new Floor());
    }

vector<shared_ptr<Block> >& GameWorld::getBlocks(const unsigned& floor) {

    return floors[floor]->blocks;
}

vector<shared_ptr<Block> > GameWorld::getBlocks(const sf::Vector2f& beginRange, const sf::Vector2f& endRange, const unsigned& floor) {

    //floor is only split into regions horizontally so you only need to check where the two points range horizontally
    const float regionSize = floors[OVERGROUND_FLOOR]->backgroundImage.getSize().x / SECTIONS_IN_FLOOR;

    //figure out which of the two points is the left most region
    float leftPoint = min(beginRange.x, endRange.x);
    float rightPoint = max(beginRange.x, endRange.x);

    //figure out which region the left and right points exist on and add all regions starting from the first region to the last region
    int leftRegion = leftPoint / regionSize;
    int rightRegion = rightPoint / regionSize;

    vector<shared_ptr<Block> > blocks;

    //starts from the first region and goes through all regions between the beginning and end points and add all blocks
    for(int currentRegion = leftRegion; currentRegion <= rightRegion; ++currentRegion) {

        //use the x position of the region because the function to get a region uses positions to calculate the region the point exists on
        float xPosition = currentRegion * regionSize;

        //make sure the x position is exactly between two regions so its not ambigious if it lies on the edge of a region
        xPosition += regionSize / 2;

        vector<shared_ptr<Block> >& blocksInRegion = getContainingSection(sf::Vector2f(xPosition, 0), floor);

        blocks.insert(blocks.end(), blocksInRegion.begin(), blocksInRegion.end());
    }

    return blocks;
}

vector<shared_ptr<ForegroundObject> >& GameWorld::getForeground(const unsigned& floor) {

    return floors[floor]->foregroundObjects;
}

vector<shared_ptr<Portal> >& GameWorld::getPortals(const unsigned& floor) {

    return floors[floor]->portals;
}

vector<shared_ptr<GunGiver> >& GameWorld::getGunGivers(const unsigned& floor) {

    return floors[floor]->gunGivers;
}

shared_ptr<FlagManager> GameWorld::getFlagManager() {

    return flagManager;
}

const sf::FloatRect& GameWorld::getSpawnArea(const unsigned& team) const {

    if(team == TEAM_A_ID) {

        return teamASpawn;
    }

    return teamBSpawn;
}

const sf::Vector2f GameWorld::getSpawnPoint(const unsigned& team) const {

    sf::FloatRect spawnAreaToUse = teamASpawn;

    if(team == TEAM_B_ID) {

        spawnAreaToUse = teamBSpawn;
    }

    //randomly generate an x and y position within the spawn area
    sf::Vector2f spawnPoint(getRand(spawnAreaToUse.left + spawnAreaToUse.width, spawnAreaToUse.left),
                             getRand(spawnAreaToUse.top + spawnAreaToUse.height, spawnAreaToUse.top));

    return spawnPoint;
}

bool GameWorld::load(string levelName) {

    clearWorld();

    bool loadedOverground = loadOvergroundLevel(levelName, floors[OVERGROUND_FLOOR], *flagManager->teamAFlag(), *flagManager->teamBFlag(),
                                      teamASpawn, teamBSpawn);

    bool loadedUnderground = loadUndergroundLevel(levelName, floors[UNDERGROUND_FLOOR]);

    createRegions();

    return loadedOverground && loadedUnderground;
}

void GameWorld::clearWorld() {

    //clear all floors
    for(auto floor : floors) {

        floor.second->blocks.clear();
        floor.second->foregroundObjects.clear();
        floor.second->portals.clear();
        floor.second->gunGivers.clear();

        floor.second->backgroundImage.clearImage();
    }

    flagManager->resetFlags();
}

void GameWorld::updateFlagPosition(PlayerBase& player) {

    if(player.isHoldingFlag()) {

        flagManager->setFlagPosition(player.getCurrentPosition(), getOpposingTeam(player.getTeam()));
    }
}

void GameWorld::drawBackground(sf::RenderWindow& window, const unsigned& floor) {

    floors[floor]->backgroundImage.draw(window);

    /*for(auto& block : floors[floor]->blocks) {

        block->draw(window);
    }*/

    /*for(auto& portal : floors[floor]->portals) {

        portal->draw(window, floor);
    }

    for(auto& gunGiver : floors[floor]->gunGivers) {

        gunGiver->draw(window);
    }*/

    flagManager->draw(window, floor);
}

void GameWorld::drawForeground(sf::RenderWindow& window, const unsigned& floor) {

    for(auto& object : floors[floor]->foregroundObjects) {

        object->draw(window);
    }
}

void GameWorld::createRegions() {

    regions.clear();

    //get all the blocks in each floor and add them to whatever section they touch
    for(auto& floor : floors) {

        //now go through all the blocks and place them in the appropriate sections
        for(auto& block : floor.second->blocks) {

            //get the section the top left and top right corners of the block exists in and add the block to the sections
            //make sure it isn't added to the same container twice

            sf::Vector2f topLeft(block->getCollisionBox().left, block->getCollisionBox().top);
            sf::Vector2f topRight(block->getCollisionBox().left + block->getCollisionBox().width, block->getCollisionBox().top);

            vector<shared_ptr<Block> >& topLeftSection = getContainingSection(topLeft, floor.first);
            vector<shared_ptr<Block> >& topRightSection = getContainingSection(topRight, floor.first);

            topLeftSection.push_back(block);

            //if the two vectors given aren't the same then add to the second one as well
            //compare the addresses of the vector because if they are the same containers then they will be at the same address
            if(&topLeftSection != &topRightSection) {

                topRightSection.push_back(block);
            }
        }
    }
}

vector<shared_ptr<Block> >& GameWorld::getContainingSection(const sf::Vector2f& point, const unsigned& floor) {

    //figure out the size of the floor in order to determine how big each section of the floor should be
    sf::Vector2f floorSize = floors[floor]->backgroundImage.getSize();

    //divide the size of the floor into smaller peices and determine which peice the point fits in
    if(point.x < floorSize.x / SECTIONS_IN_FLOOR) {

        return regions[floor].blocksSection1;

    } else if(point.x >= floorSize.x / SECTIONS_IN_FLOOR && point.x < floorSize.x / SECTIONS_IN_FLOOR * 2) {

        return regions[floor].blocksSection2;

    } else if(point.x >= floorSize.x / SECTIONS_IN_FLOOR * 2 && point.x < floorSize.x / SECTIONS_IN_FLOOR * 3) {

        return regions[floor].blocksSection3;

    } else if(point.x >= floorSize.x / SECTIONS_IN_FLOOR * 3 && point.x < floorSize.x / SECTIONS_IN_FLOOR * 4) {

        return regions[floor].blocksSection4;

    } else if(point.x >= floorSize.x / SECTIONS_IN_FLOOR * 4 && point.x < floorSize.x / SECTIONS_IN_FLOOR * 5) {

        return regions[floor].blocksSection5;

    }  else if(point.x >= floorSize.x / SECTIONS_IN_FLOOR * 5 && point.x < floorSize.x / SECTIONS_IN_FLOOR * 6) {

        return regions[floor].blocksSection6;

    } else if(point.x >= floorSize.x / SECTIONS_IN_FLOOR * 6 && point.x < floorSize.x / SECTIONS_IN_FLOOR * 7) {

        return regions[floor].blocksSection7;

    }else {

        return regions[floor].blocksSection8;
    }

    //by default return the last block section
    return regions[floor].blocksSection8;
}
