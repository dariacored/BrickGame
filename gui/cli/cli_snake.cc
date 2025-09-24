#include "./../../brick_game/snake/controller.h"
#include "./frontend.h"

using namespace brickgame;

void snakeGame() {
  Controller controller;
  bool running = true;
  while (running) {
    int input = getch();
    UserAction_t action = getSignal(input);

    if (action == Terminate) {
      running = false;
    }

    controller.userInput(action, false);

    GameInfo_t info = controller.updateCurrentState();
    renderGUI(info);
    controller.freeGameInfo(&info);
  }
}

int main() {
  initializeGUI();
  snakeGame();
  cleanupGUI();
  return 0;
}