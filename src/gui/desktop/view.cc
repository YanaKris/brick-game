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
  ShowGameScreen(m_centralWidget);

  m_TetrisController->setFocus();
}

void s21::View::on_pushButton_2_clicked() {
  // Змейка на новом MVP: dumb SnakeView + тонкий SnakeController над presenter.
  setFixedSize(CELL_SIZE * FIELD_WIDTH * 2 + 60, CELL_SIZE * FIELD_HEIGHT + 53);

  m_snakeView = new SnakeView();
  // Контроллер — потомок view: при смене экрана Qt удалит их вместе,
  // и таймер не дёрнет висячий указатель.
  m_snakeController = new SnakeController(m_snakeView, m_snakeView);
  // Queued: возврат удаляет SnakeView вместе с контроллером, который сейчас
  // эмитит сигнал, — прямой вызов дал бы use-after-free.
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
  // takeCentralWidget убирает меню из окна, НЕ удаляя его: кнопки и их
  // соединения живы, меню вернётся в ReturnToMenu.
  if (m_menu == nullptr) m_menu = takeCentralWidget();
  setCentralWidget(screen);
}

void s21::View::ReturnToMenu() {
  if (m_menu == nullptr) return;
  setCentralWidget(m_menu);  // удаляет текущий игровой экран, возвращает меню
  m_menu = nullptr;
  setFixedSize(362, 220);
}
