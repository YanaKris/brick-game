#include <gtest/gtest.h>

#include <array>
#include <memory>
#include <set>
#include <utility>
#include <vector>

#include "../brick_game/tetris/tetris_game.h"
#include "../brick_game/tetris/tetris_model.h"
#include "../brick_game/tetris/tetromino.h"

extern "C" {
#include "../brick_game/tetris/tetris.h"
}

namespace {

using s21::TetrisModel;
using s21::Tetromino;
using s21::TetrominoFactory;
using s21::TetrominoType;

constexpr int kH = TetrisModel::kFieldHeight;
constexpr int kW = TetrisModel::kFieldWidth;

struct ShapeCase {
  int legacy_fig;
  TetrominoType type;
  std::vector<std::pair<int, int>> cells;
};

const std::vector<ShapeCase>& AllShapes() {
  static const std::vector<ShapeCase> shapes = {
      {BAR, TetrominoType::kBar, {{1, 0}, {1, 1}, {1, 2}, {1, 3}}},
      {CUBE, TetrominoType::kCube, {{1, 1}, {2, 1}, {1, 2}, {2, 2}}},
      {S_TYPE, TetrominoType::kS, {{2, 1}, {2, 2}, {1, 2}, {1, 3}}},
      {Z_TYPE, TetrominoType::kZ, {{1, 1}, {1, 2}, {2, 2}, {2, 3}}},
      {L_TYPE, TetrominoType::kL, {{1, 1}, {2, 1}, {3, 1}, {3, 2}}},
      {J_TYPE, TetrominoType::kJ, {{1, 2}, {2, 2}, {3, 2}, {3, 1}}},
      {T_TYPE, TetrominoType::kT, {{1, 0}, {1, 1}, {1, 2}, {2, 1}}},
  };
  return shapes;
}

class CGame {
 public:
  CGame() {
    for (int i = 0; i < kH; ++i) {
      field_rows_[i] = field_cells_[i].data();
      next_rows_[i] = next_cells_[i].data();
    }
    info_.field = field_rows_.data();
    info_.next = next_rows_.data();
  }

  GameInfo_t* get() { return &info_; }
  int cell(int r, int c) const { return field_cells_[r][c]; }
  void set_cell(int r, int c, int v) { field_cells_[r][c] = v; }
  void FillRow(int r) {
    for (int j = 0; j < kW; ++j) field_cells_[r][j] = 1;
  }
  int CountCells() const {
    int count = 0;
    for (int i = 0; i < kH; ++i) {
      for (int j = 0; j < kW; ++j) count += field_cells_[i][j];
    }
    return count;
  }

 private:
  GameInfo_t info_{};
  std::array<std::array<int, kW>, kH> field_cells_{};
  std::array<std::array<int, kW>, kH> next_cells_{};
  std::array<int*, kH> field_rows_{};
  std::array<int*, kH> next_rows_{};
};

TEST(LegacyTetrisTest, TetrinoSetDefinesSevenShapes) {
  for (const ShapeCase& shape : AllShapes()) {
    Tetrino fig;
    tetrinoSet(&fig, shape.legacy_fig);
    int total = 0;
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) total += fig.current_fig[i][j];
    }
    EXPECT_EQ(total, 4) << "fig=" << shape.legacy_fig;
    for (const auto& [r, c] : shape.cells) {
      EXPECT_EQ(fig.current_fig[r][c], 1)
          << "fig=" << shape.legacy_fig << " cell=(" << r << "," << c << ")";
    }
  }
}

TEST(LegacyTetrisTest, SpawnPlacesBarAtTop) {
  CGame game;
  Tetrino fig;
  EXPECT_TRUE(spawnTetrino(game.get(), BAR, &fig));
  EXPECT_EQ(fig.i, -1);
  EXPECT_EQ(fig.j, 3);
  for (int c = 3; c <= 6; ++c) EXPECT_EQ(game.cell(0, c), 1);
  EXPECT_EQ(game.CountCells(), 4);
}

