#ifndef S21_GUI_DESKTOP_SNAKE_VIEW_H_
#define S21_GUI_DESKTOP_SNAKE_VIEW_H_

#include <QKeyEvent>
#include <QString>
#include <QWidget>

#include "../../interface.h"

namespace s21 {

// Dumb-view змейки: рисует исключительно из GameInfo_t и эмитит нажатия
// клавиш. Никакой игровой логики и знания о модели — всё решает контроллер
// поверх Qt-free presenter'а.
class SnakeView : public QWidget {
  Q_OBJECT

 public:
  explicit SnakeView(QWidget* parent = nullptr);

  void Render(const GameInfo_t& info);       // обновить кадр и перерисовать
  void ShowMessage(const QString& message);  // финальный оверлей

 signals:
  void KeyPressed(int key);

 protected:
  void paintEvent(QPaintEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;

 private:
  GameInfo_t info_{};
  QString message_;
};

}  // namespace s21

#endif  // S21_GUI_DESKTOP_SNAKE_VIEW_H_
