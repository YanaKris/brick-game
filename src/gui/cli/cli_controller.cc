#include "cli_controller.h"

#include <ncurses.h>

#include <algorithm>
#include <optional>
#include <utility>

#include "../../brick_game/common/high_score.h"

namespace s21 {

CliController::CliController(std::unique_ptr<IGame> game,
                             std::string high_score_path)
    : game_(std::move(game)), high_score_path_(std::move(high_score_path)) {}

void CliController::Run() {
  record_ = HighScore::Load(high_score_path_);
  if (!WaitForStart()) return;

  game_->userInput(Start, false);
  GameInfo_t info = Advance();
  timer_.Reset(info.speed);
  while (!game_->finished()) {
    view_.Render(info);
    timeout(timer_.RemainingMs());
    std::optional<UserAction_t> action = input_.Poll();
    if (action) {
      if (*action == Terminate) break;
      game_->userInput(*action, false);
    }
    if (timer_.Due()) {
      info = Advance();
      timer_.Reset(info.speed);
    }
  }
  Finish(info);
}

bool CliController::WaitForStart() {
  timeout(-1);
  view_.ShowIntro();
  while (true) {
    std::optional<UserAction_t> action = input_.Poll();
    if (!action) continue;
    if (*action == Start) return true;
    if (*action == Terminate) return false;
  }
}

GameInfo_t CliController::Advance() {
  GameInfo_t info = game_->updateCurrentState();
  info.high_score = std::max(record_, info.score);
  return info;
}

void CliController::Finish(const GameInfo_t& info) {
  const bool new_record = info.score > record_;
  if (new_record) HighScore::Save(high_score_path_, info.score);

  if (game_->state() == GameState::kWin) {
    view_.ShowWin(info.score);
  } else {
    view_.ShowGameOver(info.score, new_record);
  }

  timeout(-1);
  while (true) {
    std::optional<UserAction_t> action = input_.Poll();
    if (action && *action == Terminate) break;
  }
}

}  // namespace s21
