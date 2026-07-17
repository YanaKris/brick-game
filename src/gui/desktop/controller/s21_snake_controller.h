#ifndef GAME_H
#define GAME_H

#include <QKeyEvent>
#include <QList>
#include <QPainter>
#include <QTimer>
#include <QWidget>

#include "../../brick_game/snake/s21_snake_model.h"
#include "../../interface.h"

namespace s21 {
class SnakeController;

class SnakeController : public QWidget {
  Q_OBJECT
 public:
  SnakeController(SnakeModel* m_SnakeModel);
  ~SnakeController() = default;

 protected:
  void paintEvent(QPaintEvent* e) override;
  void keyPressEvent(QKeyEvent* e) override;

 private:
  int m_snakeItemSize;
  QTimer* m_moveSnakeTimer;
  SnakeModel* m_SnakeModel_;
  UserAction_t user_action;
  int dx = 1;
  int dy = 0;
  int prev_score = 0;

 public slots:
  void MoveSnakeSlot();
};
}  // namespace s21
#endif  // GAME_H
