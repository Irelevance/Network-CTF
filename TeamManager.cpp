#include "TeamManager.h"

#include <iostream>

using std::cout;
using std::endl;

const unsigned short TEAM_A_ID = 1;
const unsigned short TEAM_B_ID = 2;
const unsigned short TEAMS_FULL = 3;

const sf::Color TEAM_A_COLOR = sf::Color::Red;
const sf::Color TEAM_B_COLOR = sf::Color::Blue;

const int MAX_PER_TEAM = 5;

unsigned short getOpposingTeam(const unsigned short& currentTeam) {

    return (currentTeam == TEAM_A_ID) ? TEAM_B_ID : TEAM_A_ID;
}

const sf::Color getTeamColor(const unsigned short& team) {

    return (team == TEAM_A_ID) ? TEAM_A_COLOR : TEAM_B_COLOR;
}

TeamManager::TeamManager():
    numberOfPlayers(),
    teamScores()
    {
        setupPlayerCounter();
        setupScores();
    }

unsigned short TeamManager::addNewPlayer() {

    //id of team that the player was added to
    int team = TEAM_B_ID;

    //add the player to whatever team has the fewer number of players
    if(numberOfPlayers[TEAM_A_ID] <= numberOfPlayers[TEAM_B_ID]) {

        numberOfPlayers[TEAM_A_ID]++;
        team = TEAM_A_ID;

    } else {

        numberOfPlayers[TEAM_B_ID]++;
        team = TEAM_B_ID;
    }

    if(numberOfPlayers[team] > MAX_PER_TEAM) {

        numberOfPlayers[team] = MAX_PER_TEAM;
        team = TEAMS_FULL;
    }

    return team;
}

void TeamManager::removePlayer(const unsigned short& team) {

    if(numberOfPlayers[team] > 0) {

        numberOfPlayers[team]--;
    }
}

short unsigned int TeamManager::switchTeams(unsigned short currentTeam) {

    int destinationTeam = (currentTeam == TEAM_A_ID) ? TEAM_B_ID : TEAM_A_ID;

    removePlayer(currentTeam);
    numberOfPlayers[destinationTeam]++;

    return destinationTeam;
}

void TeamManager::increaseTeamScore(const unsigned short& team) {

    ++teamScores[team];
}

void TeamManager::resetScores() {

    //reseting scores is setting them to 0 which the setup function already does
    setupScores();
}

unsigned short TeamManager::getTeamScore(const unsigned short& team) {

    return teamScores[team];
}

void TeamManager::setTeamScore(const unsigned short& team, const unsigned short& score) {

    teamScores[team] = score;
}

void TeamManager::setupPlayerCounter() {

    numberOfPlayers[TEAM_A_ID] = 0;
    numberOfPlayers[TEAM_B_ID] = 0;
}

void TeamManager::setupScores() {

    teamScores[TEAM_A_ID] = 0;
    teamScores[TEAM_B_ID] = 0;
}
