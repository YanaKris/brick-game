#ifndef S21_API_GAME_API_H_
#define S21_API_GAME_API_H_

#include "../interface.h"

// Выбирает активную игру для свободных функций userInput/updateCurrentState
// (единый API из interface.h). Пересоздаёт игру с чистого состояния.
void selectGame(CurrentGame game);

#endif  // S21_API_GAME_API_H_
