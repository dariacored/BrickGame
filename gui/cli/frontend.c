#include "frontend.h"

void initializeGUI() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  curs_set(FALSE);
  initColors();
  refresh();
}

void initColors() {
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);  // base
  init_pair(2, COLOR_GREEN, COLOR_GREEN);  // blocks and apple
  init_pair(3, COLOR_WHITE, COLOR_BLUE);   // pause
  init_pair(4, COLOR_WHITE, COLOR_RED);    // gameover
  init_pair(5, COLOR_BLUE, COLOR_BLUE);    // snake head
  init_pair(6, COLOR_CYAN, COLOR_CYAN);    // snake body
  init_pair(7, COLOR_WHITE, COLOR_GREEN);  // gameover
}

void renderGUI(GameInfo_t game_info) {
  struct timespec render_delay = {
      .tv_sec = 0,
      .tv_nsec = 16666667L  // ~60 FPS (16.666 ms)
  };
  thrd_sleep(&render_delay, NULL);

  WINDOW *controls = printControls();
  wrefresh(controls);

  WINDOW *game = printGameField(game_info);
  wrefresh(game);

  WINDOW *info = printGameInfo(game_info);
  wrefresh(info);

  if (game_info.pause == GamePause) {
    WINDOW *pause = printPauseMessage();
    wrefresh(pause);
    delwin(pause);
  }

  if (game_info.pause == GOTryAgain) {
    WINDOW *gameover = printGameOverMessage();
    wrefresh(gameover);
    delwin(gameover);
  }

  if (game_info.pause == Win) {
    WINDOW *win = printWinMessage();
    wrefresh(win);
    delwin(win);
  }

  delwin(controls);
  delwin(game);
  delwin(info);
}

WINDOW *printControls() {
  WINDOW *controls_window =
      newwin(GAME_FIELD_H, CONTROLS_W, TOP_MARGIN, LEFT_MARGIN);
  box(controls_window, 0, 0);

  mvwprintw(controls_window, 0, (CONTROLS_W - 14) / 2, " CONTROL KEYS ");

  mvwprintw(controls_window, 4, 2, "START         Enter");
  mvwprintw(controls_window, 6, 2, "PAUSE         P");
  mvwprintw(controls_window, 8, 2, "ACTION        Space");
  mvwprintw(controls_window, 10, 2, "MOVE LEFT     <");
  mvwprintw(controls_window, 12, 2, "MOVE RIGHT    >");
  mvwprintw(controls_window, 14, 2, "MOVE DOWN     v");
  mvwprintw(controls_window, 16, 2, "EXIT          ESC");

  return controls_window;
}

WINDOW *printGameField(GameInfo_t game_info) {
  WINDOW *game_window =
      newwin(GAME_FIELD_H, GAME_FIELD_W, TOP_MARGIN, CONTROLS_W);

  box(game_window, 0, 0);

  mvwprintw(game_window, 0, (GAME_FIELD_W - 12) / 2, " BRICK GAME ");

  for (int i = 0; i < FIELD_H; i++) {
    for (int j = 0; j < FIELD_W; j++) {
      if (game_info.field[i][j] == 1) {
        wattron(game_window, COLOR_PAIR(2));
        mvwprintw(game_window, i + 1, 3 * j + 1, "   ");
        wattroff(game_window, COLOR_PAIR(2));
      } else if (game_info.field[i][j] == 2) {
        wattron(game_window, COLOR_PAIR(5));
        mvwprintw(game_window, i + 1, 3 * j + 1, "   ");
        wattroff(game_window, COLOR_PAIR(5));
      } else if (game_info.field[i][j] == 3) {
        wattron(game_window, COLOR_PAIR(6));
        mvwprintw(game_window, i + 1, 3 * j + 1, "   ");
        wattroff(game_window, COLOR_PAIR(6));
      } else {
        wattron(game_window, COLOR_PAIR(1));
        mvwprintw(game_window, i + 1, 3 * j + 1, " + ");
        wattroff(game_window, COLOR_PAIR(1));
      }
    }
  }

  return game_window;
}

