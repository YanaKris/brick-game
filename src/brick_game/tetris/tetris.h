#include <math.h>
#include <unistd.h>

#include "../../interface.h"

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 20
#define ENTER 10
#define UP_SLASH 47  // поворот - символ "/"
#define ESCAPE 27
#define SPACE 32

#define INTRO_MESSAGE "Press ENTER to start!"

typedef enum {
  BAR,
  CUBE,
  S_TYPE,
  Z_TYPE,
  L_TYPE,
  J_TYPE,
  T_TYPE
} TetrinosTypes;

// структура описывающая падающую фигуру

typedef struct {
  int i;  // y
  int j;  // x
  int current_fig[4][4];
} Tetrino;

bool userInputTet(GameInfo_t* game, int action, Tetrino* figue);

bool spawnTetrino(GameInfo_t* game, int fig, Tetrino* figue);

int tetrinoSet(Tetrino* figue, int fig);

GameInfo_t updateCurrentStateTet(GameInfo_t* game, int count_rows);

void replaceField(GameInfo_t* game);

void replaceNext(GameInfo_t* game, Tetrino* figue);

int addTetrino(Tetrino* tetrino, GameInfo_t* game);

int deleteTetrino(Tetrino* tetrino, GameInfo_t* game);

void nextNull(GameInfo_t* game);

bool collisionTet(GameInfo_t* game, Tetrino* tetrino);

// функции движения фигуры

void tetrinoRotate(GameInfo_t* game, Tetrino* tetrino);

bool tetrinoMoveDown(GameInfo_t* game, Tetrino* tetrino);

void tetrinoMoveRight(GameInfo_t* game, Tetrino* tetrino);

void tetrinoMoveLeft(GameInfo_t* game, Tetrino* tetrino);

// удаляем заполненные строки и считаем очки

void deleteRows(GameInfo_t*);

// временно

void printFieldForTest(GameInfo_t* game);
