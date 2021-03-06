#include "ServerGameManager.h"
#include "PacketIdentification.h"
#include "PacketManipulators.h"
#include "Bullet.h"
#include "LineSegment.h"
#include "FlagManager.h"
#include "Collision.h"
#include "math.h"
#include "Block.h"
#include "Floors.h"
#include "Portal.h"
#include "GunGiver.h"
#include "ButtonPlacer.h"
#include "PredrawnButton.h"
#include "Conversion.h"
#include "ErrorMessage.h"

#include <string>
#include <iostream>

using std::map;
using std::cout;
using std::endl;
using std::tr1::shared_ptr;
using std::vector;
using std::string;

void serverHelpMenu(sf::RenderWindow& window) {

    //save the previous screen to draw it as a background
    sf::Texture previousScreen;
    previousScreen.create(window.getSize().x, window.getSize().y);
    previousScreen.update(window);

    sf::Sprite previousScreenSprite;
    previousScreenSprite.setTexture(previousScreen);

    window.setView(window.getDefaultView());

    //load the image for the help menu
    sf::Texture helpTexture;
    helpTexture.loadFromFile("images/serverHelpScreen.png");

    sf::Sprite helpSprite;
    helpSprite.setTexture(helpTexture);

    vector<shared_ptr<PredrawnButton> > buttons;

    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("images/backButton.png")));

    placeButtons("images/serverHelpScreen.png", buttons);

    unsigned backButton = 0;

    sf::Event event;

    while(window.isOpen()) {

        while(window.pollEvent(event)) {

            if(event.type == sf::Event::Closed) {

                window.close();
            }

            if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

                sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());

                if(buttons[backButton]->checkMouseTouching(mousePosition)) {

                    PredrawnButton::playClickSound();
                    return;
                }
            }

        }

        sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        for(auto& button : buttons) {

            button->checkMouseTouching(mousePosition);
        }


        window.clear();

        window.draw(previousScreenSprite);

        window.draw(helpSprite);

        for(auto& button : buttons) {

            button->draw(window);
        }

        window.display();
    }
}

ServerGameManager::ServerGameManager() :
    GameManager(),
    inGameTexture(),
    inGameSprite(),
    inGameButtons(),
    endMatch(0),
    quitGame(1),
    server(false),
    players(),
    inputConfirmationTime(),
    inputConfirmationDelay(sf::milliseconds(40)),
    lastStateUpdateId(0),
    stateUpdateTimer(),
    stateUpdateDelay(sf::milliseconds(60)),
    lastPlayerId(0),
    teamManager()
    {
        inGameTexture.loadFromFile("images/serverInGame.png");
        inGameSprite.setTexture(inGameTexture);

        inGameButtons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("images/endMatch.png")));
        inGameButtons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("images/quitButton.png")));

        placeButtons("images/serverInGame.png", inGameButtons);
    }

