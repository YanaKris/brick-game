#include "snake_view.h"

#include <QFont>
#include <QPainter>
#include <QRect>

namespace s21 {

namespace {
constexpr int kCell = CELL_SIZE;
constexpr int kMargin = 20;
constexpr int kPanelWidth = 140;
}  // namespace

SnakeView::SnakeView(QWidget* parent) : QWidget(parent) {
  setFixedSize(kMargin * 2 + FIELD_WIDTH * kCell + kPanelWidth,
               kMargin * 2 + FIELD_HEIGHT * kCell);
  setFocusPolicy(Qt::StrongFocus);
}

void SnakeView::Render(const GameInfo_t& info) {
  info_ = info;
  update();
}

void SnakeView::ShowMessage(const QString& message) {
  message_ = message;
  update();
}

void SnakeView::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event)
  QPainter painter(this);
  const int field_w = FIELD_WIDTH * kCell;
  const int field_h = FIELD_HEIGHT * kCell;

  painter.setBrush(Qt::black);
  painter.drawRect(kMargin, kMargin, field_w, field_h);

  if (info_.field != nullptr) {
    for (int r = 0; r < FIELD_HEIGHT; ++r) {
      for (int c = 0; c < FIELD_WIDTH; ++c) {
        const int cell = info_.field[r][c];
        if (cell == 0) continue;
        const int x = kMargin + c * kCell;
        const int y = kMargin + r * kCell;
        painter.setBrush(cell == 2 ? Qt::red : Qt::green);
        painter.drawRect(x, y, kCell, kCell);
      }
    }
  }

  const int px = kMargin + field_w + 20;
  painter.setPen(Qt::black);
  painter.setFont(QFont("Arial", 11, QFont::Bold));
  auto line = [&](int y, const QString& text) {
    painter.drawText(px, y, text);
  };
  line(60, "SCORE: " + QString::number(info_.score));
  line(100, "RECORD: " + QString::number(info_.high_score));
  line(140, "LEVEL: " + QString::number(info_.level));
  if (info_.pause) line(180, "PAUSE");
  line(260, "MOVE: ARROWS");
  line(290, "PAUSE: SPACE");
  line(320, "SPEED UP: Z");
  line(350, "MENU: ESC");

  if (!message_.isEmpty()) {
    const QRect area(kMargin, kMargin, field_w, field_h);
    painter.setPen(Qt::yellow);
    painter.setFont(QFont("Arial", 20, QFont::Bold));
    painter.drawText(area, Qt::AlignCenter, message_);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(area, Qt::AlignHCenter | Qt::AlignBottom, "ENTER: MENU");
  }
}

void SnakeView::keyPressEvent(QKeyEvent* event) {
  emit KeyPressed(event->key());
}

}  // namespace s21
