#include "controller.h"

namespace brickgame {

Controller::Controller() : model_(std::make_unique<SnakeModel>()) {}

Controller::~Controller() = default;

void Controller::userInput(UserAction_t action, bool hold) {
  model_->handleInput(action, hold);
}

GameInfo_t Controller::updateCurrentState() {
  model_->update();
  return model_->getGameInfo();
}

unsigned long long Controller::processTimer() { return model_->processTimer(); }

void Controller::freeGameInfo(GameInfo_t* info) {
  if (info->field) {
    for (int i = 0; i < FIELD_H; ++i) {
      delete[] info->field[i];
    }
    delete[] info->field;
    info->field = nullptr;
  }

  if (info->next) {
    for (int i = 0; i < 4; ++i) {
      delete[] info->next[i];
    }
    delete[] info->next;
    info->next = nullptr;
  }
}

}  // namespace brickgame