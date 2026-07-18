#include "s21_snake_model.h"

#include <utility>

namespace s21 {

void SnakeModel::initSnake() {
  snake_tail_.push_back(Snake(snake_head_.x - 1, snake_head_.y));
  snake_tail_.push_back(Snake(snake_head_.x - 2, snake_head_.y));
  snake_tail_.push_back(Snake(snake_head_.x - 3, snake_head_.y));
}

SnakeModel::SnakeModel()
    : snake_head_(FIELD_WIDTH / 2, FIELD_HEIGHT / 2),
      apple(3, 5),
      eaten_apples(0),
      game_info({nullptr, nullptr, 0, 0, 0, 300, 0}) {
  initSnake();
}

SnakeModel::SnakeModel(Snake head, std::vector<Snake> tail, Snake apple)
    : snake_head_(head),
      snake_tail_(std::move(tail)),
      apple(apple),
      eaten_apples(0),
      game_info({nullptr, nullptr, 0, 0, 0, 300, 0}) {}

void SnakeModel::Rasterize(int** field) const {
  for (int y = 0; y < FIELD_HEIGHT; ++y) {
    for (int x = 0; x < FIELD_WIDTH; ++x) field[y][x] = 0;
  }
  // модель живёт в 1-based координатах, поле GameInfo_t — 0-based
  const auto put = [field](const Snake& cell, int value) {
    if (cell.x >= 1 && cell.x <= FIELD_WIDTH && cell.y >= 1 &&
        cell.y <= FIELD_HEIGHT) {
      field[cell.y - 1][cell.x - 1] = value;
    }
  };
  for (const Snake& segment : snake_tail_) put(segment, 1);
  put(snake_head_, 1);
  put(apple, 2);
}

int SnakeModel::LevelFor(int eaten_apples) {
  const int level = eaten_apples / 5;
  return level > 10 ? 10 : level;
}

int SnakeModel::SpeedFor(int level) {
  const int speed = 300 - 50 * level;
  return speed < 50 ? 50 : speed;
}

bool SnakeModel::IsWinLength(std::size_t length) { return length >= 200; }

void SnakeModel::setGameInfo(GameInfo_t* game_info) {
  this->game_info = *game_info;
}

bool SnakeModel::moveSnake(int dx, int dy) {
  // Все проверки — до любых мутаций: при неудаче состояние не меняется.
  const int new_x = snake_head_.x + dx;
  const int new_y = snake_head_.y + dy;

  // Столкновение с границами (координаты 1-based)
  if (new_x <= 0 || new_x > FIELD_WIDTH || new_y <= 0 || new_y > FIELD_HEIGHT) {
    return false;
  }

  const bool eats = (new_x == apple.x && new_y == apple.y);

  // Столкновение с хвостом. Последний сегмент в этот же тик освобождает
  // свою клетку (если змейка не растёт), поэтому не блокирует ход.
  std::size_t blocking = snake_tail_.size();
  if (!eats && blocking > 0) --blocking;
  for (std::size_t i = 0; i < blocking; ++i) {
    if (new_x == snake_tail_[i].x && new_y == snake_tail_[i].y) {
      return false;
    }
  }

  snake_tail_.insert(snake_tail_.begin(), snake_head_);  // старая голова — шея
  snake_head_.x = new_x;
  snake_head_.y = new_y;
  if (eats) {
    eaten_apples++;
    respawnApple();
  } else {
    snake_tail_.pop_back();
  }

  return true;
}

void SnakeModel::respawnApple() {
  // Змейка заняла всё поле (победа) — свободных клеток нет,
  // иначе поиск ниже зациклится навсегда.
  if (snake_tail_.size() + 1 >= FIELD_WIDTH * FIELD_HEIGHT) {
    return;
  }
  // Логика для случайного появления яблока
  static std::default_random_engine engine(
      std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<int> distX(1, FIELD_WIDTH);
  std::uniform_int_distribution<int> distY(1, FIELD_HEIGHT);

  bool isFree;
  do {
    apple.x = distX(engine);
    apple.y = distY(engine);
    isFree = true;

    // Проверяем, не находится ли яблоко под змейкой
    if (apple.x == snake_head_.x && apple.y == snake_head_.y) {
      isFree = false;
    } else {
      for (const auto& segment : snake_tail_) {
        if (apple.x == segment.x && apple.y == segment.y) {
          isFree = false;
          break;
        }
      }
    }
  } while (!isFree);
}

void SnakeModel::increaseLevel() {
  if (level_ < 10) {
    level_++;
    game_info.speed -= 50;  // уменьшаем скорость движения змейки
  }
}

int SnakeModel::getLevel() const { return level_; }

bool SnakeModel::isGameWon() const {
  return IsWinLength(snake_tail_.size() + 1);  // голова + хвост
}

const Snake& SnakeModel::getHead() const { return snake_head_; }
const std::vector<Snake>& SnakeModel::getTail() const { return snake_tail_; }
const Snake& SnakeModel::getApple() const { return apple; }
int SnakeModel::getEatenApples() const { return eaten_apples; }
int SnakeModel::getHighScore() const { return game_info.high_score; }
void SnakeModel::setHighScore(int high_score) {
  game_info.high_score = high_score;
}
void SnakeModel::setScore(int score) { eaten_apples = score; }

void SnakeModel::setLevel(int level) { level_ = level; }
void SnakeModel::setSpeed(int speed) { speed_ = speed; }
int SnakeModel::getSpeed() const { return game_info.speed; }
void SnakeModel::setFig(int fig) { fig_ = fig; }
int SnakeModel::getFig() { return fig_; }
}  // namespace s21
