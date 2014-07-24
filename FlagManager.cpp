#include "FlagManager.h"
#include "TeamManager.h"
#include "math.h"

#include <stdexcept>
#include <iostream>

using std::map;
using std::out_of_range;
using std::cout;
using std::endl;
using std::tr1::shared_ptr;

FlagManager::FlagManager(const sf::Vector2f& teamASpawn, const sf::Vector2f& teamBSpawn):
    flags()
    {
        addFlag(teamASpawn, TEAM_A_ID);
        addFlag(teamBSpawn, TEAM_B_ID);
    }

void FlagManager::setFlagPosition(const sf::Vector2f& position, unsigned short teamId) {

    //if the flag with the given team id does not exist then indicate to the player an error occured and close the game
    try {

        flags.at(teamId)->flag.setPosition(position);

    } catch(out_of_range& error) {

        cout << "A team's id is invalid and does not match one of the flags" << endl;
    }
}

bool FlagManager::canHoldFlag(unsigned short teamId) {

    //flag can be taken if its not being held already
    return !flags[teamId]->beingHeld;
}

bool FlagManager::flagAtSpawn(unsigned short teamId) {

    return flags[teamId]->atSpawn;
}

void FlagManager::holdFlag(unsigned short teamId) {

    flags[teamId]->beingHeld = true;
}

void FlagManager::dropFlag(unsigned short teamId) {

    flags[teamId]->beingHeld = false;
}

void FlagManager::resetFlags() {

    flagToSpawn(TEAM_A_ID);
    flagToSpawn(TEAM_B_ID);
}

bool FlagManager::checkFlagCollision(const sf::FloatRect& collisionRect, const unsigned short& teamId) {

    return collisionRect.intersects(flags[teamId]->flag.getGlobalBounds());
}

void FlagManager::flagToSpawn(const unsigned short& teamId) {

    flags[teamId]->flag.setPosition(flags[teamId]->spawnPosition);
    flags[teamId]->atSpawn = true;
    flags[teamId]->beingHeld = false;
}

void FlagManager::draw(sf::RenderWindow& window) {

    for(auto& flag : flags) {

        window.draw(flag.second->flag);
    }
}

void FlagManager::addFlag(const sf::Vector2f& flagSpawnLocation, unsigned short teamId) {

    //create a new flag at the given position
    shared_ptr<Flag> flag(new Flag());
    flag->flag.setPosition(flagSpawnLocation);
    flag->flag.setRadius(30);

    //center the circle of the flag
    flag->flag.setOrigin(calculateCenter(flag->flag.getLocalBounds() ));

    //colour flag according to teamId
    flag->flag.setFillColor(sf::Color(155 * teamId, 100 * teamId, 0));

    flag->spawnPosition = flagSpawnLocation;

    flags[teamId] = flag;
}
