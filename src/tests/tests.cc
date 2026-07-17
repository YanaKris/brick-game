#include <vector>

#include "../brick_game/common/field_buffer.h"
#include "../brick_game/snake/s21_snake_model.h"
#include "gtest/gtest.h"

TEST(SnakeModelTest, Constructor) {
  s21::SnakeModel game_model;
  EXPECT_EQ(game_model.getHead().x, FIELD_WIDTH / 2);
  EXPECT_EQ(game_model.getHead().y, FIELD_HEIGHT / 2);
  EXPECT_EQ(game_model.getApple().x, 3);
  EXPECT_EQ(game_model.getApple().y, 5);
  EXPECT_EQ(game_model.getEatenApples(), 0);
}

TEST(SnakeModelTest, SetGameInfo) {
  s21::SnakeModel game_model;
  GameInfo_t game_info;
  game_info.score = 10;
  game_info.level = 2;
  game_info.speed = 2;
  game_model.setGameInfo(&game_info);
  EXPECT_EQ(game_info.score, 10);
  EXPECT_EQ(game_info.level, 2);
  EXPECT_EQ(game_info.speed, 2);
}

TEST(SnakeModelTest, MoveSnake) {
  s21::SnakeModel game_model;
  EXPECT_TRUE(game_model.moveSnake(1, 0));
  EXPECT_EQ(game_model.getHead().x, FIELD_WIDTH / 2 + 1);
  EXPECT_EQ(game_model.getHead().y, FIELD_HEIGHT / 2);
  EXPECT_FALSE(game_model.moveSnake(-FIELD_WIDTH, 0));
}

TEST(SnakeModelTest, RespawnApple) {
  s21::SnakeModel game_model;
  game_model.moveSnake(1, 0);
  game_model.moveSnake(1, 0);
  game_model.respawnApple();
  // Позиция случайна: проверяем инварианты — в границах поля
  // и не под змейкой (проверка обеих координат по отдельности флакала).
  const s21::Snake& apple = game_model.getApple();
  EXPECT_GE(apple.x, 1);
  EXPECT_LE(apple.x, FIELD_WIDTH);
  EXPECT_GE(apple.y, 1);
  EXPECT_LE(apple.y, FIELD_HEIGHT);
  const s21::Snake& head = game_model.getHead();
  EXPECT_FALSE(apple.x == head.x && apple.y == head.y);
  for (const s21::Snake& segment : game_model.getTail()) {
    EXPECT_FALSE(apple.x == segment.x && apple.y == segment.y);
  }
}

TEST(SnakeModelTest, IncreaseLevel) {
  s21::SnakeModel game_model;
  game_model.increaseLevel();
  EXPECT_EQ(game_model.getLevel(), 1);
  EXPECT_EQ(game_model.getSpeed(), 250);
}

TEST(SnakeModelTest, GetHighScore) {
  s21::SnakeModel game_model;
  game_model.setHighScore(100);
  EXPECT_EQ(game_model.getHighScore(), 100);
}

TEST(SnakeModelTest, SetScore) {
  s21::SnakeModel game_model;
  game_model.setScore(50);
  EXPECT_EQ(game_model.getEatenApples(), 50);
}

TEST(SnakeModelTest, SetLevel) {
  s21::SnakeModel game_model;
  game_model.setLevel(5);
  EXPECT_EQ(game_model.getLevel(), 5);
}

TEST(SnakeModelTest, GetFig) {
  s21::SnakeModel game_model;
  game_model.setFig(1);
  EXPECT_EQ(game_model.getFig(), 1);
}

// --- Восстановление состояния (DI для фасада и тестов) ---

TEST(SnakeModelTest, StateConstructorRestoresState) {
  s21::SnakeModel model(s21::Snake(7, 8),
                        {s21::Snake(6, 8), s21::Snake(5, 8), s21::Snake(4, 8)},
                        s21::Snake(2, 2));
  EXPECT_EQ(model.getHead().x, 7);
  EXPECT_EQ(model.getHead().y, 8);
  EXPECT_EQ(model.getTail().size(), 3u);
  EXPECT_EQ(model.getApple().x, 2);
  EXPECT_EQ(model.getApple().y, 2);
}

// --- Столкновения ---

TEST(SnakeModelTest, MoveIntoWallFails) {
  s21::SnakeModel model(s21::Snake(FIELD_WIDTH, 10),
                        {s21::Snake(9, 10), s21::Snake(8, 10)},
                        s21::Snake(1, 1));
  EXPECT_FALSE(model.moveSnake(1, 0));  // правая стена
  s21::SnakeModel model_top(
      s21::Snake(5, 1), {s21::Snake(4, 1), s21::Snake(3, 1)}, s21::Snake(1, 5));
  EXPECT_FALSE(model_top.moveSnake(0, -1));  // верхняя стена
}

