#include "tetris.h"

int tetrinoSet(Tetrino* figue, int fig) {
  int res = 0;
  // инициализируем матруцу 4х4, заполняем 0
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      figue->current_fig[i][j] = 0;
    }
  }
  // // заполняем в зависимости от кейса
  if (fig == BAR) {
    figue->current_fig[1][0] = 1;
    figue->current_fig[1][1] = 1;
    figue->current_fig[1][2] = 1;
    figue->current_fig[1][3] = 1;
  }
  if (fig == CUBE) {
    figue->current_fig[1][1] = 1;
    figue->current_fig[2][1] = 1;
    figue->current_fig[1][2] = 1;
    figue->current_fig[2][2] = 1;
  }
  if (fig == S_TYPE) {
    figue->current_fig[2][1] = 1;
    figue->current_fig[2][2] = 1;
    figue->current_fig[1][2] = 1;
    figue->current_fig[1][3] = 1;
  }
  if (fig == Z_TYPE) {
    figue->current_fig[1][1] = 1;
    figue->current_fig[1][2] = 1;
    figue->current_fig[2][2] = 1;
    figue->current_fig[2][3] = 1;
  }
  if (fig == L_TYPE) {
    figue->current_fig[1][1] = 1;
    figue->current_fig[2][1] = 1;
    figue->current_fig[3][1] = 1;
    figue->current_fig[3][2] = 1;
  }
  if (fig == J_TYPE) {
    figue->current_fig[1][2] = 1;
    figue->current_fig[2][2] = 1;
    figue->current_fig[3][2] = 1;
    figue->current_fig[3][1] = 1;
  }
  if (fig == T_TYPE) {
    figue->current_fig[1][0] = 1;
    figue->current_fig[1][1] = 1;
    figue->current_fig[1][2] = 1;
    figue->current_fig[2][1] = 1;
  }
  return res;
}

// функция помещает новую фигуру на поле

bool spawnTetrino(GameInfo_t* game, int fig, Tetrino* figue) {
  bool res = true;
  // вызываем фигуру под нужным номером
  tetrinoSet(figue, fig);
  // в следующее поле пишим текущее
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      game->next[i][j] = game->field[i][j];
    }
  }
  // проверка входит ли еще фигура на поле
  // если в текущем поле клетка занята - false
  for (int i = 1; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (game->next[i - 1][j + 3] == 1) {
        res = false;
      } else {
        // иначе помещаем в следующий вид поля фигуру
        game->next[i - 1][j + 3] = figue->current_fig[i][j];
      }
    }
  }
  // если входит вупускаем новую по заданным координатам
  if (res == true) {
    figue->i = -1;
    figue->j = 3;
    // следующий пишем в текущий
    replaceField(game);
  }
  return res;
}

// функция стирает "старое положение фигуры"

void replaceNext(GameInfo_t* game, Tetrino* figue) {
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      // если координаты фигуры не больше границ поля
      if (i >= figue->i && i <= figue->i + 3 && j >= figue->j &&
          j <= figue->j + 3) {
        // если
        if (figue->current_fig[i - figue->i][j - figue->j] ==
            1) {  // стираем старое положение
          game->next[i][j] = 0;
        } else {
          game->next[i][j] = game->field[i][j];
        }
      } else {
        game->next[i][j] = game->field[i][j];
      }
    }
  }
}

// функция записывает слудующий вид поля в текущий

void replaceField(GameInfo_t* game) {
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      game->field[i][j] = game->next[i][j];
    }
  }
}

bool userInputTet(GameInfo_t* game, int action, Tetrino* tetrino) {
  bool spawn = true;
  if (action == Action) {
    tetrinoRotate(game, tetrino);
  }
  if (action == Left) {
    tetrinoMoveLeft(game, tetrino);
  }
  if (action == Right) {
    tetrinoMoveRight(game, tetrino);
  }
  if (action == Down) {
    spawn = tetrinoMoveDown(game, tetrino);
  }
  return spawn;
}

void nextNull(GameInfo_t* game) {
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      game->next[i][j] = 0;
    }
  }
}

bool collisionTet(GameInfo_t* game, Tetrino* tetrino) {
  bool res = true;
  for (int yi = 0; yi < FIGUE_SIZE; yi++) {
    for (int xj = 0; xj < FIGUE_SIZE; xj++) {
      if ((tetrino->i + yi + 1) < 20 && (tetrino->j + xj) >= 0 &&
          (tetrino->j + xj) < 10 && tetrino->current_fig[yi][xj] == 1) {
        if (game->next[tetrino->i + yi + 1][tetrino->j + xj] == 1 &&
            yi + 1 > 3) {
          res = false;
        } else if (game->next[tetrino->i + yi + 1][tetrino->j + xj] == 1 &&
                   yi + 1 < FIGUE_SIZE) {
          if (tetrino->current_fig[yi + 1][xj] == 0) {
            res = false;
          }
        }
      } else if (tetrino->current_fig[yi][xj] == 1) {
        res = false;
      }
    }
  }
  return res;
}

bool tetrinoMoveDown(GameInfo_t* game, Tetrino* tetrino) {
  bool res = true;
  replaceNext(game, tetrino);
  res = collisionTet(game, tetrino);
  if (res) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (tetrino->current_fig[i][j] == 1) {
          game->next[tetrino->i + i + 1][tetrino->j + j] = 1;
        }
      }
    }
    replaceField(game);
    tetrino->i++;
  } else
    nextNull(game);

  return res;
}