void ServerGameManager::gameLobby(sf::RenderWindow& window) {

    //save the previous screen to draw it as a background
    sf::Texture previousScreen;
    previousScreen.create(window.getSize().x, window.getSize().y);
    previousScreen.update(window);

    sf::Sprite previousScreenSprite;
    previousScreenSprite.setTexture(previousScreen);

    window.setView(window.getDefaultView());

    //load the image for the server lobby
    sf::Texture lobbyTexture;
    lobbyTexture.loadFromFile("images/serverLobby.png");

    sf::Sprite lobbySprite;
    lobbySprite.setTexture(lobbyTexture);

    vector<shared_ptr<PredrawnButton> > buttons;
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("images/helpButton.png")));
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("images/startButton.png")));
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("images/backButton.png")));
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("images/decreaseArrow.png")));
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("images/increaseArrow.png")));

    //indices to access buttons
    unsigned helpButton = 0;
    unsigned startButton = 1;
    unsigned backButton = 2;
    unsigned decreasePoints = 3;
    unsigned increasePoints = 4;

    placeButtons("images/serverLobby.png", buttons);

    sf::Event event;

    //keep track of when to send information about players connected
    sf::Clock dataSendTimer;

    sf::Time dataSendTime = sf::milliseconds(200);

    sf::Font font;
    font.loadFromFile("font.ttf");

    //text that represents how many points you need to win
    //position is based on the image file
    sf::Vector2f defaultPointsPosition(317, 666);

    sf::Text pointsText;
    pointsText.setFont(font);
    pointsText.setString(toString(pointsToWinGame));
    pointsText.setPosition(defaultPointsPosition);
    pointsText.setScale(0.6, 0.6);
    pointsText.setColor(sf::Color::Black);

    //whether the server should start sending game start packets
    //sends at the same rate as the data send timer
    bool sendGameStarts = false;

    //names of each player in their respective teams to draw onto the screen
    //has no use what so ever outside the lobby
    vector<sf::Text> redTeam;
    vector<sf::Text> blueTeam;

    //containers for red and blue boxes which are drawn underneath each team members name in the lobby
    vector<sf::Sprite> redBoxes;
    vector<sf::Sprite> blueBoxes;

    //texture for the red and blue boxes
    sf::Texture teamBoxTexture;
    teamBoxTexture.loadFromFile("images/teamSelect.png");

    //clips for the red and blue team's colored boxes in the teamboxtexture
    sf::IntRect redClip(0, 0, 255, 41);
    sf::IntRect blueClip(0, 47, 255, 41);

    //size of box for each player name in the lobby image
    sf::Vector2f nameBoxSize(255, 48);

    ///the actual player's names should be a bit farther in on to name box
    ///this is the offset from the name slot to the name position
    sf::Vector2f nameOffset(10, 10);

    //position on the lobby image where first name red team's name is displayed
    sf::Vector2f firstRedName(241, 417);
    sf::Vector2f firstBlueName(536, 417);

    //text representation of the server's public and local ip
    sf::Text localIp;
    localIp.setFont(font);
    localIp.setScale(0.55, 0.6);
    localIp.setColor(sf::Color::Black);
    localIp.setString(sf::IpAddress::getLocalAddress().toString());
    localIp.setPosition(472, 279);

    sf::Text publicIp;
    publicIp.setFont(font);
    publicIp.setScale(0.55, 0.6);
    publicIp.setColor(sf::Color::Black);
    publicIp.setString(sf::IpAddress::getPublicAddress().toString());
    publicIp.setPosition(472, 335);

    while(window.isOpen()) {

        while(window.pollEvent(event)) {

            if(event.type == sf::Event::Closed) {

                window.close();
            }

            //only let the user start the game if there is a player on each team
             if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

                sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());

                bool enoughPlayers = (teamManager.getPlayerCount(TEAM_A_ID) > 0) && (teamManager.getPlayerCount(TEAM_B_ID) > 0);

                if(buttons[startButton]->checkMouseTouching(mousePosition) && enoughPlayers && everyoneInLobby()) {

                    PredrawnButton::playClickSound();
                    sendGameStarts = true;

                } else if(buttons[backButton]->checkMouseTouching(mousePosition)) {

                    PredrawnButton::playClickSound();
                    return;

                } else if(buttons[helpButton]->checkMouseTouching(mousePosition)) {

                    PredrawnButton::playClickSound();
                    serverHelpMenu(window);
                }

                //don't let user change amount of points requried to win game
                //if he already started the game
                if(buttons[decreasePoints]->checkMouseTouching(mousePosition) && !sendGameStarts) {

                    PredrawnButton::playClickSound();
                    pointsToWinGame = pointsToWinGame > 1 ? pointsToWinGame - 1 : 1;
                    pointsText.setString(toString(pointsToWinGame));
                }

                if(buttons[increasePoints]->checkMouseTouching(mousePosition) && !sendGameStarts) {

                    PredrawnButton::playClickSound();
                    pointsToWinGame = pointsToWinGame < 99 ? pointsToWinGame + 1 : 99;
                    pointsText.setString(toString(pointsToWinGame));
                }
             }
        }

        //if there is more than one digit in the points text then make it smaller horizontally, and move it to the left that way it can fit into the text box
        if(pointsText.getString().getSize() > 1) {

            pointsText.setScale(0.38, 0.6);
            pointsText.setPosition(defaultPointsPosition.x - 1, defaultPointsPosition.y);

        } else {

            pointsText.setScale(0.6, 0.6);
            pointsText.setPosition(defaultPointsPosition);
        }

        sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        for(auto& button : buttons) {

            button->checkMouseTouching(mousePosition);
        }

        handleIncomingData();

        //clear tame names so the list can be refreshed
        redTeam.clear();
        blueTeam.clear();
        redBoxes.clear();
        blueBoxes.clear();

        //disconnect inactive players
        //add any connected player to their respective team's containers
        for(unsigned i = 0; i < players.size();) {

            if(players[i]->player.timedOut()) {

                disconnectPlayer(i);
                continue;
            }

            //add this player's name to his respective teams container to display
            sf::Text name;
            name.setFont(font);
            name.setScale(0.5, 0.5);
            name.setString(players[i]->player.getName());
            name.setColor(sf::Color::Black);

            sf::Sprite box;
            box.setTexture(teamBoxTexture);

            if(players[i]->player.getTeam() == TEAM_A_ID) {

                redTeam.push_back(name);

                box.setTextureRect(redClip);
                redBoxes.push_back(box);

            } else {

                blueTeam.push_back(name);

                box.setTextureRect(blueClip);
                blueBoxes.push_back(box);
            }

            ++i;
        }

        //set the position of all player names and boxes
        for(unsigned i = 0; i < redTeam.size(); i++) {

            sf::Vector2f position(firstRedName.x, firstRedName.y + i * nameBoxSize.y);

            sf::Text& name = redTeam[i];
            name.setPosition(position + nameOffset);
            redBoxes[i].setPosition(position);
        }

        for(unsigned int i = 0; i < blueTeam.size(); i++) {

            sf::Vector2f position(firstBlueName.x, firstBlueName.y + i * nameBoxSize.y);

            sf::Text& name = blueTeam[i];
            name.setPosition(position + nameOffset);
            blueBoxes[i].setPosition(position);
        }

        if(dataSendTimer.getElapsedTime() > dataSendTime) {

            sf::Packet packet;

            packet << LOBBY_CONNECTION_INFO;

            //give the number of poitns required to win
            packet << pointsToWinGame;

            //send data about all players
            for(auto& player: players) {

                packet << player->player.getName();
                packet << player->player.getTeam();
            }

            for(auto& player : players) {

                server.sendData(packet, player->playerIpAddress, player->playerPort);

                if(sendGameStarts) {

                    sf::Packet gameStartPacket;
                    gameStartPacket << START_GAME;
                    server.sendData(gameStartPacket, player->playerIpAddress, player->playerPort);
                }
            }

            dataSendTimer.restart();
        }

        //if all players are ready to play then start the game
        bool everyoneReady = allPlayersReady();

        if(everyoneReady && sendGameStarts) {

            runGame(window);

            sendGameStarts = false;

            //game finished so make all players not ready again
            for(auto& player : players) {

                player->readyToPlay = false;
            }

            //if here are no players connected it means the game ended because all players quit so let the server know that everyone quit
            if(players.size() == 0) {

                displayError(window, "All players have disconnected.");
            }
        }


        window.clear();

        window.draw(previousScreenSprite);

        window.draw(lobbySprite);

        window.draw(pointsText);

        for(auto& button : buttons) {

            button->draw(window);
        }

        for(auto& clip : redBoxes) {

            window.draw(clip);
        }

        for(auto& clip : blueBoxes) {

            window.draw(clip);
        }

        for(auto& name : redTeam) {

            window.draw(name);
        }

        for(auto& name : blueTeam) {

            window.draw(name);
        }

        window.draw(localIp);
        window.draw(publicIp);

        window.display();
    }
}

