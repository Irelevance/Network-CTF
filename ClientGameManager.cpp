#include "ClientGameManager.h"
#include "PacketManipulators.h"
#include "PacketIdentification.h"
#include "Bullet.h"
#include "LineSegment.h"
#include "Collision.h"
#include "InterpolatingPlayer.h"
#include "ForegroundObject.h"
#include "TeamManager.h"
#include "Block.h"
#include "Gun.h"
#include "ErrorMessage.h"
#include "math.h"

#include <cstdlib>
#include <iostream>
#include <map>
#include <iterator>
#include "PredrawnButton.h"
#include "ButtonPlacer.h"
#include "OnOffButton.h"

/**





MAKE CLIENT GAME MANAGER HAVE THE FUNCTION TO CALCULATE DELTA FRACTION FOR SERVER UPDATES AND THEN SEND THE DATA
TO THE SERVER
**/

using std::tr1::shared_ptr;
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::map;
using std::next;

void controlsScreen(sf::RenderWindow& window) {

    //save the previous window
    sf::Texture background;
    background.loadFromFile("images/defaultBackscreen.png");

    sf::Sprite previousScreenSprite;
    previousScreenSprite.setTexture(background);

    //load the image for the credits
    sf::Texture controlsTexture;
    controlsTexture.loadFromFile("images/controlsMenu.png");

    sf::Sprite controlsSprite;
    controlsSprite.setTexture(controlsTexture);

    //load the buttons
    vector<shared_ptr<PredrawnButton> > buttons;
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("images/backButton.png")));

    unsigned backButton = 0;

    placeButtons("images/controlsMenu.png", buttons);

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

        sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());

        for(auto& button : buttons) {

            button->checkMouseTouching(mousePosition);
        }

        window.clear();
        window.draw(previousScreenSprite);
        window.draw(controlsSprite);

        for(auto& button : buttons) {

            button->draw(window);
        }

        window.display();
    }
}

ClientGameManager::ClientGameManager() :
    GameManager(),
    messageManager(),
    damageDealt(),
    endMatchButtons(),
    pausedMenuButtons(),
    controlsButtons(),
    gameBgm(),
    winSoundBuffer(),
    loseSoundBuffer(),
    winSound(),
    loseSound(),
    resultToLobbyId(0),
    resumeId(0),
    controlsId(1),
    quitMatch(2),
    quitGame(3),
    musicId(4),
    soundId(5),
    backId(0),
    pausedTexture(),
    pausedSprite(),
    paused(false),
    controlsTexture(),
    controlsSprite(),
    showControls(false),
    currentState(STATE_PLAYING),
    client("70.71.114.74", 8080),
    userPlayer(),
    camera(),
    connectedPlayers(),
    stateUpdateTimer(),
    serverUpdateTime(sf::milliseconds(60)),
    lastStateUpdateId(0),
    waitingForOthers(true)
    {
        gameBgm.openFromFile("sounds/kick_shock.wav");
        gameBgm.setVolume(20);
        gameBgm.setLoop(true);

        endMatchButtons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("images/continueButton.png")) );

        //it doesn't matter if you place the buttons onto the victory or defeat screen since the buttons have to be in the same position for both
        placeButtons("images/victory.png", endMatchButtons);

        pausedMenuButtons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("images/resumeButton.png")) );
        pausedMenuButtons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("images/controlsButton.png")) );
        pausedMenuButtons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("images/quitMatchButton.png")) );
        pausedMenuButtons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("images/quitButton.png")) );
        pausedMenuButtons.push_back(shared_ptr<PredrawnButton>(new OnOffButton(GLO_PLAY_MUSIC) ));
        pausedMenuButtons.push_back(shared_ptr<PredrawnButton>(new OnOffButton(GLO_PLAY_SOUNDS) ));

        placeButtons("images/escapeMenu.png", pausedMenuButtons);

        pausedTexture.loadFromFile("images/escapeMenu.png");
        pausedSprite.setTexture(pausedTexture);

        controlsTexture.loadFromFile("images/controlsMenu.png");
        controlsSprite.setTexture(controlsTexture);

        controlsButtons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("images/backButton.png")) );

        placeButtons("images/controlsMenu.png", controlsButtons);

        winSoundBuffer.loadFromFile("sounds/victory.wav");
        winSound.setBuffer(winSoundBuffer);

        loseSoundBuffer.loadFromFile("sounds/defeat.wav");
        loseSound.setBuffer(loseSoundBuffer);
    }

