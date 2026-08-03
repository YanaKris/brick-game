#include "game_factory.h"

#include "../snake/snake_game.h"
#include "../tetris/tetris_game.h"

namespace s21 {

std::unique_ptr<IGame> GameFactory::Make(CurrentGame game) {
  switch (game) {
    case kSnake:
      return std::make_unique<SnakeGame>();
    case kTetris:
    default:
      return std::make_unique<TetrisGame>();
  }
}

}  // namespace s21