void ServerGameManager::handleIncomingData() {

    //try and receive data and if there is any valid data then process it according to the data type
    sf::Packet incomingData;
    sf::IpAddress senderIp;
    unsigned short senderPort;

    while(server.receiveData(incomingData, senderIp, senderPort)) {

        bool handledData = false;

        //data is available, check if the sender already has a player connected
        for(auto& player : players) {

            if(player->playerIpAddress == senderIp && player->playerPort == senderPort) {

                handleData(player, incomingData);
                handledData = true;
                break;
            }
        }

        //if the packet was handles already then don't check if its a connection attempt
        //because sometimes a connection attempt might be received from the client after its already connected
        //so this extra connection attempt is caught in the above loop and theres no need to handle it again
        if(handledData) {

            continue;
        }

        //don't accept new connections if game is already in sessions
        //if all players are ready to play it means the game started
        //because the clients are sending state updates which means the match is in session
        if(allPlayersReady()) {

            continue;
        }

        //data was not handled yet so check the data type and handle accordingly
        //if connection attempt has not been handled yet it means the player has not been added yet, so add him and respond with the player's id
        //if teams are full then ignore request
        if(checkPacketType(incomingData, CONNECTION_ATTEMPT) && createNewConnection(senderIp, senderPort, lastPlayerId + 1)) {

            //strip off the packet id
            getPacketType(incomingData);

            //the name of the player isn't set yet so download the name data
            string name;
            incomingData >> name;

            (*players.rbegin())->player.setName(name);

            incomingData.clear();

            //tell the player who connected that he is connected by sending back the connection request packet
            //except this time add the player's id so the client knows what his own id is
            incomingData << CONNECTION_ATTEMPT << lastPlayerId + 1;
            server.sendData(incomingData, senderIp, senderPort);
            cout << "new connection: " << name << endl;

            //player added so increase the player's ids
            lastPlayerId += 1;
        }

    }
}

