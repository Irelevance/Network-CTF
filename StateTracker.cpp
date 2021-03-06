#include "StateTracker.h"
#include "Lerp.h"

StateTracker::StateTracker(const int& idOfPlayer, const int& statesSaved) :
    playerId(idOfPlayer),
    pastStates(),
    maxStatesSaved(statesSaved)
    {

    }

void StateTracker::insertState(const sf::Uint32& stateId, const sf::Vector2f& position) {

    stateUpdate state;
    state.stateId = stateId;
    state.position = position;

    pastStates.push_back(state);

    //if there are too many states saved delete the first one added
    if(pastStates.size() > maxStatesSaved) {

        pastStates.erase(pastStates.begin());
    }
}

sf::Vector2f StateTracker::approximatePosition(const sf::Uint32& idInitialState, const float& deltaFraction) {

    //can't approximate position if there aren't any states saved
    if(pastStates.size() == 0) {

        return sf::Vector2f(0, 0);
    }

    //check if the given state is still saved
    if(!(pastStates[0].stateId <= idInitialState && pastStates[pastStates.size() - 1].stateId >= idInitialState )) {

        return sf::Vector2f(0, 0);
    }

    //lastly check if the given state is the last state added to the tracker, if it is then there is no state past this state and theres nothing to interpolate towards
    if(pastStates[pastStates.size() - 1].stateId == idInitialState) {

        return pastStates[pastStates.size() - 1].position;
    }

    sf::Vector2f initialPosition(0, 0);
    sf::Vector2f finalPosition(0, 0);

    //find the initial position and final positions in order to interpolate between the two
    for(unsigned int index = 0; index < pastStates.size(); index++) {

        if(pastStates[index].stateId == idInitialState) {

            //we can assume that there is index + 1 is a valid counter for the vector because we know the inital state is not the last object in the vector
            initialPosition = pastStates[index].position;
            finalPosition = pastStates[index + 1].position;
        }
    }

    sf::Vector2f interpolatedPosition(0, 0);
    interpolatedPosition.x = initialPosition.x + interpolate(initialPosition.x, finalPosition.x, deltaFraction);
    interpolatedPosition.y = initialPosition.y + interpolate(initialPosition.y, finalPosition.y, deltaFraction);

    return interpolatedPosition;
}

unsigned StateTracker::getStateCount() {

    return pastStates.size();
}

const int StateTracker::getPlayerId() {

    return playerId;
}
