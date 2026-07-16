#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// в идеале больше ничего не добавляю
#define FIELD_WIDTH 10
#define FIELD_HEIGHT 20
#define FIGUE_SIZE 4

#define CELL_SIZE 20

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int** field;
  int** next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

enum CurrentGame { kTetris = 0, kSnake = 1 };

void userInput(UserAction_t action, bool hold);

GameInfo_t updateCurrentState();

#endif  // INTERFACE_H