TEST(SnakeModelTest, MoveIntoTailFails) {
  s21::SnakeModel model(s21::Snake(5, 5),
                        {s21::Snake(4, 5), s21::Snake(4, 4), s21::Snake(5, 4)},
                        s21::Snake(1, 1));
  EXPECT_FALSE(model.moveSnake(-1, 0));  // сегмент хвоста в (4,5)
}

// --- Рост и победа ---

TEST(SnakeModelTest, EatingAppleGrowsTailAndScore) {
  s21::SnakeModel model(s21::Snake(5, 5),
                        {s21::Snake(4, 5), s21::Snake(3, 5), s21::Snake(2, 5)},
                        s21::Snake(6, 5));
  EXPECT_TRUE(model.moveSnake(1, 0));
  EXPECT_EQ(model.getEatenApples(), 1);
  EXPECT_EQ(model.getTail().size(), 4u);
  // яблоко переспавнилось в свободную клетку
  const bool apple_moved = model.getApple().x != 6 || model.getApple().y != 5;
  EXPECT_TRUE(apple_moved);
}

TEST(SnakeModelTest, MovingWithoutAppleKeepsLength) {
  s21::SnakeModel model(s21::Snake(5, 5),
                        {s21::Snake(4, 5), s21::Snake(3, 5), s21::Snake(2, 5)},
                        s21::Snake(1, 1));
  EXPECT_TRUE(model.moveSnake(1, 0));
  EXPECT_EQ(model.getTail().size(), 3u);
  EXPECT_EQ(model.getEatenApples(), 0);
}

TEST(SnakeModelTest, IsWinLengthBoundary) {
  EXPECT_FALSE(s21::SnakeModel::IsWinLength(199));
  EXPECT_TRUE(s21::SnakeModel::IsWinLength(200));
}

TEST(SnakeModelTest, WinWhenLengthReaches200) {
  // Хвост 198 сегментов: все клетки поля, кроме головы (2,1) и яблока (3,1).
  std::vector<s21::Snake> tail;
  for (int y = 1; y <= FIELD_HEIGHT; ++y) {
    for (int x = 1; x <= FIELD_WIDTH; ++x) {
      const bool is_head = (x == 2 && y == 1);
      const bool is_apple = (x == 3 && y == 1);
      if (!is_head && !is_apple) tail.push_back(s21::Snake(x, y));
    }
  }
  ASSERT_EQ(tail.size(), 198u);
  s21::SnakeModel model(s21::Snake(2, 1), tail, s21::Snake(3, 1));
  EXPECT_FALSE(model.isGameWon());
  EXPECT_TRUE(model.moveSnake(1, 0));  // съедает яблоко: длина 200
  EXPECT_TRUE(model.isGameWon());
}

// --- Уровень и скорость (бонусная механика + защита от ухода в минус) ---

TEST(SnakeModelTest, LevelForTable) {
  EXPECT_EQ(s21::SnakeModel::LevelFor(0), 0);
  EXPECT_EQ(s21::SnakeModel::LevelFor(4), 0);
  EXPECT_EQ(s21::SnakeModel::LevelFor(5), 1);
  EXPECT_EQ(s21::SnakeModel::LevelFor(25), 5);
  EXPECT_EQ(s21::SnakeModel::LevelFor(50), 10);
  EXPECT_EQ(s21::SnakeModel::LevelFor(75), 10);  // потолок 10
}

TEST(SnakeModelTest, SpeedForFloorsAtMinimum) {
  EXPECT_EQ(s21::SnakeModel::SpeedFor(0), 300);
  EXPECT_EQ(s21::SnakeModel::SpeedFor(1), 250);
  EXPECT_EQ(s21::SnakeModel::SpeedFor(5), 50);
  EXPECT_EQ(s21::SnakeModel::SpeedFor(10), 50);  // не уходит в минус
}

// --- Растеризация в GameInfo_t.field ---

TEST(SnakeModelTest, RasterizeDrawsSnakeAndApple) {
  s21::SnakeModel model;  // head (5,10), tail (4..2,10), apple (3,5)
  s21::FieldBuffer buffer;
  buffer.data()[0][0] = 9;  // мусор должен быть стёрт
  model.Rasterize(buffer.data());
  EXPECT_EQ(buffer.data()[9][4], 1);  // голова: (5,10) -> [y-1][x-1]
  EXPECT_EQ(buffer.data()[9][3], 1);
  EXPECT_EQ(buffer.data()[9][2], 1);
  EXPECT_EQ(buffer.data()[9][1], 1);
  EXPECT_EQ(buffer.data()[4][2], 2);  // яблоко: (3,5)
  int total = 0;
  for (int r = 0; r < FIELD_HEIGHT; ++r) {
    for (int c = 0; c < FIELD_WIDTH; ++c) total += buffer.data()[r][c];
  }
  EXPECT_EQ(total, 4 * 1 + 2);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}