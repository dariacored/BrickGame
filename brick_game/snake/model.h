#ifndef SRC_BRICK_GAME_SNAKE_BACKEND_MODEL_H_
#define SRC_BRICK_GAME_SNAKE_BACKEND_MODEL_H_

#include <sqlite3.h>

#include <chrono>
#include <cstdlib>

#include "./../../brick_game.h"
#include "fsm.h"

namespace brickgame {

class SnakeModel {
 public:
  enum class Direction_t { UP, DOWN, LEFT, RIGHT };

  SnakeModel();
  ~SnakeModel();

  void handleInput(UserAction_t action, bool hold);
  void update();
  unsigned long long processTimer();
  GameInfo_t getGameInfo();

  SnakeFSM::State_t getState() const;
  void reset();

 private:
  void move();
  void spawnSnake();
  void pause();
  void changeDirection(Direction_t new_direction);
  bool checkCollision() const;
  void generateApple();
  void accelerate();
  void resetAcceleration();
  void updateLevel();
  void saveMaxScore();

  int initDB();
  void closeDB();
  int getHighScoreFromDB();
  void saveHighScoreToDB(int score);

  int **getField() const;
  int **getNext() const;
  int getScore();
  int getHighScore();
  int getLevel();
  int getSpeed();
  int getPauseState() const;

  static const int NEW_LEVEL_THRESHOLD_SNAKE = 5;
  static const int MAX_SPEED = 0;
  static const int MAX_SNAKE_LEN = 200;

  SnakeFSM fsm_;
  std::vector<std::pair<int, int>> snake_;
  Direction_t current_direction_;
  Direction_t next_direction_;
  int score_;
  int high_score_;
  int level_;
  int base_speed_;
  int current_speed_;
  bool is_accelerated_;
  int apple_x_;
  int apple_y_;
  sqlite3 *db_;
  std::chrono::time_point<std::chrono::steady_clock> last_update_time_;
};

}  // namespace brickgame

#endif  // SRC_BRICK_GAME_SNAKE_BACKEND_MODEL_H_