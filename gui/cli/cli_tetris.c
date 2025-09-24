#include "./../../brick_game/tetris/backend.h"
#include "./frontend.h"

void tetrisGame() {
  bool termination_requested = false;
  GameInfo_t info = updateCurrentState();
  renderGUI(info);
  freeGameInfo(&info);

  while (!termination_requested) {
    unsigned long long time_left = processTimer();
    timeout(time_left);
    int c = getch();
    
    if (c == TERMINATE_KEY) {
      userInput(Terminate, false);
      termination_requested = true;
    } else {
      userInput(getSignal(c), false);
    }

    if (!termination_requested) {
      info = updateCurrentState();
      renderGUI(info);
      freeGameInfo(&info);
    }
  }
}

int main() {
  initializeGUI();
  tetrisGame();
  cleanupGUI();
  return 0;
}