void ClientGameManager::setPlayerName(string name) {

    userPlayer.setName(name);
}

bool ClientGameManager::connectToServer(string serverIp, unsigned short serverPort) {

    int playerId = 0;

    client.setServerIp(serverIp);
    client.setServerPort(serverPort);

    if(client.connectToServer(playerId, userPlayer.getName(), sf::seconds(2))) {

        userPlayer.setId(playerId);

        return true;
    }

    return false;
}

void ClientGameManager::gameLobby(sf::RenderWindow& window, sf::Font& font, sf::Music& lobbyBgm) {

    //save the previous screen, that way this screen can just draw overtop of it
    sf::Texture previousScreen;
    previousScreen.create(window.getSize().x, window.getSize().y);
    previousScreen.update(window);

    sf::Sprite previousScreenSprite;
    previousScreenSprite.setTexture(previousScreen);

    //load the image of the game lobby
    sf::Texture lobbyTexture;
    lobbyTexture.loadFromFile("images/lobby.png");

    sf::Sprite lobbySprite;
    lobbySprite.setTexture(lobbyTexture);

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

    //position on the lobby image where first name red team's name is displayed
    sf::Vector2f firstRedName(241, 297);
    sf::Vector2f firstBlueName(536, 297);

    ///the actual player's names should be a bit farther in on to name box
    ///this is the offset from the name slot to the name position
    sf::Vector2f nameOffset(10, 10);

    vector<shared_ptr<PredrawnButton> > buttons;

    buttons.push_back(shared_ptr<PredrawnButton> (new PredrawnButton("images/switchTeams.png")));
    buttons.push_back(shared_ptr<PredrawnButton> (new PredrawnButton("images/backButton.png")));

    unsigned switchTeams = 0;
    unsigned backButton = 1;

    placeButtons("images/lobby.png", buttons);

    sf::Event event;

    //set window to default properties that way all objects are drawn on screen
    window.setView(window.getDefaultView());

    sf::Clock dataReceiveTimer;
    sf::Time timeOutTime = sf::seconds(6);

    //timer to send heartbeat packets to server
    sf::Clock heartbeatTimer;
    sf::Time heartBeatTime = sf::seconds(1);

    while(window.isOpen()) {

        while(window.pollEvent(event)) {

            if(event.type == sf::Event::Closed) {

                window.close();
            }

            if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

                sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                if(buttons[backButton]->checkMouseTouching(mousePosition)) {

                    PredrawnButton::playClickSound();
                    return;
                }

                if(buttons[switchTeams]->checkMouseTouching(mousePosition)) {

                    PredrawnButton::playClickSound();
                    sf::Packet packet;

                    //tell server user wants to switch teams
                    packet << CHANGE_TEAM;

                    client.sendToServer(packet);
                }
            }
        }

        sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        for(auto& button : buttons) {

            button->checkMouseTouching(mousePosition);
        }

        if(heartbeatTimer.getElapsedTime() > heartBeatTime) {

            sf::Packet packet;
            packet << HEART_BEAT;

            client.sendToServer(packet);

            heartbeatTimer.restart();
        }

        //recreate the list of players in each time whenever the server sends an update
        sf::Packet packet;

        if(dataReceiveTimer.getElapsedTime() > timeOutTime) {

            ///do something to draw that you've disconnected
            displayError(window, "You have been disconnected\nfrom the server.");
            return;
        }

        //get new data from the server
        while(client.receiveFromServer(packet)) {

            dataReceiveTimer.restart();

            int packetType = getPacketType(packet);

            if(packetType == LOBBY_CONNECTION_INFO) {

                redTeam.clear();
                blueTeam.clear();
                redBoxes.clear();
                blueBoxes.clear();

                //first read the number of points required to win
                packet >> pointsToWinGame;

                while(!packet.endOfPacket()) {

                    //read data about all players, first thing read is the player name
                    string playerName;

                    unsigned short team = 0;

                    packet >> playerName;
                    packet >> team;

                    sf::Text nameText;
                    nameText.setFont(font);
                    nameText.setString(playerName);
                    nameText.setScale(0.50, 0.50);
                    nameText.setColor(sf::Color::Black);

                    if(team == TEAM_A_ID) {

                        redTeam.push_back(nameText);

                        sf::Sprite redBox;
                        redBox.setTexture(teamBoxTexture);
                        redBox.setTextureRect(redClip);

                        redBoxes.push_back(redBox);

                    } else {

                        blueTeam.push_back(nameText);

                        sf::Sprite blueBox;
                        blueBox.setTexture(teamBoxTexture);
                        blueBox.setTextureRect(blueClip);

                        blueBoxes.push_back(blueBox);
                    }
                }

            } else if(packetType == START_GAME) {

                //disable current bgm so that the ingame bgm starts playing
                lobbyBgm.stop();

                //delete all previously connected players that way they can move to their spawns
                connectedPlayers.clear();

                //run the game, will go to the game stage but won't start until server sends state update packet
                runGame(window);

                //disable ingame bgm
                gameBgm.stop();

                if(GLO_PLAY_MUSIC) {

                    //replay current bgm
                    lobbyBgm.play();
                }


                //reset the view that way you cna see the menu
                window.setView(window.getDefaultView());

                //reset the data receive timer because the time will be very high right now
                //and clients will get disconnected
                dataReceiveTimer.restart();

                //if the user player timed out it means the game finished because hwas disconnected so exit lobby
                if(userPlayer.timedOut()) {

                    return;
                }

            } else if(packetType == PLAYER_STATE_UPDATE || packetType == WORLD_STATE_UPDATE) {

                //if the server sends a packet that should be sent in the middle of a game
                //it means the client quit the game but is still connected to the server, so exit the lobby as well
                return;
            }
        }



        //set the position of all names so that they line up with the boxes in the image file
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

        window.clear();

        window.draw(previousScreenSprite);

        window.draw(lobbySprite);

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

        for(auto& button : buttons) {

            button->draw(window);
        }

        window.display();
    }
}

