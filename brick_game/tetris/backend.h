#ifndef SRC_BRICK_GAME_TETRIS_BACKEND_H_
#define SRC_BRICK_GAME_TETRIS_BACKEND_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sqlite3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "./../../brick_game.h"

#define NEW_LEVEL_THRESHOLD 600

typedef enum {
  Initial,
  Spawn,
  Moving,
  Shifting,
  Attaching,
  GameOver,
  Paused
} Status_t;

typedef struct {
  int status;
  int previous_status;
  int **field;
  int **block;
  int block_size;
  int **next_block;
  int next_block_size;
  int x;
  int y;
  int score;
  int level;
  int speed;
  int pause;
  unsigned long long start_time;
  unsigned long long time_left;
  unsigned long long pause_start_time;
  bool terminate_requested;
} State_t;

typedef enum {
  I_BLOCK,
  L_BLOCK,
  J_BLOCK,
  O_BLOCK,
  Z_BLOCK,
  T_BLOCK,
  S_BLOCK
} Block_t;

GameInfo_t updateCurrentState();
void freeGameInfo(GameInfo_t *info);
void userInput(UserAction_t action, bool hold);

State_t *getCurrentState();
void initializeState();
void startGame();
void pauseGame();
void finishAndRestartGame();
void requestTermination();

int **createMatrix(int height, int width);
void copyMatrix(int **dest, int **src, int height, int width);
void freeMatrix(int **matrix, int size);
void rotate(int **new_block, int **old_block, int size);
int **generateNewBlock(int *block_size);
void spawnNewBlock();

void moveBlockLeft();
void moveBlockRight();
void shiftBlock();
int isBlockAttached();
void attachBlock();
int canRotateBlock(int **new_block);
void rotateBlock();

unsigned long long currentTime();
unsigned long long processTimer();

void saveMaxScore();
void updateLevel();
void deleteLines();

sqlite3 **getDB();
int initDB();
void closeDB();
int getHighScoreFromDB();
void saveHighScoreToDB(int score);

#ifdef __cplusplus
}
#endif

#endif  // SRC_BRICK_GAME_TETRIS_BACKEND_H_