void ServerGameManager::handleData(shared_ptr<ConnectedPlayer> player, sf::Packet& data) {

    //if the packet is a connection request then just tell the client he has already connected
    if(checkPacketType(data, CONNECTION_ATTEMPT)) {

        //add the player's id to the end of hte packet so client can figure out its id
        data << player->player.getId();
        server.sendData(data, player->playerIpAddress, player->playerPort);

    } else if(checkPacketType(data, PLAYER_INPUT)) {

        handlePlayerInput(player, data);

    } else if(checkPacketType(data, PLAYER_GUNFIRE)) {

        handlePlayerGunfire(player, data);

    } else if(checkPacketType(data, PLAYER_KEYSTATE_UPDATE)) {

        handlePlayerKeystate(player, data);

        //keystate update means the player is ready to start playing the game
        //however this is only if he isn't at the end screen
        if(!player->atEndScreen) {

           player->readyToPlay = true;
        }

    }  else if(checkPacketType(data, CLIENT_DAMAGE_REPORT)) {

        handleDamageReport(player, data);

    } else if(checkPacketType(data, CHANGE_TEAM)) {

        unsigned short destinationTeam = getOpposingTeam(player->player.getTeam());

        if(!teamManager.isTeamFull(destinationTeam)) {

            player->player.setTeam(getOpposingTeam(player->player.getTeam() ));
            teamManager.switchTeams(getOpposingTeam(destinationTeam));
        }

    } else if(checkPacketType(data, HEART_BEAT)) {

        player->player.resetDataTimer();

        //heartbeats are only sent out in the lobby so player is no longer in end game screen
        player->atEndScreen = false;

    } else if(checkPacketType(data, AT_END_SCREEN)) {

        player->player.resetDataTimer();
        player->atEndScreen = true;
    }
}

void ServerGameManager::handlePlayerInput(shared_ptr<ConnectedPlayer> player, sf::Packet& inputPacket) {

    //first peice of data on the input packet is the packet type because it hasn't been removed yet so remove it
    int packetType = 0;
    inputPacket >> packetType;

    //next data is the number of inputs the player created
    int inputCount = 0;
    inputPacket >> inputCount;

    //handle inputs until there are no more inputs left to handle
    for(int inputsCreated = 0; inputsCreated < inputCount; inputsCreated++) {

        //read the input and make the player handle the given input
        //set initial input to -1 that way if it fails to read the input then player doesn't handle it
        int playerInput = -1;
        inputPacket >> playerInput;

        sf::Uint32 inputId = 0;
        inputPacket >> inputId;

        UserPlayer::Input clientInput;
        clientInput.inputId = inputId;
        clientInput.action = static_cast<UserPlayer::Action>(playerInput);

        //check if this is a newer input, if it isn't then ignore it
        if(inputId >= player->lastConfirmedInputId) {

            player->player.handleClientInput(clientInput);
            player->lastConfirmedInputId = inputId;
        }
    }

    //last data is the player's rotation
    float rotation = 0;
    inputPacket >> rotation;
    player->player.setRotation(rotation);

}