TEST(LegacyTetrisTest, SpawnFailsWhenTopIsOccupied) {
  CGame game;
  for (int c = 3; c <= 6; ++c) game.set_cell(0, c, 1);
  Tetrino fig;
  EXPECT_FALSE(spawnTetrino(game.get(), BAR, &fig));
}

TEST(LegacyTetrisTest, MoveDownShiftsFigureOneRow) {
  CGame game;
  Tetrino fig;
  spawnTetrino(game.get(), BAR, &fig);
  EXPECT_TRUE(tetrinoMoveDown(game.get(), &fig));
  EXPECT_EQ(fig.i, 0);
  for (int c = 3; c <= 6; ++c) {
    EXPECT_EQ(game.cell(0, c), 0);
    EXPECT_EQ(game.cell(1, c), 1);
  }
}

TEST(LegacyTetrisTest, MoveDownAttachesAtBottom) {
  CGame game;
  Tetrino fig;
  spawnTetrino(game.get(), BAR, &fig);
  int moves = 0;
  while (tetrinoMoveDown(game.get(), &fig)) ++moves;
  EXPECT_EQ(moves, 19);
  for (int c = 3; c <= 6; ++c) EXPECT_EQ(game.cell(19, c), 1);
  EXPECT_EQ(game.CountCells(), 4);
}

TEST(LegacyTetrisTest, MoveDownStopsOnStack) {
  CGame game;
  game.FillRow(19);
  Tetrino fig;
  spawnTetrino(game.get(), BAR, &fig);
  int moves = 0;
  while (tetrinoMoveDown(game.get(), &fig)) ++moves;
  EXPECT_EQ(moves, 18);
  for (int c = 3; c <= 6; ++c) EXPECT_EQ(game.cell(18, c), 1);
}

TEST(LegacyTetrisTest, MoveLeftStopsAtWall) {
  CGame game;
  Tetrino fig;
  spawnTetrino(game.get(), BAR, &fig);
  for (int step = 0; step < 3; ++step) tetrinoMoveLeft(game.get(), &fig);
  EXPECT_EQ(fig.j, 0);
  for (int c = 0; c <= 3; ++c) EXPECT_EQ(game.cell(0, c), 1);
  tetrinoMoveLeft(game.get(), &fig);  // в стену — без движения
  EXPECT_EQ(fig.j, 0);
  for (int c = 0; c <= 3; ++c) EXPECT_EQ(game.cell(0, c), 1);
}

TEST(LegacyTetrisTest, MoveRightStopsAtWall) {
  CGame game;
  Tetrino fig;
  spawnTetrino(game.get(), BAR, &fig);
  for (int step = 0; step < 3; ++step) tetrinoMoveRight(game.get(), &fig);
  EXPECT_EQ(fig.j, 6);
  for (int c = 6; c <= 9; ++c) EXPECT_EQ(game.cell(0, c), 1);
  tetrinoMoveRight(game.get(), &fig);  // в стену — без движения
  EXPECT_EQ(fig.j, 6);
  for (int c = 6; c <= 9; ++c) EXPECT_EQ(game.cell(0, c), 1);
}

TEST(LegacyTetrisTest, RotateTurnsBarVertical) {
  CGame game;
  Tetrino fig;
  spawnTetrino(game.get(), BAR, &fig);
  tetrinoMoveDown(game.get(), &fig);  // fig.i=0: место для поворота есть
  tetrinoRotate(game.get(), &fig);
  for (int r = 0; r <= 3; ++r) EXPECT_EQ(game.cell(r, 5), 1);
  EXPECT_EQ(game.CountCells(), 4);
}

TEST(LegacyTetrisTest, RotateBlockedAboveField) {
  CGame game;
  Tetrino fig;
  spawnTetrino(game.get(), BAR, &fig);  // fig.i=-1: повёрнутый BAR не влезет
  tetrinoRotate(game.get(), &fig);
  for (int c = 3; c <= 6; ++c) EXPECT_EQ(game.cell(0, c), 1);
  EXPECT_EQ(game.CountCells(), 4);
}

