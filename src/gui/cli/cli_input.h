#ifndef S21_GUI_CLI_CLI_INPUT_H_
#define S21_GUI_CLI_CLI_INPUT_H_

#include <optional>

#include "../../interface.h"

namespace s21 {
class CliInput {
 public:
  std::optional<UserAction_t> Poll() const;
};

}  // namespace s21

#endif  // S21_GUI_CLI_CLI_INPUT_H_
