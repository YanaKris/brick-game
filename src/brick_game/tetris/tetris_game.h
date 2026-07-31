#ifndef S21_BRICK_GAME_TETRIS_TETRIS_GAME_H_
#define S21_BRICK_GAME_TETRIS_TETRIS_GAME_H_

#include <functional>
#include <memory>

#include "../common/field_buffer.h"
#include "../common/fsm.h"
#include "../common/game_base.h"
#include "tetris_model.h"

namespace s21 {

enum class TetrisEvent {
  kStartPressed,
  kSpawned,
  kSpawnBlocked,
  kLanded,
  kAttached,
  kPauseToggled
};

class TetrisGame : public IGame {
 public:
  using FigureGenerator = std::function<std::unique_ptr<Tetromino>()>;

  TetrisGame();
  explicit TetrisGame(FigureGenerator generator);
  TetrisGame(TetrisModel model, FigureGenerator generator);

  void userInput(UserAction_t action, bool hold) override;
  GameInfo_t updateCurrentState() override;
  GameInfo_t render() override;
  GameState state() const override;
  bool finished() const override;

 private:
  bool Moving() const;
  void Tick();
  void HandleLanding();
  void RefreshField();
  GameInfo_t MakeInfo();

  TetrisModel model_;
  FigureGenerator generator_;
  Fsm<TetrisEvent> fsm_;
  FieldBuffer field_;
  FieldBuffer next_;
};

}  // namespace s21

#endif  // S21_BRICK_GAME_TETRIS_TETRIS_GAME_H_
