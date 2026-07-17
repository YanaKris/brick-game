#ifndef HELPFIELD_H
#define HELPFIELD_H

#include <QPainter>
#include <QTimer>
#include <QWidget>

#include "../../brick_game/snake/s21_snake_model.h"

namespace s21 {
class HelpField : public QWidget {
  Q_OBJECT
 public:
  HelpField(SnakeModel* m_SnakeModel);
  // HelpField(TetrisController *tetris_controller);
  void loadHighScore(int& high_score);
  void saveHighScore(int high_score);

 protected:
  void paintEvent(QPaintEvent* e) override;

 private:
  GameInfo_t game_info;
  std::string m_gameName;  // С‡Р»РµРЅ РєР»Р°СЃСЃР° РґР»СЏ С…СЂР°РЅРµРЅРёСЏ
                           // РЅР°Р·РІР°РЅРёСЏ РёРіСЂС‹
  SnakeModel* m_SnakeModel_;
  // TetrisController *m_tetris_controller_;
  QTimer* m_moveSnakeTimer;
  int prev_score = 0;
  int level_ = 0;
  int speed_ = 350;

 public slots:
  void MoveSnakeSlot();
};
}  // namespace s21

#endif  // HELPFIELD_H