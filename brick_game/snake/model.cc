#include "model.h"

namespace brickgame {

SnakeModel::SnakeModel()
    : fsm_(),
      current_direction_(Direction_t::UP),
      next_direction_(Direction_t::UP),
      score_(0),
      high_score_(0),
      level_(1),
      base_speed_(INIT_SPEED),
      current_speed_(INIT_SPEED),
      is_accelerated_(false),
      apple_x_(-1),
      apple_y_(-1),
      db_(nullptr),
      last_update_time_(std::chrono::steady_clock::now()) {
  initDB();
  high_score_ = getHighScoreFromDB();
}

SnakeModel::~SnakeModel() {
  saveMaxScore();
  closeDB();
}

void SnakeModel::handleInput(UserAction_t action, bool hold) {
  static_cast<void>(hold);
  SnakeFSM::State_t currentState = fsm_.getState();

  switch (action) {
    case Start:
      if (currentState == SnakeFSM::State_t::INITIAL ||
          currentState == SnakeFSM::State_t::GAME_OVER ||
          currentState == SnakeFSM::State_t::WIN) {
        reset();
        spawnSnake();
      }
      break;
    case Pause:
      if (currentState == SnakeFSM::State_t::MOVING) {
        fsm_.paused();
      } else if (currentState == SnakeFSM::State_t::PAUSED) {
        fsm_.moving();
      }
      break;
    case Terminate:
      if (currentState != SnakeFSM::State_t::INITIAL) {
        saveMaxScore();
        reset();
      }
      break;
    case Left:
    case Right:
    case Up:
    case Down:
      if (currentState == SnakeFSM::State_t::MOVING) {
        Direction_t new_dir;
        if (action == Left)
          new_dir = Direction_t::LEFT;
        else if (action == Right)
          new_dir = Direction_t::RIGHT;
        else if (action == Up)
          new_dir = Direction_t::UP;
        else if (action == Down)
          new_dir = Direction_t::DOWN;
        else
          return;
        changeDirection(new_dir);
      }
      break;
    case Action:
      if (currentState == SnakeFSM::State_t::MOVING) {
        accelerate();
      }
      break;
    default:
      break;
  }
}

GameInfo_t SnakeModel::getGameInfo() {
  GameInfo_t info = {};
  info.field = getField();
  info.next = getNext();
  info.score = getScore();
  info.high_score = getHighScore();
  info.level = getLevel();
  info.speed = getSpeed();
  info.pause = getPauseState();
  return info;
}

unsigned long long SnakeModel::processTimer() {
  if (fsm_.getState() == SnakeFSM::State_t::MOVING) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                       now - last_update_time_)
                       .count();

    if (elapsed >= current_speed_) {
      move();
      last_update_time_ = now;
    }
  }

  return 0;
}

void SnakeModel::update() { processTimer(); }

int** SnakeModel::getField() const {
  int** field = new int*[FIELD_H];
  for (int i = 0; i < FIELD_H; ++i) {
    field[i] = new int[FIELD_W]{0};
  }

  // apple = 1
  if (apple_x_ >= 0 && apple_x_ < FIELD_H && apple_y_ >= 0 &&
      apple_y_ < FIELD_W) {
    field[apple_x_][apple_y_] = 1;
  }

  if (!snake_.empty()) {
    // body = 2, head = 3
    for (const auto& segment : snake_) {
      if (segment.first >= 0 && segment.first < FIELD_H &&
          segment.second >= 0 && segment.second < FIELD_W) {
        field[segment.first][segment.second] = 2;
      }
    }
    int head_x = snake_[0].first, head_y = snake_[0].second;
    if (head_x >= 0 && head_x < FIELD_H && head_y >= 0 && head_y < FIELD_W) {
      field[head_x][head_y] = 3;
    }
  }

  return field;
}

int** SnakeModel::getNext() const {
  int** next = new int*[4];
  for (int i = 0; i < 4; ++i) {
    next[i] = new int[4]{0};
  }
  return next;
}

int SnakeModel::getScore() { return score_; }

int SnakeModel::getHighScore() { return high_score_; }

int SnakeModel::getLevel() { return level_; }

int SnakeModel::getSpeed() { return base_speed_; }

int SnakeModel::getPauseState() const {
  switch (fsm_.getState()) {
    case SnakeFSM::State_t::INITIAL:
      return StartMenu;
    case SnakeFSM::State_t::PAUSED:
      return GamePause;
    case SnakeFSM::State_t::GAME_OVER:
      return GOTryAgain;
    case SnakeFSM::State_t::WIN:
      return Win;
    default:
      return Empty;
  }
}

SnakeFSM::State_t SnakeModel::getState() const { return fsm_.getState(); }

void SnakeModel::spawnSnake() {
  snake_ = {{10, 5}, {11, 5}, {12, 5}, {13, 5}};
  generateApple();

  fsm_.spawn();
  fsm_.moving();
}

