#include "cli_input.h"

#include <ncurses.h>

namespace s21 {

namespace {
constexpr int kEnter = 10;
constexpr int kEscape = 27;
constexpr int kSpace = 32;
constexpr int kSlash = 47;
}  // namespace

std::optional<UserAction_t> CliInput::Poll() const {
  switch (getch()) {
    case KEY_LEFT:
      return Left;
    case KEY_RIGHT:
      return Right;
    case KEY_UP:
      return Up;
    case KEY_DOWN:
      return Down;
    case kEnter:
      return Start;
    case kSpace:
      return Pause;
    case kSlash:
      return Action;
    case kEscape:
      return Terminate;
    default:
      return std::nullopt;
  }
}

}  // namespace s21