TEST(LegacyTetrisTest, DeleteRowsScoringTable) {
  const std::array<int, 5> expected_score = {0, 100, 300, 700, 1500};
  for (int rows = 1; rows <= 4; ++rows) {
    CGame game;
    for (int r = kH - rows; r < kH; ++r) game.FillRow(r);
    deleteRows(game.get());
    EXPECT_EQ(game.get()->score, expected_score[rows]) << "rows=" << rows;
    EXPECT_EQ(game.CountCells(), 0) << "rows=" << rows;
  }
}

TEST(LegacyTetrisTest, DeleteRowsShiftsStackDown) {
  CGame game;
  game.FillRow(19);
  game.set_cell(18, 0, 1);
  deleteRows(game.get());
  EXPECT_EQ(game.cell(19, 0), 1);
  EXPECT_EQ(game.cell(18, 0), 0);
  EXPECT_EQ(game.CountCells(), 1);
  EXPECT_EQ(game.get()->score, 100);
}

TEST(LegacyTetrisTest, LevelFormulaCapsAtTen) {
  CGame game;
  game.get()->score = 1200;
  updateCurrentStateTet(game.get(), 0);
  EXPECT_EQ(game.get()->level, 2);
  game.get()->score = 6601;
  updateCurrentStateTet(game.get(), 0);
  EXPECT_EQ(game.get()->level, 10);
}

TEST(TetrominoTest, FactoryShapesMatchLegacyTetrinoSet) {
  for (const ShapeCase& shape : AllShapes()) {
    std::unique_ptr<Tetromino> fig = TetrominoFactory::Create(shape.type);
    ASSERT_NE(fig, nullptr);
    EXPECT_EQ(fig->type(), shape.type);
    Tetrino legacy;
    tetrinoSet(&legacy, shape.legacy_fig);
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        EXPECT_EQ(fig->matrix()[i][j], legacy.current_fig[i][j])
            << "fig=" << shape.legacy_fig << " cell=(" << i << "," << j << ")";
      }
    }
  }
}

TEST(TetrominoTest, RotatedMatrixMatchesLegacyFormula) {
  std::unique_ptr<Tetromino> fig =
      TetrominoFactory::Create(TetrominoType::kBar);
  Tetromino::Matrix rotated = fig->RotatedMatrix();
  for (int r = 0; r <= 3; ++r) EXPECT_EQ(rotated[r][2], 1);
  int total = 0;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) total += rotated[i][j];
  }
  EXPECT_EQ(total, 4);
}

TEST(TetrominoTest, CreateRandomProducesValidShapes) {
  std::set<TetrominoType> seen;
  for (int draw = 0; draw < 50; ++draw) {
    std::unique_ptr<Tetromino> fig = TetrominoFactory::CreateRandom();
    ASSERT_NE(fig, nullptr);
    seen.insert(fig->type());
    int total = 0;
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) total += fig->matrix()[i][j];
    }
    EXPECT_EQ(total, 4);
  }
  EXPECT_GE(seen.size(), 2u);
}

TetrisModel::Field MakeFieldWithFullBottomRows(int rows) {
  TetrisModel::Field field{};
  for (int r = kH - rows; r < kH; ++r) {
    for (int c = 0; c < kW; ++c) field[r][c] = 1;
  }
  return field;
}

int CountCells(const TetrisModel::Field& field) {
  int count = 0;
  for (int i = 0; i < kH; ++i) {
    for (int j = 0; j < kW; ++j) count += field[i][j];
  }
  return count;
}

TEST(TetrisModelTest, SpawnPlacesBarAtTop) {
  TetrisModel model;
  EXPECT_TRUE(model.Spawn(TetrominoType::kBar));
  for (int c = 3; c <= 6; ++c) EXPECT_EQ(model.field()[0][c], 1);
  EXPECT_EQ(CountCells(model.field()), 4);
  EXPECT_TRUE(model.has_active_figure());
}

