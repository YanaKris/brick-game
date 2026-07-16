#include "tetromino.h"

#include <initializer_list>
#include <random>
#include <utility>

namespace s21 {

namespace {

// Формы в координатах (row, col) матрицы 4×4 — в точности таблица
// из legacy tetrinoSet (tetris.c); паритет закреплён тестами.
Tetromino::Matrix MakeShape(std::initializer_list<std::pair<int, int>> cells) {
  Tetromino::Matrix matrix{};
  for (const auto& [row, col] : cells) matrix[row][col] = 1;
  return matrix;
}

}  // namespace

Tetromino::Matrix Tetromino::RotatedMatrix() const {
  Matrix rotated{};
  for (int i = 0; i < kTetrominoSize; ++i) {
    for (int j = 0; j < kTetrominoSize; ++j) {
      rotated[j][kTetrominoSize - 1 - i] = matrix_[i][j];
    }
  }
  return rotated;
}

BarTetromino::BarTetromino()
    : Tetromino(TetrominoType::kBar,
                MakeShape({{1, 0}, {1, 1}, {1, 2}, {1, 3}})) {}

CubeTetromino::CubeTetromino()
    : Tetromino(TetrominoType::kCube,
                MakeShape({{1, 1}, {2, 1}, {1, 2}, {2, 2}})) {}

STetromino::STetromino()
    : Tetromino(TetrominoType::kS,
                MakeShape({{2, 1}, {2, 2}, {1, 2}, {1, 3}})) {}

ZTetromino::ZTetromino()
    : Tetromino(TetrominoType::kZ,
                MakeShape({{1, 1}, {1, 2}, {2, 2}, {2, 3}})) {}

LTetromino::LTetromino()
    : Tetromino(TetrominoType::kL,
                MakeShape({{1, 1}, {2, 1}, {3, 1}, {3, 2}})) {}

JTetromino::JTetromino()
    : Tetromino(TetrominoType::kJ,
                MakeShape({{1, 2}, {2, 2}, {3, 2}, {3, 1}})) {}

TTetromino::TTetromino()
    : Tetromino(TetrominoType::kT,
                MakeShape({{1, 0}, {1, 1}, {1, 2}, {2, 1}})) {}

std::unique_ptr<Tetromino> TetrominoFactory::Create(TetrominoType type) {
  switch (type) {
    case TetrominoType::kBar:
      return std::make_unique<BarTetromino>();
    case TetrominoType::kCube:
      return std::make_unique<CubeTetromino>();
    case TetrominoType::kS:
      return std::make_unique<STetromino>();
    case TetrominoType::kZ:
      return std::make_unique<ZTetromino>();
    case TetrominoType::kL:
      return std::make_unique<LTetromino>();
    case TetrominoType::kJ:
      return std::make_unique<JTetromino>();
    case TetrominoType::kT:
      return std::make_unique<TTetromino>();
  }
  return nullptr;  // недостижимо: все типы разобраны выше
}

std::unique_ptr<Tetromino> TetrominoFactory::CreateRandom() {
  static std::mt19937 generator{std::random_device{}()};
  std::uniform_int_distribution<int> pick(0, kTetrominoTypeCount - 1);
  return Create(static_cast<TetrominoType>(pick(generator)));
}

}  // namespace s21
