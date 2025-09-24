#ifndef SRC_BRICK_GAME_H_
#define SRC_BRICK_GAME_H_

#define FIELD_H 20
#define FIELD_W 10

#define INIT_SPEED 500
#define SPEED_STEP 30
#define MAX_LEVEL 10

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

typedef enum { Empty, GamePause, GOTryAgain, Win, StartMenu } Pause_t;

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

#endif  // SRC_BRICK_GAME_H_