void ClientGameManager::animateEntities() {

    userPlayer.animate();

    for(auto& player : connectedPlayers) {

        player->animate();
    }
}

void ClientGameManager::interpolateEntities() {

    ///delta fraction for user player and other players are different
    ///because userplayer is interpolated based on physics update rate
    ///and other players are updated at a different time interval (time interval is dependant on server, for now its 60 milliseconds)
    userPlayer.interpolate(calculateDeltaFraction());

    getGameWorld().updateFlagPosition(userPlayer);

    //make sure the denominator isn't 0
    //calculate the delta fraction to interpolate other connected players
    //this uses the time between server updates for interpolation rather than time between physics update cycle
    float deltaFraction = calculateServerDelta();

    for(auto& player : connectedPlayers) {

        player->interpolate(deltaFraction);

        getGameWorld().updateFlagPosition(*player);
    }
}

const float ClientGameManager::calculateServerDelta() {

    //make sure the denominator isn't 0
    //calculate the delta fraction to interpolate other connected players
    //this uses the time between server updates for interpolation rather than time between physics update cycle
    float deltaFraction = stateUpdateTimer.getElapsedTime().asSeconds() / serverUpdateTime.asSeconds();

    return deltaFraction;
}


void ClientGameManager::sendInputsToServer() {

    sf::Packet packagedInputs;

    //if game is over jst tell the server teh player is currently in the end game screen
    if(matchEnded) {

        packagedInputs << AT_END_SCREEN;
        client.sendToServer(packagedInputs);
        return;
    }

    //if there was any new inputs added, then send to server
    if(createInputPacket(userPlayer, packagedInputs)) {

        client.sendToServer(packagedInputs);
    }

    userPlayer.clearInputsToSend();

    //now send the players state if needed
    if(userPlayer.shouldSendKeystate()) {

        sf::Packet statePacket;
        createStatePacket(userPlayer, statePacket);
        userPlayer.resetKeystateTimer();

        client.sendToServer(statePacket);
    }
}

