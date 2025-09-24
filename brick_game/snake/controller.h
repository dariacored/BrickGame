#ifndef SRC_BRICK_GAME_SNAKE_BACKEND_CONTROLLER_H_
#define SRC_BRICK_GAME_SNAKE_BACKEND_CONTROLLER_H_

#include <memory>

#include "model.h"
namespace brickgame {

class Controller {
 public:
  Controller();
  ~Controller();

  void userInput(UserAction_t action, bool hold);
  GameInfo_t updateCurrentState();
  unsigned long long processTimer();
  void freeGameInfo(GameInfo_t* info);

 private:
  std::unique_ptr<SnakeModel> model_;
};

}  // namespace brickgame

#endif  // SRC_BRICK_GAME_SNAKE_BACKEND_CONTROLLER_H_