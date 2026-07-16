#include "s21_tetris_controller.h"

s21::TetrisController::TetrisController(GameModel* m_gameModel)
    : m_gameModel_(m_gameModel) {
  setFixedSize(CELL_SIZE * FIELD_WIDTH + 40, CELL_SIZE * FIELD_HEIGHT + 40);
  setFocusPolicy(Qt::StrongFocus);  // подключение кнопок
  m_tetrisItemSize = CELL_SIZE;
  m_tetrino = new Tetrino();
  m_gameInfo = new GameInfo_t();
  // выделение памяти для полей
  m_gameInfo->field = new int*[20];
  for (int i = 0; i < 20; i++) {
    m_gameInfo->field[i] = new int[10];
    for (int j = 0; j < 10; j++) {
      m_gameInfo->field[i][j] = 0;
    }
  }
  m_gameInfo->next = new int*[20];
  for (int i = 0; i < 20; i++) {
    m_gameInfo->next[i] = new int[10];
    for (int j = 0; j < 10; j++) {
      m_gameInfo->next[i][j] = 0;
    }
  }
  m_gameInfo->speed = 400;
  m_gameInfo->score = 0;
  m_gameInfo->level = 0;
  m_gameInfo->high_score = 0;
  currentFig = rand() % 7;

  m_moveTetrisTimer = new QTimer();
  connect(m_moveTetrisTimer, &QTimer::timeout, this,
          &TetrisController::MoveTetrisSlot);
  m_moveTetrisTimer->start(m_gameInfo->speed);  // скорость падения фигуры
}

void s21::TetrisController::paintEvent(QPaintEvent* e) {
  Q_UNUSED(e)
  QPainter painter;
  painter.begin(this);

  // блок для вывода текста проигрыша
  if (m_user_action == Terminate) {
    QFont font("Arial", 20, 250);
    QFontMetrics fontMetrics(font);
    int textWidth = fontMetrics.horizontalAdvance("Game Over");
    int textHeight = fontMetrics.height();
    QRect textRect(width() / 2 - textWidth / 2, height() / 2 - textHeight / 2,
                   textWidth, textHeight);
    painter.setFont(font);
    painter.drawText(textRect, Qt::AlignHCenter | Qt::AlignVCenter,
                     "Game Over");
    return;
  }

  painter.setBrush(Qt::white);
  painter.setOpacity(0.5);
  painter.drawRect(0, 0, width() - 40, height() - 40);
  painter.setOpacity(1);
  // рисую тетрино
  painter.setBrush(Qt::red);
  printField(m_gameInfo->field, 0, 0, FIELD_WIDTH, FIELD_HEIGHT);
  painter.end();
}

void s21::TetrisController::keyPressEvent(QKeyEvent* e) {
  if (e->key() == Qt::Key_Left) {
    m_user_action = Left;
  }
  if (e->key() == Qt::Key_Right) {
    m_user_action = Right;
  }
  if (e->key() == Qt::Key_Up) {
    m_user_action = Up;
  }
  if (e->key() == Qt::Key_Down) {
    m_user_action = Down;
  }
  if (e->key() == Qt::Key_Space) {
    m_user_action = Pause;
  }
  if (e->key() == Qt::Key_Slash) {
    m_user_action = Action;
  }
  if (e->key() == Qt::Key_Enter) {
    m_user_action = Start;
  }
}

void s21::TetrisController::MoveTetrisSlot() {
  if (m_user_action == Pause) {
    return;
  }
  m_moveTetrisTimer->start(m_gameInfo->speed);

  srand(time(nullptr));

  if (spawn == false) {
    deleteRows(m_gameInfo);
    work = ::spawnTetrino(m_gameInfo, currentFig, m_tetrino);
    if (work == false) {
      m_user_action = Terminate;
      repaint();
      m_moveTetrisTimer->stop();
      return;
    }
    m_gameModel_->setFig(rand() % 7);
    currentFig = m_gameModel_->getFig();
    spawn = true;
  }
  spawn = ::userInputTet(m_gameInfo, m_user_action, m_tetrino);

  if (::tetrinoMoveDown(m_gameInfo, m_tetrino) == false) {
    spawn = false;
  }
  m_user_action = Start;
  repaint();

  m_gameModel_->setScore(m_gameInfo->score);
  m_gameModel_->setLevel(m_gameInfo->level);
  m_gameModel_->setHighScore(m_gameInfo->high_score);
  m_gameModel_->setSpeed(m_gameInfo->speed);
}

void s21::TetrisController::printField(int** field, int x, int y, int w,
                                       int h) {
  if (field) {
    QPainter painter(this);
    for (int i = 0; i < h; ++i) {
      for (int j = 0; j < w; ++j) {
        if (field[i][j]) {
          painter.setBrush(Qt::red);
          painter.drawEllipse((x + j) * CELL_SIZE, (y + i) * CELL_SIZE,
                              CELL_SIZE, CELL_SIZE);
        }
      }
    }
  }
}

void s21::TetrisController::increaseLevel() {
  if (m_gameInfo->level < 10) {
    m_gameInfo->speed -= 20;
  }
}
