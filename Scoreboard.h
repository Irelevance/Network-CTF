#ifndef PLAYERINFODISPLAY_H_INCLUDED
#define PLAYERINFODISPLAY_H_INCLUDED

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "TeamManager.h"
#include "Conversion.h"

#include <vector>
#include <tr1/memory>

class PlayerBase;

//determines if player is trying to look at the information about all players connected
//displays each player's statistics, i.e their kills, deaths, flag captures, flag returns
//all statistics, once created, cannot be changed so the only way to add information for new players
//or update information for existing players is to reset the entire scoreboard and recreate all the information
//********SCORES MUST BE CLEARED BY THE USER, SCOREBOARD WILL NOT DO IT FOR YOU
class Scoreboard {

    private:

        //list of information that is displayed for each player
        //the stats are positioned according to the image used for the information display
        struct InfoToDisplay {

            sf::Text name;
            sf::Text kills;
            sf::Text deaths;
            sf::Text captures;
            sf::Text returns;
        };

        //distances from the left edge of the scoreboard to the position where the given information is displayed on the scoreboard
        //these offsets are based on the image file
        const float NAME_OFFSET;
        const float KILLS_OFFSET;
        const float DEATHS_OFFSET;
        const float CAPTURES_OFFSET;
        const float RETURNS_OFFSET;

        //all text in the scoreboard is too big so it must be scaled by this factor
        const float TEXT_SCALE;

        //the name should be scaled by a different factor since its longer than the other texts
        const float NAME_SCALE;

        sf::Font font;

        //the key player needs to press in order to bring up the display
        const sf::Keyboard::Key TOGGLE_DISPLAY_KEY;

        bool drawDisplay;

        sf::Texture scoreTexture;
        sf::Sprite scoreSprite;

        //info about the players that exist on both teams
        //a vector is used to seperate players into the two teams that way when drawing their stats you can use the index in the vector to determine the position
        //that the information should be drawn on
        std::vector<std::tr1::shared_ptr<InfoToDisplay> > redTeam;
        std::vector<std::tr1::shared_ptr<InfoToDisplay> > blueTeam;

        //the info created for the given class is not positioned so it has to be positioned
        template<class Player>
        std::tr1::shared_ptr<InfoToDisplay> createInfo(Player& player);

        //draw the given container of infos
        void drawTeamInfo(sf::RenderWindow& window, const std::vector<std::tr1::shared_ptr<InfoToDisplay> >& teamInfo);

        //draw the given info object
        void drawInfo(sf::RenderWindow& window, std::tr1::shared_ptr<InfoToDisplay> toDisplay);

        //set the position of the player's information that way they draw on the scoreboard properly
        //the given positioning function is used because each team's position is calculated differently
        //the red team's score is set from the left side of the scoreboard while the blue team's scores are set from the right side
        //the position calculation function is used to properly set the position from the left side or the right side
        //it should only calculate the horizontal or vertical position
        void positionInfo(std::vector<std::tr1::shared_ptr<InfoToDisplay> >& teamInfo, float (Scoreboard::*calculatePosition)(const float& offset, const float& stringSize));

        //offset is the distance from the edge of the scoreboard to the position where the info should display
        //uses the offset to calculate the position on the scoreboard
        //each team calculates the position differently
        //the size of the text whose position is being set is also used
        float calculateRedScorePosition(const float& offset, const float& stringSize);
        float calculateBlueScorePosition(const float& offset, const float& stringSize);

    public:

        Scoreboard();

        void handleStateEvents();

        //center the scoreboard around the given position
        //give the size of the screen if you want to draw it on screen
        void setDisplayCenter(const sf::Vector2f& center);

        bool canDisplayInfo();

        void clearScoreboard();

        //update the list of players on each team, erases previous list of players
        //can't convert a vector<shared_ptr<derived> > to vector<shared_ptr<base> > implicitly
        //so use a template to allow a vector for any type of players
        template<class Player>
        void updatePlayerList(std::vector<std::tr1::shared_ptr<Player> >& players);

        //add the given player to the bottom of the list of his respective team, this player's information is displayed at the bottom of the list
        //this function doesn't take in a shared ptr, and instead takes the object itself because client game manager uses a player object instead of a shared pointer
        template<class Player>
        void addPlayerInfo(Player& player);

        //setup the positions of each of the info for the player's information that are drawn on the scoreboard
        void setupInfoPosition();

        void draw(sf::RenderWindow& window);
};

template<class Player>
void Scoreboard::updatePlayerList(std::vector<std::tr1::shared_ptr<Player> >& players) {

    //add each player to his respective team so that his info will be drawn later
    for(auto& player : players) {

        addPlayerInfo(*player);
    }

    setupInfoPosition();
}

template<class Player>
void Scoreboard::addPlayerInfo(Player& player) {

    //figure out his team and add him to the correct team container
    std::tr1::shared_ptr<InfoToDisplay> newInfo = createInfo(player);

    if(player.getTeam() == TEAM_A_ID) {

        redTeam.push_back(newInfo);

    } else {

        blueTeam.push_back(newInfo);
    }
}

template<class Player>
std::tr1::shared_ptr<Scoreboard::InfoToDisplay> Scoreboard::createInfo(Player& player) {

    std::tr1::shared_ptr<InfoToDisplay> newInfo(new InfoToDisplay);

    sf::Color translucent(255, 255, 255, 175);

    newInfo->name.setFont(font);
    newInfo->name.setScale(NAME_SCALE, TEXT_SCALE);
    newInfo->name.setColor(translucent);
    newInfo->name.setString(player.getName());

    newInfo->kills.setFont(font);
    newInfo->kills.setScale(TEXT_SCALE, TEXT_SCALE);
    newInfo->kills.setColor(translucent);
    newInfo->kills.setString(toString(player.getKills() ));

    newInfo->deaths.setFont(font);
    newInfo->deaths.setScale(TEXT_SCALE, TEXT_SCALE);
    newInfo->deaths.setColor(translucent);
    newInfo->deaths.setString(toString(player.getDeaths() ));

    newInfo->captures.setFont(font);
    newInfo->captures.setScale(TEXT_SCALE, TEXT_SCALE);
    newInfo->captures.setColor(translucent);
    newInfo->captures.setString(toString(player.getFlagCaptures() ));

    newInfo->returns.setFont(font);
    newInfo->returns.setScale(TEXT_SCALE, TEXT_SCALE);
    newInfo->returns.setColor(translucent);
    newInfo->returns.setString(toString(player.getFlagReturns() ));

    return newInfo;
}

#endif // PLAYERINFODISPLAY_H_INCLUDED
