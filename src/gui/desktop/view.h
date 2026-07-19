#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGridLayout>
#include <QMainWindow>

#include "controller/s21_snake_controller.h"
#include "controller/s21_tetris_controller.h"
#include "snake_view.h"
#include "tetris_view.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class View;
}
QT_END_NAMESPACE

namespace s21 {
class View : public QMainWindow {
  Q_OBJECT

 public:
  View(QWidget* parent = nullptr);
  ~View();

 private slots:
  void on_pushButton_clicked();
  void on_pushButton_2_clicked();
  void on_pushButton_3_clicked();
  void ReturnToMenu();

 private:

  void ShowGameScreen(QWidget* screen);

  Ui::View* ui;
  SnakeView* m_snakeView;
  SnakeController* m_snakeController;
  TetrisView* m_tetrisView;
  TetrisController* m_TetrisController;
  QWidget* m_centralWidget;
  QGridLayout* m_gridLayout;
  QWidget* m_menu = nullptr; 
};
}  // namespace s21
#endif  // MAINWINDOW_H
