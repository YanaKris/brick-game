#ifndef S21_API_GAME_API_H_
#define S21_API_GAME_API_H_

#include "../interface.h"

void selectGame(CurrentGame game);

int gameFinished();
int gameState();

GameInfo_t renderCurrentState();

#endif  // S21_API_GAME_API_H_
