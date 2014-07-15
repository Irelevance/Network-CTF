#include "ClientGameManager.h"
#include "PacketManipulators.h"
#include "PacketIdentification.h"
#include "Bullet.h"
#include "LineSegment.h"
#include "Collision.h"
#include "InterpolatingPlayer.h"

#include <iostream>

/**





MAKE CLIENT GAME MANAGER HAVE THE FUNCTION TO CALCULATE DELTA FRACTION FOR SERVER UPDATES AND THEN SEND THE DATA
TO THE SERVER
**/

using std::tr1::shared_ptr;
using std::cout;
using std::endl;
using std::vector;

ClientGameManager::ClientGameManager() :
    GameManager(),
    currentState(STATE_PLAYING),
    client("192.168.0.14", 8080),
    userPlayer(),
    connectedPlayers(),
    stateUpdateTimer(),
    serverUpdateTime(sf::milliseconds(60)),
    lastStateUpdateId(0)
    {

    }

void ClientGameManager::interpolateEntities() {

    ///delta fraction for user player and other players are different
    ///because userplayer is interpolated based on physics update rate
    ///and other players are updated at a different time interval (time interval is dependant on server, for now its 60 milliseconds)
    userPlayer.interpolate(calculateDeltaFraction());

    //make sure the denominator isn't 0
    //calculate the delta fraction to interpolate other connected players
    //this uses the time between server updates for interpolation rather than time between physics update cycle
    float deltaFraction = calculateServerDelta();

    for(auto player : connectedPlayers) {

        player->interpolate(deltaFraction);
    }
}

const float ClientGameManager::calculateServerDelta() {

    //make sure the denominator isn't 0
    //calculate the delta fraction to interpolate other connected players
    //this uses the time between server updates for interpolation rather than time between physics update cycle
    float deltaFraction = stateUpdateTimer.getElapsedTime().asSeconds() / (serverUpdateTime.asSeconds() > 0.f ? serverUpdateTime.asSeconds() : 1.f);

    return deltaFraction;
}


void ClientGameManager::sendInputsToServer() {

    sf::Packet packagedInputs;

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

    //check if the user shot any bullets and send any gunshot data to the server
    if(userPlayer.getGunshotsToSend().size() == 0) {

        return;
    }

    sf::Packet packetToSend;

    const float deltaFraction = calculateServerDelta();

    createGunfirePacket(userPlayer, deltaFraction, lastStateUpdateId, packetToSend);

    //clear the queued gunfires
    userPlayer.clearGunshotQueue();

    client.sendToServer(packetToSend);
}

void ClientGameManager::handleServerUpdates() {

    //try and receive data and if there is any data read it
    sf::Packet downloadedData;

    //not server data so no point handling the data
    if(!client.receiveFromServer(downloadedData)) {

        return;
    }

    //check the type of data received
    const int packetType = getPacketType(downloadedData);

    //respond to the packet depending on the type of packet
    if(packetType == PLAYER_STATE_UPDATE) {

        applyPlayerUpdate(userPlayer, downloadedData);


    } else if(packetType == WORLD_STATE_UPDATE) {

        applyStateUpdate(connectedPlayers, userPlayer, lastStateUpdateId, downloadedData);
        serverUpdateTime = stateUpdateTimer.restart();
    }
}

void ClientGameManager::updateUserPlayer(const float& delta, sf::RenderWindow& window) {

    userPlayer.update(delta, sf::Vector2f(window.getSize().x, window.getSize().y));
    userPlayer.updateRotation(window.mapPixelToCoords(sf::Mouse::getPosition(window) ));
}

void ClientGameManager::updateConnectedPlayers(const float& delta) {

    for(auto player : connectedPlayers) {

        player->updateGun(delta);
    }
}

void ClientGameManager::handleBulletCollision() {

    //get the bullets from the player and check for collision with others
    vector<shared_ptr<Bullet> > bullets = userPlayer.getBullets();

    for(auto bullet : bullets) {

        if(!bullet->checkCanCollide()) {

            continue;
        }

        //fist check for colision with blocks
        bulletEntityCollision(bullet, getBlocks());

        //handle collision with other players
        bulletEntityCollision<InterpolatingPlayer>(bullet, connectedPlayers);
    }
}

void ClientGameManager::setup() {

    int playerId = 0;

    if(client.connectToServer(playerId)) {

        userPlayer.setId(playerId);
    }
}

void ClientGameManager::handleStateInputs() {

    ///intentionally left blank for now
}

void ClientGameManager::handleComponentInputs(sf::Event& event, sf::RenderWindow& window) {

    //handle the player's inputs
    userPlayer.handleEvents(event);
}

void ClientGameManager::updateComponents(sf::RenderWindow& window) {

    sendInputsToServer();
    sendGunshotsToServer();
    handleServerUpdates();

    ///handle the bullet collision here isntead of in the time components
    ///update part because the loop may never run because there isn't enough
    ///time accumulate for an update loop to occur, if that happens you get
    ///weird bullet drawins because the bullet never checked for collision
    handleBulletCollision();
}

void ClientGameManager::updateTimeComponents(const float& delta, sf::RenderWindow& window) {

    updateUserPlayer(delta, window);
    updateConnectedPlayers(delta);
}

void ClientGameManager::handlePostUpdate() {

    //linearly interpolate all entities to their destination positions
    interpolateEntities();
}

void ClientGameManager::drawComponents(sf::RenderWindow& window) {

    userPlayer.draw(window);

    for(auto player : connectedPlayers) {

        player->draw(window);
    }
}

void ClientGameManager::handleCollisions() {

    //no broadphase as of yet, check if player collides with any blocks and handle collision accordingly
    //check for collision between players and blocks
    for(auto block : getBlocks()) {

        if(userPlayer.getDestinationBox().intersects(block->getCollisionBox())) {

            sf::Vector2f movementOffset = calculateCollisionOffset(userPlayer.getDestinationBox(), block->getCollisionBox());

            //make player move the required distance to escape collision
            userPlayer.move(movementOffset);
        }
    }
}
