#include "game_api.h"

#include <memory>

#include "../brick_game/common/game_base.h"
#include "../brick_game/snake/snake_game.h"
#include "../brick_game/tetris/tetris_game.h"

namespace {

std::unique_ptr<s21::IGame> MakeGame(CurrentGame game) {
  switch (game) {
    case kSnake:
      return std::make_unique<s21::SnakeGame>();
    case kTetris:
    default:
      return std::make_unique<s21::TetrisGame>();
  }
}

std::unique_ptr<s21::IGame>& CurrentGamePtr() {
  static std::unique_ptr<s21::IGame> game = MakeGame(kTetris);
  return game;
}

}  // namespace

void selectGame(CurrentGame game) { CurrentGamePtr() = MakeGame(game); }

void userInput(UserAction_t action, bool hold) {
  CurrentGamePtr()->userInput(action, hold);
}

GameInfo_t updateCurrentState() {
  return CurrentGamePtr()->updateCurrentState();
}