void ServerGameManager::handlePlayerKeystate(shared_ptr<ConnectedPlayer> player, sf::Packet& statePacket) {

    //packet type hasn't been read from packet so remove it
    int packetType = 0;
    statePacket >> packetType;

    //read the keystate now
    UserPlayer::KeyState keystate;

    statePacket >> keystate.pressedLeft;
    statePacket >> keystate.pressedRight;
    statePacket >> keystate.pressedUp;
    statePacket >> keystate.pressedDown;

    float rotation = 0;
    statePacket >> rotation;

    sf::Vector2f position(0, 0);
    statePacket >> position.x >> position.y;

    unsigned floor = 0;
    statePacket >> floor;

    unsigned health = 0;
    statePacket >> health;

    //read the id of this state update
    sf::Uint32 inputId = 0;
    statePacket >> inputId;

    //if the input id is older than the last input processed by the server it means its old data so ignore
    if(inputId < player->lastConfirmedInputId) {

        return;
    }

    player->lastConfirmedInputId = inputId;

    player->player.handleClientKeystate(keystate);
    player->player.setRotation(rotation);

    ///if client has no health it means he died on the server side but hasn't respawned yet so don't let client set position
    ///if client is on a different floor it means the player used a portal on teh server side so client's position must be determiend by the server in the case of portal use
    if(floor == player->player.getFloor() && health != 0) {

        player->player.setInterpolationPosition(position);
    }
}

void ServerGameManager::handleDamageReport(shared_ptr<ConnectedPlayer> player, sf::Packet& packet) {

    //read the id of the packet first
    unsigned packetId = 0;
    packet >> packetId;

    //now take all the playrs that have received damage and damage them
    while(!packet.endOfPacket()) {

        unsigned playerId = 0;
        int damage = 0;

        packet >> playerId;
        packet >> damage;

        //find player with the given id and if he is still alive damage him
        for(auto& playerToHit : players) {

            if(playerToHit->player.getId() == playerId && playerToHit->player.isAlive()) {

                playerToHit->player.getHit(damage);

                //if he died then increase the kill count for the player who shot the bullet
                if(!playerToHit->player.isAlive()) {

                    player->player.setKills(player->player.getKills() + 1);
                }

                //create a bullet from the shooters current position to the position of the player being hit
                //that way the bullet actually looks like it hit the player
                player->player.getGun()->fire(player->player.getDestinationPosition(), playerToHit->player.getDestinationPosition(), playerToHit->player.getFloor());

                //prepare the last gunfire to send to client
                player->player.getGun()->sendLastBulletToClient();
            }
        }
    }
}

void ServerGameManager::handlePlayerGunfire(shared_ptr<ConnectedPlayer> player, sf::Packet& inputPacket) {

    //what fraction of time has passed on the clients side since the server last sent an update
    float deltaFraction = 0;

    //id of last update the client received from the server in order to figure out where to start interpolating the world from in order
    //to check for collision with the gunfire, default value is the latest data
    sf::Uint32 clientUpdateId = lastStateUpdateId;

    readGunfirePacket(player->player, deltaFraction, clientUpdateId, inputPacket);

    ///handleGunfireCollision(player, deltaFraction, clientUpdateId);
}

