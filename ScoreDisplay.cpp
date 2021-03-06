#include "ScoreDisplay.h"
#include "Conversion.h"

ScoreDisplay::ScoreDisplay(const sf::Vector2u& screenSize):
    scoreTexture(),
    scoreSprite(),
    RED_SCORE_OFFSET(125, 88),
    BLUE_SCORE_OFFSET(168, 88),
    font(),
    TEXT_SCALE(0.35),
    redScore(),
    blueScore(),
    redScoreNum(0),
    blueScoreNum(0),
    messageTexture(),
    allyStoleFlag(),
    allyCapturedFlag(),
    enemyStoleFlag(),
    enemyCapturedFlag(),
    allyCaptureTimer(),
    enemyCaptureTimer(),
    captureDisplayTime(sf::seconds(3)),
    drawAllyStolen(false),
    drawEnemyStolen(false)
    {
        scoreTexture.loadFromFile("images/scoredisplay.png");
        scoreSprite.setTexture(scoreTexture);

        font.loadFromFile("font.ttf");

        redScore.setFont(font);
        redScore.setScale(TEXT_SCALE, TEXT_SCALE);

        blueScore.setFont(font);
        blueScore.setScale(TEXT_SCALE, TEXT_SCALE);

        //make everything transparent so players can see
        scoreSprite.setColor(sf::Color(255, 255, 255, 150));
        redScore.setColor(sf::Color(255, 255, 255, 150));
        blueScore.setColor(sf::Color(255, 255, 255, 150));

        positionDisplay(screenSize);

        initMessages();
    }

void ScoreDisplay::handleScreenResize(const sf::Vector2u& screenSize) {

    positionDisplay(screenSize);
}

void ScoreDisplay::setRedScore(unsigned short& score) {

    redScore.setString(toString(score));
    redScoreNum = score;
}

void ScoreDisplay::setBlueScore(unsigned short& score) {

    blueScore.setString(toString(score));
    blueScoreNum = score;
}

void ScoreDisplay::draw(sf::RenderWindow& window) {

    window.draw(scoreSprite);
    window.draw(redScore);
    window.draw(blueScore);

    if(drawAllyStolen) {

        window.draw(allyStoleFlag);
    }

    if(allyCaptureTimer.getElapsedTime() < captureDisplayTime) {

        window.draw(allyCapturedFlag);
    }

    if(drawEnemyStolen) {

        window.draw(enemyStoleFlag);
    }

    if(enemyCaptureTimer.getElapsedTime() < captureDisplayTime) {

        window.draw(enemyCapturedFlag);
    }
}

void ScoreDisplay::positionDisplay(const sf::Vector2u& screenSize) {

    scoreSprite.setPosition(screenSize.x / 2 - scoreSprite.getGlobalBounds().width / 2, 0);

    positionScores();
}

void ScoreDisplay::positionScores() {

    redScore.setPosition(scoreSprite.getPosition().x + RED_SCORE_OFFSET.x,
                         scoreSprite.getPosition().y + RED_SCORE_OFFSET.y);


    blueScore.setPosition(scoreSprite.getPosition().x + BLUE_SCORE_OFFSET.x,
                          scoreSprite.getPosition().y + BLUE_SCORE_OFFSET.y);

}

void ScoreDisplay::initMessages() {

    messageTexture.loadFromFile("images/gameText.png");

    //window is always 1024 by 768 so position messages from bottom of the window
    //leave a 2 message gap from the bottom of the screen to the ally messages
    //and a 1 message gap from the bottom of the screen to the enemy messages
    allyStoleFlag.setTexture(messageTexture);
    allyStoleFlag.setTextureRect(sf::IntRect(1, 1, 382, 31));
    allyStoleFlag.scale(1.25, 1.25);
    allyStoleFlag.setPosition(1024 / 2 - allyStoleFlag.getGlobalBounds().width / 2, 768 - allyStoleFlag.getGlobalBounds().height * 3);

    allyCapturedFlag.setTexture(messageTexture);
    allyCapturedFlag.setTextureRect(sf::IntRect(1, 33, 382, 31));
    allyCapturedFlag.scale(1.25, 1.25);
    allyCapturedFlag.setPosition(1024 / 2 - allyCapturedFlag.getGlobalBounds().width / 2,
                                 scoreSprite.getGlobalBounds().top + scoreSprite.getGlobalBounds().height);

    enemyStoleFlag.setTexture(messageTexture);
    enemyStoleFlag.setTextureRect(sf::IntRect(1, 65, 382, 31));
    enemyStoleFlag.scale(1.25, 1.25);
    enemyStoleFlag.setPosition(1024 / 2 - enemyStoleFlag.getGlobalBounds().width / 2, 768 - enemyStoleFlag.getGlobalBounds().height * 2);

    enemyCapturedFlag.setTexture(messageTexture);
    enemyCapturedFlag.setTextureRect(sf::IntRect(1, 97, 382, 31));
    enemyCapturedFlag.scale(1.25, 1.25);
    enemyCapturedFlag.setPosition(1024 / 2 - enemyCapturedFlag.getGlobalBounds().width / 2,
                                  scoreSprite.getGlobalBounds().top + scoreSprite.getGlobalBounds().height);
}
