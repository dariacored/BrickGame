#ifndef SRC_BRICK_GAME_FRONTEND_CLI_H_
#define SRC_BRICK_GAME_FRONTEND_CLI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ncurses.h>
#include <stdlib.h>
#include <threads.h>

#include "./../../brick_game.h"

#define GAME_FIELD_H (FIELD_H + 2)
#define GAME_FIELD_W (FIELD_W * 3 + 2)
#define START_MENU_H 22
#define START_MENU_W 80
#define PAUSE_MENU_H 5
#define PAUSE_MENU_W 24
#define GAMEOVER_MENU_H 5
#define GAMEOVER_MENU_W 24
#define WIN_MENU_H 5
#define WIN_MENU_W 24
#define GAME_INFO_W 24
#define CONTROLS_W 24

#define TOP_MARGIN 0
#define LEFT_MARGIN 0

#define ACTION_KEY 32
#define TERMINATE_KEY 27
#define PAUSE_KEY 112
#define START_KEY 10

void initializeGUI();
void initColors();
void renderGUI(GameInfo_t game_info);
WINDOW *printControls();
WINDOW *printGameField(GameInfo_t game_info);
WINDOW *printGameInfo(GameInfo_t game_info);
WINDOW *printPauseMessage();
void cleanupGUI();
WINDOW *printGameOverMessage();
WINDOW *printWinMessage();
UserAction_t getSignal(int input);

#ifdef __cplusplus
}
#endif

#endif  // SRC_BRICK_GAME_FRONTEND_CLI_H_