void ClientGameManager::sendGunshotsToServer() {

    //don't send updates if game is over
    if(matchEnded) {

        return;
    }

    //check if the user shot any bullets and send any gunshot data to the server
    if(userPlayer.getGun()->getBulletsForClients().size() == 0) {

        return;
    }

    sf::Packet packetToSend;

    const float deltaFraction = calculateServerDelta();

    createGunfirePacket(userPlayer, deltaFraction, lastStateUpdateId, packetToSend);

    //clear the queued gunfires
    userPlayer.clearGunshotQueue();

    client.sendToServer(packetToSend);
}

void ClientGameManager::sendDamageToServer() {

    //don't send updates if game is over
    if(matchEnded) {

        return;
    }

    //don't send packet if usser hasn't damaged anyone
    if(damageDealt.size() == 0) {

        return;
    }

    sf::Packet packet;

    packet << CLIENT_DAMAGE_REPORT;

    //add all data to packet and send to server
    for(auto& damage : damageDealt) {

        packet << damage.playerId;
        packet << damage.damage;
    }

    client.sendToServer(packet);

    //erase all damages dealth
    damageDealt.clear();
}

void ClientGameManager::handleServerUpdates() {

    //try and receive data and if there is any data read it
    sf::Packet downloadedData;

    //not server data so no point handling the data
    while(client.receiveFromServer(downloadedData)) {

        //check the type of data received
        const int packetType = getPacketType(downloadedData);

        //respond to the packet depending on the type of packet
        if(packetType == PLAYER_STATE_UPDATE) {

            applyPlayerUpdate(getFlagManager(), userPlayer, downloadedData, getScoreDisplay(), messageManager, pointsToWinGame);


        } else if(packetType == WORLD_STATE_UPDATE) {

            if(applyStateUpdate(getFlagManager(), connectedPlayers, userPlayer, lastStateUpdateId, downloadedData, messageManager)) {

                serverUpdateTime = stateUpdateTimer.restart();

                //if he was previously waiting for others it means the game has now started so start playing music and stop waiting for others
                if(waitingForOthers) {

                    waitingForOthers = false;

                    if(GLO_PLAY_MUSIC) {

                        gameBgm.play();
                    }

                }
            }

            //when looking at victory or defeat only load textures
            //if they haven't been loaded already
            //textures haven't been loaded if matchEnded is still false
        } else if(packetType == VICTORY) {

            if(matchEnded == false) {

                loadVictoryTexture(matchResultTexture);
                matchResultSprite.setTexture(matchResultTexture);

                if(GLO_PLAY_SOUNDS) {

                    //disable the previous bgm and play the victor sound
                    winSound.play();
                }

                gameBgm.pause();
            }

            matchEnded = true;

            //after match ends the client begins to ignore server packets that give score updates
            //so the team scores aren't updated
            //add the last score to whatever team won
            ///increases player's team score
            if(userPlayer.getTeam() == TEAM_A_ID) {

                getHeadsUpDisplay().getScoreDisplay().setRedScore(pointsToWinGame);

            } else {

                getHeadsUpDisplay().getScoreDisplay().setBlueScore(pointsToWinGame);
            }

            userPlayer.resetDataTimer();

        } else if(packetType == DEFEAT) {

            if(matchEnded == false) {

                loadDefeatTexture(matchResultTexture);
                matchResultSprite.setTexture(matchResultTexture);

                //disable the previous bgm and play the loss sound
                if(GLO_PLAY_SOUNDS) {

                    loseSound.play();
                }

                gameBgm.pause();
            }

            matchEnded = true;

            //after match ends the client begins to ignore server packets that give score updates
            //so the team scores aren't updated
            //add the last score to whatever team won
            ///increases other teams score
            if(userPlayer.getTeam() == TEAM_A_ID) {

                getHeadsUpDisplay().getScoreDisplay().setBlueScore(pointsToWinGame);

            } else {

                getHeadsUpDisplay().getScoreDisplay().setRedScore(pointsToWinGame);
            }

            userPlayer.resetDataTimer();

        } else if(packetType == LOBBY_CONNECTION_INFO) {

            ///servers at the lobby so client should return as well
            ///but only if the client is not at the game over screen, because once the game is over the server
            ///automatically returns to lobby but the client should stay at the game over screen
            if(!matchEnded) {

                exitGameLoop = true;

            } else {

                //if the match has ended then jsut use this packet as a "heartbeat" from the server so client knows he is still connected
                userPlayer.resetDataTimer();
            }
        }
    }
}

