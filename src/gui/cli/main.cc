extern "C" {
#include "s21_front.h"
}

#include <ncurses.h>

#include "s21_snake_controller.h"

void tetrisloop() {
  GameInfo_t tetrisGame;
  clear();
  tetrisGame.field = new int*[20];
  for (int i = 0; i < 20; i++) {
    tetrisGame.field[i] = new int[10];
    for (int j = 0; j < 10; j++) {
      tetrisGame.field[i][j] = 0;
    }
  }
  tetrisGame.next = new int*[20];
  for (int i = 0; i < 20; i++) {
    tetrisGame.next[i] = new int[10];
    for (int j = 0; j < 10; j++) {
      tetrisGame.next[i][j] = 0;
    }
  }
  startGame(&tetrisGame);
  // освободить память
  for (int i = 0; i < 20; i++) {
    delete[] tetrisGame.next[i];
    delete[] tetrisGame.field[i];
  }
  delete[] tetrisGame.field;
  delete[] tetrisGame.next;
}

void gameLoop() {
  int choice;
  while (true) {
    clear();
    mvprintw(6, 10, "1. TETRIS");
    mvprintw(8, 10, "2. SNAKE");
    mvprintw(10, 10, "3. EXIT");
    refresh();
    choice = getch();
    switch (choice) {
      case '1':
        tetrisloop();
        break;
      case '2': {
        clear();
        GameController controller;
        controller.run();
        break;
      }
      case '3':
        return;
      default:
        break;
    }
  }
}

int main() {
  // инициализировать ncurses
  initscr();
  keypad(stdscr, true);
  noecho();
  curs_set(0);

  gameLoop();

  // выйти из ncurses
  endwin();
  return 0;
}