TEST(TetrisModelTest, SpawnFailsWhenTopIsOccupied) {
  TetrisModel::Field initial{};
  for (int c = 3; c <= 6; ++c) initial[0][c] = 1;
  TetrisModel model(initial);
  EXPECT_FALSE(model.Spawn(TetrominoType::kBar));
  EXPECT_EQ(CountCells(model.field()), 4);  // поле не тронуто
}

TEST(TetrisModelTest, SpawnRandomPlacesFourCells) {
  TetrisModel model;
  EXPECT_TRUE(model.SpawnRandom());
  EXPECT_EQ(CountCells(model.field()), 4);
}

TEST(TetrisModelTest, MoveDownShiftsFigureOneRow) {
  TetrisModel model;
  model.Spawn(TetrominoType::kBar);
  EXPECT_TRUE(model.MoveDown());
  for (int c = 3; c <= 6; ++c) {
    EXPECT_EQ(model.field()[0][c], 0);
    EXPECT_EQ(model.field()[1][c], 1);
  }
}

TEST(TetrisModelTest, MoveDownAttachesAtBottom) {
  TetrisModel model;
  model.Spawn(TetrominoType::kBar);
  int moves = 0;
  while (model.MoveDown()) ++moves;
  EXPECT_EQ(moves, 19);
  for (int c = 3; c <= 6; ++c) EXPECT_EQ(model.field()[19][c], 1);
  EXPECT_EQ(CountCells(model.field()), 4);
  EXPECT_FALSE(model.has_active_figure());
}

TEST(TetrisModelTest, MoveDownStopsOnStack) {
  TetrisModel model(MakeFieldWithFullBottomRows(1));
  model.Spawn(TetrominoType::kBar);
  int moves = 0;
  while (model.MoveDown()) ++moves;
  EXPECT_EQ(moves, 18);
  for (int c = 3; c <= 6; ++c) EXPECT_EQ(model.field()[18][c], 1);
}

TEST(TetrisModelTest, AttachedFigureIgnoresInput) {
  TetrisModel model;
  model.Spawn(TetrominoType::kBar);
  while (model.MoveDown()) {
  }
  TetrisModel::Field before = model.field();
  model.MoveLeft();
  model.MoveRight();
  model.Rotate();
  EXPECT_EQ(model.field(), before);
}

TEST(TetrisModelTest, MoveLeftStopsAtWall) {
  TetrisModel model;
  model.Spawn(TetrominoType::kBar);
  for (int step = 0; step < 3; ++step) model.MoveLeft();
  for (int c = 0; c <= 3; ++c) EXPECT_EQ(model.field()[0][c], 1);
  model.MoveLeft();
  for (int c = 0; c <= 3; ++c) EXPECT_EQ(model.field()[0][c], 1);
  EXPECT_EQ(CountCells(model.field()), 4);
}

TEST(TetrisModelTest, MoveRightStopsAtWall) {
  TetrisModel model;
  model.Spawn(TetrominoType::kBar);
  for (int step = 0; step < 3; ++step) model.MoveRight();
  for (int c = 6; c <= 9; ++c) EXPECT_EQ(model.field()[0][c], 1);
  model.MoveRight();
  for (int c = 6; c <= 9; ++c) EXPECT_EQ(model.field()[0][c], 1);
  EXPECT_EQ(CountCells(model.field()), 4);
}

TEST(TetrisModelTest, RotateTurnsBarVertical) {
  TetrisModel model;
  model.Spawn(TetrominoType::kBar);
  model.MoveDown();
  model.Rotate();
  for (int r = 0; r <= 3; ++r) EXPECT_EQ(model.field()[r][5], 1);
  EXPECT_EQ(CountCells(model.field()), 4);
}

TEST(TetrisModelTest, RotateBlockedAboveField) {
  TetrisModel model;
  model.Spawn(TetrominoType::kBar);
  model.Rotate();
  for (int c = 3; c <= 6; ++c) EXPECT_EQ(model.field()[0][c], 1);
  EXPECT_EQ(CountCells(model.field()), 4);
}

