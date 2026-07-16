#ifndef S21_BRICK_GAME_COMMON_FSM_H_
#define S21_BRICK_GAME_COMMON_FSM_H_

#include <initializer_list>
#include <vector>

namespace s21 {

// Игровые состояния из спецификации; каждая игра использует
// своё подмножество (змейке не нужны kShifting/kAttaching).
enum class GameState {
  kStart,
  kSpawn,
  kMoving,
  kShifting,
  kAttaching,
  kPause,
  kGameOver,
  kWin
};

// Табличный конечный автомат: переходы (from, event) -> to.
// Event — свой enum у каждой игры; таблицы переходов живут в *_game.cc.
template <typename Event>
class Fsm {
 public:
  struct Transition {
    GameState from;
    Event event;
    GameState to;
  };

  Fsm(GameState initial, std::initializer_list<Transition> table)
      : state_(initial), table_(table) {}

  GameState state() const { return state_; }

  // Возвращает true, если переход сработал; на неизвестную пару
  // (state, event) состояние не меняется.
  bool Dispatch(Event event) {
    for (const Transition& t : table_) {
      if (t.from == state_ && t.event == event) {
        state_ = t.to;
        return true;
      }
    }
    return false;
  }

 private:
  GameState state_;
  std::vector<Transition> table_;
};

}  // namespace s21

#endif  // S21_BRICK_GAME_COMMON_FSM_H_
