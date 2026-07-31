#include "game_api.h"

#include <memory>

#include "../brick_game/common/game_base.h"
#include "../brick_game/common/game_factory.h"

namespace {

std::unique_ptr<s21::IGame>& CurrentGamePtr() {
  static std::unique_ptr<s21::IGame> game = s21::GameFactory::Make(kTetris);
  return game;
}

}  // namespace

void selectGame(CurrentGame game) {
  CurrentGamePtr() = s21::GameFactory::Make(game);
}

void userInput(UserAction_t action, bool hold) {
  CurrentGamePtr()->userInput(action, hold);
}

GameInfo_t updateCurrentState() {
  return CurrentGamePtr()->updateCurrentState();
}

int gameFinished() { return CurrentGamePtr()->finished() ? 1 : 0; }

int gameState() { return static_cast<int>(CurrentGamePtr()->state()); }

GameInfo_t renderCurrentState() { return CurrentGamePtr()->render(); }
