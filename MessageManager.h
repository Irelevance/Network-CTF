#ifndef MESSAGEMANAGER_H_INCLUDED
#define MESSAGEMANAGER_H_INCLUDED

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "Message.h"

#include <vector>

///class that keeps track of messages and displays them to the user
///holds messages for a certain period of time, and holds a maximum of x messages
///if a message is too old or there are too many messages it will delete the oldest message
///when messages are created place the newest messages at the front of the container
///that way you can use the indice of the message to position them
class MessageManager {

    private:

        std::vector<Message> messages;

        int maxMessages;

        //all messages are positioned from this point going upwards
        sf::Vector2f initialPosition;

        //approximate height of a single message
        //global bounds of the message text can't be used because it doesn't get the proper height for some characters, like y or g
        float messageHeight;

    public:

        MessageManager():
            maxMessages(5),
            initialPosition(0, 650),
            messageHeight(18)
            {

            }

        //repositions all messages so oldest ones appear at bottom and newest ones appear at the top
        //and erases all timed out messages
        void updateMessageList();

        void drawMessages(sf::RenderWindow& window);

        void createAllySlain() {

            messages.insert(messages.begin(), Message(Message::allySlainClip));
        }

        void createEnemySlain() {

            messages.insert(messages.begin(), Message(Message::enemySlainClip));
        }

        void createPlayerReceivedKill() {

            messages.insert(messages.begin(), Message(Message::receivedKillClip));
        }

        void createPlayerDied() {

            messages.insert(messages.begin(), Message(Message::playerDiedClip));
        }
};

#endif // MESSAGEMANAGER_H_INCLUDED