WINDOW *printGameInfo(GameInfo_t game_info) {
  WINDOW *info_window =
      newwin(GAME_FIELD_H, GAME_INFO_W, TOP_MARGIN, CONTROLS_W + GAME_FIELD_W);
  box(info_window, 0, 0);

  mvwprintw(info_window, 0, (GAME_INFO_W - 18) / 2, " GAME INFORMATION ");

  mvwprintw(info_window, 2, 2, "NEXT BLOCK");

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (game_info.next[i][j] == 1) {
        wattron(info_window, COLOR_PAIR(2));
        mvwprintw(info_window, i + 4, j * 3 + 4, "   ");
        wattroff(info_window, COLOR_PAIR(2));
      }
    }
  }

  mvwprintw(info_window, 8, 2, "HIGH SCORE:  %d", game_info.high_score);
  mvwprintw(info_window, 11, 2, "SCORE:       %d", game_info.score);
  mvwprintw(info_window, 14, 2, "LEVEL:       %d", game_info.level);
  mvwprintw(info_window, 17, 2, "SPEED:       %d", game_info.speed);

  return info_window;
}

WINDOW *printPauseMessage() {
  WINDOW *pause_menu =
      newwin(PAUSE_MENU_H, PAUSE_MENU_W, (GAME_FIELD_H - PAUSE_MENU_H) / 2,
             GAME_FIELD_W - (GAME_FIELD_W - PAUSE_MENU_W) / 2);
  box(pause_menu, 0, 0);
  wbkgd(pause_menu, COLOR_PAIR(3));

  mvwprintw(pause_menu, 1, 5, "GAME IS PAUSED");
  mvwprintw(pause_menu, 3, 3, "PRESS P TO CONTINUE");

  wrefresh(pause_menu);
  return pause_menu;
}

void cleanupGUI() { endwin(); }

WINDOW *printGameOverMessage() {
  WINDOW *gameover_menu = newwin(
      GAMEOVER_MENU_H, GAMEOVER_MENU_W, (GAME_FIELD_H - GAMEOVER_MENU_H) / 2,
      GAME_FIELD_W - (GAME_FIELD_W - GAMEOVER_MENU_W) / 2);
  box(gameover_menu, 0, 0);
  wbkgd(gameover_menu, COLOR_PAIR(4));

  mvwprintw(gameover_menu, 1, 8, "GAME OVER");
  mvwprintw(gameover_menu, 2, 7, "PRESS ENTER");
  mvwprintw(gameover_menu, 3, 6, "TO TRY AGAIN!");

  wrefresh(gameover_menu);
  return gameover_menu;
}

WINDOW *printWinMessage() {
  WINDOW *win_menu =
      newwin(WIN_MENU_H, WIN_MENU_W, (GAME_FIELD_H - WIN_MENU_H) / 2,
             GAME_FIELD_W - (GAME_FIELD_W - WIN_MENU_W) / 2);
  box(win_menu, 0, 0);
  wbkgd(win_menu, COLOR_PAIR(7));

  mvwprintw(win_menu, 1, 9, "YOU WIN");
  mvwprintw(win_menu, 2, 7, "PRESS ENTER");
  mvwprintw(win_menu, 3, 6, "TO TRY AGAIN!");

  wrefresh(win_menu);
  return win_menu;
}

UserAction_t getSignal(int input) {
  UserAction_t action = -1;
  switch (input) {
    case START_KEY:
      action = Start;
      break;
    case KEY_LEFT:
      action = Left;
      break;
    case KEY_RIGHT:
      action = Right;
      break;
    case KEY_DOWN:
      action = Down;
      break;
    case KEY_UP:
      action = Up;
      break;
    case ACTION_KEY:
      action = Action;
      break;
    case PAUSE_KEY:
      action = Pause;
      break;
    case TERMINATE_KEY:
      action = Terminate;
      break;
    default:
      break;
  }
  return action;
}