#ifndef S21_GUI_DESKTOP_SNAKE_VIEW_H_
#define S21_GUI_DESKTOP_SNAKE_VIEW_H_

#include <QKeyEvent>
#include <QString>
#include <QWidget>

#include "../../interface.h"

namespace s21 {

class SnakeView : public QWidget {
  Q_OBJECT

 public:
  explicit SnakeView(QWidget* parent = nullptr);

  void Render(const GameInfo_t& info);
  void ShowMessage(const QString& message);

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