void ClientGameManager::updateScoreboard(sf::RenderWindow& window) {

    if(score.canDisplayInfo()) {

        score.setDisplayCenter(sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2));
        score.clearScoreboard();
        score.addPlayerInfo(userPlayer);
        score.updatePlayerList(connectedPlayers);
        score.setupInfoPosition();
    }
}

void ClientGameManager::updateUserPlayer(const float& delta, sf::RenderWindow& window) {

    userPlayer.update(delta, sf::Vector2f(window.getSize().x, window.getSize().y));
    userPlayer.updateRotation(window.mapPixelToCoords(sf::Mouse::getPosition(window) ));

    //if the player timed out return him to the lobby which will then end the match
    if(userPlayer.timedOut()) {

        displayError(window, "You have disconnected from\nthe server.");
        exitGameLoop = true;
    }
}

void ClientGameManager::updateConnectedPlayers(const float& delta) {

    for(unsigned int index = 0; index < connectedPlayers.size();) {

        if(connectedPlayers[index]->timedOut()) {

            connectedPlayers.erase(connectedPlayers.begin() + index);
            continue;
        }

        connectedPlayers[index]->updateGun(delta);
        ++index;
    }
}

void ClientGameManager::updateStatDisplay() {

    getStatDisplay().setPlayerHealth(userPlayer.getHealth());

    getStatDisplay().setPlayerAmmo(userPlayer.getGun()->getCurrentAmmo(),
                                   userPlayer.getGun()->getMaxCurrentAmmo(),
                                   userPlayer.getGun()->getTotalAmmo());
}

void ClientGameManager::handleBulletCollision() {

    //get the bullets from the player and check for collision with others
    vector<shared_ptr<Bullet> > bullets = userPlayer.getBullets();

    for(auto& bullet : bullets) {

        if(!bullet->checkCanCollide()) {

            continue;
        }

        //first check for colision with blocks, bullets only collide with blocks that don't let bullets pass
        bulletEntityCollision<Block>(bullet, getGameWorld().getBlocks(bullet->getLine()->getStartPoint(), bullet->getLine()->getEndPoint(), userPlayer.getFloor()),
                                     [&](shared_ptr<Block> block) {

                                     return !block->getPassBullets();
                                     });


        playerBulletCollision(bullet);

        //disable the bullets collisoin since it should no longerb e able to collide
        bullet->disableCollision();
    }
}

void ClientGameManager::playerBulletCollision(shared_ptr<Bullet> bullet) {

    //determine the player that was first hit and handle collision with him
    //default nearest collision is the shooter
    unsigned idNearestPlayer = userPlayer.getId();
    sf::Vector2f nearestCollisionPoint = bullet->getLine()->getEndPoint();

    for(auto& player : connectedPlayers) {

        if(player->getTeam() == userPlayer.getTeam() || !player->isAlive()) {

            continue;
        }

        //if player and bullet aren't on same floor don't check collision
        if(player->getFloor() != bullet->getFloor()) {

            continue;
        }

        sf::FloatRect collisionBox = player->getCurrentHitbox();

        //point of collision if there is one
        sf::Vector2f collisionPoint(0, 0);

        //if there is a collision determine if the player that was shot is the nearest player
        if(checkCollision(collisionBox, bullet->getLine(), collisionPoint) &&
            distanceToPoint(bullet->getLine()->getStartPoint(), collisionPoint) < distanceToPoint(bullet->getLine()->getStartPoint(), nearestCollisionPoint)) {

            //this collision point is now the nearest point of collision so this player is the closest
            idNearestPlayer = player->getId();
            nearestCollisionPoint = collisionPoint;
        }
    }

    //now collide with the nearest player if the nearest player is not the shooter
    if(idNearestPlayer != userPlayer.getId()) {

        //player was hit just make the line smaller and indicate it collided with something
        bullet->setEndPoint(nearestCollisionPoint);
        bullet->disableCollision();

        damageDealt.push_back(BulletDamage(idNearestPlayer, bullet->getDamage()));

    } else {

        //if the bullet didn't hit a player then send it to the server for drawing because it doesn't matter where its drawn on the server
        //bullets that collide with a player aren't sent to server and instead the server will create a bullet from the shooter to the person being hit
        userPlayer.getGun()->getBulletsForClients().push_back(bullet);
    }
}

