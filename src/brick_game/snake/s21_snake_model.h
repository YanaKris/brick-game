#ifndef S21_GAME_MODEL_H
#define S21_GAME_MODEL_H

#include <chrono>
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

class GameModel {
 public:
  GameModel();
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

  std::string printDebugInfo() const;
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

#endif  // S21_GAME_MODEL_H