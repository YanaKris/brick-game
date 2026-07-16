#include "s21_game_view_cli.h"

#include <vector>
using namespace std;

void GameView::showMap(const s21::GameModel& model) {
  clear();
  mvprintw(1, 15, "SCORE");
  move(2, 17);
  printw("%d", model.getEatenApples());
  mvprintw(4, 14, "RECORD");
  move(5, 17);
  printw("%d", model.getHighScore());
  mvprintw(7, 15, "LEVEL");
  move(8, 17);
  printw("%d", model.getLevel());
  mvprintw(9, 15, "SPEED");
  move(10, 17);
  printw("%d", model.getLevel());
  mvprintw(11, 15, "PAUSE");
  mvprintw(12, 15, "-----");
  mvprintw(13, 14, "|SPACE|");
  mvprintw(14, 15, "-----");

  // mvprintw(25, 1, model.printDebugInfo().c_str());

  // Рисуем границы
  drawBorders();

  // Рисуем яблоко
  move(model.getApple().y, model.getApple().x);
  printw("@");

  // Рисуем змейку
  for (const auto& segment : model.getTail()) {
    move(segment.y, segment.x);
    printw("#");
  }

  move(model.getHead().y, model.getHead().x);
  printw("$");

  refresh();
}

void GameView::drawBorders() {
  mvaddch(0, 0, ACS_ULCORNER);
  mvaddch(0, FIELD_WIDTH + 1, ACS_URCORNER);
  mvaddch(FIELD_HEIGHT + 1, 0, ACS_LLCORNER);
  mvaddch(FIELD_HEIGHT + 1, FIELD_WIDTH + 1, ACS_LRCORNER);

  for (int i = 0; i < FIELD_WIDTH; i++) {
    mvaddch(0, i + 1, ACS_HLINE);
    mvaddch(FIELD_HEIGHT + 1, i + 1, ACS_HLINE);
  }

  for (int i = 0; i < FIELD_HEIGHT; i++) {
    mvaddch(i + 1, 0, ACS_VLINE);
    mvaddch(i + 1, FIELD_WIDTH + 1, ACS_VLINE);
  }
  // поле инфы
  mvaddch(0, 13, ACS_ULCORNER);
  mvaddch(0, FIELD_WIDTH + 11, ACS_URCORNER);
  mvaddch(FIELD_HEIGHT + 1, 13, ACS_LLCORNER);
  mvaddch(FIELD_HEIGHT + 1, FIELD_WIDTH + 11, ACS_LRCORNER);
  for (int i = 0; i < FIELD_WIDTH - 3; i++) {
    mvaddch(0, i + 14, ACS_HLINE);
    mvaddch(FIELD_HEIGHT + 1, i + 14, ACS_HLINE);
  }
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    mvaddch(i + 1, 13, ACS_VLINE);
    mvaddch(i + 1, 21, ACS_VLINE);
  }
}

void GameView::loadHighScore(int& high_score) {
  FILE* file = fopen("high_score_snake.txt", "r");
  if (file != NULL) {
    fscanf(file, "%d", &high_score);
    fclose(file);
  } else {
    high_score = 0;
  }
}

void GameView::saveHighScore(int high_score) {
  FILE* file = fopen("high_score_snake.txt", "w");
  if (file != NULL) {
    fprintf(file, "%d", high_score);
    fclose(file);
  }
}

void GameView::showWinScreen() const {
  clear();
  mvprintw(10, 3, "Congratulations! You win!");
  mvprintw(12, 5, "to exit press escape");
  while (true) {
    int u_input = getch();
    if (u_input == ESCAPE) {
      break;
    }
  }
  refresh();
  getch();
}