#include "view.h"

#include "ui_view.h"

s21::View::View(QWidget* parent) : QMainWindow(parent), ui(new Ui::View) {
  ui->setupUi(this);
}

s21::View::~View() { delete ui; }

void s21::View::on_pushButton_clicked() {
  setFixedSize(CELL_SIZE * FIELD_WIDTH * 2 + 60, CELL_SIZE * FIELD_HEIGHT + 53);

  m_tetrisView = new TetrisView();
  m_TetrisController = new TetrisController(m_tetrisView, m_tetrisView);
  connect(m_TetrisController, &TetrisController::ExitToMenu, this,
          &View::ReturnToMenu, Qt::QueuedConnection);

  m_centralWidget = new QWidget();
  m_gridLayout = new QGridLayout();
  m_gridLayout->addWidget(m_tetrisView, 0, 0);

  m_centralWidget->setLayout(m_gridLayout);
  ShowGameScreen(m_centralWidget);

  m_tetrisView->setFocus();
}

void s21::View::on_pushButton_2_clicked() {
  setFixedSize(CELL_SIZE * FIELD_WIDTH * 2 + 60, CELL_SIZE * FIELD_HEIGHT + 53);

  m_snakeView = new SnakeView();
  m_snakeController = new SnakeController(m_snakeView, m_snakeView);
  connect(m_snakeController, &SnakeController::ExitToMenu, this,
          &View::ReturnToMenu, Qt::QueuedConnection);

  m_centralWidget = new QWidget();
  m_gridLayout = new QGridLayout();
  m_gridLayout->addWidget(m_snakeView, 0, 0);

  m_centralWidget->setLayout(m_gridLayout);
  ShowGameScreen(m_centralWidget);

  m_snakeView->setFocus();
}

void s21::View::on_pushButton_3_clicked() { this->close(); }

void s21::View::ShowGameScreen(QWidget* screen) {
  if (m_menu == nullptr) m_menu = takeCentralWidget();
  setCentralWidget(screen);
}

void s21::View::ReturnToMenu() {
  if (m_menu == nullptr) return;
  setCentralWidget(m_menu);
  m_menu = nullptr;
  setFixedSize(362, 220);
}
