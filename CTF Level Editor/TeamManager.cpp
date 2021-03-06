#include "TeamManager.h"

const unsigned short TEAM_A_ID = 1;
const unsigned short TEAM_B_ID = 2;

const sf::Color TEAM_A_COLOR = sf::Color::Red;
const sf::Color TEAM_B_COLOR = sf::Color::Blue;

unsigned short getOpposingTeam(const unsigned short& currentTeam) {

    return (currentTeam == TEAM_A_ID) ? TEAM_B_ID : TEAM_A_ID;
}

const sf::Color getTeamColor(const unsigned short& team) {

    return (team == TEAM_A_ID) ? TEAM_A_COLOR : TEAM_B_COLOR;
}
