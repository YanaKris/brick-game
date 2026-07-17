#include <gtest/gtest.h>

#include "../api/game_api.h"

namespace {

int CountFilledCells(const GameInfo_t& info) {
  int count = 0;
  for (int r = 0; r < FIELD_HEIGHT; ++r) {
    for (int c = 0; c < FIELD_WIDTH; ++c)
      count += info.field[r][c] != 0 ? 1 : 0;
  }
  return count;
}

TEST(GameApiTest, SnakeRunsThroughFreeFunctions) {
  selectGame(kSnake);
  userInput(Start, false);
  GameInfo_t info = updateCurrentState();
  EXPECT_EQ(info.field[9][5], 1);  // голова сместилась в (6,10)
  EXPECT_EQ(info.field[9][1], 0);  // хвост освободил (2,10)
}

TEST(GameApiTest, TetrisRunsThroughFreeFunctions) {
  selectGame(kTetris);
  userInput(Start, false);
  GameInfo_t info = updateCurrentState();  // спавн
  EXPECT_EQ(CountFilledCells(info), 4);
}

TEST(GameApiTest, SelectGameResetsState) {
  selectGame(kSnake);
  userInput(Start, false);
  updateCurrentState();  // сдвигаем змейку

  selectGame(kSnake);  // пересоздать игру -> состояние сброшено
  GameInfo_t info = updateCurrentState();  // ещё не стартовали -> без движения
  EXPECT_EQ(info.field[9][4], 1);          // голова на исходном месте
  EXPECT_EQ(info.field[9][5], 0);
}

}  // namespace
