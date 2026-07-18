#include <gtest/gtest.h>

#include <utility>
#include <vector>

#include "../brick_game/snake/snake_game.h"

namespace {

using s21::GameState;
using s21::Snake;
using s21::SnakeGame;
using s21::SnakeModel;

SnakeModel MakeModel() {
  return SnakeModel(Snake(5, 10), {Snake(4, 10), Snake(3, 10), Snake(2, 10)},
                    Snake(1, 1));
}

TEST(SnakeGameTest, StartsInStartState) {
  SnakeGame game;
  EXPECT_EQ(game.state(), GameState::kStart);
  EXPECT_FALSE(game.finished());
}

TEST(SnakeGameTest, TickBeforeStartDoesNotMove) {
  SnakeGame game(MakeModel());
  GameInfo_t info = game.updateCurrentState();
  EXPECT_EQ(info.field[9][4], 1);  // голова на месте
  info = game.updateCurrentState();
  EXPECT_EQ(info.field[9][4], 1);
  EXPECT_EQ(info.field[9][5], 0);
  EXPECT_EQ(game.state(), GameState::kStart);
}

TEST(SnakeGameTest, StartTransitionsToMoving) {
  SnakeGame game;
  game.userInput(Start, false);
  EXPECT_EQ(game.state(), GameState::kMoving);
}

TEST(SnakeGameTest, TickMovesSnakeRight) {
  SnakeGame game(MakeModel());
  game.userInput(Start, false);
  GameInfo_t info = game.updateCurrentState();
  EXPECT_EQ(info.field[9][5], 1);
  EXPECT_EQ(info.field[9][1], 0);
}

TEST(SnakeGameTest, PauseFreezesGame) {
  SnakeGame game(MakeModel());
  game.userInput(Start, false);
  game.userInput(Pause, false);
  EXPECT_EQ(game.state(), GameState::kPause);
  GameInfo_t info = game.updateCurrentState();
  EXPECT_EQ(info.pause, 1);
  EXPECT_EQ(info.field[9][4], 1);
  EXPECT_EQ(info.field[9][5], 0);
  game.userInput(Pause, false);
  EXPECT_EQ(game.state(), GameState::kMoving);
}

TEST(SnakeGameTest, ReverseTurnIsIgnored) {
  SnakeGame game(MakeModel());
  game.userInput(Start, false);
  game.userInput(Left, false);
  GameInfo_t info = game.updateCurrentState();
  EXPECT_EQ(info.field[9][5], 1);
}

TEST(SnakeGameTest, DoubleTurnWithinOneTickCannotReverse) {
  SnakeGame game(MakeModel());
  game.userInput(Start, false);
  game.userInput(Up, false);
  game.userInput(Left, false);
  GameInfo_t info = game.updateCurrentState();
  EXPECT_FALSE(game.finished());
  EXPECT_EQ(info.field[8][4], 1);

TEST(SnakeGameTest, ReverseCheckedAgainstLastMovedDirection) {
  SnakeGame game(MakeModel());
  game.userInput(Start, false);
  game.userInput(Up, false);
  game.updateCurrentState();
  game.userInput(Down, false);
  GameInfo_t info = game.updateCurrentState();
  EXPECT_EQ(info.field[7][4], 1);
  EXPECT_FALSE(game.finished());
}

TEST(SnakeGameTest, TurnUpWorks) {
  SnakeGame game(MakeModel());
  game.userInput(Start, false);
  game.userInput(Up, false);
  GameInfo_t info = game.updateCurrentState();
  EXPECT_EQ(info.field[8][4], 1);
}

TEST(SnakeGameTest, CrashIntoWallEndsGame) {
  SnakeModel model(Snake(FIELD_WIDTH, 10),
                   {Snake(9, 10), Snake(8, 10), Snake(7, 10)}, Snake(1, 1));
  SnakeGame game(std::move(model));
  game.userInput(Start, false);
  game.updateCurrentState();
  EXPECT_EQ(game.state(), GameState::kGameOver);
  EXPECT_TRUE(game.finished());
}

TEST(SnakeGameTest, ReachingLength200Wins) {
  std::vector<Snake> tail;
  for (int y = 1; y <= FIELD_HEIGHT; ++y) {
    for (int x = 1; x <= FIELD_WIDTH; ++x) {
      const bool is_head = (x == 2 && y == 1);
      const bool is_apple = (x == 3 && y == 1);
      if (!is_head && !is_apple) tail.push_back(Snake(x, y));
    }
  }
  SnakeGame game(SnakeModel(Snake(2, 1), tail, Snake(3, 1)));
  game.userInput(Start, false);
  game.updateCurrentState();
  EXPECT_EQ(game.state(), GameState::kWin);
  EXPECT_TRUE(game.finished());
}

TEST(SnakeGameTest, ActionAcceleratesForOneTick) {
  SnakeGame game(MakeModel());
  game.userInput(Start, false);
  GameInfo_t info = game.updateCurrentState();
  EXPECT_EQ(info.speed, 300);
  game.userInput(Action, false);
  info = game.updateCurrentState();
  EXPECT_EQ(info.speed, 150);
  info = game.updateCurrentState();
  EXPECT_EQ(info.speed, 300);
}

TEST(SnakeGameTest, EatingAppleUpdatesScoreAndHighScore) {
  SnakeModel model(Snake(5, 5), {Snake(4, 5), Snake(3, 5), Snake(2, 5)},
                   Snake(6, 5));
  SnakeGame game(std::move(model));
  game.userInput(Start, false);
  GameInfo_t info = game.updateCurrentState();
  EXPECT_EQ(info.score, 1);
  EXPECT_EQ(info.high_score, 1);
  EXPECT_EQ(info.level, 0);
}

}  // namespace
