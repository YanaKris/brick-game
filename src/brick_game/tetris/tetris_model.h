#ifndef S21_BRICK_GAME_TETRIS_TETRIS_MODEL_H_
#define S21_BRICK_GAME_TETRIS_TETRIS_MODEL_H_

#include <array>
#include <memory>

#include "tetromino.h"

namespace s21 {

// Модель тетриса: чистая логика без UI (порт legacy tetris.c).
// Как и в legacy-коде, поле всегда содержит и осевшие блоки,
// и текущую (управляемую) фигуру.
class TetrisModel {
 public:
  static constexpr int kFieldHeight = 20;
  static constexpr int kFieldWidth = 10;
  static constexpr int kMaxLevel = 10;
  static constexpr int kScorePerLevel = 600;

  using Field = std::array<std::array<int, kFieldWidth>, kFieldHeight>;

  TetrisModel() = default;
  explicit TetrisModel(const Field& initial);

  // Спавн фигуры вверху поля (позиция row=-1/col=3, как в legacy).
  // false => фигура не помещается (game over); поле не меняется.
  bool Spawn(TetrominoType type);
  bool Spawn(std::unique_ptr<Tetromino> figure);
  bool SpawnRandom();

  // false => фигура легла (остаётся в поле, управление снимается).
  bool MoveDown();
  void MoveLeft();
  void MoveRight();
  void Rotate();

  // Удаляет заполненные строки, начисляет очки и пересчитывает
  // уровень; возвращает число удалённых строк.
  int ClearFullRows();

  // Таблица очков legacy: 1/2/3/4 строки -> 100/300/700/1500.
  static int ScoreFor(int rows);
  // Уровень = score / 600, потолок 10.
  static int LevelFor(int score);

  const Field& field() const { return field_; }
  int score() const { return score_; }
  int level() const { return level_; }
  bool has_active_figure() const { return current_ != nullptr; }

 private:
  bool Fits(const Tetromino::Matrix& matrix, int row, int col) const;
  void Draw(const Tetromino::Matrix& matrix, int row, int col, int value);
  bool TryMove(int d_row, int d_col);

  Field field_{};
  std::unique_ptr<Tetromino> current_;
  int score_ = 0;
  int level_ = 0;
};

}  // namespace s21

#endif  // S21_BRICK_GAME_TETRIS_TETRIS_MODEL_H_
