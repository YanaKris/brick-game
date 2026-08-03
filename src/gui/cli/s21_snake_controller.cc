#include "s21_snake_controller.h"

#include <iostream>

bool GameController::userInput(UserAction_t action, bool hold) {
  switch (action) {
    case Left:
      if (!hold && dx != 1) {
        // Обработка нажатия клавиши "Влево"
        dx = -1;
        dy = 0;
      }
      break;
    case Right:
      if (!hold && dx != -1) {
        // Обработка нажатия клавиши "Вправо"
        dx = 1;
        dy = 0;
      }
      break;
    case Up:
      if (!hold && dy != 1) {
        // Обработка нажатия клавиши "Вверх"
        dx = 0;
        dy = -1;
      }
      break;
    case Down:
      if (!hold && dy != -1) {
        // Обработка нажатия клавиши "Вниз"
        dx = 0;
        dy = 1;
      }
      break;
    case Action:
      break;
    case Pause:
      mvprintw(14, 6, "Game Paused");
      mvprintw(16, 6, "Press Enter");
      refresh();
      while (true) {
        int u_input = getch();
        if (u_input == ENTER) {
          break;
        }
      }
      // Очистить сообщение о паузе
      mvprintw(14, 6, "              ");
      mvprintw(14, 6, "              ");
      refresh();
      break;
    case Terminate:
      return false;
    default:
      break;
  }
  return true;
}
UserAction_t GameController::userKeyPress() {
  int input = getch();
  switch (input) {
    case KEY_LEFT:
      user_action = Left;
      break;
    case KEY_RIGHT:
      user_action = Right;
      break;
    case KEY_UP:
      user_action = Up;
      break;
    case KEY_DOWN:
      user_action = Down;
      break;
    case ESCAPE:
      user_action = Terminate;
      break;
    case ENTER:
      user_action = Start;
      break;
    case SPACE:
      user_action = Pause;
      break;
    case ERR:
      user_action = Action;  // нет нажатия - двигает в текущем направлении
      break;
    default:
      break;
  }
  return user_action;
}

void GameController::run() {
  initscr();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);

  int temp_speed = 350;

  timeout(temp_speed);  // использовать timeout для управления скоростью змейки

  UserAction_t user_action = Start;
  while (user_action != Start) {
    mvprintw(8, 1, INTRO_MESSAGE);
    user_action = userKeyPress();
    refresh();
    clear();
  }

  int high_score;
  view.loadHighScore(high_score);
  model.setHighScore(high_score);
  int prev_score = 0;

  while (true) {
    if (model.isGameWon()) {
      view.showWinScreen();
      break;
    }
    if (model.getEatenApples() % 5 == 0 &&
        model.getEatenApples() != prev_score) {
      model.increaseLevel();
      prev_score = model.getEatenApples();
    }
    view.showMap(model);

    UserAction_t user_action = userKeyPress();

    if (user_action != Action) {
      if (!userInput(user_action, false)) {
        break;  // закончить игру, если userInput вернул false
      }
    }

    if (model.moveSnake(dx, dy)) {
    } else {
      gameOver();  // вызовите функцию gameOver, если столкновение произошло
      break;
    }
    if (model.getEatenApples() > high_score) {
      high_score = model.getEatenApples();
      view.saveHighScore(high_score);
      model.setHighScore(high_score);
    }
  }

  endwin();
}

void GameController::gameOver() {
  clear();
  mvprintw(10, 10, "Game Over!");
  mvprintw(12, 5, "to exit press escape");
  refresh();
  while (true) {
    int u_input = getch();
    if (u_input == ESCAPE) {
      break;
    }
  }
}