void ServerGameManager::handleGunfireCollision(shared_ptr<ConnectedPlayer> player, const float& deltaFraction, const sf::Uint32& clientUpdateId) {

    //get all of the bullets from the player and check if any of them collide with any other objects
    vector<shared_ptr<Bullet> > bullets = player->player.getBullets();

    //check if each active bullet collides with anything and determine the first object it collides with
    for(auto& bullet : bullets) {

        if(!bullet->checkCanCollide()) {

            //can't collide so just move on to next bullet
            continue;
        }

        ///first check if the bullet collides with any obstacles because when it does it's range is shortened
        ///and bullets can't pass through blocks and such so once the range is shortened it will automatically prevent it from shooting players behind walls
        bulletEntityCollision<Block>(bullet, getGameWorld().getBlocks(bullet->getLine()->getStartPoint(), bullet->getLine()->getEndPoint(), player->player.getFloor()),
                                     [&](shared_ptr<Block> block) {

                                     return !block->getPassBullets();
                                     });

        //now handle collision with players
        handleBulletCollision(player, bullet, deltaFraction, clientUpdateId);

        bullet->disableCollision();
    }
}

void ServerGameManager::handleBulletCollision(shared_ptr<ConnectedPlayer> shootingPlayer, shared_ptr<Bullet> bullet, const float& deltaFraction, const sf::Uint32& clientUpdateId) {

    //move every player back in time to what the client was seeing when he fired the bullet, and check for collision between the bullet
    //and the player
    //determine the player that was first hit and handle collision with him

    //default nearest collision is the shooter
    shared_ptr<ConnectedPlayer> nearestPlayer = shootingPlayer;
    sf::Vector2f nearestCollisionPoint = bullet->getLine()->getEndPoint();

    for(auto& player : players) {

        if(player == shootingPlayer || player->player.getTeam() == shootingPlayer->player.getTeam() || !player->player.isAlive()) {

            continue;
        }

        //if player and bullet aren't on same floor don't check collision
        if(player->player.getFloor() != bullet->getFloor()) {

            continue;
        }

        //get the position of this player at the time the bullet was fired, default position is the player's destination position
        sf::Vector2f pastPosition(player->player.getDestinationPosition());

        //try to get the players position in the past, if a state tracker doesn't exist it means this player has no older positions so just use his current position
        //make sure there are states saved
        if(player->state && player->state->getStateCount() > 0) {

            //client update id is the current state update on the client side, it is the destination state that the entities on the client are interpolating towards
            //the starting point for the interpolation is the state id - 1
            pastPosition = player->state->approximatePosition(clientUpdateId - 1, deltaFraction);
        }

        //set the player to this position and check for collision between the bullet and the player
        sf::FloatRect collisionBox = player->player.getCurrentHitbox();

        //the collision box has to be centered  at the pastposition because the past position is the center of the player
        ///collisionBox.left = pastPosition.x - collisionBox.width / 2;
        ///collisionBox.top = pastPosition.y - collisionBox.height / 2;

        //point of collision if there is one
        sf::Vector2f collisionPoint(0, 0);

        //if there is a collision determine if the player that was shot is the nearest player
        if(checkCollision(collisionBox, bullet->getLine(), collisionPoint) &&
            distanceToPoint(bullet->getLine()->getStartPoint(), collisionPoint) < distanceToPoint(bullet->getLine()->getStartPoint(), nearestCollisionPoint)) {

            //this collision point is now the nearest point of collision so this player is the closest
            nearestPlayer = player;
            nearestCollisionPoint = collisionPoint;
        }
    }

    //now collide with the nearest player if the nearest player is not the shooter
    if(nearestPlayer != shootingPlayer) {

        //player was hit just make the line smaller and indicate it collided with something
        ///nearestPlayer->player.getHit(bullet->getDamage());
        bullet->setEndPoint(nearestCollisionPoint);
        bullet->disableCollision();

        //if the player being hit died then increase the kill count for the shooter
        //if the nearest player was hit it means he was alive before getting hit because of previous for loop
        if(!nearestPlayer->player.isAlive()) {

            shootingPlayer->player.setKills(shootingPlayer->player.getKills() + 1);
        }
    }
}

bool ServerGameManager::createNewConnection(sf::IpAddress& connectedIp, unsigned short& connectedPort, const int& playerId) {

    shared_ptr<ConnectedPlayer> player(new ConnectedPlayer());
    player->player.setId(playerId);
    player->player.setTeam(teamManager.addNewPlayer());

    if(player->player.getTeam() == TEAMS_FULL) {

        return false;
    }

    player->playerIpAddress = connectedIp;
    player->playerPort = connectedPort;
    player->lastConfirmedInputId = 0;

    players.push_back(player);

    return true;
}

