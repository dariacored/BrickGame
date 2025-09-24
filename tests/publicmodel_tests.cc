#include "test_includes.h"

// =============================================================================
// SnakeModel Tests - Testing SnakeModel through controller methods
// =============================================================================

class SnakePublicModelTest : public ::testing::Test {
 protected:
  void SetUp() override { controller = std::make_unique<brickgame::Controller>(); }
  void TearDown() override { controller.reset(); }
  std::unique_ptr<brickgame::Controller> controller;
};

TEST_F(SnakePublicModelTest, TestCollisionThroughGameOver) {
  controller->userInput(Start, false);

  for (int i = 0; i < 15; ++i) {
    controller->userInput(Up, false);
    controller->processTimer();
    auto info = controller->updateCurrentState();

    if (info.pause == GOTryAgain) {
      EXPECT_EQ(info.pause, GOTryAgain);
      controller->userInput(Start, false);
      EXPECT_EQ(info.pause, Empty);
    }
    controller->freeGameInfo(&info);
  }
}

TEST_F(SnakePublicModelTest, TestAppleGenerationThroughEating) {
  controller->userInput(Start, false);
  auto info = controller->updateCurrentState();

  int apple_count = 0;
  for (int i = 0; i < FIELD_H; ++i) {
    for (int j = 0; j < FIELD_W; ++j) {
      if (info.field[i][j] == 1) {
        apple_count++;
      }
    }
  }

  EXPECT_EQ(apple_count, 1);
  controller->freeGameInfo(&info);
}

TEST_F(SnakePublicModelTest, TestLevel) {
  controller->userInput(Start, false);
  auto info = controller->updateCurrentState();

  int initial_level = info.level;
  EXPECT_EQ(initial_level, 1);

  controller->freeGameInfo(&info);
}

TEST_F(SnakePublicModelTest, TestAccelerationThroughAction) {
  controller->userInput(Start, false);

  auto info = controller->updateCurrentState();
  int initial_speed = info.speed;
  controller->freeGameInfo(&info);

  controller->userInput(Action, false);

  info = controller->updateCurrentState();
  EXPECT_EQ(info.speed, initial_speed);
  controller->freeGameInfo(&info);
}

TEST_F(SnakePublicModelTest, TestPauseFunctionality) {
  controller->userInput(Start, false);
  auto info = controller->updateCurrentState();
  EXPECT_EQ(info.pause, Empty);
  controller->freeGameInfo(&info);

  controller->userInput(Pause, false);
  info = controller->updateCurrentState();
  EXPECT_EQ(info.pause, GamePause);
  controller->freeGameInfo(&info);

  controller->userInput(Pause, false);
  info = controller->updateCurrentState();
  EXPECT_EQ(info.pause, Empty);
  controller->freeGameInfo(&info);
}

TEST_F(SnakePublicModelTest, TestPauseStateTransitions) {
  auto info = controller->updateCurrentState();
  EXPECT_EQ(info.pause, StartMenu);
  controller->freeGameInfo(&info);

  controller->userInput(Start, false);
  info = controller->updateCurrentState();
  EXPECT_EQ(info.pause, Empty);
  controller->freeGameInfo(&info);

  controller->userInput(Pause, false);
  info = controller->updateCurrentState();
  EXPECT_EQ(info.pause, GamePause);
  controller->freeGameInfo(&info);

  controller->userInput(Pause, false);
  info = controller->updateCurrentState();
  EXPECT_EQ(info.pause, Empty);
  controller->freeGameInfo(&info);
}

TEST_F(SnakePublicModelTest, TestDirectionChangeLeft) {
  controller->userInput(Start, false);
  auto info = controller->updateCurrentState();

  int head_x = -1, head_y = -1;
  for (int i = 0; i < FIELD_H; ++i) {
    for (int j = 0; j < FIELD_W; ++j) {
      if (info.field[i][j] == 3) {
        head_x = i;
        head_y = j;
      }
    }
  }
  controller->freeGameInfo(&info);

  controller->userInput(Left, false);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  controller->processTimer();
  info = controller->updateCurrentState();

  int new_head_x = -1, new_head_y = -1;
  for (int i = 0; i < FIELD_H; ++i) {
    for (int j = 0; j < FIELD_W; ++j) {
      if (info.field[i][j] == 3) {
        new_head_x = i;
        new_head_y = j;
      }
    }
  }

  EXPECT_EQ(new_head_x, head_x);
  EXPECT_EQ(new_head_y, head_y - 1);
  controller->freeGameInfo(&info);
}

TEST_F(SnakePublicModelTest, TestDirectionChangeRight) {
  controller->userInput(Start, false);
  auto info = controller->updateCurrentState();

  int head_x = -1, head_y = -1;
  for (int i = 0; i < FIELD_H; ++i) {
    for (int j = 0; j < FIELD_W; ++j) {
      if (info.field[i][j] == 3) {
        head_x = i;
        head_y = j;
      }
    }
  }
  controller->freeGameInfo(&info);

  controller->userInput(Right, false);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  controller->processTimer();
  info = controller->updateCurrentState();

  int new_head_x = -1, new_head_y = -1;
  for (int i = 0; i < FIELD_H; ++i) {
    for (int j = 0; j < FIELD_W; ++j) {
      if (info.field[i][j] == 3) {
        new_head_x = i;
        new_head_y = j;
      }
    }
  }

  EXPECT_EQ(new_head_x, head_x);
  EXPECT_EQ(new_head_y, head_y + 1);
  controller->freeGameInfo(&info);
}

TEST_F(SnakePublicModelTest, TestDirectionChangeUp) {
  controller->userInput(Start, false);
  auto info = controller->updateCurrentState();

  int head_x = -1, head_y = -1;
  for (int i = 0; i < FIELD_H; ++i) {
    for (int j = 0; j < FIELD_W; ++j) {
      if (info.field[i][j] == 3) {
        head_x = i;
        head_y = j;
      }
    }
  }
  controller->freeGameInfo(&info);

  controller->userInput(Up, false);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  controller->processTimer();
  info = controller->updateCurrentState();

  int new_head_x = -1, new_head_y = -1;
  for (int i = 0; i < FIELD_H; ++i) {
    for (int j = 0; j < FIELD_W; ++j) {
      if (info.field[i][j] == 3) {
        new_head_x = i;
        new_head_y = j;
      }
    }
  }

  EXPECT_EQ(new_head_x, head_x - 1);
  EXPECT_EQ(new_head_y, head_y);
  controller->freeGameInfo(&info);
}

TEST_F(SnakePublicModelTest, TestDirectionChangeDownBlocked) {
  controller->userInput(Start, false);
  auto info = controller->updateCurrentState();

  int head_x = -1, head_y = -1;
  for (int i = 0; i < FIELD_H; ++i) {
    for (int j = 0; j < FIELD_W; ++j) {
      if (info.field[i][j] == 3) {
        head_x = i;
        head_y = j;
      }
    }
  }
  controller->freeGameInfo(&info);

  controller->userInput(Down, false);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  controller->processTimer();
  info = controller->updateCurrentState();

  int new_head_x = -1, new_head_y = -1;
  for (int i = 0; i < FIELD_H; ++i) {
    for (int j = 0; j < FIELD_W; ++j) {
      if (info.field[i][j] == 3) {
        new_head_x = i;
        new_head_y = j;
      }
    }
  }

  EXPECT_EQ(new_head_x, head_x - 1);
  EXPECT_EQ(new_head_y, head_y);
  controller->freeGameInfo(&info);
}