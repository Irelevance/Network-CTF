#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "ClientGameManager.h"
#include "ServerGameManager.h"
#include "Input.h"
#include "PredrawnButton.h"
#include "ButtonPlacer.h"
#include "TypeChecker.h"
#include "ErrorMessage.h"

#include <tr1/memory>
#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>

using std::vector;
using std::tr1::shared_ptr;
using std::cout;
using std::endl;
using std::string;
using std::atoi;

void clientTitleScreen(sf::RenderWindow& window);
void clientHelpMenu(sf::RenderWindow& window);
void findMatchScreen(sf::RenderWindow& window);
void creditsScreen(sf::RenderWindow& window);

void serverTitleScreen(sf::RenderWindow& window);

int main() {

    srand(static_cast<long>(time(0) ));

    sf::Vector2f screenSize(1024, 768);

    //disable the resizing in the window at the beginning because the title screen and such have a fixed size
    sf::RenderWindow window(sf::VideoMode(screenSize.x, screenSize.y), "Network Test", sf::Style::Titlebar | sf::Style::Close);
    window.setKeyRepeatEnabled(false);

    clientTitleScreen(window);
    ///serverTitleScreen(window);

    return 0;
}

void clientTitleScreen(sf::RenderWindow& window) {

    //load up the image of the title screen
    sf::Texture titleScreenTexture;
    titleScreenTexture.loadFromFile("clientMainMenu.png");

    sf::Sprite titleScreenSprite;
    titleScreenSprite.setTexture(titleScreenTexture);

    //create buttons and place them
    vector<shared_ptr<PredrawnButton> > buttons;

    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("playButton.png")));
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("controlsButton.png")));
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("creditsButton.png")));
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("quitButton.png")));

    //indices for each button
    unsigned playButton = 0;
    unsigned controlsButton = 1;
    unsigned creditsButton = 2;
    unsigned quitButton = 3;

    placeButtons("clientMainMenu.png", buttons);

    sf::Event event;

    while(window.isOpen()) {

        while(window.pollEvent(event)) {

            if(event.type == sf::Event::Closed) {

                window.close();
            }

            if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

                sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());

                if(buttons[playButton]->checkMouseTouching(mousePosition)) {

                    findMatchScreen(window);
                }

                if(buttons[quitButton]->checkMouseTouching(mousePosition)) {

                    window.close();
                }

                if(buttons[creditsButton]->checkMouseTouching(mousePosition)) {

                    creditsScreen(window);
                }

                if(buttons[controlsButton]->checkMouseTouching(mousePosition)) {

                    controlsScreen(window);
                }
            }
        }

        sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());

        for(auto& button : buttons) {

            button->checkMouseTouching(mousePosition);
        }

        window.clear();

        window.draw(titleScreenSprite);

        for(auto& button : buttons) {

            button->draw(window);
        }

        window.display();
    }
}

void clientHelpMenu(sf::RenderWindow& window) {

    //save the previous window image as a backgroud image that way you don't have to load the background again
    sf::Texture background;
    background.create(window.getSize().x, window.getSize().y);
    background.update(window);

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(background);

    //load image for the help menu
    sf::Texture helpScreen;
    helpScreen.loadFromFile("clientHelpMenu.png");

    sf::Sprite helpScreenSprite;
    helpScreenSprite.setTexture(helpScreen);

    vector<shared_ptr<PredrawnButton> > buttons;

    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("backButton.png")));

    placeButtons("clientHelpMenu.png", buttons);

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

                    return;
                }
            }
        }

        sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());

        for(auto& button : buttons) {

            button->checkMouseTouching(mousePosition);
        }

        window.clear();
        window.draw(backgroundSprite);

        window.draw(helpScreenSprite);

        for(auto& button : buttons) {

            button->draw(window);
        }

        window.display();
    }
}