void tetrinoMoveRight(GameInfo_t* game, Tetrino* tetrino) {
  bool res = true;
  replaceNext(game, tetrino);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if ((tetrino->j + j + 1) >= 0 && (tetrino->j + j + 1) < 10 &&
          tetrino->current_fig[i][j] == 1) {
        if (game->next[tetrino->i + i][tetrino->j + j + 1] == 1 && j + 1 > 3) {
          res = false;
        } else if (game->next[tetrino->i + i][tetrino->j + j + 1] == 1 &&
                   j + 1 < 4) {
          if (tetrino->current_fig[i][j + 1] == 0) {
            res = false;
          }
        }
      } else if (tetrino->current_fig[i][j] == 1) {
        res = false;
      }
    }
  }
  if (res) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (tetrino->current_fig[i][j] == 1) {
          game->next[tetrino->i + i][tetrino->j + j + 1] = 1;
        }
      }
    }
    replaceField(game);
    tetrino->j++;
  } else {
    nextNull(game);
  }
}

void tetrinoMoveLeft(GameInfo_t* game, Tetrino* tetrino) {
  bool check = true;
  replaceNext(game, tetrino);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if ((tetrino->j + j - 1) >= 0 && (tetrino->j + j - 1) < 10 &&
          tetrino->current_fig[i][j] == 1) {
        if (game->next[tetrino->i + i][tetrino->j + j - 1] == 1 && j - 1 < 0) {
          check = false;
        } else if (game->next[tetrino->i + i][tetrino->j + j - 1] == 1 &&
                   j - 1 >= 0) {
          if (tetrino->current_fig[i][j - 1] == 0) {
            check = false;
          }
        }
      } else if (tetrino->current_fig[i][j] == 1) {
        check = false;
      }
    }
  }
  if (check) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (tetrino->current_fig[i][j] == 1) {
          game->next[tetrino->i + i][tetrino->j + j - 1] = 1;
        }
      }
    }
    replaceField(game);
    tetrino->j--;
  } else {
    nextNull(game);
  }
}

void tetrinoRotate(GameInfo_t* game, Tetrino* tetrino) {
  int future_fig[4][4];
  bool res = true;
  // положим в переменнную future_fig положение фигуры после поворота
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      // (3 - 1) - чтобы фигура по кругу поворачивалась, а не в 2 положениях
      future_fig[j][3 - i] = tetrino->current_fig[i][j];
    }
  }
  replaceNext(game, tetrino);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (i + tetrino->i < 20 && j + tetrino->j < 10 && j + tetrino->j >= 0 &&
          i + tetrino->i >= 0) {
        if (game->next[i + tetrino->i][j + tetrino->j] == 1 &&
            future_fig[i][j] == 1) {
          res = false;
        }
      } else {
        if (future_fig[i][j] == 1) {
          res = false;
        }
      }
    }
  }
  if (res) {
    // кладем будующую фигуру в текущюю
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        tetrino->current_fig[i][j] = future_fig[i][j];
      }
    }
    // если в текущей координате 1 в будующее поле пишем 1
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (future_fig[i][j] == 1) {
          game->next[tetrino->i + i][tetrino->j + j] = 1;
        }
      }
    }
    replaceField(game);
  }
}

void deleteRows(GameInfo_t* game) {
  int count_rows = 0;
  int index[4];
  // идем по строке - если встретили 0 - строка не заполнена
  for (int i = 0; i < 20; i++) {
    bool check = true;
    for (int j = 0; j < 10; j++) {
      if (game->field[i][j] == 0) {
        check = false;
      }
      // else count_rows++;
    }
    if (check == true) {
      index[count_rows] = i;
      count_rows++;
    }
  }
  if (count_rows > 0) {
    for (int i = 0; i < 20; i++) {
      for (int j = 0; j < 10; j++) {
        game->next[i][j] = game->field[i][j];
      }
    }
    for (int x = 0; x < count_rows; x++) {
      for (int i = index[x]; i > 0; i--) {
        for (int j = 0; j < 10; j++) {
          game->next[i][j] = game->next[i - 1][j];
        }
      }
    }
    for (int i = 0; i < count_rows; i++) {
      for (int j = 0; j < 10; j++) {
        game->next[i][j] = 0;
      }
    }
    replaceField(game);
  }
  updateCurrentStateTet(game, count_rows);
}

// считаем уровни

GameInfo_t updateCurrentStateTet(GameInfo_t* game, int count_rows) {
  if (count_rows == 1) {
    game->score = game->score + 100;
  }
  if (count_rows == 2) {
    game->score = game->score + 300;
  }
  if (count_rows == 3) {
    game->score = game->score + 700;
  }
  if (count_rows == 4) {
    game->score = game->score + 1500;
  }
  game->level = game->score / 600;
  if (game->level > 10) {
    game->level = 10;
  }
  // изменение переменной game->speed в зависимости от уровня
  count_rows = 0;
  return *game;
}

// for debag
// рисуем поле из 0
void printFieldForTest(GameInfo_t* game) {
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      printf("%d ", game->field[i][j]);
    }
    printf("\n");
  }
}

void printFieldForTestNext(GameInfo_t* game) {
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      printf("%d ", game->next[i][j]);
    }
    printf("\n");
  }
}