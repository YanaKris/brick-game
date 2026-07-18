#ifndef S21_GUI_CLI_CLI_VIEW_H_
#define S21_GUI_CLI_CLI_VIEW_H_

#include "../../interface.h"

namespace s21 {

// Dumb-view: рисует исключительно содержимое GameInfo_t средствами
// ncurses. Ни игровой логики, ни файлового ввода-вывода, ни знания
// о том, какая это игра — только отрисовка поля и панели.
class CliView {
 public:
  void Render(const GameInfo_t& info) const;
  void ShowIntro() const;
  void ShowGameOver(int score, bool new_record) const;
  void ShowWin(int score) const;

 private:
  void DrawBorders() const;
  void DrawField(const GameInfo_t& info) const;
  void DrawPanel(const GameInfo_t& info) const;
};

}  // namespace s21

#endif  // S21_GUI_CLI_CLI_VIEW_H_
