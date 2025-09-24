#ifndef SRC_BRICK_GAME_FRONTEND_DESKTOP_H_
#define SRC_BRICK_GAME_FRONTEND_DESKTOP_H_

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "./../../brick_game.h"
#include "./../../brick_game/tetris/controller.h"

namespace brickgame {
class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 public slots:
  void quitApp();

 private slots:
  void updateGUI();

 private:
  void initializeGUI();
  void keyPressEvent(QKeyEvent *event) override;
  UserAction_t getSignal(int input) const;
  void handleUserInput(int input);
  void renderGUI(GameInfo_t info);

  Controller *controller;
  QGraphicsView *gameView;
  QGraphicsScene *gameScene;
  QTimer *gameTimer;
  QLabel *controlsLabel;
  QLabel *nextLabel;
  QGraphicsView *nextBlockView;
  QGraphicsScene *nextBlockScene;
  QLabel *highScoreLabel;
  QLabel *scoreLabel;
  QLabel *levelLabel;
  QLabel *speedLabel;

  bool gameStarted;
  bool gameEnded;
  bool gamePaused;
};

}  // namespace brickgame

#endif  // SRC_BRICK_GAME_FRONTEND_DESKTOP_H_