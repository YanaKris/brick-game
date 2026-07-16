#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGridLayout>
#include <QMainWindow>

#include "../../brick_game/snake/s21_snake_model.h"
#include "controller/s21_snake_controller.h"
#include "controller/s21_tetris_controller.h"

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

  QString getGameName() {
    return game_name;
  }  // получение названия выбранной игры

 private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

 private:
  Ui::View* ui;  // интерфейс окна
  QString
      game_name;  /// название выбранной игры возможно флаг для выбора методов
  SnakeController* m_SnakeController;
  TetrisController* m_TetrisController;
  HelpField* m_helpField;
  QWidget* m_centralWidget;
  QGridLayout* m_gridLayout;

  GameModel* m_gameModel;
};
}  // namespace s21
#endif  // MAINWINDOW_H
