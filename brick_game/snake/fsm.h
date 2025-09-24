#ifndef SRC_BRICK_GAME_SNAKE_BACKEND_FSM_H_
#define SRC_BRICK_GAME_SNAKE_BACKEND_FSM_H_

#include <algorithm>
#include <map>
#include <vector>

namespace brickgame {

class SnakeFSM {
 public:
  enum class State_t { INITIAL, SPAWN, MOVING, PAUSED, GAME_OVER, WIN };

  SnakeFSM();

  State_t getState() const;

  void initial();
  void spawn();
  void moving();
  void paused();
  void gameOver();
  void win();

 private:
  bool canTransitTo(State_t newState) const;
  void transitTo(State_t newState);
  State_t currentState_;
  std::map<State_t, std::vector<State_t>> transitions_;
};

}  // namespace brickgame

#endif  // SRC_BRICK_GAME_SNAKE_BACKEND_FSM_H_