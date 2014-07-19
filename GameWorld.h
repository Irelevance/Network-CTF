#ifndef GAMEWORLD_H_INCLUDED
#define GAMEWORLD_H_INCLUDED

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"

#include <vector>
#include <tr1/memory>
#include <string>

class Block;

//class to manage the game world by keeping track of all of the objects and events
class GameWorld {

    private:

        std::vector<std::tr1::shared_ptr<Block> > blocks;

        void drawBlocks();

    public:

        GameWorld();

        std::vector<std::tr1::shared_ptr<Block> >& getBlocks();

        //load the given level, indicate success or failure
        bool load(std::string levelName);

        //clear everything currently saved
        void clearWorld();

        void drawBackground(sf::RenderWindow& window);
};

#endif // GAMEWORLD_H_INCLUDED
