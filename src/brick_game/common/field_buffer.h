#ifndef S21_BRICK_GAME_COMMON_FIELD_BUFFER_H_
#define S21_BRICK_GAME_COMMON_FIELD_BUFFER_H_

#include <array>

namespace s21 {

// Владеющий буфер поля 20×10 под GameInfo_t: спецификация требует
// int**, RAII-обёртка избавляет фасады от ручных new/delete.
// Некопируемый: указатели строк смотрят внутрь объекта.
class FieldBuffer {
 public:
  static constexpr int kHeight = 20;
  static constexpr int kWidth = 10;

  FieldBuffer() {
    for (int r = 0; r < kHeight; ++r) rows_[r] = cells_[r].data();
  }
  FieldBuffer(const FieldBuffer&) = delete;
  FieldBuffer& operator=(const FieldBuffer&) = delete;

  int** data() { return rows_.data(); }

  void Clear() {
    for (auto& row : cells_) row.fill(0);
  }

 private:
  std::array<std::array<int, kWidth>, kHeight> cells_{};
  std::array<int*, kHeight> rows_{};
};

}  // namespace s21

#endif  // S21_BRICK_GAME_COMMON_FIELD_BUFFER_H_
