#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGridLayout>
#include <QMainWindow>

#include "../../brick_game/snake/s21_snake_model.h"
#include "controller/s21_snake_controller.h"
#include "controller/s21_tetris_controller.h"
#include "snake_view.h"

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
  }  // РїРѕР»СѓС‡РµРЅРёРµ РЅР°Р·РІР°РЅРёСЏ РІС‹Р±СЂР°РЅРЅРѕР№ РёРіСЂС‹

 private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void ReturnToMenu();

 private:
  // Показать игровой экран, сохранив меню (не удаляя его), чтобы можно
  // было вернуться назад через ReturnToMenu.
  void ShowGameScreen(QWidget* screen);

  Ui::View* ui;       // РёРЅС‚РµСЂС„РµР№СЃ РѕРєРЅР°
  QString game_name;  /// РЅР°Р·РІР°РЅРёРµ РІС‹Р±СЂР°РЅРЅРѕР№ РёРіСЂС‹
                      /// РІРѕР·РјРѕР¶РЅРѕ С„Р»Р°Рі РґР»СЏ РІС‹Р±РѕСЂР°
                      /// РјРµС‚РѕРґРѕРІ
  SnakeView* m_snakeView;
  SnakeController* m_snakeController;
  TetrisController* m_TetrisController;
  HelpField* m_helpField;
  QWidget* m_centralWidget;
  QGridLayout* m_gridLayout;
  QWidget* m_menu = nullptr;  // сохранённое меню на время игры

  SnakeModel* m_SnakeModel;  // нужен legacy-тетрису до под-шага 3
};
}  // namespace s21
#endif  // MAINWINDOW_H