void findMatchScreen(sf::RenderWindow& window) {

    //save the previous window image as a backgroud image that way you don't have to load the background again
    sf::Texture background;
    background.create(window.getSize().x, window.getSize().y);
    background.update(window);

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(background);

    //load the images and buttons required for the find match screen
    sf::Texture findMatchTexture;
    findMatchTexture.loadFromFile("playMenu.png");

    sf::Sprite findMatchSprite;
    findMatchSprite.setTexture(findMatchTexture);

    //create buttons for entering text
    sf::FloatRect serverIpBox(473, 327, 250, 20);
    sf::FloatRect serverPortBox(473, 391, 250, 20);
    sf::FloatRect nameBox(473, 455, 250, 20);

    vector<shared_ptr<PredrawnButton> > buttons;

    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("textButton.png")));
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("textButton.png")));
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("textButton.png")));
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("playButton.png")));
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("backButton.png")));
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("helpButton.png")));

    placeButtons("playMenu.png", buttons);

    //indices for buttons
    unsigned playButton = 3;
    unsigned backButton = 4;
    unsigned helpButton = 5;

    sf::Font font;
    font.loadFromFile("font.ttf");

    const float textScale = 0.55;

    //create a text representation of the current server ip and port the user is trying to connect to
    sf::Text serverIp;
    serverIp.setString("70.71.114.74");
    serverIp.setFont(font);
    serverIp.setPosition(serverIpBox.left, serverIpBox.top);
    serverIp.setColor(sf::Color::Black);
    serverIp.setScale(textScale, textScale);

    sf::Text serverPort;
    serverPort.setString("8080");
    serverPort.setFont(font);
    serverPort.setPosition(serverPortBox.left, serverPortBox.top);
    serverPort.setColor(sf::Color::Black);
    serverPort.setScale(textScale, textScale);

    sf::Text playerName;
    playerName.setString("Name");
    playerName.setFont(font);
    playerName.setPosition(nameBox.left, nameBox.top);
    playerName.setColor(sf::Color::Black);
    playerName.setScale(textScale, textScale);

    sf::Event event;

    while(window.isOpen()) {

        while(window.pollEvent(event)) {

            if(event.type == sf::Event::Closed) {

                window.close();
            }

            if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

                sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());

                if(serverIpBox.contains(mousePosition)) {

                    string ipToConnectTo = receiveInput(window, "Enter the server's Ip Address", font, serverIpBox.width, textScale);
                    serverIp.setString(ipToConnectTo);

                } else if(serverPortBox.contains(mousePosition)) {

                    string portToConnectTo = receiveInput(window, "Enter the server's port", font, serverPortBox.width, textScale);
                    serverPort.setString(portToConnectTo);

                } else if(nameBox.contains(mousePosition)) {

                    string name = receiveInput(window, "Enter your player name", font, nameBox.width, textScale);

                    if(name != "") {

                        playerName.setString(name);
                    }

                } else if(buttons[playButton]->checkMouseTouching(mousePosition)) {

                    //if the port is invalid then don't let player try to connect and make him enter a valid port
                    if(!isDigit(serverPort.getString().toAnsiString())) {

                        displayError(window, "The server port is invalid.");
                        continue;
                    }

                    ClientGameManager client;
                    client.setPlayerName(playerName.getString());

                    string ipAddress = serverIp.getString();
                    unsigned short port = static_cast<unsigned short>(atoi(serverPort.getString().toAnsiString().c_str()) );

                    if(client.connectToServer(ipAddress, port)) {

                        client.gameLobby(window, font);

                    } else {

                        displayError(window, "Failed to connect to server.");
                    }

                } else if(buttons[backButton]->checkMouseTouching(mousePosition)) {

                    return;

                } else if(buttons[helpButton]->checkMouseTouching(mousePosition)) {

                    clientHelpMenu(window);
                }
            }
        }

        sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());

        for(auto& button : buttons) {

            button->checkMouseTouching(mousePosition);
        }

        window.clear();

        window.draw(backgroundSprite);

        window.draw(findMatchSprite);

        for(auto& button : buttons) {

            button->draw(window);
        }

        window.draw(serverIp);
        window.draw(serverPort);
        window.draw(playerName);

        window.display();
    }
}

