#ifndef S21_BRICK_GAME_SNAKE_S21_SNAKE_MODEL_H_
#define S21_BRICK_GAME_SNAKE_S21_SNAKE_MODEL_H_

#include <chrono>
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>

#include "../../interface.h"

namespace s21 {
class Snake {
 public:
  int x;
  int y;

  Snake(int startX, int startY) : x(startX), y(startY) {}
};

class SnakeModel {
 public:
  SnakeModel();
  // Восстановление произвольного состояния (DI для фасада и тестов).
  SnakeModel(Snake head, std::vector<Snake> tail, Snake apple);

  // Растеризация в поле 20×10 (0-based): голова/хвост = 1, яблоко = 2.
  void Rasterize(int** field) const;

  // Бонусная механика: +1 уровень за каждые 5 яблок, потолок 10.
  static int LevelFor(int eaten_apples);
  // Скорость тика: 300 - 50 * уровень, но не ниже 50 (не уходит в минус).
  static int SpeedFor(int level);
  // Победа при длине змейки (голова + хвост) 200.
  static bool IsWinLength(std::size_t length);

  void initSnake();
  void setGameInfo(GameInfo_t* game_info);
  bool moveSnake(int dx, int dy);
  void respawnApple();
  void increaseLevel();
  int getLevel() const;
  bool isGameWon() const;

  const Snake& getHead() const;
  const std::vector<Snake>& getTail() const;
  const Snake& getApple() const;
  int getEatenApples() const;
  int getHighScore() const;
  void setHighScore(int high_score);
  int getSpeed() const;

  void setScore(int score);
  void setLevel(int level);
  void setSpeed(int speed);
  void setFig(int fig);
  int getFig();

  std::string game_name;

 private:
  Snake snake_head_;
  std::vector<Snake> snake_tail_;  // хвост змейки
  Snake apple;
  int eaten_apples;
  GameInfo_t game_info;
  int level_ = 0;
  int speed_ = 350;
  int fig_;
};

}  // namespace s21

#endif  // S21_BRICK_GAME_SNAKE_S21_SNAKE_MODEL_H_