void SnakeModel::move() {
  if (fsm_.getState() != SnakeFSM::State_t::MOVING) return;

  current_direction_ = next_direction_;
  int head_x = snake_[0].first;
  int head_y = snake_[0].second;

  switch (current_direction_) {
    case Direction_t::UP:
      head_x--;
      break;
    case Direction_t::DOWN:
      head_x++;
      break;
    case Direction_t::LEFT:
      head_y--;
      break;
    case Direction_t::RIGHT:
      head_y++;
      break;
  }

  snake_.insert(snake_.begin(), {head_x, head_y});
  if (head_x == apple_x_ && head_y == apple_y_) {
    score_++;
    updateLevel();
    saveMaxScore();
    generateApple();
    resetAcceleration();
  } else {
    snake_.pop_back();
  }

  if (checkCollision()) {
    resetAcceleration();
    if (snake_.size() >= MAX_SNAKE_LEN) {
      fsm_.win();
    } else {
      fsm_.gameOver();
    }
    saveMaxScore();
  }
}

void SnakeModel::pause() {
  if (fsm_.getState() == SnakeFSM::State_t::MOVING) {
    fsm_.paused();
  } else if (fsm_.getState() == SnakeFSM::State_t::PAUSED) {
    fsm_.moving();
  }
}

void SnakeModel::reset() {
  score_ = 0;
  level_ = 1;
  is_accelerated_ = false;
  snake_.clear();
  current_direction_ = Direction_t::UP;
  next_direction_ = Direction_t::UP;
  base_speed_ = INIT_SPEED;
  current_speed_ = base_speed_;
  last_update_time_ = std::chrono::steady_clock::now();
  fsm_.initial();
}

void SnakeModel::changeDirection(Direction_t new_direction) {
  if ((current_direction_ == Direction_t::UP &&
       new_direction != Direction_t::DOWN) ||
      (current_direction_ == Direction_t::DOWN &&
       new_direction != Direction_t::UP) ||
      (current_direction_ == Direction_t::LEFT &&
       new_direction != Direction_t::RIGHT) ||
      (current_direction_ == Direction_t::RIGHT &&
       new_direction != Direction_t::LEFT)) {
    next_direction_ = new_direction;
  }
}

bool SnakeModel::checkCollision() const {
  const auto& head = snake_[0];
  if (head.first < 0 || head.first >= FIELD_H || head.second < 0 ||
      head.second >= FIELD_W)
    return true;

  for (size_t i = 1; i < snake_.size(); i++) {
    if (head == snake_[i]) return true;
  }

  return false;
}

void SnakeModel::generateApple() {
  std::vector<std::pair<int, int>> empty_cells;
  for (int i = 0; i < FIELD_H; ++i) {
    for (int j = 0; j < FIELD_W; ++j) {
      bool is_empty = true;
      for (const auto& segment : snake_) {
        if (segment.first == i && segment.second == j) {
          is_empty = false;
          break;
        }
      }
      if (is_empty) empty_cells.push_back({i, j});
    }
  }

  if (!empty_cells.empty()) {
    int index = rand() % empty_cells.size();
    apple_x_ = empty_cells[index].first;
    apple_y_ = empty_cells[index].second;
  } else {
    fsm_.win();
    saveMaxScore();
  }
}

void SnakeModel::accelerate() {
  if (!is_accelerated_) {
    is_accelerated_ = true;
    current_speed_ = MAX_SPEED;
  }
}

void SnakeModel::resetAcceleration() {
  if (is_accelerated_) {
    is_accelerated_ = false;
    current_speed_ = base_speed_;
  }
}

void SnakeModel::updateLevel() {
  int new_level = score_ / NEW_LEVEL_THRESHOLD_SNAKE + 1;
  new_level = new_level > MAX_LEVEL ? MAX_LEVEL : new_level;
  if (new_level != level_) {
    level_ = new_level;
    base_speed_ = INIT_SPEED - (level_ - 1) * SPEED_STEP;
    current_speed_ = base_speed_;
  }
}

void SnakeModel::saveMaxScore() {
  int high_score = getHighScoreFromDB();
  if (score_ > high_score) {
    saveHighScoreToDB(score_);
    high_score_ = score_;
  }
}

int SnakeModel::initDB() {
  int rc = sqlite3_open("snake.db", &db_);
  if (rc) return -1;

  const char* sql =
      "CREATE TABLE IF NOT EXISTS snake_scores ("
      "id INTEGER PRIMARY KEY,"
      "value INTEGER NOT NULL);"
      "INSERT OR IGNORE INTO snake_scores (id, value) VALUES (1, 0);";
  char* err;
  rc = sqlite3_exec(db_, sql, 0, 0, &err);
  if (rc != SQLITE_OK) {
    sqlite3_free(err);
    return -1;
  }
  return 0;
}

void SnakeModel::closeDB() {
  if (db_) sqlite3_close(db_);
}

int SnakeModel::getHighScoreFromDB() {
  if (!db_) return 0;
  sqlite3_stmt* stmt;
  const char* sql = "SELECT value FROM snake_scores WHERE id = 1;";
  if (sqlite3_prepare_v2(db_, sql, -1, &stmt, 0) != SQLITE_OK) return 0;

  int score = 0;
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    score = sqlite3_column_int(stmt, 0);
  }
  sqlite3_finalize(stmt);
  return score;
}

void SnakeModel::saveHighScoreToDB(int score) {
  if (!db_) return;
  sqlite3_stmt* stmt;
  const char* sql = "UPDATE snake_scores SET value = ? WHERE id = 1;";
  sqlite3_prepare_v2(db_, sql, -1, &stmt, 0);
  sqlite3_bind_int(stmt, 1, score);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

}  // namespace brickgame