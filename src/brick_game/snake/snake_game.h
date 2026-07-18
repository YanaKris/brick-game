#ifndef S21_BRICK_GAME_SNAKE_SNAKE_GAME_H_
#define S21_BRICK_GAME_SNAKE_SNAKE_GAME_H_

#include "../common/field_buffer.h"
#include "../common/fsm.h"
#include "../common/game_base.h"
#include "s21_snake_model.h"

namespace s21 {

// События FSM змейки; таблица переходов — в snake_game.cc.
enum class SnakeEvent { kStartPressed, kPauseToggled, kCrashed, kWon };

// Фасад IGame над SnakeModel + FSM: держит направление движения,
// паузу, ускорение по Action и собирает GameInfo_t для фронтендов.
class SnakeGame : public IGame {
 public:
  SnakeGame();
  explicit SnakeGame(SnakeModel model);

  void userInput(UserAction_t action, bool hold) override;
  GameInfo_t updateCurrentState() override;
  GameState state() const override;
  bool finished() const override;

 private:
  void HandleTurn(int dx, int dy);
  void Tick();
  GameInfo_t MakeInfo();

  SnakeModel model_;
  Fsm<SnakeEvent> fsm_;
  FieldBuffer field_;
  FieldBuffer next_;  // змейке не нужен, но контракт требует валидный next
  int dx_ = 1;
  int dy_ = 0;
  // Направление последнего выполненного хода: развороты проверяются
  // против него, иначе два ввода между тиками дают разворот на 180°.
  int moved_dx_ = 1;
  int moved_dy_ = 0;
  bool accelerated_ = false;
  int high_score_ = 0;
};

}  // namespace s21

#endif  // S21_BRICK_GAME_SNAKE_SNAKE_GAME_H_
