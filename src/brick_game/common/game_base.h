#ifndef S21_BRICK_GAME_COMMON_GAME_BASE_H_
#define S21_BRICK_GAME_COMMON_GAME_BASE_H_

#include "../../interface.h"
#include "fsm.h"

namespace s21 {

// Strategy-интерфейс игры: фронтенды держат std::unique_ptr<IGame>
// и не знают о конкретной игре (Snake/Tetris).
// Имена userInput/updateCurrentState повторяют публичный контракт
// interface.h — спецификация важнее стиля именования.
class IGame {
 public:
  virtual ~IGame() = default;

  virtual void userInput(UserAction_t action, bool hold) = 0;
  virtual GameInfo_t updateCurrentState() = 0;
  virtual GameState state() const = 0;
  virtual bool finished() const = 0;
};

}  // namespace s21

#endif  // S21_BRICK_GAME_COMMON_GAME_BASE_H_
