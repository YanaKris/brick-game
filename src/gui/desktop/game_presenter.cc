#include "game_presenter.h"

#include <algorithm>
#include <utility>

#include "../../brick_game/common/high_score.h"

namespace s21 {

GamePresenter::GamePresenter(std::unique_ptr<IGame> game,
                             std::string high_score_path)
    : game_(std::move(game)), high_score_path_(std::move(high_score_path)) {}

void GamePresenter::Begin() {
  record_ = HighScore::Load(high_score_path_);
  saved_ = false;
  game_->userInput(Start, false);
}

GameInfo_t GamePresenter::Advance() {
  GameInfo_t info = game_->updateCurrentState();
  info.high_score = std::max(record_, info.score);
  last_info_ = info;
  return info;
}

GameInfo_t GamePresenter::Tick() {
  if (game_->finished()) return last_info_;

  GameInfo_t info = Advance();

  if (game_->finished() && !saved_) {
    if (info.score > record_) {
      HighScore::Save(high_score_path_, info.score);
      record_ = info.score;
    }
    info.high_score = record_;
    last_info_ = info;
    saved_ = true;
  }
  return info;
}

GameInfo_t GamePresenter::Input(UserAction_t action) {
  if (game_->finished()) return last_info_;
  game_->userInput(action, false);
  // Поле в last_info_.field указывает на внутренний буфер игры: для тетриса
  // userInput уже обновил его (RefreshField), змейка перерисуется на тике.
  last_info_.high_score = std::max(record_, last_info_.score);
  return last_info_;
}

GameInfo_t GamePresenter::Snapshot() const { return last_info_; }

bool GamePresenter::Finished() const { return game_->finished(); }

GameState GamePresenter::State() const { return game_->state(); }

}  // namespace s21