void ClientGameManager::playerForegroundCollision() {

    //if the player or his teammates are hidinging behind something make it visible so you can see
    for(auto& entity : getForeground(userPlayer.getFloor())) {

        bool hidingPlayer = false;

        //check if the player is hiding behind the cover
        if(userPlayer.getDestinationBox().intersects(entity->getCollisionBox())) {

            hidingPlayer = true;

        }

        //check if any of your teammates are hiding behind the foreground
        for(auto player : connectedPlayers) {

            if(player->getDestinationBox().intersects(entity->getCollisionBox()) && player->getTeam() == userPlayer.getTeam()) {

                hidingPlayer = true;
            }
        }

        entity->setHidingPlayer(hidingPlayer);
    }
}

void ClientGameManager::checkButtons(const sf::Vector2f& mousePosition) {

    for(auto& button : endMatchButtons) {

        button->checkMouseTouching(mousePosition, matchEnded);
    }

    for(auto& button : pausedMenuButtons) {

        button->checkMouseTouching(mousePosition, paused && !showControls);
    }

    for(auto& button : controlsButtons) {

        button->checkMouseTouching(mousePosition, showControls);
    }
}

void ClientGameManager::setup(sf::RenderWindow& window) {

    //setup camera
    camera.setDefaultSize(window);

    //setup the player name on the H.U.D
    getStatDisplay().setPlayerName(userPlayer.getName());
    getStatDisplay().setPlayerHealth(userPlayer.getHealth());

    //set player's health to 0 that way player can't move until all other players are loaded
    //and also so player will set his position to server's position when the game starts
    userPlayer.setHealth(0);

    //reset the user players stats
    userPlayer.resetStats();
    userPlayer.resetGun();

    userPlayer.setFloor(OVERGROUND_FLOOR);

    waitingForOthers = true;
    matchEnded = false;

    //delete all previous players so new ones can be created
    connectedPlayers.clear();

    //clear scoreboards so they draw the correct scores the next time players are created
    score.clearScoreboard();

    //reset all flags
    getFlagManager()->resetFlags();
}

void ClientGameManager::handleWindowEvents(sf::Event& event, sf::RenderWindow& window) {

    //window size was changed so change the camera
    if(event.type == sf::Event::Resized) {

        camera.setDefaultSize(window);
    }
}

void ClientGameManager::handleComponentInputs(sf::Event& event, sf::RenderWindow& window) {

    if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

        sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());

        if(matchEnded) {

            if(endMatchButtons[resultToLobbyId]->checkMouseTouching(mousePosition)) {

                PredrawnButton::playClickSound();
                exitGameLoop = true;
            }

            //don't handle other inputs if match has ended
            return;
        }

        //don't let user click on paused menu buttons if he is viewing controls
        if(paused && !showControls) {

            if(pausedMenuButtons[resumeId]->checkMouseTouching(mousePosition)) {

                PredrawnButton::playClickSound();
                paused = false;
            }

            if(pausedMenuButtons[quitMatch]->checkMouseTouching(mousePosition)) {

                PredrawnButton::playClickSound();
                exitGameLoop = true;
            }

            if(pausedMenuButtons[quitGame]->checkMouseTouching(mousePosition)) {

                PredrawnButton::playClickSound();
                window.close();
            }

            if(pausedMenuButtons[controlsId]->checkMouseTouching(mousePosition)) {

                PredrawnButton::playClickSound();
                showControls = true;
            }

            if(pausedMenuButtons[musicId]->checkMouseTouching(mousePosition)) {

                PredrawnButton::playClickSound();
                pausedMenuButtons[musicId]->onClick();
                GLO_PLAY_MUSIC = pausedMenuButtons[musicId]->checkIsOn();

                if(!GLO_PLAY_MUSIC) {

                    gameBgm.pause();

                } else {

                    gameBgm.play();
                }
            }

            if(pausedMenuButtons[soundId]->checkMouseTouching(mousePosition)) {

                PredrawnButton::playClickSound();
                pausedMenuButtons[soundId]->onClick();
                GLO_PLAY_SOUNDS = pausedMenuButtons[soundId]->checkIsOn();
            }

            //don't handle player inputs if game is paused
            return;
        }

        if(showControls) {

            if(controlsButtons[backId]->checkMouseTouching(mousePosition)) {

                PredrawnButton::playClickSound();
                showControls = false;
            }

            return;
        }
    }

    if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {

        paused = !paused;
    }

    //if the game isn't paused and the music should be playing but its not playing then play the music
    if(!paused && GLO_PLAY_MUSIC && gameBgm.getStatus() != sf::Sound::Playing) {

        gameBgm.play();
    }

    //handle the player's inputs
    userPlayer.handleEvents(event);
}