void ServerGameManager::sendInputConfirmation() {

    //for every client connected send them their position update along with the last input that the server confirmed from them
    for(auto& player : players) {

        sf::Packet playerUpdate;
        createUpdatePacket(getFlagManager(), player->player, player->lastConfirmedInputId, playerUpdate, teamManager);

        //send the info to the player
        server.sendData(playerUpdate, player->playerIpAddress, player->playerPort);
    }
}

void ServerGameManager::sendStateUpdates() {

    sf::Packet stateUpdatePacket;

    //create a state update packet and send it to all clients connected
    createStateUpdate(players, lastStateUpdateId, stateUpdatePacket);

    //save the state before its sent
    savePlayerStates(lastStateUpdateId);
    lastStateUpdateId++;

    for(auto& player : players) {

        server.sendData(stateUpdatePacket, player->playerIpAddress, player->playerPort);
    }
}

void ServerGameManager::sendResultPackets() {

    sf::Packet victory;
    victory << VICTORY;

    sf::Packet defeat;
    defeat << DEFEAT;

    for(auto& player : players) {

        if(onWinningTeam(player)) {

            server.sendData(victory, player->playerIpAddress, player->playerPort);

        } else {

            server.sendData(defeat, player->playerIpAddress, player->playerPort);
        }
    }
}

void ServerGameManager::savePlayerStates(const int stateId) {

    //loop through all the players and update their state, if they don't exist then add a state for them
    for(auto& player : players) {

        //a record doesn't exist, so create a new one
        if(!player->state) {

            player->state.reset(new StateTracker(player->player.getId(), calculateMaxStatesSaved() ));
        }

        //update the state
        player->state->insertState(stateId, player->player.getDestinationPosition());
    }
}

int ServerGameManager::calculateMaxStatesSaved() {

    //number of states saved is equal to the number of updates sent to the clients in one second, rounded up (always)
    //calculations done in milliseconds
    return (1000 / stateUpdateDelay.asMilliseconds()) + 1;
}

void ServerGameManager::spawnPlayer(shared_ptr<ServerGameManager::ConnectedPlayer>& player) {

    player->player.respawn(getGameWorld().getSpawnPoint(player->player.getTeam()));

    player->player.setFloor(OVERGROUND_FLOOR);
}

void ServerGameManager::disconnectPlayer(unsigned playerIndex) {

    if(playerIndex > players.size()) {

        return;
    }

    //remove him from the current team so the teammanager can assign
    //his team to another player who connects
    teamManager.removePlayer(players[playerIndex]->player.getTeam());

    cout << "A player has disconnected, Name: " << players[playerIndex]->player.getName() << endl;

    players.erase(players.begin() + playerIndex);
}

void ServerGameManager::drawPlayers(sf::RenderWindow& window) {

    for(auto& player : players) {

        player->player.draw(window);
    }
}

void ServerGameManager::playerSpawnCollision(shared_ptr<ServerGameManager::ConnectedPlayer>& player) {

    const sf::FloatRect& spawnArea = getGameWorld().getSpawnArea(player->player.getTeam());

    if(player->player.getCollisionBox().intersects(spawnArea) && player->player.isAlive()) {

        player->player.regenerateHealth();
    }
}

void ServerGameManager::setup(sf::RenderWindow& window) {

    //respawn all the players taht way they are in their proper spawn zones
    //reset all player's stats as well and reset their guns as well
    for(auto& player : players) {

        player->player.setHealth(0);
        player->player.respawn(getGameWorld().getSpawnPoint(player->player.getTeam() ));
        player->player.resetStats();
        player->player.resetGun();
        player->player.setFloor(OVERGROUND_FLOOR);
    }

    //reset all scores
    teamManager.resetScores();

    //reset flag positions
    getFlagManager()->resetFlags();
}

void ServerGameManager::handleWindowEvents(sf::Event& event, sf::RenderWindow& window) {

    ///intentionally left blank
}

