#ifndef S21_BRICK_GAME_COMMON_GAME_BASE_H_
#define S21_BRICK_GAME_COMMON_GAME_BASE_H_

#include "../../interface.h"
#include "fsm.h"

namespace s21 {

class IGame {
 public:
  virtual ~IGame() = default;

  virtual void userInput(UserAction_t action, bool hold) = 0;
  virtual GameInfo_t updateCurrentState() = 0;
  virtual GameInfo_t render() = 0;
  virtual GameState state() const = 0;
  virtual bool finished() const = 0;
};

}  // namespace s21

#endif  // S21_BRICK_GAME_COMMON_GAME_BASE_H_
