#include "fsm.h"

namespace brickgame {

SnakeFSM::SnakeFSM()
    : currentState_(State_t::INITIAL),
      transitions_({{State_t::INITIAL, {State_t::SPAWN}},
                    {State_t::SPAWN, {State_t::MOVING}},
                    {State_t::MOVING,
                     {State_t::PAUSED, State_t::GAME_OVER, State_t::WIN}},
                    {State_t::PAUSED, {State_t::MOVING}},
                    {State_t::GAME_OVER, {State_t::INITIAL}},
                    {State_t::WIN, {State_t::INITIAL}}}) {}

SnakeFSM::State_t SnakeFSM::getState() const { return currentState_; }

bool SnakeFSM::canTransitTo(State_t newState) const {
  const auto it = transitions_.find(currentState_);
  if (it == transitions_.end()) return false;

  const auto& validTransitions = it->second;
  return std::find(validTransitions.cbegin(), validTransitions.cend(),
                   newState) != validTransitions.cend();
}

void SnakeFSM::transitTo(State_t newState) {
  if (canTransitTo(newState)) {
    currentState_ = newState;
  }
}

void SnakeFSM::initial() { transitTo(State_t::INITIAL); }
void SnakeFSM::spawn() { transitTo(State_t::SPAWN); }
void SnakeFSM::moving() { transitTo(State_t::MOVING); }
void SnakeFSM::paused() { transitTo(State_t::PAUSED); }
void SnakeFSM::gameOver() { transitTo(State_t::GAME_OVER); }
void SnakeFSM::win() { transitTo(State_t::WIN); }

}  // namespace brickgame