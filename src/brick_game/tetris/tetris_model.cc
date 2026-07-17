#include "tetris_model.h"

#include <utility>

namespace s21 {

TetrisModel::TetrisModel(const Field& initial) : field_(initial) {}

bool TetrisModel::Spawn(TetrominoType type) {
  return Spawn(TetrominoFactory::Create(type));
}

bool TetrisModel::SpawnRandom() {
  return Spawn(TetrominoFactory::CreateRandom());
}

bool TetrisModel::Spawn(std::unique_ptr<Tetromino> figure) {
  // Стартовая позиция из legacy spawnTetrino: фигура появляется
  // в строках 0..2, колонках 3..6 (row=-1 — матрица 4×4 «свисает»
  // над полем пустой нулевой строкой).
  constexpr int kSpawnRow = -1;
  constexpr int kSpawnCol = 3;
  if (!figure || !Fits(figure->matrix(), kSpawnRow, kSpawnCol)) {
    return false;
  }
  figure->SetPosition(kSpawnRow, kSpawnCol);
  Draw(figure->matrix(), kSpawnRow, kSpawnCol, 1);
  current_ = std::move(figure);
  return true;
}

bool TetrisModel::MoveDown() {
  if (!TryMove(1, 0)) {
    current_.reset();  // фигура легла — управление снимается
    return false;
  }
  return true;
}

void TetrisModel::MoveLeft() { TryMove(0, -1); }

void TetrisModel::MoveRight() { TryMove(0, 1); }

void TetrisModel::Rotate() {
  if (!current_) return;
  const Tetromino::Matrix rotated = current_->RotatedMatrix();
  Draw(current_->matrix(), current_->row(), current_->col(), 0);
  if (Fits(rotated, current_->row(), current_->col())) {
    current_->set_matrix(rotated);
  }
  Draw(current_->matrix(), current_->row(), current_->col(), 1);
}

int TetrisModel::ClearFullRows() {
  Field next{};
  int target = kFieldHeight - 1;
  int cleared = 0;
  for (int row = kFieldHeight - 1; row >= 0; --row) {
    bool full = true;
    for (int col = 0; col < kFieldWidth; ++col) {
      if (field_[row][col] == 0) full = false;
    }
    if (full) {
      ++cleared;
    } else {
      next[target--] = field_[row];
    }
  }
  field_ = next;
  score_ += ScoreFor(cleared);
  level_ = LevelFor(score_);
  return cleared;
}

int TetrisModel::ScoreFor(int rows) {
  switch (rows) {
    case 1:
      return 100;
    case 2:
      return 300;
    case 3:
      return 700;
    case 4:
      return 1500;
    default:
      return 0;
  }
}

int TetrisModel::LevelFor(int score) {
  const int level = score / kScorePerLevel;
  return level > kMaxLevel ? kMaxLevel : level;
}

bool TetrisModel::Fits(const Tetromino::Matrix& matrix, int row,
                       int col) const {
  for (int i = 0; i < kTetrominoSize; ++i) {
    for (int j = 0; j < kTetrominoSize; ++j) {
      if (matrix[i][j] == 0) continue;
      const int r = row + i;
      const int c = col + j;
      if (r < 0 || r >= kFieldHeight || c < 0 || c >= kFieldWidth) {
        return false;
      }
      if (field_[r][c] == 1) return false;
    }
  }
  return true;
}

void TetrisModel::Draw(const Tetromino::Matrix& matrix, int row, int col,
                       int value) {
  for (int i = 0; i < kTetrominoSize; ++i) {
    for (int j = 0; j < kTetrominoSize; ++j) {
      if (matrix[i][j] == 1) field_[row + i][col + j] = value;
    }
  }
}

// Пробное смещение текущей фигуры: стереть -> проверить -> нарисовать
// (эквивалент двойного буфера field/next из legacy-кода).
bool TetrisModel::TryMove(int d_row, int d_col) {
  if (!current_) return false;
  const int new_row = current_->row() + d_row;
  const int new_col = current_->col() + d_col;
  Draw(current_->matrix(), current_->row(), current_->col(), 0);
  const bool ok = Fits(current_->matrix(), new_row, new_col);
  if (ok) current_->SetPosition(new_row, new_col);
  Draw(current_->matrix(), current_->row(), current_->col(), 1);
  return ok;
}

}  // namespace s21
