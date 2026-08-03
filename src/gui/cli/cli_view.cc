#include "cli_view.h"

#include <ncurses.h>

namespace s21 {

namespace {
constexpr int kApple = 2;
constexpr int kPanelX = FIELD_WIDTH + 3;
constexpr const char* kIntroMessage = "Press ENTER to start!";
}  // namespace

void CliView::Render(const GameInfo_t& info) const {
  erase();
  DrawBorders();
  DrawField(info);
  DrawPanel(info);
  refresh();
}

void CliView::DrawField(const GameInfo_t& info) const {
  for (int r = 0; r < FIELD_HEIGHT; ++r) {
    for (int c = 0; c < FIELD_WIDTH; ++c) {
      const int cell = info.field[r][c];
      if (cell == 0) {
        mvaddch(r + 1, c + 1, ' ');
      } else if (cell == kApple) {
        mvaddch(r + 1, c + 1, '@');
      } else {
        mvaddch(r + 1, c + 1, ACS_BLOCK);
      }
    }
  }
}

void CliView::DrawPanel(const GameInfo_t& info) const {
  mvprintw(1, kPanelX, "SCORE");
  mvprintw(2, kPanelX, "%d", info.score);
  mvprintw(4, kPanelX, "RECORD");
  mvprintw(5, kPanelX, "%d", info.high_score);
  mvprintw(7, kPanelX, "LEVEL");
  mvprintw(8, kPanelX, "%d", info.level);
  mvprintw(10, kPanelX, "SPEED");
  mvprintw(11, kPanelX, "%d", info.speed);
  if (info.pause) mvprintw(13, kPanelX, "PAUSE");
}

void CliView::DrawBorders() const {
  // поле игры
  mvaddch(0, 0, ACS_ULCORNER);
  mvaddch(0, FIELD_WIDTH + 1, ACS_URCORNER);
  mvaddch(FIELD_HEIGHT + 1, 0, ACS_LLCORNER);
  mvaddch(FIELD_HEIGHT + 1, FIELD_WIDTH + 1, ACS_LRCORNER);
  for (int i = 0; i < FIELD_WIDTH; ++i) {
    mvaddch(0, i + 1, ACS_HLINE);
    mvaddch(FIELD_HEIGHT + 1, i + 1, ACS_HLINE);
  }
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    mvaddch(i + 1, 0, ACS_VLINE);
    mvaddch(i + 1, FIELD_WIDTH + 1, ACS_VLINE);
  }
}

void CliView::ShowIntro() const {
  erase();
  mvprintw(8, 1, "%s", kIntroMessage);
  refresh();
}

void CliView::ShowGameOver(int score, bool new_record) const {
  erase();
  mvprintw(8, 3, "GAME OVER");
  mvprintw(10, 3, "SCORE %d", score);
  if (new_record) mvprintw(12, 3, "NEW RECORD!");
  mvprintw(14, 3, "press ESC to exit");
  refresh();
}

void CliView::ShowWin(int score) const {
  erase();
  mvprintw(8, 3, "YOU WIN!");
  mvprintw(10, 3, "SCORE %d", score);
  mvprintw(14, 3, "press ESC to exit");
  refresh();
}

}  // namespace s21
