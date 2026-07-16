#ifndef S21_SNAKE_CONTROLLER_H
#define S21_SNAKE_CONTROLLER_H

#include "../../brick_game/snake/s21_snake_model.h"
#include "s21_game_view_cli.h"

class GameController {
 public:
  bool userInput(UserAction_t action, bool hold);
  UserAction_t userKeyPress();
  void run();
  void gameOver();

 private:
  s21::GameModel model;
  GameView view;
  UserAction_t user_action;
  int dx = 1;
  int dy = 0;
};

#endif  // S21_SNAKE_CONTROLLER_H