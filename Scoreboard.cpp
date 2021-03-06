#include "Scoreboard.h"
#include "PlayerBase.h"

using std::vector;
using std::tr1::shared_ptr;
using std::tr1::weak_ptr;

/*

set the position of all text stating the stats of the players
currently only the string that is displayed on the text has been created, they have not been positioned properly

must change all function pointers to member function pointers

*/

Scoreboard::Scoreboard():
    NAME_OFFSET(55),
    KILLS_OFFSET(228),
    DEATHS_OFFSET(260),
    CAPTURES_OFFSET(164),
    RETURNS_OFFSET(196),
    TEXT_SCALE(0.4),
    NAME_SCALE(0.2),
    font(),
    TOGGLE_DISPLAY_KEY(sf::Keyboard::Tab),
    drawDisplay(false),
    scoreTexture(),
    scoreSprite(),
    redTeam(),
    blueTeam()
    {
        scoreTexture.loadFromFile("images/scoreboard.png");
        scoreSprite.setTexture(scoreTexture);
        scoreSprite.setColor(sf::Color(255, 255, 255, 225));

        font.loadFromFile("font.ttf");
    }

void Scoreboard::handleStateEvents() {

    drawDisplay = sf::Keyboard::isKeyPressed(TOGGLE_DISPLAY_KEY);
}

void Scoreboard::setDisplayCenter(const sf::Vector2f& center) {

    scoreSprite.setPosition(center.x - scoreSprite.getGlobalBounds().width / 2, center.y - scoreSprite.getGlobalBounds().height / 2);
}

bool Scoreboard::canDisplayInfo() {

    return drawDisplay;
}

void Scoreboard::clearScoreboard() {

    redTeam.clear();
    blueTeam.clear();
}

void Scoreboard::setupInfoPosition() {

    positionInfo(redTeam, &Scoreboard::calculateRedScorePosition);
    positionInfo(blueTeam, &Scoreboard::calculateBlueScorePosition);
}

void Scoreboard::draw(sf::RenderWindow& window) {

    window.draw(scoreSprite);

    drawTeamInfo(window, redTeam);
    drawTeamInfo(window, blueTeam);
}

void Scoreboard::drawTeamInfo(sf::RenderWindow& window, const vector<shared_ptr<InfoToDisplay> >& teamInfo) {

    for(auto infoDisplayed : teamInfo) {

        drawInfo(window, infoDisplayed);
    }
}

void Scoreboard::drawInfo(sf::RenderWindow& window, shared_ptr<InfoToDisplay> toDisplay) {

    window.draw(toDisplay->name);
    window.draw(toDisplay->kills);
    window.draw(toDisplay->deaths);
    window.draw(toDisplay->captures);
    window.draw(toDisplay->returns);
}

void Scoreboard::positionInfo(vector<shared_ptr<InfoToDisplay> >& teamInfo, float (Scoreboard::*calculatePosition)(const float& offset, const float& stringSize)) {

    //place each players information onto the scoreboard
    for(unsigned index = 0; index < teamInfo.size(); ++index) {

        //each peice of information on the scoreboard has its own offset from the edge of the screen

        //distance from the top of the scoreboard to the first character info slot
        const float scoreboardTopThickness = 127;

        //used to calculate the vertical position of all info
        const float characterSlotHeight = 48;

        shared_ptr<InfoToDisplay>& info = teamInfo[index];

        ///horizontal offset of each information is based on image
        info->name.setPosition((this->*calculatePosition)(NAME_OFFSET, info->name.getGlobalBounds().width),
                               scoreSprite.getPosition().y + scoreboardTopThickness + characterSlotHeight * index);

        info->kills.setPosition((this->*calculatePosition)(KILLS_OFFSET, info->kills.getGlobalBounds().width),
                                scoreSprite.getPosition().y + scoreboardTopThickness + characterSlotHeight * index);

        info->deaths.setPosition((this->*calculatePosition)(DEATHS_OFFSET, info->deaths.getGlobalBounds().width),
                                 scoreSprite.getPosition().y + scoreboardTopThickness + characterSlotHeight * index);

        info->captures.setPosition((this->*calculatePosition)(CAPTURES_OFFSET, info->captures.getGlobalBounds().width),
                                   scoreSprite.getPosition().y + scoreboardTopThickness + characterSlotHeight * index);

        info->returns.setPosition((this->*calculatePosition)(RETURNS_OFFSET, info->returns.getGlobalBounds().width),
                                  scoreSprite.getPosition().y + scoreboardTopThickness + characterSlotHeight * index);
    }
}

float Scoreboard::calculateRedScorePosition(const float& offset, const float& stringSize) {

    //red team ais on the left side so set the position from the left side of the scoreboard
    //size of the string being set doesn't matter
    return scoreSprite.getPosition().x + offset;
}

float Scoreboard::calculateBlueScorePosition(const float& offset, const float& stringSize) {

    //blue team's information is positioned from the right side of the scoreboard
    return scoreSprite.getPosition().x + scoreSprite.getGlobalBounds().width - (offset + stringSize);
}
