#include <ncurses.h>

#include "../../brick_game/common/game_factory.h"
#include "cli_controller.h"

namespace {

void RunMenu() {
  while (true) {
    timeout(-1); 
    erase();
    mvprintw(6, 10, "1. TETRIS");
    mvprintw(8, 10, "2. SNAKE");
    mvprintw(10, 10, "3. EXIT");
    refresh();
    switch (getch()) {
      case '1':
        s21::CliController(s21::GameFactory::Make(kTetris), "record.txt").Run();
        break;
      case '2':
        s21::CliController(s21::GameFactory::Make(kSnake),
                           "high_score_snake.txt")
            .Run();
        break;
      case '3':
        return;
      default:
        break;
    }
  }
}

}  // namespace

int main() {
  initscr();
  keypad(stdscr, true);
  noecho();
  curs_set(0);
  RunMenu();
  endwin();
  return 0;
}
