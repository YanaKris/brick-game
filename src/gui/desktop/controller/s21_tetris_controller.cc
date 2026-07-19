#include "s21_tetris_controller.h"

#include <QKeyEvent>
#include <memory>

#include "../../../brick_game/tetris/tetris_game.h"

namespace s21 {

TetrisController::TetrisController(TetrisView* view, QObject* parent)
    : QObject(parent),
      view_(view),
      presenter_(std::make_unique<TetrisGame>(), "record.txt") {
  connect(view_, &TetrisView::KeyPressed, this, &TetrisController::OnKey);
  connect(&timer_, &QTimer::timeout, this, &TetrisController::OnTick);

  presenter_.Begin();
  GameInfo_t info = presenter_.Tick();
  view_->Render(info);
  timer_.start(info.speed);
}

void TetrisController::OnTick() {
  GameInfo_t info = presenter_.Tick();
  view_->Render(info);
  timer_.setInterval(info.speed);
  if (presenter_.Finished()) {
    timer_.stop();
    view_->ShowMessage("GAME OVER");
  }
}

void TetrisController::OnKey(int key) {
  const bool over = presenter_.Finished();
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
    case Qt::Key_Down:
      action = Down;
      break;
    case Qt::Key_Up:
      action = Action; 
      break;
    case Qt::Key_Space:
      action = Pause;
      break;
    default:
      return;
  }
  view_->Render(presenter_.Input(action));
}

}  // namespace s21
