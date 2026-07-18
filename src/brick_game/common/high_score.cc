#include "high_score.h"

#include <fstream>

namespace s21 {

int HighScore::Load(const std::string& path) {
  std::ifstream file(path);
  int value = 0;
  if (file >> value) return value;
  return 0;
}

void HighScore::Save(const std::string& path, int value) {
  std::ofstream file(path);
  if (file) file << value;
}

}  // namespace s21
