#include "snake_game.h"

#include <utility>

namespace s21 {

namespace {

// Таблица переходов FSM змейки (подмножество GameState).
Fsm<SnakeEvent> MakeSnakeFsm() {
  using S = GameState;
  using E = SnakeEvent;
  return Fsm<E>(S::kStart, {{S::kStart, E::kStartPressed, S::kMoving},
                            {S::kMoving, E::kPauseToggled, S::kPause},
                            {S::kPause, E::kPauseToggled, S::kMoving},
                            {S::kPause, E::kStartPressed, S::kMoving},
                            {S::kMoving, E::kCrashed, S::kGameOver},
                            {S::kMoving, E::kWon, S::kWin}});
}

}  // namespace

SnakeGame::SnakeGame() : SnakeGame(SnakeModel()) {}

SnakeGame::SnakeGame(SnakeModel model)
    : model_(std::move(model)), fsm_(MakeSnakeFsm()) {}

void SnakeGame::userInput(UserAction_t action, bool) {
  switch (action) {
    case Start:
      fsm_.Dispatch(SnakeEvent::kStartPressed);
      break;
    case Pause:
      fsm_.Dispatch(SnakeEvent::kPauseToggled);
      break;
    case Terminate:
      fsm_.Dispatch(SnakeEvent::kCrashed);
      break;
    case Action:
      accelerated_ = true;  // ускорение: ближайший тик отдаст speed/2
      break;
    case Left:
      HandleTurn(-1, 0);
      break;
    case Right:
      HandleTurn(1, 0);
      break;
    case Up:
      HandleTurn(0, -1);
      break;
    case Down:
      HandleTurn(0, 1);
      break;
    default:
      break;
  }
}

GameInfo_t SnakeGame::updateCurrentState() {
  Tick();
  return MakeInfo();
}

GameState SnakeGame::state() const { return fsm_.state(); }

bool SnakeGame::finished() const {
  return fsm_.state() == GameState::kGameOver ||
         fsm_.state() == GameState::kWin;
}

void SnakeGame::HandleTurn(int dx, int dy) {
  if (fsm_.state() != GameState::kMoving) return;
  if (dx == -dx_ && dy == -dy_) return;  // разворот на 180° запрещён
  dx_ = dx;
  dy_ = dy;
}

void SnakeGame::Tick() {
  if (fsm_.state() != GameState::kMoving) return;
  if (!model_.moveSnake(dx_, dy_)) {
    fsm_.Dispatch(SnakeEvent::kCrashed);
    return;
  }
  if (model_.isGameWon()) fsm_.Dispatch(SnakeEvent::kWon);
  if (model_.getEatenApples() > high_score_) {
    high_score_ = model_.getEatenApples();
  }
}

GameInfo_t SnakeGame::MakeInfo() {
  model_.Rasterize(field_.data());
  GameInfo_t info{};
  info.field = field_.data();
  info.next = next_.data();
  info.score = model_.getEatenApples();
  info.high_score = high_score_;
  info.level = SnakeModel::LevelFor(info.score);
  const int base_speed = SnakeModel::SpeedFor(info.level);
  info.speed = accelerated_ ? base_speed / 2 : base_speed;
  accelerated_ = false;
  info.pause = fsm_.state() == GameState::kPause ? 1 : 0;
  return info;
}

}  // namespace s21
