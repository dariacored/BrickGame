#include "backend.h"

sqlite3 **getDB() {
  static sqlite3 *db = NULL;
  return &db;
}

int initDB() {
  sqlite3 **pdb = getDB();
  int rc = sqlite3_open("tetris.db", pdb);
  if (rc) return -1;

  const char *sql =
      "CREATE TABLE IF NOT EXISTS tetris_scores ("
      "id INTEGER PRIMARY KEY,"
      "value INTEGER NOT NULL);"
      "INSERT OR IGNORE INTO tetris_scores (id, value) VALUES (1, 0);";
  char *err;
  rc = sqlite3_exec(*pdb, sql, 0, 0, &err);
  if (rc != SQLITE_OK) {
    sqlite3_free(err);
    sqlite3_close(*pdb);
    *pdb = NULL;
    return -1;
  }
  return 0;
}

void closeDB() {
  sqlite3 **pdb = getDB();
  if (*pdb) {
    sqlite3_close(*pdb);
    *pdb = NULL;
  }
}

int getHighScoreFromDB() {
  sqlite3 **pdb = getDB();
  if (!*pdb) return 0;
  sqlite3_stmt *stmt;
  const char *sql = "SELECT value FROM tetris_scores WHERE id = 1;";
  int rc = sqlite3_prepare_v2(*pdb, sql, -1, &stmt, 0);
  if (rc != SQLITE_OK) return 0;

  int score = 0;
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    score = sqlite3_column_int(stmt, 0);
  }
  sqlite3_finalize(stmt);
  return score;
}

void saveHighScoreToDB(int score) {
  sqlite3 **pdb = getDB();
  if (!*pdb) return;
  sqlite3_stmt *stmt;
  const char *sql = "UPDATE tetris_scores SET value = ? WHERE id = 1;";
  sqlite3_prepare_v2(*pdb, sql, -1, &stmt, 0);
  sqlite3_bind_int(stmt, 1, score);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

GameInfo_t updateCurrentState() {
  State_t *state = getCurrentState();
  GameInfo_t info = {0};
  int **field = createMatrix(FIELD_H, FIELD_W);
  copyMatrix(field, state->field, FIELD_H, FIELD_W);
  for (int i = 0; i < state->block_size; i++) {
    for (int j = 0; j < state->block_size; j++) {
      int new_x = state->x - i;
      int new_y = state->y + j;
      if (state->block[i][j] == 1 && new_x >= 0 && new_y < FIELD_W) {
        field[new_x][new_y] = 1;
      }
    }
  }
  info.field = field;
  info.score = state->score;
  info.level = state->level;
  info.speed = state->speed;
  int **next = createMatrix(4, 4);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      next[i][j] = 0;
    }
  }
  int offset_x = (4 - state->next_block_size) / 2,
      offset_y = (4 - state->next_block_size) / 2;
  for (int i = 0; i < state->next_block_size; i++) {
    for (int j = 0; j < state->next_block_size; j++) {
      next[offset_x + i][offset_y + j] = state->next_block[i][j];
    }
  }
  info.next = next;
  info.high_score = getHighScoreFromDB();
  info.pause = Empty;
  if (state->status == Paused) {
    info.pause = GamePause;
  }
  if (state->status == GameOver) {
    info.pause = GOTryAgain;
  }
  return info;
}

void freeGameInfo(GameInfo_t* info) {
  if (info->field) {
    for (int i = 0; i < FIELD_H; i++) {
        free(info->field[i]);
    }
    free(info->field);
    info->field = NULL;
  }
  if (info->next) {
    for (int i = 0; i < 4; i++) {
        free(info->next[i]);
    }
    free(info->next);
    info->next = NULL;
  }
}

void userInput(UserAction_t action, bool hold) {
  (void)hold;
  if (action == Start) initializeState();
  State_t *state = getCurrentState();

  switch (action) {

    case Start:
      if (state->status == Initial)
        startGame();
      else if (state->status == GameOver)
        finishAndRestartGame();
      break;

    case Pause:
      pauseGame();
      break;

    case Terminate:
      requestTermination();
      break;
    
    case Left:
      if (state->status == Moving) moveBlockLeft();
      break;

    case Right:
      if (state->status == Moving) moveBlockRight();
      break;

    case Down:
      if (state->status == Moving) {
        while (isBlockAttached() == 0) {
          state->x++;
        }
        state->status = Attaching;
      }
      break;

    case Action:
      if (state->status == Moving) rotateBlock();
      break;

    default:
      if (state->status == Moving) {
        state->status = Shifting;
        shiftBlock();
      } else if (state->status == Spawn) {
        spawnNewBlock();
      } else if (state->status == Attaching) {
        attachBlock();
      }
  }
}