void ServerGameManager::handleComponentInputs(sf::Event& event, sf::RenderWindow& window) {

    if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

        sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());

        if(inGameButtons[quitGame]->checkMouseTouching(mousePosition)) {

            PredrawnButton::playClickSound();
            window.close();
        }

        if(inGameButtons[endMatch]->checkMouseTouching(mousePosition)) {

            PredrawnButton::playClickSound();
            exitGameLoop = true;
        }
    }
}

 void ServerGameManager::handleStateEvents(sf::RenderWindow& window) {

    sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());

    for(auto& button : inGameButtons) {

        button->checkMouseTouching(mousePosition);
    }
}

void ServerGameManager::updateComponents(sf::RenderWindow& window) {

    //receive new data
    handleIncomingData();

    //if there are no players or there is only one player then return to lobby

    //if all players are at the end game screen then server needs to return to the lobby
    if(allEndGameScreen()) {

        exitGameLoop = true;
        return;
    }

    //tell clients what the last input the server confirmed was
    if(inputConfirmationTime.getElapsedTime() > inputConfirmationDelay) {

        sendInputConfirmation();
        inputConfirmationTime.restart();
    }

    //if match is over then only send a victor or defeat packet
    //use the state update timer to send result packet
    if(isGameFinished()) {

        if(stateUpdateTimer.getElapsedTime() > stateUpdateDelay) {

            //also send an input confirmation update that way
            //clients can see everyones final scores
            sendInputConfirmation();
            sendResultPackets();
        }

        return;
    }

    //send information about other players to connected clients
    //however if there are less than 2 players then theres no need to update them
    //because there are no other player's whose data must be sent
    if(stateUpdateTimer.getElapsedTime() > stateUpdateDelay && players.size() > 1) {

        sendStateUpdates();
        stateUpdateTimer.restart();
    }
}

void ServerGameManager::updateTimeComponents(const float& delta, sf::RenderWindow& window) {

    //if the game ended then don't update any components
    if(isGameFinished()) {

        return;
    }

    //update the players of all the connected clients, if any have timed out, delete them
    for(unsigned index = 0; index < players.size();) {

        if(players[index]->player.timedOut()) {

            disconnectPlayer(index);
            continue;
        }

        //if the player if alive update him, otherwise try to respawn him
        if(players[index]->player.isAlive()) {

            //players[index]->player.update(delta, sf::Vector2f(window.getSize().x, window.getSize().y));
            players[index]->player.updateGun(delta);

        } else if(players[index]->player.shouldRespawn()) {

            spawnPlayer(players[index]);
        }


        index++;
    }
}

void ServerGameManager::handlePostUpdate(sf::RenderWindow& window) {

    for(auto& player : players) {

        player->player.interpolate(calculateDeltaFraction());
        getGameWorld().updateFlagPosition(player->player);
    }
}

void ServerGameManager::drawComponents(sf::RenderWindow& window) {

    ///drawPlayers(window);
}

void ServerGameManager::drawUI(sf::RenderWindow& window) {

    window.draw(inGameSprite);

    for(auto& button : inGameButtons) {

        button->draw(window);
    }
}

const unsigned ServerGameManager::getFloor() const {

    return OVERGROUND_FLOOR;
}

void ServerGameManager::handleCollisions() {

    //handle collision between player and all other objects that he can collide with
    //do in this one loop so you don't repeat loops
    for(auto& player : players) {

        //top left and right points of the player's collision box which will be used to determine which blocks he can collide with
        sf::Vector2f topLeft(player->player.getCollisionBox().left, player->player.getCollisionBox().top);
        sf::Vector2f topRight(player->player.getCollisionBox().left + player->player.getCollisionBox().width, topLeft.y);

        playerStaticCollision(player->player, getGameWorld().getBlocks(topLeft, topRight, player->player.getFloor()));
        playerStaticCollision(player->player, getPortals(player->player.getFloor()));
        playerStaticCollision(player->player, getGunGivers(player->player.getFloor()));

        collidePlayerFlag(player->player, *getFlagManager(), teamManager);

        playerSpawnCollision(player);

        //reset all player's gun picking up state that way they aren't constantly trying to pick up a gun
        player->player.setPickUpGun(false);
    }
}
