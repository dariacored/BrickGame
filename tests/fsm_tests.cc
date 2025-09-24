#include "test_includes.h"

// =============================================================================
// FSM Tests - Testing all state transitions and edge cases
// =============================================================================

class SnakeFSMTest : public ::testing::Test {
 protected:
  void SetUp() override { fsm = std::make_unique<SnakeFSM>(); }
  std::unique_ptr<SnakeFSM> fsm;
};

TEST_F(SnakeFSMTest, InitialState) {
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::INITIAL);
}

TEST_F(SnakeFSMTest, TransitionsFromInitial) {
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::INITIAL);

  fsm->moving();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::INITIAL);

  fsm->paused();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::INITIAL);

  fsm->gameOver();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::INITIAL);

  fsm->win();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::INITIAL);

  fsm->spawn();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::SPAWN);
}

TEST_F(SnakeFSMTest, TransitionsFromSpawn) {
  fsm->spawn();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::SPAWN);

  fsm->initial();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::SPAWN);

  fsm->spawn();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::SPAWN);

  fsm->paused();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::SPAWN);

  fsm->gameOver();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::SPAWN);

  fsm->win();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::SPAWN);

  fsm->moving();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::MOVING);
}

TEST_F(SnakeFSMTest, TransitionsFromMoving) {
  fsm->spawn();
  fsm->moving();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::MOVING);

  fsm->paused();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::PAUSED);

  fsm->initial();
  fsm->spawn();
  fsm->moving();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::MOVING);

  fsm->gameOver();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::GAME_OVER);

  fsm->initial();
  fsm->spawn();
  fsm->moving();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::MOVING);

  fsm->win();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::WIN);

  fsm->initial();
  fsm->spawn();
  fsm->moving();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::MOVING);

  fsm->initial();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::MOVING);

  fsm->spawn();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::MOVING);

  fsm->moving();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::MOVING);
}

TEST_F(SnakeFSMTest, TransitionsFromPaused) {
  fsm->spawn();
  fsm->moving();
  fsm->paused();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::PAUSED);

  fsm->moving();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::MOVING);

  fsm->paused();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::PAUSED);

  fsm->initial();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::PAUSED);

  fsm->spawn();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::PAUSED);

  fsm->paused();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::PAUSED);

  fsm->gameOver();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::PAUSED);

  fsm->win();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::PAUSED);
}

TEST_F(SnakeFSMTest, TransitionsFromGameOver) {
  fsm->spawn();
  fsm->moving();
  fsm->gameOver();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::GAME_OVER);

  fsm->initial();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::INITIAL);

  fsm->spawn();
  fsm->moving();
  fsm->gameOver();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::GAME_OVER);

  fsm->spawn();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::GAME_OVER);

  fsm->moving();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::GAME_OVER);

  fsm->paused();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::GAME_OVER);

  fsm->gameOver();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::GAME_OVER);

  fsm->win();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::GAME_OVER);
}

TEST_F(SnakeFSMTest, TransitionsFromWin) {
  fsm->spawn();
  fsm->moving();
  fsm->win();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::WIN);

  fsm->initial();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::INITIAL);

  fsm->spawn();
  fsm->moving();
  fsm->win();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::WIN);

  fsm->spawn();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::WIN);

  fsm->moving();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::WIN);

  fsm->paused();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::WIN);

  fsm->gameOver();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::WIN);

  fsm->win();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::WIN);
}

TEST_F(SnakeFSMTest, CompleteGameFlow) {
  // Initial -> Spawn -> Moving -> Paused -> Moving -> Game Over -> Initial
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::INITIAL);

  fsm->spawn();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::SPAWN);

  fsm->moving();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::MOVING);

  fsm->paused();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::PAUSED);

  fsm->moving();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::MOVING);

  fsm->gameOver();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::GAME_OVER);

  fsm->initial();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::INITIAL);
}

TEST_F(SnakeFSMTest, WinningGameFlow) {
  // Initial -> Spawn -> Moving -> Win -> Initial
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::INITIAL);

  fsm->spawn();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::SPAWN);

  fsm->moving();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::MOVING);

  fsm->win();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::WIN);

  fsm->initial();
  EXPECT_EQ(fsm->getState(), SnakeFSM::State_t::INITIAL);
}