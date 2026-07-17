#include "view.h"

#include "ui_view.h"

s21::View::View(QWidget* parent) : QMainWindow(parent), ui(new Ui::View) {
  ui->setupUi(this);
}

s21::View::~View() { delete ui; }

void s21::View::on_pushButton_clicked() {
  setFixedSize(CELL_SIZE * FIELD_WIDTH * 2 + 60, CELL_SIZE * FIELD_HEIGHT + 53);

  m_SnakeModel = new SnakeModel();
  m_SnakeModel->game_name = "TETRIS";

  m_TetrisController = new TetrisController(m_SnakeModel);
  m_helpField = new HelpField(m_SnakeModel);

  m_centralWidget = new QWidget();
  m_gridLayout = new QGridLayout();

  m_gridLayout->addWidget(m_TetrisController, 0, 0);
  m_gridLayout->addWidget(m_helpField, 0, 1);

  m_centralWidget->setLayout(m_gridLayout);
  setCentralWidget(m_centralWidget);

  m_TetrisController->setFocus();
}

void s21::View::on_pushButton_2_clicked() {
  // СЂР°Р·РјРµСЂ РѕСЃРЅРѕРІРЅРѕРіРѕ РѕРєРЅР°
  setFixedSize(CELL_SIZE * FIELD_WIDTH * 2 + 60, CELL_SIZE * FIELD_HEIGHT + 53);

  m_SnakeModel = new SnakeModel();
  m_SnakeModel->game_name = "SNAKE";

  m_SnakeController = new SnakeController(m_SnakeModel);
  m_helpField = new HelpField(m_SnakeModel);

  m_centralWidget = new QWidget();
  m_gridLayout = new QGridLayout();

  m_gridLayout->addWidget(m_SnakeController, 0, 0);
  m_gridLayout->addWidget(m_helpField, 0, 1);

  m_centralWidget->setLayout(m_gridLayout);
  setCentralWidget(m_centralWidget);

  m_SnakeController->setFocus();
}

void s21::View::on_pushButton_3_clicked() { this->close(); }
