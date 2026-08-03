#include <gtest/gtest.h>

#include <algorithm>
#include <cstdio>
#include <memory>
#include <utility>
#include <vector>

#include "../brick_game/common/high_score.h"
#include "../brick_game/snake/snake_game.h"
#include "../gui/desktop/game_presenter.h"

namespace {

using s21::GamePresenter;
using s21::GameState;
using s21::IGame;
using s21::Snake;
using s21::SnakeGame;
using s21::SnakeModel;

// Программируемый IGame: очередь шагов (счёт + флаг завершения), которые
// по одному отдаёт updateCurrentState, и журнал прокинутых действий.
class ScriptedGame : public IGame {
 public:
  struct Step {
    int score;
    bool finished;
  };

  explicit ScriptedGame(std::vector<Step> steps) : steps_(std::move(steps)) {}

  void userInput(UserAction_t action, bool) override {
    inputs.push_back(action);
  }

  GameInfo_t updateCurrentState() override {
    GameInfo_t info{};
    if (!steps_.empty()) {
      const Step& step = steps_[std::min(index_, steps_.size() - 1)];
      info.score = step.score;
      finished_ = step.finished;
    }
    ++index_;
    return info;
  }

  GameInfo_t render() override {
    GameInfo_t info{};
    if (!steps_.empty())
      info.score = steps_[std::min(index_, steps_.size() - 1)].score;
    return info;
  }

  GameState state() const override { return GameState::kMoving; }
  bool finished() const override { return finished_; }

  std::vector<UserAction_t> inputs;

 private:
  std::vector<Step> steps_;
  std::size_t index_ = 0;
  bool finished_ = false;
};

std::unique_ptr<IGame> Scripted(std::vector<ScriptedGame::Step> steps) {
  return std::make_unique<ScriptedGame>(std::move(steps));
}

SnakeModel MakeModel() {
  return SnakeModel(Snake(5, 10), {Snake(4, 10), Snake(3, 10), Snake(2, 10)},
                    Snake(1, 1));
}

GamePresenter SnakePresenter(const std::string& path) {
  return GamePresenter(std::make_unique<SnakeGame>(MakeModel()), path);
}

// Уникальный файл рекорда на тест; чистится в конструкторе и деструкторе.
class PresenterFile {
 public:
  explicit PresenterFile(const char* name) : path_(name) {
    std::remove(path_.c_str());
  }
  ~PresenterFile() { std::remove(path_.c_str()); }
  void Write(int value) const { s21::HighScore::Save(path_, value); }
  const std::string& path() const { return path_; }