void creditsScreen(sf::RenderWindow& window) {

    //save the previous window
    sf::Texture previousScreen;
    previousScreen.create(window.getSize().x, window.getSize().y);
    previousScreen.update(window);

    sf::Sprite previousScreenSprite;
    previousScreenSprite.setTexture(previousScreen);

    //load the image for the credits
    sf::Texture creditsTexture;
    creditsTexture.loadFromFile("creditsMenu.png");

    sf::Sprite creditsSprite;
    creditsSprite.setTexture(creditsTexture);

    //load the buttons
    vector<shared_ptr<PredrawnButton> > buttons;
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("backButton.png")));

    unsigned backButton = 0;

    placeButtons("creditsMenu.png", buttons);

    sf::Event event;

    while(window.isOpen()) {

        while(window.pollEvent(event)) {

            if(event.type == sf::Event::Closed) {

                window.close();
            }

            if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

                sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());

                if(buttons[backButton]->checkMouseTouching(mousePosition)) {

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
        window.draw(creditsSprite);

        for(auto& button : buttons) {

            button->draw(window);
        }

        window.display();
    }
}

void serverTitleScreen(sf::RenderWindow& window) {

    sf::Texture titleScreenTexture;
    sf::Sprite titleScreenSprite;

    titleScreenTexture.loadFromFile("serverTitleScreen.png");
    titleScreenSprite.setTexture(titleScreenTexture);

    //create buttons for user to press
    vector<shared_ptr<PredrawnButton> > buttons;

    //create all buttons
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("serverTextBox.png")) );
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("hostGameButton.png")) );
    buttons.push_back(shared_ptr<PredrawnButton>(new PredrawnButton("quitButton.png")) );

    //id for each button that is in the vector that way its easy to access
    unsigned hostGameId = 1;
    unsigned quitButton = 2;

    placeButtons<shared_ptr<PredrawnButton> >("serverTitlescreen.png" , buttons);

    //create a button for user to press in order to change port numbers
    sf::FloatRect changePortButton(528, 351, 131, 20);

    sf::Font font;
    font.loadFromFile("font.ttf");

    //text to save the server port
    sf::Text serverPortText;
    serverPortText.setFont(font);
    serverPortText.setScale(0.6, 0.6);
    serverPortText.setString("8080");
    serverPortText.setPosition(changePortButton.left, changePortButton.top);
    serverPortText.setColor(sf::Color::Black);

    sf::Event event;

    ServerGameManager server;

    while(window.isOpen()) {

        while(window.pollEvent(event)) {

            if(event.type == sf::Event::Closed) {

                window.close();
            }

            if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

                sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                //don't attempt to host a game if port isn't even a number
                if(buttons[hostGameId]->checkMouseTouching(mousePosition) && isDigit(serverPortText.getString().toAnsiString())) {

                    unsigned port = atoi(serverPortText.getString().toAnsiString().c_str());

                    //attempt to bind to the socket, if failed then let user know
                    if(server.bindToPort(port)) {

                        server.gameLobby(window);

                    } else {

                        displayError(window, "This port is unavailable.\nPlease choose a different\nport.");
                    }
                }

                if(buttons[quitButton]->checkMouseTouching(mousePosition)) {

                    window.close();
                }

                if(changePortButton.contains(mousePosition)) {

                    string port = receiveInput(window, "Enter the host port.", font, changePortButton.width);

                    if(port.size() > 0) {

                        serverPortText.setString(port);
                    }
                }
            }
        }

        sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());

        for(auto& button : buttons) {

            button->checkMouseTouching(mousePosition);
        }

        window.clear();

        window.draw(titleScreenSprite);

        for(auto& button : buttons) {

            button->draw(window);
        }

        window.draw(serverPortText);

        window.display();
    }
}
