#ifndef S21_BRICK_GAME_TETRIS_TETROMINO_H_
#define S21_BRICK_GAME_TETRIS_TETROMINO_H_

#include <array>
#include <memory>

namespace s21 {

// Порядок повторяет TetrinosTypes из legacy tetris.h —
// на этом держатся тесты паритета.
enum class TetrominoType { kBar, kCube, kS, kZ, kL, kJ, kT };

inline constexpr int kTetrominoTypeCount = 7;
inline constexpr int kTetrominoSize = 4;

// Базовая фигура: матрица 4×4 + позиция на поле. row может быть
// отрицательным сразу после спавна — как в legacy-коде.
class Tetromino {
 public:
  using Matrix = std::array<std::array<int, kTetrominoSize>, kTetrominoSize>;

  virtual ~Tetromino() = default;

  TetrominoType type() const { return type_; }
  const Matrix& matrix() const { return matrix_; }
  void set_matrix(const Matrix& matrix) { matrix_ = matrix; }

  // Матрица после поворота на 90°: rotated[j][3-i] = cur[i][j]
  // (формула из legacy tetrinoRotate).
  Matrix RotatedMatrix() const;

  int row() const { return row_; }
  int col() const { return col_; }
  void SetPosition(int row, int col) {
    row_ = row;
    col_ = col;
  }

 protected:
  Tetromino(TetrominoType type, const Matrix& shape)
      : type_(type), matrix_(shape) {}

 private:
  TetrominoType type_;
  Matrix matrix_;
  int row_ = 0;
  int col_ = 0;
};

// Иерархия фигур: каждый подкласс задаёт свою форму в конструкторе.
class BarTetromino : public Tetromino {
 public:
  BarTetromino();
};

class CubeTetromino : public Tetromino {
 public:
  CubeTetromino();
};

class STetromino : public Tetromino {
 public:
  STetromino();
};

class ZTetromino : public Tetromino {
 public:
  ZTetromino();
};

class LTetromino : public Tetromino {
 public:
  LTetromino();
};

class JTetromino : public Tetromino {
 public:
  JTetromino();
};

class TTetromino : public Tetromino {
 public:
  TTetromino();
};

// Factory: изолирует код от конкретных классов фигур
// (замена цепочки if (fig == ...) из legacy tetrinoSet).
class TetrominoFactory {
 public:
  static std::unique_ptr<Tetromino> Create(TetrominoType type);
  static std::unique_ptr<Tetromino> CreateRandom();
};

}  // namespace s21

#endif  // S21_BRICK_GAME_TETRIS_TETROMINO_H_