 private:
  std::string path_;
};

// --- ввод/тик прокидываются в IGame ---

TEST(GamePresenterTest, BeginLoadsRecordAndStartsGame) {
  PresenterFile file("test_presenter_begin.txt");
  file.Write(42);
  auto* raw = new ScriptedGame({});
  GamePresenter presenter(std::unique_ptr<IGame>(raw), file.path());
  presenter.Begin();
  EXPECT_EQ(presenter.record(), 42);
  EXPECT_EQ(presenter.State(), GameState::kMoving);
  ASSERT_EQ(raw->inputs.size(), 1u);
  EXPECT_EQ(raw->inputs[0], Start);
}

TEST(GamePresenterTest, InputForwardsActionToGame) {
  PresenterFile file("test_presenter_input.txt");
  auto* raw = new ScriptedGame({{0, false}});
  GamePresenter presenter(std::unique_ptr<IGame>(raw), file.path());
  presenter.Begin();
  presenter.Tick();
  presenter.Input(Left);
  ASSERT_FALSE(raw->inputs.empty());
  EXPECT_EQ(raw->inputs.back(), Left);
}

// --- наложение рекорда на кадр ---

TEST(GamePresenterTest, TickOverlaysStoredRecordWhenHigher) {
  PresenterFile file("test_presenter_overlay.txt");
  file.Write(50);
  GamePresenter presenter(Scripted({{5, false}}), file.path());
  presenter.Begin();
  GameInfo_t info = presenter.Tick();
  EXPECT_EQ(info.score, 5);
  EXPECT_EQ(info.high_score, 50);
}

TEST(GamePresenterTest, TickShowsCurrentScoreAsHighScoreWhenItExceedsRecord) {
  PresenterFile file("test_presenter_overlay2.txt");
  file.Write(10);
  GamePresenter presenter(Scripted({{30, false}}), file.path());
  presenter.Begin();
  GameInfo_t info = presenter.Tick();
  EXPECT_EQ(info.high_score, 30);
}

TEST(GamePresenterTest, SnapshotReturnsLastTickInfo) {
  PresenterFile file("test_presenter_snap.txt");
  GamePresenter presenter(Scripted({{5, false}}), file.path());
  presenter.Begin();
  presenter.Tick();
  EXPECT_EQ(presenter.Snapshot().score, 5);
}

// --- сохранение рекорда при завершении ---

TEST(GamePresenterTest, SavesNewRecordOnFinish) {
  PresenterFile file("test_presenter_save.txt");
  file.Write(10);
  GamePresenter presenter(Scripted({{100, true}}), file.path());
  presenter.Begin();
  GameInfo_t info = presenter.Tick();
  EXPECT_EQ(info.score, 100);
  EXPECT_EQ(info.high_score, 100);
  EXPECT_EQ(s21::HighScore::Load(file.path()), 100);
  EXPECT_EQ(presenter.record(), 100);
}

TEST(GamePresenterTest, KeepsStoredRecordWhenFinishScoreIsLower) {
  PresenterFile file("test_presenter_keep.txt");
  file.Write(80);
  GamePresenter presenter(Scripted({{30, true}}), file.path());
  presenter.Begin();
  GameInfo_t info = presenter.Tick();
  EXPECT_EQ(info.high_score, 80);
  EXPECT_EQ(s21::HighScore::Load(file.path()), 80);
}

// --- после завершения тик/ввод больше не двигают игру ---

TEST(GamePresenterTest, TickAfterFinishDoesNotAdvance) {
  PresenterFile file("test_presenter_noadvance.txt");
  GamePresenter presenter(Scripted({{100, true}, {999, false}}), file.path());
  presenter.Begin();
  GameInfo_t first = presenter.Tick();   // завершает игру
  GameInfo_t second = presenter.Tick();  // no-op
  EXPECT_EQ(first.score, 100);
  EXPECT_EQ(second.score, 100);  // не 999 — второй шаг не выполнен
  EXPECT_TRUE(presenter.Finished());
}

TEST(GamePresenterTest, InputAfterFinishIsIgnored) {
  PresenterFile file("test_presenter_inputfin.txt");
  auto* raw = new ScriptedGame({{5, true}});
  GamePresenter presenter(std::unique_ptr<IGame>(raw), file.path());
  presenter.Begin();
  presenter.Tick();  // игра завершилась
  const std::size_t before = raw->inputs.size();
  presenter.Input(Left);
  EXPECT_EQ(raw->inputs.size(), before);  // ввод не прокинут
}

TEST(GamePresenterTest, SavesRecordOnlyOnceOnFinish) {
  PresenterFile file("test_presenter_once.txt");
  file.Write(0);
  GamePresenter presenter(Scripted({{100, true}}), file.path());
  presenter.Begin();
  presenter.Tick();
  // Портим файл: повторное сохранение presenter'ом перезаписало бы 7.
  s21::HighScore::Save(file.path(), 7);
  presenter.Tick();  // no-op, файл трогать не должен
  EXPECT_EQ(s21::HighScore::Load(file.path()), 7);
}

// --- интеграция с реальным SnakeGame ---

TEST(GamePresenterTest, IntegrationSnakeTickMovesHead) {
  PresenterFile file("test_presenter_snake1.txt");
  GamePresenter presenter = SnakePresenter(file.path());
  presenter.Begin();
  GameInfo_t info = presenter.Tick();
  ASSERT_NE(info.field, nullptr);
  EXPECT_EQ(info.field[9][5], 1);  // голова сместилась в (6,10)
}

TEST(GamePresenterTest, IntegrationSnakeInputTurns) {
  PresenterFile file("test_presenter_snake2.txt");
  GamePresenter presenter = SnakePresenter(file.path());
  presenter.Begin();
  presenter.Input(Up);
  GameInfo_t info = presenter.Tick();
  ASSERT_NE(info.field, nullptr);
  EXPECT_EQ(info.field[8][4], 1);  // голова ушла вверх: (5,9)
}

}  // namespace