TEST(TetrisModelTest, ClearFullRowsScoringTable) {
  const std::array<int, 5> expected_score = {0, 100, 300, 700, 1500};
  for (int rows = 1; rows <= 4; ++rows) {
    TetrisModel model(MakeFieldWithFullBottomRows(rows));
    EXPECT_EQ(model.ClearFullRows(), rows);
    EXPECT_EQ(model.score(), expected_score[rows]) << "rows=" << rows;
    EXPECT_EQ(CountCells(model.field()), 0) << "rows=" << rows;
  }
}

TEST(TetrisModelTest, ClearRowsShiftsStackDown) {
  TetrisModel::Field initial = MakeFieldWithFullBottomRows(1);
  initial[18][0] = 1;
  TetrisModel model(initial);
  EXPECT_EQ(model.ClearFullRows(), 1);
  EXPECT_EQ(model.field()[19][0], 1);
  EXPECT_EQ(model.field()[18][0], 0);
  EXPECT_EQ(model.score(), 100);
}

TEST(TetrisModelTest, ScoreAccumulatesAndLevelGrows) {
  TetrisModel model(MakeFieldWithFullBottomRows(4));
  model.ClearFullRows();
  EXPECT_EQ(model.score(), 1500);
  EXPECT_EQ(model.level(), 2);
}

TEST(TetrisModelTest, ScoreForMatchesLegacyTable) {
  EXPECT_EQ(TetrisModel::ScoreFor(0), 0);
  EXPECT_EQ(TetrisModel::ScoreFor(1), 100);
  EXPECT_EQ(TetrisModel::ScoreFor(2), 300);
  EXPECT_EQ(TetrisModel::ScoreFor(3), 700);
  EXPECT_EQ(TetrisModel::ScoreFor(4), 1500);
}

TEST(TetrisModelTest, LevelForCapsAtTen) {
  EXPECT_EQ(TetrisModel::LevelFor(0), 0);
  EXPECT_EQ(TetrisModel::LevelFor(599), 0);
  EXPECT_EQ(TetrisModel::LevelFor(600), 1);
  EXPECT_EQ(TetrisModel::LevelFor(1200), 2);
  EXPECT_EQ(TetrisModel::LevelFor(6601), 10);
}

TEST(TetrisModelTest, SpawnAcceptsInjectedFigure) {
  TetrisModel model;
  EXPECT_TRUE(model.Spawn(TetrominoFactory::Create(TetrominoType::kBar)));
  for (int c = 3; c <= 6; ++c) EXPECT_EQ(model.field()[0][c], 1);
}

using s21::GameState;
using s21::TetrisGame;

TetrisGame::FigureGenerator BarGenerator() {
  return [] { return TetrominoFactory::Create(TetrominoType::kBar); };
}

int CountInfoCells(const GameInfo_t& info) {
  int count = 0;
  for (int r = 0; r < kH; ++r) {
    for (int c = 0; c < kW; ++c) count += info.field[r][c] != 0 ? 1 : 0;
  }
  return count;
}

TEST(TetrisGameTest, StartsInStartState) {
  TetrisGame game;
  EXPECT_EQ(game.state(), GameState::kStart);
  EXPECT_FALSE(game.finished());
}

TEST(TetrisGameTest, StartMovesToSpawnState) {
  TetrisGame game(BarGenerator());
  game.userInput(Start, false);
  EXPECT_EQ(game.state(), GameState::kSpawn);
}

TEST(TetrisGameTest, FirstTickSpawnsFigure) {
  TetrisGame game(BarGenerator());
  game.userInput(Start, false);
  GameInfo_t info = game.updateCurrentState();
  EXPECT_EQ(game.state(), GameState::kMoving);
  for (int c = 3; c <= 6; ++c) EXPECT_EQ(info.field[0][c], 1);
  EXPECT_EQ(CountInfoCells(info), 4);
}

TEST(TetrisGameTest, TickDropsFigureOneRow) {
  TetrisGame game(BarGenerator());
  game.userInput(Start, false);
  game.updateCurrentState();                  
  GameInfo_t info = game.updateCurrentState();
  for (int c = 3; c <= 6; ++c) EXPECT_EQ(info.field[1][c], 1);
  EXPECT_EQ(CountInfoCells(info), 4);
}

