#include "s21_snake_controller.h"

s21::SnakeController::SnakeController(SnakeModel* m_SnakeModel)
    : m_SnakeModel_(m_SnakeModel) {
  setFixedSize(CELL_SIZE * FIELD_WIDTH + 40, CELL_SIZE * FIELD_HEIGHT + 40);
  setFocusPolicy(Qt::StrongFocus);  // РїРѕРґРєР»СЋС‡РµРЅРёРµ РєРЅРѕРїРѕРє

  m_snakeItemSize = CELL_SIZE;
  m_moveSnakeTimer = new QTimer();
  connect(m_moveSnakeTimer, &QTimer::timeout, this,
          &SnakeController::MoveSnakeSlot);
  m_moveSnakeTimer->start(
      m_SnakeModel_->getSpeed());  // СЃРєРѕСЂРѕСЃС‚СЊ Р·РјРµР№РєРё
}

void s21::SnakeController::paintEvent(QPaintEvent* e) {
  Q_UNUSED(e)
  QPainter painter;
  painter.begin(this);

  if (user_action == Action) {
    QFont font("Arial", 20, 250);
    QFontMetrics fontMetrics(font);

    int textWidth = fontMetrics.horizontalAdvance("Game Over");
    int textHeight = fontMetrics.height();

    QRect textRect(width() / 2 - textWidth / 2, height() / 2 - textHeight / 2,
                   textWidth, textHeight);

    painter.setFont(font);
    painter.drawText(textRect, Qt::AlignHCenter | Qt::AlignVCenter,
                     "Game Over");
    m_moveSnakeTimer->stop();
    return;
  }

  painter.setBrush(Qt::white);
  painter.setOpacity(0.5);
  painter.drawRect(20, 20, width() - 40, height() - 20);

  painter.setOpacity(1);
  // СЂРёСЃСѓСЋ РіРѕР»РѕРІСѓ
  painter.setBrush(Qt::red);
  painter.drawEllipse(m_SnakeModel_->getHead().x * m_snakeItemSize,
                      m_SnakeModel_->getHead().y * m_snakeItemSize,
                      m_snakeItemSize, m_snakeItemSize);
  // СЂРёСЃСѓСЋ С…РІРѕСЃС‚
  painter.setBrush(Qt::cyan);
  for (size_t i = 0; i < m_SnakeModel_->getTail().size(); i++) {
    painter.drawEllipse(m_SnakeModel_->getTail()[i].x * m_snakeItemSize,
                        m_SnakeModel_->getTail()[i].y * m_snakeItemSize,
                        m_snakeItemSize, m_snakeItemSize);
  }

  // СЂРёСЃСѓСЋ СЏР±Р»РѕРєРѕ
  painter.setBrush(Qt::red);
  painter.drawEllipse(m_SnakeModel_->getApple().x * m_snakeItemSize,
                      m_SnakeModel_->getApple().y * m_snakeItemSize,
                      m_snakeItemSize, m_snakeItemSize);

  painter.end();
}

void s21::SnakeController::keyPressEvent(QKeyEvent* e) {
  if (e->key() == Qt::Key_Left) {
    user_action = Left;
  }
  if (e->key() == Qt::Key_Right) {
    user_action = Right;
  }
  if (e->key() == Qt::Key_Up) {
    user_action = Up;
  }
  if (e->key() == Qt::Key_Down) {
    user_action = Down;
  }
  if (e->key() == Qt::Key_Space) {
    user_action = Pause;
  }
}

void s21::SnakeController::MoveSnakeSlot() {
  switch (user_action) {
    case Left:
      if (dx != 1) {
        // РћР±СЂР°Р±РѕС‚РєР° РЅР°Р¶Р°С‚РёСЏ РєР»Р°РІРёС€Рё "Р’Р»РµРІРѕ"
        dx = -1;
        dy = 0;
      }
      break;
    case Right:
      if (dx != -1) {
        // РћР±СЂР°Р±РѕС‚РєР° РЅР°Р¶Р°С‚РёСЏ РєР»Р°РІРёС€Рё "Р’РїСЂР°РІРѕ"
        dx = 1;
        dy = 0;
      }
      break;
    case Up:
      if (dy != 1) {
        // РћР±СЂР°Р±РѕС‚РєР° РЅР°Р¶Р°С‚РёСЏ РєР»Р°РІРёС€Рё "Р’РІРµСЂС…"
        dx = 0;
        dy = -1;
      }
      break;
    case Down:
      if (dy != -1) {
        // РћР±СЂР°Р±РѕС‚РєР° РЅР°Р¶Р°С‚РёСЏ РєР»Р°РІРёС€Рё "Р’РЅРёР·"
        dx = 0;
        dy = 1;
      }
      break;
    case Pause:
      return;
      break;
    default:
      break;
  }

  if (m_SnakeModel_->moveSnake(dx, dy)) {
  } else {
    user_action = Action;
  }

  if (m_SnakeModel_->getEatenApples() % 5 == 0 &&
      m_SnakeModel_->getEatenApples() != prev_score) {
    m_SnakeModel_->increaseLevel();
    prev_score = m_SnakeModel_->getEatenApples();
  }

  m_moveSnakeTimer->start(m_SnakeModel_->getSpeed());
  repaint();
}