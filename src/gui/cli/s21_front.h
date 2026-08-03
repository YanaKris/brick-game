#ifndef S21_FRONT_H_
#define S21_FRONT_H_

#include <ncurses.h>

#include "../../brick_game/tetris/tetris.h"

int userKeyPress(UserAction_t action, bool hold);

// отрисовка игрового такта

void printField(GameInfo_t* game);

void startGame(GameInfo_t* game);

// рисуем в поле информации ожидаемую фигуру

void printNextTetrino(int fig);

// extern void tetrisloop();

#endif  // S21_FRONT_MAIN_H_
