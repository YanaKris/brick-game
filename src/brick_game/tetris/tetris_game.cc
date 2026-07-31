#include "tetris_game.h"

#include <utility>

namespace s21 {

namespace {

constexpr int kBaseSpeedMs = 300;

Fsm<TetrisEvent> MakeTetrisFsm() {
  using S = GameState;
  using E = TetrisEvent;
  return Fsm<E>(S::kStart, {{S::kStart, E::kStartPressed, S::kSpawn},
                            {S::kSpawn, E::kSpawned, S::kMoving},
                            {S::kSpawn, E::kSpawnBlocked, S::kGameOver},
                            {S::kMoving, E::kLanded, S::kAttaching},
                            {S::kAttaching, E::kAttached, S::kSpawn},
                            {S::kMoving, E::kPauseToggled, S::kPause},
                            {S::kPause, E::kPauseToggled, S::kMoving}});
}

}  // namespace

TetrisGame::TetrisGame()
    : TetrisGame(TetrisModel(), &TetrominoFactory::CreateRandom) {}

TetrisGame::TetrisGame(FigureGenerator generator)
    : TetrisGame(TetrisModel(), std::move(generator)) {}

TetrisGame::TetrisGame(TetrisModel model, FigureGenerator generator)
    : model_(std::move(model)),
      generator_(std::move(generator)),
      fsm_(MakeTetrisFsm()) {
  next_figure_ = generator_();
}

void TetrisGame::userInput(UserAction_t action, bool) {
  switch (action) {
    case Start:
      fsm_.Dispatch(TetrisEvent::kStartPressed);
      break;
    case Pause:
      fsm_.Dispatch(TetrisEvent::kPauseToggled);
      break;
    case Left:
      if (Moving()) model_.MoveLeft();
      break;
    case Right:
      if (Moving()) model_.MoveRight();
      break;
    case Action:
      if (Moving()) model_.Rotate();
      break;
    case Down:
      if (Moving() && !model_.MoveDown()) HandleLanding();
      break;
    default:
      break;
  }

  RefreshField();
}

GameInfo_t TetrisGame::updateCurrentState() {
  Tick();
  return MakeInfo();
}

GameInfo_t TetrisGame::render() { return MakeInfo(); }

GameState TetrisGame::state() const { return fsm_.state(); }

bool TetrisGame::finished() const {
  return fsm_.state() == GameState::kGameOver;
}

bool TetrisGame::Moving() const { return fsm_.state() == GameState::kMoving; }

void TetrisGame::Tick() {
  if (fsm_.state() == GameState::kSpawn) {
    if (model_.Spawn(std::move(next_figure_))) {
      fsm_.Dispatch(TetrisEvent::kSpawned);
    } else {
      fsm_.Dispatch(TetrisEvent::kSpawnBlocked);
    }
    next_figure_ = generator_();
    return;
  }
  if (!Moving()) return;
  if (!model_.MoveDown()) HandleLanding();
}

void TetrisGame::HandleLanding() {
  fsm_.Dispatch(TetrisEvent::kLanded);
  model_.ClearFullRows();
  fsm_.Dispatch(TetrisEvent::kAttached);
}

void TetrisGame::RefreshField() {
  const TetrisModel::Field& cells = model_.field();
  for (int r = 0; r < FieldBuffer::kHeight; ++r) {
    for (int c = 0; c < FieldBuffer::kWidth; ++c) {
      field_.data()[r][c] = cells[r][c];
    }
  }
}

void TetrisGame::RefreshNext() {
  next_.Clear();
  if (next_figure_ == nullptr) return;
  const Tetromino::Matrix& shape = next_figure_->matrix();
  for (int r = 0; r < kTetrominoSize; ++r) {
    for (int c = 0; c < kTetrominoSize; ++c) {
      if (shape[r][c]) next_.data()[r][c] = shape[r][c];
    }
  }
}

GameInfo_t TetrisGame::MakeInfo() {
  RefreshField();
  RefreshNext();
  GameInfo_t info{};
  info.field = field_.data();
  info.next = next_.data();
  info.score = model_.score();
  info.high_score = 0;
  info.level = model_.level();
  info.speed = kBaseSpeedMs;
  info.pause = fsm_.state() == GameState::kPause ? 1 : 0;
  return info;
}

}  // namespace s21