void ClientGameManager::updateComponents(sf::RenderWindow& window) {

    sendInputsToServer();
    sendGunshotsToServer();
    sendDamageToServer();
    handleServerUpdates();

    updateScoreboard(window);

    updateStatDisplay();

    messageManager.updateMessageList();

    //don't handle bullet collision once match is over
    if(matchEnded) {

        return;
    }

    ///handle the bullet collision here isntead of in the time components
    ///update part because the loop may never run because there isn't enough
    ///time accumulate for an update loop to occur, if that happens you get
    ///weird bullet drawins because the bullet never checked for collision
    handleBulletCollision();
}

void ClientGameManager::updateTimeComponents(const float& delta, sf::RenderWindow& window) {

    //don't update anything if match is over
    if(matchEnded) {

        return;
    }

    //only update the player if he is alive
    if(userPlayer.isAlive()) {

        //if the game is paused then don't let the player move, do so by setting time as 0 so he can't move
        float playerDelta = paused ? 0 : delta;

        updateUserPlayer(playerDelta, window);
    }

    updateConnectedPlayers(delta);
}

void ClientGameManager::handlePostUpdate(sf::RenderWindow& window) {

    //don't update anything if match is over
    if(matchEnded) {

        return;
    }

    animateEntities();

    //linearly interpolate all entities to their destination positions
    interpolateEntities();

    //make the minimap centerd on the player
    getHeadsUpDisplay().getMinimap().setTarget(userPlayer.getCurrentPosition());

    //apply camera before setting the center that way camera is already resized before it is set in the center
    //because applycamera causes camera to zoom in or out if the camera started zooming
    camera.applyCamera(window);

    //move camera to new track players new position, unless he died then leave it as is
    ///currently there is no level bounds or properties so just give some default level properties for now
    if(userPlayer.isAlive()) {

        ///level size if hard coded
        camera.setCameraCenter(sf::Vector2f(userPlayer.getCollisionBox().left, userPlayer.getCollisionBox().top), sf::Vector2f(3968, 1472));
    }
}

void ClientGameManager::drawComponents(sf::RenderWindow& window) {

    //if player is waiting for others don't draw the screen
    if(waitingForOthers) {

        return;

    }

    userPlayer.draw(window);

    for(auto& player : connectedPlayers) {

        if(player->getFloor() == userPlayer.getFloor() && player->isDrawingEnabled()) {

            player->draw(window, userPlayer.getFloor());
        }
    }
}

void ClientGameManager::drawUI(sf::RenderWindow& window) {

    //if player is waiting for others don't draw the screen
    if(waitingForOthers) {

        return;

    }

    if(score.canDisplayInfo()) {

        score.draw(window);
    }

    getHeadsUpDisplay().drawGunUI(window, userPlayer.getGun());

    //if the match is already over then draw the appropriate texture
    if(matchEnded) {

        window.draw(matchResultSprite);

        endMatchButtons[resultToLobbyId]->draw(window);

        //if match ended pausing doesn't matter
        return;
    }

    if(paused) {

        window.draw(pausedSprite);

        for(auto& button : pausedMenuButtons) {

            button->draw(window);
        }
    }

    if(showControls) {

        window.draw(controlsSprite);

        for(auto& button : controlsButtons) {

            button->draw(window);
        }
    }

    messageManager.drawMessages(window);

    //draw any messages the gun givers have
    getGameWorld().drawGunGiverMessages(window);
}

