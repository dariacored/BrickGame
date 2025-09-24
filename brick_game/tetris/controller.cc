#include "controller.h"

namespace brickgame {

Controller::Controller() {}

void Controller::userInput(UserAction_t action, bool hold) {
  ::userInput(action, hold);
}

GameInfo_t Controller::updateCurrentState() { return ::updateCurrentState(); }

unsigned long long Controller::processTimer() { return ::processTimer(); }

void Controller::freeGameInfo(GameInfo_t* info) { return ::freeGameInfo(info); }

}  // namespace brickgame