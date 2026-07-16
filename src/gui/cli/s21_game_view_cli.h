#ifndef S21_GAME_VIEW_CLI_H
#define S21_GAME_VIEW_CLI_H

#include <ncurses.h>

#include <vector>

#include "../../brick_game/snake/s21_snake_model.h"

#define ENTER 10
#define ESCAPE 27
#define SPACE 32

#define INTRO_MESSAGE "Press ENTER to start!"

class GameView {
 public:
  void showMap(const s21::GameModel& model);
  void loadHighScore(int& high_score);
  void saveHighScore(int high_score);
  void showWinScreen() const;

 private:
  void drawBorders();
};

#endif  // S21_GAME_VIEW_CLI_H