State_t *getCurrentState() {
  static State_t state = {0};
  static bool initialized = false;
  
  if (!initialized) {
    state.field = NULL;
    state.block = NULL;
    state.next_block = NULL;
    state.status = Initial;
    initialized = true;
  }
  
  return &state;
}

void initializeState() {
  initDB();
  State_t *state = getCurrentState();

  state->terminate_requested = false;
  state->status = Initial;

  int **field = createMatrix(FIELD_H, FIELD_W);

  state->field = field;
  state->score = 0;
  state->level = 1;
  state->speed = INIT_SPEED;
  state->time_left = state->speed;
  state->pause = 0;
  state->x = -1;
  state->y = 4;

  int next_block_size;
  int **next_block = generateNewBlock(&next_block_size);
  int **block = createMatrix(next_block_size, next_block_size);
  copyMatrix(block, next_block, next_block_size, next_block_size);
  state->next_block_size = next_block_size;
  state->next_block = next_block;
  state->block_size = next_block_size;
  state->block = block;

  srand(currentTime());
}

void startGame() {
  State_t *state = getCurrentState();
  if (state->status == Initial) {
    state->status = Spawn;
  }
}

void pauseGame() {
  State_t *state = getCurrentState();

  if (state->status == Moving || state->status == Shifting) {
    state->previous_status = state->status;
    state->status = Paused;
    state->pause = GamePause;
    state->pause_start_time = currentTime();

  } else if (state->status == Paused) {
    state->status = state->previous_status;
    state->pause = Empty;
    unsigned long long pause_duration = currentTime() - state->pause_start_time;
    state->start_time += pause_duration;
  }
}

void finishAndRestartGame() {
  closeDB();

  State_t *state = getCurrentState();

  if (state->block) {
    freeMatrix(state->block, state->block_size);
    state->block = NULL;
  }
  
  if (state->next_block) {
    freeMatrix(state->next_block, state->next_block_size);
    state->next_block = NULL;
  }
  
  if (state->field) {
    freeMatrix(state->field, FIELD_H);
    state->field = NULL;
  }

  initializeState();
}

void requestTermination() {
  State_t *state = getCurrentState();
  state->terminate_requested = true;
  finishAndRestartGame();
}

int **createMatrix(int height, int width) {
  int **matrix = (int **)malloc(height * sizeof(int *));
  for (int i = 0; i < height; i++) {
    int *row = (int *)malloc(width * sizeof(int));
    matrix[i] = row;
    for (int j = 0; j < width; j++) {
      matrix[i][j] = 0;
    }
  }
  return matrix;
}

void copyMatrix(int **dest, int **src, int height, int width) {
  if (src == NULL || dest == NULL) return;
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      dest[i][j] = src[i][j];
    }
  }
}

void freeMatrix(int **matrix, int size) {
  if (!matrix) return;
  
  for (int i = 0; i < size; i++) {
    if (matrix[i]) {
      free(matrix[i]);
      matrix[i] = NULL;
    }
  }
  free(matrix);
}

void rotate(int **new_block, int **old_block, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      new_block[i][j] = 0;
    }
  }

  if (size == 4) {
    if (old_block[0][0] == 1) {
      new_block[0][1] = 1;
      new_block[1][1] = 1;
      new_block[2][1] = 1;
      new_block[3][1] = 1;
    } else {
      new_block[0][0] = 1;
      new_block[0][1] = 1;
      new_block[0][2] = 1;
      new_block[0][3] = 1;
    }
  } else {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        new_block[j][size - 1 - i] = old_block[i][j];
      }
    }
  }
}

