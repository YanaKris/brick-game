#ifndef S21_GUI_DESKTOP_CONTROLLER_TETRIS_CONTROLLER_H_
#define S21_GUI_DESKTOP_CONTROLLER_TETRIS_CONTROLLER_H_

#include <QObject>
#include <QTimer>

#include "../game_presenter.h"
#include "../tetris_view.h"

namespace s21 {

class TetrisController : public QObject {
  Q_OBJECT

 public:
  explicit TetrisController(TetrisView* view, QObject* parent = nullptr);

 signals:
  void ExitToMenu();

 private slots:
  void OnTick();
  void OnKey(int key);

 private:
  TetrisView* view_;
  GamePresenter presenter_;
  QTimer timer_;
};

}  // namespace s21

#endif  // S21_GUI_DESKTOP_CONTROLLER_TETRIS_CONTROLLER_H_
