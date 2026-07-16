#include "s21_front.h"

int userKeyPress(UserAction_t action, bool hold) {
  action = Down;
  int inp = getch();
  hold = true;

  if (inp == UP_SLASH && hold == true)
    action = Action;
  else if (inp == KEY_DOWN)
    action = Down;
  else if (inp == KEY_LEFT)
    action = Left;
  else if (inp == KEY_RIGHT)
    action = Right;
  else if (inp == ESCAPE)
    action = Terminate;
  else if (inp == ENTER)
    action = Start;
  else if (inp == SPACE)
    action = Pause;
  else if (inp == ERR)  // нет нажатия - двигает вниз
    action = Down;

  return action;
}

void startGame(GameInfo_t* game) {
  bool hold = false;
  UserAction_t t = Start;

  initscr();             // запуск библиотеки режим курсоров
  keypad(stdscr, true);  // разрешен ввод стрелок и тд
  noecho();
  curs_set(0);  // курсор убран
  mvprintw(8, 1, INTRO_MESSAGE);

  srand(time(NULL));

  int action = userKeyPress(t, hold);
  // пока не нажат enter - принтуем INTRO_MESSAGE
  while (action != Start) {
    mvprintw(8, 1, INTRO_MESSAGE);
    action = userKeyPress(t, hold);
    refresh();
    clear();
  }

  clear();
  bool work = true;

  bool spawn = false;
  Tetrino tetrino;
  int temp_speed = 25;
  game->score = 0;
  game->level = 0;
  game->high_score = 0;
  FILE* file = fopen("record.txt", "r");
  fscanf(file, "%d",
         &game->high_score);  // присваиваем переменной число лежащее в файле
  fclose(file);

  int currentFig = rand() % 7;

  while (work) {
    if (spawn == false) {
      int fig = rand() % 7;
      work = spawnTetrino(game, currentFig, &tetrino);
      currentFig = (++fig) % 7;
      spawn = true;
    }

    printNextTetrino(currentFig);
    mvprintw(10, 15, "SPEED");
    mvprintw(11, 17, "%d", temp_speed);

    printField(game);
    // функция halfdelay ждет пользовательский ввод заданное время (в
    // миллисекундах) если в заданное время ввода не последовало - возвращается
    // ERR чем меньше число тем выше скорость
    halfdelay(temp_speed);

    action = userKeyPress(t, hold);
    // пауза
    if (action == Terminate) {
      break;
    } else if (action == Pause) {
      temp_speed = 255;  // maks число, которое может принять halfdelay
    } else if (action == Start) {
      temp_speed = 25;
    }

    spawn = userInputTet(game, action, &tetrino);
    if (game->level == 1) {
      temp_speed = 23;
    } else if (game->level == 2) {
      temp_speed = 21;
    } else if (game->level == 3) {
      temp_speed = 19;
    } else if (game->level == 4) {
      temp_speed = 17;
    } else if (game->level == 5) {
      temp_speed = 15;
    } else if (game->level == 6) {
      temp_speed = 13;
    } else if (game->level == 7) {
      temp_speed = 11;
    } else if (game->level == 8) {
      temp_speed = 9;
    } else if (game->level == 9) {
      temp_speed = 7;
    } else if (game->level == 10) {
      temp_speed = 5;
    }

    printField(game);
    if (spawn == false) {
      deleteRows(game);
    }
    refresh();
    clear();
    printField(game);
  }
  refresh();
  clear();
  mvprintw(8, 10, "GAME OVER");
  mvprintw(10, 10, "SCORE %d", game->score);
  if (game->score > game->high_score) {
    mvprintw(12, 10, "NEW RECORD");
    if ((file = fopen("record.txt", "w")) != NULL) {
      fprintf(file, "%d", game->score);
    }
  }
  getch();
  endwin();
}

// рисуем
void printField(GameInfo_t* game) {
  mvprintw(1, 15, "SCORE");
  mvprintw(2, 17, "%d", game->score);
  mvprintw(4, 14, "RECORD");
  mvprintw(5, 15, "%d", game->high_score);
  mvprintw(7, 15, "LEVEL");
  mvprintw(8, 17, "%d", game->level);
  mvprintw(15, 15, "NEXT!");

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
  // поле тетриса
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

  for (int i = 1; i < FIELD_HEIGHT + 1; i++) {
    for (int j = 1; j < FIELD_WIDTH + 1; j++) {
      if (game->field[i - 1][j - 1] == 1) {
        mvaddch(i, j, ACS_BLOCK);  // кубик: mvaddch(i, j, ' ' | A_REVERSE)
      } else {
        mvprintw(i, j, " ");
      }
    }
  }
}

void printNextTetrino(int fig) {
  if ((fig % 7) == BAR) {
    mvprintw(17, 15, "#");
    mvprintw(17, 16, "#");
    mvprintw(17, 17, "#");
    mvprintw(17, 18, "#");
  } else if ((fig % 7) == CUBE) {
    mvprintw(17, 16, "#");
    mvprintw(17, 17, "#");
    mvprintw(18, 16, "#");
    mvprintw(18, 17, "#");
  } else if ((fig % 7) == S_TYPE) {
    mvprintw(18, 16, "#");
    mvprintw(18, 17, "#");
    mvprintw(17, 17, "#");
    mvprintw(17, 18, "#");
  } else if ((fig % 7) == Z_TYPE) {
    mvprintw(17, 16, "#");
    mvprintw(17, 17, "#");
    mvprintw(18, 17, "#");
    mvprintw(18, 18, "#");
  } else if ((fig % 7) == L_TYPE) {
    mvprintw(17, 17, "#");
    mvprintw(18, 17, "#");
    mvprintw(19, 17, "#");
    mvprintw(19, 18, "#");
  } else if ((fig % 7) == J_TYPE) {
    mvprintw(17, 18, "#");
    mvprintw(18, 18, "#");
    mvprintw(19, 18, "#");
    mvprintw(19, 17, "#");
  } else if ((fig % 7) == T_TYPE) {
    mvprintw(17, 16, "#");
    mvprintw(17, 17, "#");
    mvprintw(17, 18, "#");
    mvprintw(18, 17, "#");
  }
}
