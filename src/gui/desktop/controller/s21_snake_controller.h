#ifndef S21_GUI_DESKTOP_CONTROLLER_SNAKE_CONTROLLER_H_
#define S21_GUI_DESKTOP_CONTROLLER_SNAKE_CONTROLLER_H_

#include <QObject>
#include <QTimer>

#include "../game_presenter.h"
#include "../snake_view.h"

namespace s21 {

// Тонкий Qt-адаптер змейки: связывает dumb SnakeView с Qt-free
// GamePresenter. Таймер продвигает игру, клавиши маппятся в UserAction_t —
// вся логика (движение, рекорд, скорость) живёт в presenter/SnakeGame.
class SnakeController : public QObject {
  Q_OBJECT

 public:
  explicit SnakeController(SnakeView* view, QObject* parent = nullptr);

 signals:
  void ExitToMenu();

 private slots:
  void OnTick();
  void OnKey(int key);

 private:
  SnakeView* view_;
  GamePresenter presenter_;
  QTimer timer_;
};

}  // namespace s21

#endif  // S21_GUI_DESKTOP_CONTROLLER_SNAKE_CONTROLLER_H_