int **generateNewBlock(int *block_size) {
  static const struct {
    int size;
    int coords[4][2];
  } BLOCKS[7] = {[I_BLOCK] = {4, {{0, 0}, {0, 1}, {0, 2}, {0, 3}}},
                 [L_BLOCK] = {3, {{1, 0}, {0, 0}, {0, 1}, {0, 2}}},
                 [J_BLOCK] = {3, {{1, 2}, {0, 0}, {0, 1}, {0, 2}}},
                 [O_BLOCK] = {2, {{0, 0}, {0, 1}, {1, 0}, {1, 1}}},
                 [Z_BLOCK] = {3, {{0, 0}, {0, 1}, {1, 1}, {1, 2}}},
                 [T_BLOCK] = {3, {{0, 0}, {0, 1}, {0, 2}, {1, 1}}},
                 [S_BLOCK] = {3, {{1, 0}, {1, 1}, {0, 1}, {0, 2}}}};

  int block_type = rand() % 7;
  *block_size = BLOCKS[block_type].size;

  int **block = createMatrix(*block_size, *block_size);
  for (int i = 0; i < 4; i++) {
    int x = BLOCKS[block_type].coords[i][0];
    int y = BLOCKS[block_type].coords[i][1];
    if (x < *block_size && y < *block_size) {
      block[x][y] = 1;
    }
  }

  int **temp = createMatrix(*block_size, *block_size);
  for (int i = rand() % 4; i > 0; i--) {
    rotate(temp, block, *block_size);
    copyMatrix(block, temp, *block_size, *block_size);
  }

  freeMatrix(temp, *block_size);
  return block;
}

void spawnNewBlock() {
  State_t *state = getCurrentState();

  freeMatrix(state->block, state->block_size);
  state->block_size = state->next_block_size;
  state->block = state->next_block;

  state->x = -1;
  if (state->block_size == 2) {
    state->y = 4;
  } else {
    state->y = 3;
  }

  int next_block_size;
  int **next_block = generateNewBlock(&next_block_size);
  state->next_block_size = next_block_size;
  state->next_block = next_block;

  state->status = Moving;
  state->start_time = currentTime();
  state->time_left = state->speed;
}

void moveBlockLeft() {
  State_t *state = getCurrentState();
  int can_move_left = 1;

  for (int i = 0; i < state->block_size && can_move_left; i++) {
    for (int j = 0; j < state->block_size && can_move_left; j++) {
      if (state->block[i][j] != 1) continue;

      const int new_y = state->y - 1 + j;
      const int new_x = state->x - i;

      if (new_y < 0) {
        can_move_left = 0;
        break;
      }

      if (new_x >= 0 && new_y < FIELD_W && state->field[new_x][new_y] == 1) {
        can_move_left = 0;
        break;
      }
    }
  }

  if (can_move_left) {
    state->y--;
    state->status = isBlockAttached() ? Attaching : Moving;
  } else {
    state->status = Moving;
  }
}

void moveBlockRight() {
  State_t *state = getCurrentState();
  int can_move_right = 1;

  for (int i = 0; i < state->block_size && can_move_right; i++) {
    for (int j = 0; j < state->block_size && can_move_right; j++) {
      if (state->block[i][j] != 1) continue;

      const int new_y = state->y + 1 + j;
      const int new_x = state->x - i;

      if (new_y >= FIELD_W) {
        can_move_right = 0;
        break;
      }

      if (new_x >= 0 && new_y >= 0 && state->field[new_x][new_y] == 1) {
        can_move_right = 0;
        break;
      }
    }
  }

  if (can_move_right) {
    state->y++;
    state->status = isBlockAttached() ? Attaching : Moving;
  } else {
    state->status = Moving;
  }
}

void shiftBlock() {
  State_t *state = getCurrentState();

  int attached = isBlockAttached();
  if (attached == 0) {
    (state->x)++;
    state->start_time = currentTime();
    state->time_left = state->speed;
    state->status = Moving;
  } else {
    state->status = Attaching;
  }
}

int isBlockAttached() {
  const State_t *state = getCurrentState();
  int attached = 0;

  for (int i = 0; i < state->block_size; i++) {
    for (int j = 0; j < state->block_size; j++) {
      int new_x = state->x + 1 - i;
      int new_y = state->y + j;

      if (state->block[i][j] == 1) {
        if (new_x >= FIELD_H) {
          attached = 1;
        } else if (new_x >= 0) {
          if (new_y < 0 || new_y >= FIELD_W) {
            attached = 1;
          } else if (state->field[new_x][new_y] == 1) {
            attached = 1;
          }
        }
      }
    }
  }
  return attached;
}

