#include <gtest/gtest.h>

#include "../brick_game/common/field_buffer.h"
#include "../gui/web/web_bridge.h"

namespace {

constexpr int kCells = FIELD_HEIGHT * FIELD_WIDTH;

int CountNonZero(const int* buf) {
  int n = 0;
  for (int i = 0; i < kCells; ++i) n += buf[i] != 0 ? 1 : 0;
  return n;
}

int CountValue(const int* buf, int value) {
  int n = 0;
  for (int i = 0; i < kCells; ++i) n += buf[i] == value ? 1 : 0;
  return n;
}

TEST(WebFlattenFieldTest, CopiesEveryCellRowMajor) {
  s21::FieldBuffer fb;
  int** field = fb.data();
  for (int r = 0; r < FIELD_HEIGHT; ++r)
    for (int c = 0; c < FIELD_WIDTH; ++c) field[r][c] = r * FIELD_WIDTH + c;

  int out[kCells];
  for (int i = 0; i < kCells; ++i) out[i] = -1;

  WebFlattenField(field, out);

  bool all_match = true;
  for (int r = 0; r < FIELD_HEIGHT; ++r)
    for (int c = 0; c < FIELD_WIDTH; ++c)
      if (out[r * FIELD_WIDTH + c] != r * FIELD_WIDTH + c) all_match = false;
  EXPECT_TRUE(all_match);

  EXPECT_EQ(out[0], 0);
  EXPECT_EQ(out[1 * FIELD_WIDTH + 2], 1 * FIELD_WIDTH + 2);
  EXPECT_EQ(out[2 * FIELD_WIDTH + 1], 2 * FIELD_WIDTH + 1);
  EXPECT_EQ(out[kCells - 1], kCells - 1);
}

TEST(WebBridgeTest, SnakeStartHasFourBodyAndOneApple) {
  web_start(kSnake);
  web_input(Start, 0);
  web_tick();

  const int* field = web_field_ptr();
  ASSERT_NE(field, nullptr);
  EXPECT_EQ(CountValue(field, 1), 4);
  EXPECT_EQ(CountValue(field, 2), 1);
  EXPECT_EQ(CountNonZero(field), 5);
}

TEST(WebBridgeTest, TetrisStartHasOneTetromino) {
  web_start(kTetris);
  web_input(Start, 0);
  web_tick();

  const int* field = web_field_ptr();
  ASSERT_NE(field, nullptr);
  EXPECT_EQ(CountNonZero(field), 4);
}

TEST(WebBridgeTest, HighScorePassesThrough) {
  web_set_high_score(42);
  web_start(kSnake);
  web_input(Start, 0);
  web_tick();

  EXPECT_EQ(web_high_score(), 42);
}

TEST(WebBridgeTest, NextPointerIsValid) {
  web_start(kTetris);
  web_input(Start, 0);
  web_tick();
  EXPECT_NE(web_next_ptr(), nullptr);
}

}  // namespace
