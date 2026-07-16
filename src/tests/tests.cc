#include "../brick_game/snake/s21_snake_model.h"
#include "gtest/gtest.h"

TEST(GameModelTest, Constructor) {
  s21::GameModel game_model;
  EXPECT_EQ(game_model.getHead().x, FIELD_WIDTH / 2);
  EXPECT_EQ(game_model.getHead().y, FIELD_HEIGHT / 2);
  EXPECT_EQ(game_model.getApple().x, 3);
  EXPECT_EQ(game_model.getApple().y, 5);
  EXPECT_EQ(game_model.getEatenApples(), 0);
}

TEST(GameModelTest, SetGameInfo) {
  s21::GameModel game_model;
  GameInfo_t game_info;
  game_info.score = 10;
  game_info.level = 2;
  game_info.speed = 2;
  game_model.setGameInfo(&game_info);
  EXPECT_EQ(game_info.score, 10);
  EXPECT_EQ(game_info.level, 2);
  EXPECT_EQ(game_info.speed, 2);
}

TEST(GameModelTest, MoveSnake) {
  s21::GameModel game_model;
  EXPECT_TRUE(game_model.moveSnake(1, 0));
  EXPECT_EQ(game_model.getHead().x, FIELD_WIDTH / 2 + 1);
  EXPECT_EQ(game_model.getHead().y, FIELD_HEIGHT / 2);
  EXPECT_FALSE(game_model.moveSnake(-FIELD_WIDTH, 0));
}

TEST(GameModelTest, RespawnApple) {
  s21::GameModel game_model;
  game_model.moveSnake(1, 0);
  game_model.moveSnake(1, 0);
  game_model.respawnApple();
  EXPECT_NE(game_model.getApple().x, 3);
  EXPECT_NE(game_model.getApple().y, 5);
}

TEST(GameModelTest, IncreaseLevel) {
  s21::GameModel game_model;
  game_model.increaseLevel();
  EXPECT_EQ(game_model.getLevel(), 1);
  EXPECT_EQ(game_model.getSpeed(), 250);
}

TEST(GameModelTest, GetHighScore) {
  s21::GameModel game_model;
  game_model.setHighScore(100);
  EXPECT_EQ(game_model.getHighScore(), 100);
}

TEST(GameModelTest, SetScore) {
  s21::GameModel game_model;
  game_model.setScore(50);
  EXPECT_EQ(game_model.getEatenApples(), 50);
}

TEST(GameModelTest, SetLevel) {
  s21::GameModel game_model;
  game_model.setLevel(5);
  EXPECT_EQ(game_model.getLevel(), 5);
}

TEST(GameModelTest, GetFig) {
  s21::GameModel game_model;
  game_model.setFig(1);
  EXPECT_EQ(game_model.getFig(), 1);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}