#ifndef SRC_BRICK_GAME_TETRIS_BACKEND_CONTROLLER_H_
#define SRC_BRICK_GAME_TETRIS_BACKEND_CONTROLLER_H_

#include "backend.h"
namespace brickgame {

class Controller {
 public:
  Controller();
  void userInput(UserAction_t action, bool hold);
  GameInfo_t updateCurrentState();
  unsigned long long processTimer();
  void freeGameInfo(GameInfo_t* info);
};

}  // namespace brickgame

#endif  // SRC_BRICK_GAME_TETRIS_BACKEND_CONTROLLER_H_