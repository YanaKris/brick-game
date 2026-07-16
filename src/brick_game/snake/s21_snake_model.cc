#include "s21_snake_model.h"

namespace s21 {

void GameModel::initSnake() {
  snake_tail_.push_back(Snake(snake_head_.x - 1, snake_head_.y));
  snake_tail_.push_back(Snake(snake_head_.x - 2, snake_head_.y));
  snake_tail_.push_back(Snake(snake_head_.x - 3, snake_head_.y));
}

GameModel::GameModel()
    : snake_head_(FIELD_WIDTH / 2, FIELD_HEIGHT / 2),
      apple(3, 5),
      eaten_apples(0),
      game_info({nullptr, nullptr, 0, 0, 0, 300, 0}) {
  initSnake();
}

std::string GameModel::printDebugInfo() const {
  std::string debugInfo = "\nGameModel:\n";
  debugInfo += "  snake_head_: (" + std::to_string(snake_head_.x) + ", " +
               std::to_string(snake_head_.y) + ")\n";
  debugInfo += "  snake_tail_: ";
  for (const auto& snake : snake_tail_) {
    debugInfo +=
        "(" + std::to_string(snake.x) + ", " + std::to_string(snake.y) + ") ";
  }
  debugInfo += "begin x:" + std::to_string(snake_tail_.begin()->x) +
               " y:" + std::to_string(snake_tail_.begin()->y) + " ";
  debugInfo += "back x:" + std::to_string(snake_tail_.back().x) +
               " y:" + std::to_string(snake_tail_.back().y) + "" + "\n";
  debugInfo += "  apple: (" + std::to_string(apple.x) + ", " +
               std::to_string(apple.y) + ")\n";
  debugInfo += "  eaten_apples: " + std::to_string(eaten_apples) + "\n";
  debugInfo += "  game_info: (" + std::to_string(game_info.score) + ", " +
               std::to_string(game_info.level) + ", " +
               std::to_string(game_info.speed) + ", " +
               std::to_string(game_info.pause) + ")\n";
  debugInfo += "  level_: " + std::to_string(level_) + "\n";
  debugInfo += "  speed_: " + std::to_string(speed_) + "\n";
  return debugInfo;
}

void GameModel::setGameInfo(GameInfo_t* game_info) {
  this->game_info = *game_info;
}

bool GameModel::moveSnake(int dx, int dy) {
  // Переменные dx и dy определяют направление движения змейки

  // Проверка на столкновение с границами
  if (snake_head_.x + dx <= 0 || snake_head_.x + dx >= FIELD_WIDTH + 1 ||
      snake_head_.y + dy <= 0 || snake_head_.y + dy >= FIELD_HEIGHT + 1) {
    return false;
  } else {
    // Обновляем позицию головы змейки
    snake_head_.x += dx;
    snake_head_.y += dy;
    // Проверка на столкновение с хвостом
    for (const auto& segment : snake_tail_) {
      if (snake_head_.x == segment.x && snake_head_.y == segment.y) {
        return false;
      }
    }
  }

  // Проверка на съеденное яблоко
  if (snake_head_.x == apple.x && snake_head_.y == apple.y) {
    eaten_apples++;

    snake_tail_.insert(snake_tail_.begin(),
                       Snake(snake_head_.x - dx, snake_head_.y - dy));
    respawnApple();
  } else {
    if (!snake_tail_.empty()) {
      snake_tail_.pop_back();
    }

    snake_tail_.insert(snake_tail_.begin(),
                       Snake(snake_head_.x - dx, snake_head_.y - dy));
  }

  return true;
}

void GameModel::respawnApple() {
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

void GameModel::increaseLevel() {
  if (level_ < 10) {
    level_++;
    game_info.speed -= 50;  // уменьшаем скорость движения змейки
  }
}

int GameModel::getLevel() const { return level_; }

bool GameModel::isGameWon() const { return snake_tail_.size() >= 199; }

const Snake& GameModel::getHead() const { return snake_head_; }
const std::vector<Snake>& GameModel::getTail() const { return snake_tail_; }
const Snake& GameModel::getApple() const { return apple; }
int GameModel::getEatenApples() const { return eaten_apples; }
int GameModel::getHighScore() const { return game_info.high_score; }
void GameModel::setHighScore(int high_score) {
  game_info.high_score = high_score;
}
void GameModel::setScore(int score) { eaten_apples = score; }

void GameModel::setLevel(int level) { level_ = level; }
void GameModel::setSpeed(int speed) { speed_ = speed; }
int GameModel::getSpeed() const { return game_info.speed; }
void GameModel::setFig(int fig) { fig_ = fig; }
int GameModel::getFig() { return fig_; }
}  // namespace s21
