#include "s21_help_field.h"

#include "../../interface.h"

s21::HelpField::HelpField(SnakeModel* m_SnakeModel)
    : m_SnakeModel_(m_SnakeModel) {
  m_gameName = m_SnakeModel_->game_name;
  setFixedSize(CELL_SIZE / 2 * FIELD_WIDTH,
               CELL_SIZE * FIELD_HEIGHT + 40);  // CELL_SIZE

  m_moveSnakeTimer = new QTimer();
  connect(m_moveSnakeTimer, &QTimer::timeout, this, &HelpField::MoveSnakeSlot);
  m_moveSnakeTimer->start(50);  // СЃРєРѕСЂРѕСЃС‚СЊ Р·РјРµР№РєРё
}

void s21::HelpField::loadHighScore(int& high_score) {
  const char* fileName = (m_gameName == "TETRIS")
                             ? "../../record.txt"
                             : "../../high_score_snake.txt";
  FILE* file = fopen(fileName, "r");

  if (file != NULL) {
    fscanf(file, "%d", &high_score);
    fclose(file);

    m_SnakeModel_->setHighScore(high_score);
  }
}

void s21::HelpField::saveHighScore(int high_score) {
  const char* fileName = (m_gameName == "TETRIS")
                             ? "../../record.txt"
                             : "../../high_score_snake.txt";
  FILE* file = fopen(fileName, "w");

  if (file != NULL) {
    fprintf(file, "%d", high_score);
    fclose(file);
  }
}

void s21::HelpField::paintEvent(QPaintEvent* e) {
  Q_UNUSED(e)
  QPainter painter;
  painter.begin(this);
  painter.drawRect(0, 20, width() - 1, height() - 40);
  painter.setFont(QFont("Arial", 10, 700));
  loadHighScore(game_info.high_score);
  game_info.score = 0;
  int fig = m_SnakeModel_->getFig();

  painter.drawText(
      QRect(0, height() / 2 - 140, width(), 20), Qt::AlignCenter,
      "SCORE: " + QString::number(m_SnakeModel_->getEatenApples()));
  painter.drawText(QRect(0, height() / 2 - 90, width(), 20), Qt::AlignCenter,
                   "RECORD: " + QString::number(game_info.high_score));
  painter.drawText(QRect(0, height() / 2 - 40, width(), 20), Qt::AlignCenter,
                   "LEVEL: " + QString::number(m_SnakeModel_->getLevel()));
  painter.drawText(QRect(0, height() / 2 + 10, width(), 20), Qt::AlignCenter,
                   "SPEED: " + QString::number(m_SnakeModel_->getLevel()));
  painter.drawText(QRect(0, height() / 2 + 60, width(), 20), Qt::AlignCenter,
                   "PAUSE: SPACE");
  painter.drawText(QRect(0, height() / 2 + 110, width(), 20), Qt::AlignCenter,
                   "NEXT FIGURE");
  m_SnakeModel_->getFig();
  switch (fig) {
    case 0:
      painter.setBrush(Qt::white);
      painter.drawEllipse(2 * CELL_SIZE - 30, FIELD_HEIGHT * CELL_SIZE - 30,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE - 10, FIELD_HEIGHT * CELL_SIZE - 30,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE + 10, FIELD_HEIGHT * CELL_SIZE - 30,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE + 30, FIELD_HEIGHT * CELL_SIZE - 30,
                          CELL_SIZE, CELL_SIZE);
      break;
    case 1:
      painter.setBrush(Qt::white);
      painter.drawEllipse(2 * CELL_SIZE - 10, FIELD_HEIGHT * CELL_SIZE - 40,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE + 10, FIELD_HEIGHT * CELL_SIZE - 40,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE - 10, FIELD_HEIGHT * CELL_SIZE - 20,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE + 10, FIELD_HEIGHT * CELL_SIZE - 20,
                          CELL_SIZE, CELL_SIZE);
      break;
    case 2:
      painter.setBrush(Qt::white);
      painter.drawEllipse(2 * CELL_SIZE - 15, FIELD_HEIGHT * CELL_SIZE - 40,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE + 5, FIELD_HEIGHT * CELL_SIZE - 40,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE + 5, FIELD_HEIGHT * CELL_SIZE - 20,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE + 25, FIELD_HEIGHT * CELL_SIZE - 20,
                          CELL_SIZE, CELL_SIZE);
      break;
    case 3:
      painter.setBrush(Qt::white);
      painter.drawEllipse(2 * CELL_SIZE + 5, FIELD_HEIGHT * CELL_SIZE - 40,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE + 25, FIELD_HEIGHT * CELL_SIZE - 40,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE + 5, FIELD_HEIGHT * CELL_SIZE - 20,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE - 15, FIELD_HEIGHT * CELL_SIZE - 20,
                          CELL_SIZE, CELL_SIZE);
      break;
    case 4:
      painter.setBrush(Qt::white);
      painter.drawEllipse(2 * CELL_SIZE - 10, FIELD_HEIGHT * CELL_SIZE - 50,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE - 10, FIELD_HEIGHT * CELL_SIZE - 30,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE - 10, FIELD_HEIGHT * CELL_SIZE - 10,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE + 10, FIELD_HEIGHT * CELL_SIZE - 10,
                          CELL_SIZE, CELL_SIZE);
      break;
    case 5:
      painter.setBrush(Qt::white);
      painter.drawEllipse(2 * CELL_SIZE + 10, FIELD_HEIGHT * CELL_SIZE - 50,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE + 10, FIELD_HEIGHT * CELL_SIZE - 30,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE + 10, FIELD_HEIGHT * CELL_SIZE - 10,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE - 10, FIELD_HEIGHT * CELL_SIZE - 10,
                          CELL_SIZE, CELL_SIZE);
      break;
    case 6:
      painter.setBrush(Qt::white);
      painter.drawEllipse(2 * CELL_SIZE, FIELD_HEIGHT * CELL_SIZE - 40,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE, FIELD_HEIGHT * CELL_SIZE - 20,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE - 20, FIELD_HEIGHT * CELL_SIZE - 20,
                          CELL_SIZE, CELL_SIZE);
      painter.drawEllipse(2 * CELL_SIZE + 20, FIELD_HEIGHT * CELL_SIZE - 20,
                          CELL_SIZE, CELL_SIZE);

      break;
  }
  painter.end();
}

void s21::HelpField::MoveSnakeSlot() {
  if (m_SnakeModel_->getEatenApples() > m_SnakeModel_->getHighScore()) {
    saveHighScore(m_SnakeModel_->getEatenApples());
  }
  repaint();
}