TEST(TetrisGameTest, LeftShiftsFigure) {
  TetrisGame game(BarGenerator());
  game.userInput(Start, false);
  game.updateCurrentState(); 
  game.userInput(Left, false);
  GameInfo_t info = game.updateCurrentState();
  for (int c = 2; c <= 5; ++c) EXPECT_EQ(info.field[1][c], 1);
}

TEST(TetrisGameTest, InputRefreshesFieldBufferImmediately) {
  TetrisGame game(BarGenerator());
  game.userInput(Start, false);
  GameInfo_t info = game.updateCurrentState();
  game.userInput(Left, false);
  EXPECT_EQ(info.field[0][2], 1);
  EXPECT_EQ(info.field[0][6], 0);
}

TEST(TetrisGameTest, RightShiftsFigure) {
  TetrisGame game(BarGenerator());
  game.userInput(Start, false);
  game.updateCurrentState();
  game.userInput(Right, false);
  GameInfo_t info = game.updateCurrentState();
  for (int c = 4; c <= 7; ++c) EXPECT_EQ(info.field[1][c], 1);
}

TEST(TetrisGameTest, ActionRotatesFigure) {
  TetrisGame game(BarGenerator());
  game.userInput(Start, false);
  game.updateCurrentState();
  game.updateCurrentState();
  game.userInput(Action, false);
  GameInfo_t info = game.updateCurrentState();
  for (int r = 1; r <= 4; ++r) EXPECT_EQ(info.field[r][5], 1);
  EXPECT_EQ(CountInfoCells(info), 4);
}

TEST(TetrisGameTest, DownDropsImmediately) {
  TetrisGame game(BarGenerator());
  game.userInput(Start, false);
  game.updateCurrentState();
  game.userInput(Down, false);
  GameInfo_t info = game.updateCurrentState();
  for (int c = 3; c <= 6; ++c) EXPECT_EQ(info.field[2][c], 1);
}

TEST(TetrisGameTest, PauseStopsFalling) {
  TetrisGame game(BarGenerator());
  game.userInput(Start, false);
  game.updateCurrentState();
  game.userInput(Pause, false);
  EXPECT_EQ(game.state(), GameState::kPause);
  GameInfo_t info = game.updateCurrentState();
  EXPECT_EQ(info.pause, 1);
  for (int c = 3; c <= 6; ++c) EXPECT_EQ(info.field[0][c], 1);
  game.userInput(Pause, false);
  EXPECT_EQ(game.state(), GameState::kMoving);
}

TEST(TetrisGameTest, LandingSpawnsNextFigure) {
  TetrisGame game(BarGenerator());
  game.userInput(Start, false);
  int cells = 0;
  for (int tick = 0; tick < 30 && cells != 8; ++tick) {
    cells = CountInfoCells(game.updateCurrentState());
  }
  EXPECT_EQ(cells, 8);
  EXPECT_EQ(game.state(), GameState::kMoving);
}

TEST(TetrisGameTest, ClearingRowAddsScore) {
  TetrisModel::Field initial{};
  for (int c = 0; c < kW; ++c) {
    if (c < 3 || c > 6) initial[kH - 1][c] = 1;
  }
  TetrisGame game(TetrisModel(initial), BarGenerator());
  game.userInput(Start, false);
  int score = 0;
  for (int tick = 0; tick < 30 && score != 100; ++tick) {
    score = game.updateCurrentState().score;
  }
  EXPECT_EQ(score, 100);
  EXPECT_FALSE(game.finished());
}

TEST(TetrisGameTest, BlockedSpawnEndsGame) {
  TetrisModel::Field initial{};
  for (int r = 0; r <= 2; ++r) {
    for (int c = 3; c <= 6; ++c) initial[r][c] = 1;
  }
  TetrisGame game(TetrisModel(initial), BarGenerator());
  game.userInput(Start, false);
  game.updateCurrentState();
  EXPECT_EQ(game.state(), GameState::kGameOver);
  EXPECT_TRUE(game.finished());
}

}  // namespace