void ClientGameManager::drawMinimap(sf::RenderWindow& window) {

    //only draw on minimap if they are on the overground floor
    if(userPlayer.getFloor() == OVERGROUND_FLOOR) {

        userPlayer.drawMinimap(window);
    }


    ///only draw players on minimap if they are in the overground section
    //only draw another player on the minimap if they are on the players team, or they are near someone on the players team, and only if they are overground
    for(auto& player : connectedPlayers) {

        if((player->getTeam() == userPlayer.getTeam() || (player->getCollisionBox().intersects(camera.getCameraBounds()) && userPlayer.getFloor() == OVERGROUND_FLOOR)) && player->getFloor() == OVERGROUND_FLOOR) {

            player->drawMinimap(window);

        } else {

            //not on player's team, so figure out if he is near a teammate
            for(auto teammate : connectedPlayers) {

                //if the player is within the screens of the user's team mate and they are on the overground floor then draw him on the minimap
                if(teammate->getTeam() == userPlayer.getTeam() && player->getCollisionBox().intersects(camera.getCameraBounds(teammate->getCurrentPosition())) &&
                   player->getFloor() == OVERGROUND_FLOOR && teammate->getFloor() == OVERGROUND_FLOOR) {

                    player->drawMinimap(window);

                    //already drawn so leave loop
                    break;
                }
            }
        }
    }
}

const unsigned ClientGameManager::getFloor() const {

    return userPlayer.getFloor();
}

void ClientGameManager::handleCollisions() {

    //no broadphase as of yet
    //check for collision between players and blocks
    //top left and right points of the player's collision box which will be used to determine which blocks he can collide with
    sf::Vector2f topLeft(userPlayer.getCollisionBox().left, userPlayer.getCollisionBox().top);
    sf::Vector2f topRight(userPlayer.getCollisionBox().left + userPlayer.getCollisionBox().width, topLeft.y);

    playerStaticCollision(userPlayer, getGameWorld().getBlocks(topLeft, topRight, userPlayer.getFloor()));

    playerForegroundCollision();

    userPlayer.setPlayRegenAnimation(getGameWorld().getSpawnArea(userPlayer.getTeam()).intersects(userPlayer.getCollisionBox()));

    //disable gun sounds for any other player thats not in the player screen
    //handle collision between all palyers and their spawn zones so they can run the regen animation
    for(auto& player : connectedPlayers) {

        player->setPlayRegenAnimation(getGameWorld().getSpawnArea(player->getTeam()).intersects(player->getCollisionBox()));

        player->setPlaySounds(player->getCollisionBox().intersects(camera.getCameraBounds()));

        //only check for collision with the first foreground object this player collides with that way further collision checks dont enable drawing

        //if a large part of another player collides with a foreground object and the user player is not colliding with it then hide the colliding player
        for(auto& obj : getForeground(player->getFloor())) {

            sf::FloatRect intersectingArea;
            bool collides = obj->getCollisionBox().intersects(player->getCollisionBox(), intersectingArea);

            if(player->getTeam() != userPlayer.getTeam() && collides && intersectingArea.width >= 25 && intersectingArea.height >= 25 && !obj->getCollisionBox().intersects(userPlayer.getCollisionBox())) {

                player->disableDrawing();
                break;

            } else {

                player->enableDrawing();
            }
        }
    }

    //if player collides with a gun giver then make the gun giver display a message to tell user to pick up the gun
    vector<shared_ptr<GunGiver> >& givers = getGameWorld().getGunGivers(userPlayer.getFloor());

    for(auto& giver : givers) {

        giver->setDrawMessage(giver->getCollisionBox().intersects(userPlayer.getCollisionBox()));

        if(giver->getCollisionBox().intersects(userPlayer.getCollisionBox())) {

            giver->handleRefillAmmo(userPlayer);
        }
    }
}
