#include "mainwindow.h"

#define BLOCK_SIZE 20

using namespace brickgame;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      gameStarted(false),
      gameEnded(false),
      gamePaused(false) {
  controller = new Controller();
  initializeGUI();
  gameTimer = new QTimer(this);
  connect(gameTimer, &QTimer::timeout, this, &MainWindow::updateGUI);
  gameTimer->start(100);
}

MainWindow::~MainWindow() { delete controller; }

void MainWindow::initializeGUI() {
  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
  mainLayout->setSpacing(20);

  QVBoxLayout *leftPanel = new QVBoxLayout();
  controlsLabel = new QLabel(this);
  controlsLabel->setText(
      "CONTROL KEYS:\n"
      "START     Enter\n"
      "PAUSE         P\n"
      "ACTION    Space\n"
      "MOVE LEFT     <\n"
      "MOVE RIGHT    >\n"
      "MOVE DOWN     v\n"
      "MOVE UP       ^\n"
      "EXIT        ESC\n");
  leftPanel->addWidget(controlsLabel);
  leftPanel->addStretch();
  mainLayout->addLayout(leftPanel);

  QVBoxLayout *centerPanel = new QVBoxLayout();
  gameView = new QGraphicsView(this);

  gameScene = new QGraphicsScene(0, 0, FIELD_W * BLOCK_SIZE,
                                 FIELD_H * BLOCK_SIZE, this);

  gameView->setScene(gameScene);
  gameView->setFixedSize(FIELD_W * BLOCK_SIZE + 2, FIELD_H * BLOCK_SIZE + 2);
  gameView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  gameView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  gameView->setFocusPolicy(Qt::NoFocus);
  gameView->setInteractive(false);

  centerPanel->addWidget(gameView, 0, Qt::AlignCenter);
  mainLayout->addLayout(centerPanel);

  QVBoxLayout *rightPanel = new QVBoxLayout();

  nextLabel = new QLabel("NEXT BLOCK:");
  nextBlockScene =
      new QGraphicsScene(0, 0, 4 * BLOCK_SIZE, 4 * BLOCK_SIZE, this);

  nextBlockView = new QGraphicsView(this);
  nextBlockView->setScene(nextBlockScene);
  nextBlockView->setFixedSize(4 * BLOCK_SIZE + 2, 4 * BLOCK_SIZE + 2);
  nextBlockView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  nextBlockView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  nextBlockView->setFocusPolicy(Qt::NoFocus);
  nextBlockView->setInteractive(false);

  highScoreLabel = new QLabel("HIGH SCORE: 0");
  scoreLabel = new QLabel("SCORE: 0");
  levelLabel = new QLabel("LEVEL: 1");
  speedLabel = new QLabel("SPEED: 500");

  QVBoxLayout *infoLayout = new QVBoxLayout();
  infoLayout->addWidget(highScoreLabel);
  infoLayout->addWidget(scoreLabel);
  infoLayout->addWidget(levelLabel);
  infoLayout->addWidget(speedLabel);

  rightPanel->addWidget(nextLabel, 0, Qt::AlignCenter);
  rightPanel->addWidget(nextBlockView, 0, Qt::AlignCenter);
  rightPanel->addLayout(infoLayout);
  rightPanel->addStretch();

  mainLayout->addLayout(rightPanel);
  centralWidget->setFocusPolicy(Qt::StrongFocus);
  centralWidget->setFocus();
}

void MainWindow::updateGUI() {
  unsigned long long time_left = controller->processTimer();
  if (time_left == 0) {
    controller->userInput(static_cast<UserAction_t>(-1), false);
  }

  GameInfo_t info = controller->updateCurrentState();

  if (info.pause == GOTryAgain || info.pause == Win) {
    gameEnded = true;
    gameTimer->stop();

    QString message = (info.pause == Win) ? "YOU WIN" : "GAME OVER";
    if (QMessageBox::question(this, message, "TRY AGAIN?",
                              QMessageBox::Yes | QMessageBox::No) ==
        QMessageBox::Yes) {
      controller->userInput(Start, false);
      gameEnded = false;
      gameTimer->start(100);
    } else {
      controller->freeGameInfo(&info);
      quitApp();
    }
  }

  renderGUI(info);
  controller->freeGameInfo(&info);
}

void MainWindow::renderGUI(GameInfo_t info) {
  if (!info.field || !info.next) return;

  highScoreLabel->setText(QString("HIGH SCORE: %1").arg(info.high_score));
  scoreLabel->setText(QString("SCORE: %1").arg(info.score));
  levelLabel->setText(QString("LEVEL: %1").arg(info.level));
  speedLabel->setText(QString("SPEED: %1").arg(info.speed));

  gameScene->clear();
  nextBlockScene->clear();

  for (int i = 0; i < FIELD_H; ++i) {
    for (int j = 0; j < FIELD_W; ++j) {
      QColor color = Qt::white;
      switch (info.field[i][j]) {
        case 1:
          color = Qt::green;
          break;
        case 2:
          color = Qt::blue;
          break;
        case 3:
          color = Qt::darkBlue;
          break;
      }

      QBrush brush(color);
      QPen pen(Qt::white);
      gameScene->addRect(j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE,
                         pen, brush);
    }
  }

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (info.next[i][j] != 0) {
        QBrush brush(Qt::green);
        QPen pen(Qt::white);
        nextBlockScene->addRect(j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE,
                                BLOCK_SIZE, pen, brush);
      }
    }
  }

  if (info.pause == GamePause) {
    QGraphicsRectItem *overlay =
        gameScene->addRect(0, 0, FIELD_W * BLOCK_SIZE, FIELD_H * BLOCK_SIZE);
    overlay->setBrush(QColor(0, 0, 0, 150));
    overlay->setPen(Qt::NoPen);

    QGraphicsTextItem *text = gameScene->addText("PAUSED");
    text->setDefaultTextColor(Qt::black);
    text->setFont(QFont("Arial", 24, QFont::Bold));
    text->setPos((FIELD_W * BLOCK_SIZE) / 2 - 70,
                 (FIELD_H * BLOCK_SIZE) / 2 - 20);
  }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Space:
    case Qt::Key_Return:
    case Qt::Key_P:
    case Qt::Key_Escape:
      handleUserInput(event->key());
      event->accept();
      return;
    default:
      break;
  }
  QMainWindow::keyPressEvent(event);
}

void MainWindow::handleUserInput(int input) {
  UserAction_t action = getSignal(input);

  if (action == Terminate) {
    controller->userInput(action, false);
    quitApp();
    return;
  }

  if (action == Pause) {
    if (gamePaused) {
      gamePaused = false;
    } else {
      gamePaused = true;
    }
  }

  controller->userInput(action, false);

  if (action == Start) {
    gameStarted = true;
    updateGUI();
  }
}

void MainWindow::quitApp() { QApplication::quit(); }

UserAction_t MainWindow::getSignal(int input) const {
  switch (input) {
    case Qt::Key_Up:
      return Up;
    case Qt::Key_Down:
      return Down;
    case Qt::Key_Left:
      return Left;
    case Qt::Key_Right:
      return Right;
    case Qt::Key_Space:
      return Action;
    case Qt::Key_Return:
      return Start;
    case Qt::Key_P:
      return Pause;
    case Qt::Key_Escape:
      return Terminate;
    default:
      return static_cast<UserAction_t>(-1);
  }
}