#include <gtest/gtest.h>

#include <memory>

#include "../api/game_api.h"
#include "../brick_game/common/game_factory.h"

namespace {

using s21::GameFactory;
using s21::GameState;
using s21::IGame;

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
  EXPECT_EQ(info.field[9][5], 1);
  EXPECT_EQ(info.field[9][1], 0);
}

TEST(GameApiTest, TetrisRunsThroughFreeFunctions) {
  selectGame(kTetris);
  userInput(Start, false);
  GameInfo_t info = updateCurrentState();
  EXPECT_EQ(CountFilledCells(info), 4);
}

TEST(GameApiTest, SelectGameResetsState) {
  selectGame(kSnake);
  userInput(Start, false);
  updateCurrentState();

  selectGame(kSnake);
  GameInfo_t info = updateCurrentState();
  EXPECT_EQ(info.field[9][4], 1);
  EXPECT_EQ(info.field[9][5], 0);
}

TEST(GameFactoryTest, MakesDistinctGamesInStartState) {
  std::unique_ptr<IGame> snake = GameFactory::Make(kSnake);
  std::unique_ptr<IGame> tetris = GameFactory::Make(kTetris);
  ASSERT_NE(snake, nullptr);
  ASSERT_NE(tetris, nullptr);
  EXPECT_EQ(snake->state(), GameState::kStart);
  EXPECT_EQ(tetris->state(), GameState::kStart);
}

TEST(GameFactoryTest, SnakeAndTetrisDifferByStartTransition) {
  std::unique_ptr<IGame> snake = GameFactory::Make(kSnake);
  std::unique_ptr<IGame> tetris = GameFactory::Make(kTetris);
  snake->userInput(Start, false);
  tetris->userInput(Start, false);
  EXPECT_EQ(snake->state(), GameState::kMoving);
  EXPECT_EQ(tetris->state(), GameState::kSpawn);
}

}  // namespace
