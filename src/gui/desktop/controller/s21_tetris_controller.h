#ifndef S21_TETRIS_CONTROLLER_H
#define s21_TETRIS_CONTROLLER_H

#include <QKeyEvent>
#include <QList>
#include <QPainter>
#include <QTimer>
#include <QWidget>

#include "../../interface.h"
#include "../s21_help_field.h"

extern "C" {  // РїРѕРґРєР»СЋС‡Р°РµРј С„Р°Р№Р» РёР· РЎ СЏР·С‹РєР°
#include "../../../brick_game/tetris/tetris.h"
}

extern "C" bool userInputTet(GameInfo_t* game, int action, Tetrino* tetrino);

namespace s21 {
class TetrisController;

class TetrisController : public QWidget {
  Q_OBJECT
 public:
  TetrisController(SnakeModel* m_SnakeModel);
  ~TetrisController() = default;
  void printField(int** field, int x, int y, int w, int h);
  void increaseLevel();

 protected:
  void paintEvent(QPaintEvent* e) override;
  void keyPressEvent(QKeyEvent* e) override;  // РІРјРµСЃС‚Рѕ userKeyPress

 private:
  int m_tetrisItemSize;
  QTimer* m_moveTetrisTimer;
  Tetrino* m_tetrino;
  GameInfo_t* m_gameInfo;
  UserAction_t m_user_action = Start;
  int prev_score = 0;
  bool spawn = false;
  bool work = true;
  SnakeModel* m_SnakeModel_;
  int currentFig;

 public slots:
  void MoveTetrisSlot();
};
}  // namespace s21

#endif  // S21_TETRIS_CONTROLLER_H