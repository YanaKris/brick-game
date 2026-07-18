#ifndef S21_BRICK_GAME_COMMON_GAME_FACTORY_H_
#define S21_BRICK_GAME_COMMON_GAME_FACTORY_H_

#include <memory>

#include "../../interface.h"
#include "game_base.h"

namespace s21 {

class GameFactory {
 public:
  static std::unique_ptr<IGame> Make(CurrentGame game);
};

}  // namespace s21

#endif  // S21_BRICK_GAME_COMMON_GAME_FACTORY_H_
