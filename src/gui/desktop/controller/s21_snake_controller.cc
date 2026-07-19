#include "s21_snake_controller.h"

#include <QKeyEvent>
#include <memory>

#include "../../../brick_game/snake/snake_game.h"

namespace s21 {

SnakeController::SnakeController(SnakeView* view, QObject* parent)
    : QObject(parent),
      view_(view),
      presenter_(std::make_unique<SnakeGame>(), "high_score_snake.txt") {
  connect(view_, &SnakeView::KeyPressed, this, &SnakeController::OnKey);
  connect(&timer_, &QTimer::timeout, this, &SnakeController::OnTick);

  presenter_.Begin();
  GameInfo_t info = presenter_.Tick();
  view_->Render(info);
  timer_.start(info.speed);
}

void SnakeController::OnTick() {
  GameInfo_t info = presenter_.Tick();
  view_->Render(info);
  timer_.setInterval(info.speed);
  if (presenter_.Finished()) {
    timer_.stop();
    const bool win = presenter_.State() == GameState::kWin;
    view_->ShowMessage(win ? "YOU WIN" : "GAME OVER");
  }
}

void SnakeController::OnKey(int key) {
  const bool over = presenter_.Finished();
  // Esc — выход в меню в любой момент; Enter — после проигрыша/победы.
  const bool restart = over && (key == Qt::Key_Return || key == Qt::Key_Enter);
  if (key == Qt::Key_Escape || restart) {
    emit ExitToMenu();
    return;
  }
  if (over) return;

  UserAction_t action;
  switch (key) {
    case Qt::Key_Left:
      action = Left;
      break;
    case Qt::Key_Right:
      action = Right;
      break;
    case Qt::Key_Up:
      action = Up;
      break;
    case Qt::Key_Down:
      action = Down;
      break;
    case Qt::Key_Space:
      action = Pause;
      break;
    case Qt::Key_Z:
      action = Action;
      break;
    default:
      return;
  }
  view_->Render(presenter_.Input(action));
}

}  // namespace s21