void attachBlock() {
  State_t *state = getCurrentState();
  int game_over = 0;

  for (int i = 0; i < state->block_size; i++) {
    for (int j = 0; j < state->block_size; j++) {
      int new_x = state->x - i;
      int new_y = state->y + j;

      if (new_x >= 0 && new_x < FIELD_H && new_y >= 0 && new_y < FIELD_W &&
          state->block[i][j] == 1) {
        state->field[new_x][new_y] = 1;
      } else if (state->block[i][j] == 1 && new_x < 0) {
        game_over = 1;
      }
    }
  }

  if (game_over == 1) {
    state->status = GameOver;
    state->pause = GOTryAgain;
  } else {
    deleteLines();
    state->status = Spawn;
  }
}

int canRotateBlock(int **new_block) {
  State_t *state = getCurrentState();
  int can_rotate = 1;

  int **old_block = state->block;
  state->block = new_block;

  for (int i = 0; i < state->block_size; i++) {
    for (int j = 0; j < state->block_size; j++) {
      int new_x = state->x - i;
      int new_y = state->y + j;

      if (state->block[i][j] == 1 && (new_y < 0 || new_y >= FIELD_W)) {
        can_rotate = 0;
      } else if (new_y >= 0 && new_y < FIELD_W && new_x >= 0) {
        if ((state->block[i][j] + state->field[new_x][new_y]) == 2) {
          can_rotate = 0;
        }
      }
    }
  }
  state->block = old_block;

  return can_rotate;
}

void rotateBlock() {
  State_t *state = getCurrentState();

  int **new_block = createMatrix(state->block_size, state->block_size);
  rotate(new_block, state->block, state->block_size);

  if (canRotateBlock(new_block) == 1) {
    freeMatrix(state->block, state->block_size);
    state->block = new_block;
  } else {
    freeMatrix(new_block, state->block_size);
  }

  int attached = isBlockAttached();

  if (attached == 0) {
    state->status = Moving;
  } else {
    state->status = Attaching;
  }
}

unsigned long long currentTime() {
  struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  return (unsigned long long)ts.tv_sec * 1000 +
         (unsigned long long)ts.tv_nsec / 1000000;
}

unsigned long long processTimer() {
  State_t *state = getCurrentState();

  unsigned long long time_left;

  if (state->status == Initial) {
    time_left = -1;
  } else {
    unsigned long long elapsed_time = currentTime() - state->start_time;
    if (elapsed_time >= state->time_left) {
      time_left = 0;
    } else {
      state->time_left -= elapsed_time;
      state->start_time = currentTime();
      time_left = state->time_left;
    }
  }
  return time_left;
}

void saveMaxScore() {
  const State_t *state = getCurrentState();
  int high_score = getHighScoreFromDB();

  if (state->score > high_score) {
    saveHighScoreToDB(state->score);
  }
}

void updateLevel() {
  State_t *state = getCurrentState();

  int new_level = state->score / NEW_LEVEL_THRESHOLD + 1;
  if (new_level > MAX_LEVEL) {
    state->status = Win;
  }
  state->speed -= (new_level - state->level) * SPEED_STEP;
  state->level = new_level;
}

void deleteLines() {
  State_t *state = getCurrentState();
  int full_lines = 0;

  for (int i = FIELD_H - 1; i > 0; i--) {
    int block_count = 0;
    for (int j = 0; j < FIELD_W; j++) {
      block_count += state->field[i][j];
    }

    if (block_count == FIELD_W) {
      for (int z = i; z > 0; z--) {
        for (int j = 0; j < FIELD_W; j++) {
          state->field[z][j] = state->field[z - 1][j];
        }
      }
      i++;
      full_lines++;
    }
  }

  if (full_lines == 1) {
    state->score += 100;
  } else if (full_lines == 2) {
    state->score += 300;
  } else if (full_lines == 3) {
    state->score += 700;
  } else if (full_lines == 4) {
    state->score += 1500;
  }

  saveMaxScore();
  updateLevel();
}