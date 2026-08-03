#ifndef S21_BRICK_GAME_COMMON_HIGH_SCORE_H_
#define S21_BRICK_GAME_COMMON_HIGH_SCORE_H_

#include <string>

namespace s21 {

class HighScore {
 public:
  static int Load(const std::string& path);
  static void Save(const std::string& path, int value);
};

}  // namespace s21

#endif  // S21_BRICK_GAME_COMMON_HIGH_SCORE_H_
