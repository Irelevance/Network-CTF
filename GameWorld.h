#ifndef GAMEWORLD_H_INCLUDED
#define GAMEWORLD_H_INCLUDED

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "Floors.h"

#include <map>
#include <vector>
#include <tr1/memory>
#include <string>

class Block;
class ForegroundObject;
class FlagManager;
class PlayerBase;
class Portal;

//class to manage the game world by keeping track of all of the objects and events
class GameWorld {

    private:

        //objects that exist in a floor
        struct GameFloor {

            std::vector<std::tr1::shared_ptr<Block> > blocks;
            std::vector<std::tr1::shared_ptr<ForegroundObject> > foregroundObjects;
            std::vector<std::tr1::shared_ptr<Portal> > portals;
        };

        std::map<const unsigned, std::tr1::shared_ptr<GameFloor> > floors;

        std::tr1::shared_ptr<FlagManager> flagManager;

    public:

        GameWorld();

        std::vector<std::tr1::shared_ptr<Block> >& getBlocks(const unsigned& floor = OVERGROUND_FLOOR);
        std::vector<std::tr1::shared_ptr<ForegroundObject> >& getForeground(const unsigned& floor = OVERGROUND_FLOOR);
        std::vector<std::tr1::shared_ptr<Portal> >& getPortals(const unsigned& floor = OVERGROUND_FLOOR);

        std::tr1::shared_ptr<FlagManager> getFlagManager();

        //load the given level, indicate success or failure
        bool load(std::string levelName);

        //clear everything currently saved
        void clearWorld();

        //update the position of the given that the given player may have
        void updateFlagPosition(PlayerBase& player);

        //draw the objects of the given floor
        void drawBackground(sf::RenderWindow& window, const unsigned& floor);
        void drawForeground(sf::RenderWindow& window, const unsigned& floor);
};

#endif // GAMEWORLD_H_INCLUDED
