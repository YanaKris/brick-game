#include <gtest/gtest.h>

#include <chrono>
#include <cstdio>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../brick_game/common/field_buffer.h"
#include "../brick_game/common/fsm.h"
#include "../brick_game/common/game_base.h"
#include "../brick_game/common/high_score.h"
#include "../brick_game/common/observer.h"
#include "../gui/cli/tick_timer.h"

namespace {

using s21::Fsm;
using s21::GameState;
using s21::IGame;
using s21::Subject;
using s21::Subscription;

// --- FSM ---

enum class Event { kGo, kPause, kResume, kDie };

Fsm<Event> MakeFsm() {
  return Fsm<Event>(GameState::kStart,
                    {{GameState::kStart, Event::kGo, GameState::kMoving},
                     {GameState::kMoving, Event::kPause, GameState::kPause},
                     {GameState::kPause, Event::kResume, GameState::kMoving},
                     {GameState::kMoving, Event::kDie, GameState::kGameOver}});
}

TEST(FsmTest, StartsInInitialState) {
  Fsm<Event> fsm = MakeFsm();
  EXPECT_EQ(fsm.state(), GameState::kStart);
}

TEST(FsmTest, FollowsTransitionTable) {
  struct Step {
    Event event;
    GameState expected;
  };
  const std::vector<Step> steps = {{Event::kGo, GameState::kMoving},
                                   {Event::kPause, GameState::kPause},
                                   {Event::kResume, GameState::kMoving},
                                   {Event::kDie, GameState::kGameOver}};
  Fsm<Event> fsm = MakeFsm();
  for (const Step& step : steps) {
    EXPECT_TRUE(fsm.Dispatch(step.event));
    EXPECT_EQ(fsm.state(), step.expected);
  }
}

TEST(FsmTest, IgnoresUnknownEvent) {
  Fsm<Event> fsm = MakeFsm();
  EXPECT_FALSE(fsm.Dispatch(Event::kResume));
  EXPECT_EQ(fsm.state(), GameState::kStart);
}

TEST(FsmTest, TerminalStateHasNoTransitions) {
  Fsm<Event> fsm = MakeFsm();
  fsm.Dispatch(Event::kGo);
  fsm.Dispatch(Event::kDie);
  ASSERT_EQ(fsm.state(), GameState::kGameOver);
  EXPECT_FALSE(fsm.Dispatch(Event::kGo));
  EXPECT_EQ(fsm.state(), GameState::kGameOver);
}

TEST(ObserverTest, NotifyReachesSubscriber) {
  Subject<int> subject;
  int received = 0;
  Subscription sub = subject.Subscribe([&received](int v) { received = v; });
  subject.Notify(42);
  EXPECT_EQ(received, 42);
}

TEST(ObserverTest, SubscriptionUnsubscribesInDestructor) {
  Subject<int> subject;
  int calls = 0;
  {
    Subscription sub = subject.Subscribe([&calls](int) { ++calls; });
    subject.Notify(1);
  }
  subject.Notify(2);
  EXPECT_EQ(calls, 1);
  EXPECT_EQ(subject.observer_count(), 0u);
}

TEST(ObserverTest, MovedSubscriptionStaysActive) {
  Subject<int> subject;
  int calls = 0;
  Subscription outer;
  {
    Subscription inner = subject.Subscribe([&calls](int) { ++calls; });
    outer = std::move(inner);
  }
  subject.Notify(1);
  EXPECT_EQ(calls, 1);
  EXPECT_TRUE(outer.active());
}

TEST(ObserverTest, ResetUnsubscribesEarly) {
  Subject<int> subject;
  int calls = 0;
  Subscription sub = subject.Subscribe([&calls](int) { ++calls; });
  sub.Reset();
  subject.Notify(1);
  EXPECT_EQ(calls, 0);
  EXPECT_FALSE(sub.active());
}

TEST(ObserverTest, SubscriptionOutlivingSubjectIsSafe) {
  auto subject = std::make_unique<Subject<int>>();
  Subscription sub = subject->Subscribe([](int) {});
  subject.reset();
  sub.Reset();
  SUCCEED();
}

TEST(ObserverTest, AllObserversNotified) {
  Subject<int> subject;
  int first = 0;
  int second = 0;
  Subscription sub_a = subject.Subscribe([&first](int v) { first += v; });
  Subscription sub_b = subject.Subscribe([&second](int v) { second += v; });
  EXPECT_EQ(subject.observer_count(), 2u);
  subject.Notify(5);
  EXPECT_EQ(first, 5);
  EXPECT_EQ(second, 5);
}

// --- FieldBuffer ---

TEST(FieldBufferTest, StartsZeroed) {
  s21::FieldBuffer buffer;
  ASSERT_NE(buffer.data(), nullptr);
  int total = 0;
  for (int r = 0; r < s21::FieldBuffer::kHeight; ++r) {
    for (int c = 0; c < s21::FieldBuffer::kWidth; ++c) {
      total += buffer.data()[r][c];
    }
  }
  EXPECT_EQ(total, 0);
}

TEST(FieldBufferTest, WritableAndClearable) {
  s21::FieldBuffer buffer;
  buffer.data()[5][7] = 3;
  EXPECT_EQ(buffer.data()[5][7], 3);
  buffer.Clear();
  EXPECT_EQ(buffer.data()[5][7], 0);
}

// --- IGame (Strategy-контракт) ---

class FakeGame : public IGame {
 public:
  void userInput(UserAction_t action, bool hold) override {
    last_action_ = action;
    last_hold_ = hold;
  }
  GameInfo_t updateCurrentState() override {
    GameInfo_t info{};
    info.score = 7;
    return info;
  }
  GameState state() const override { return GameState::kMoving; }
  bool finished() const override { return false; }

  UserAction_t last_action_ = Start;
  bool last_hold_ = false;
};

TEST(GameBaseTest, PolymorphicUseThroughInterface) {
  std::unique_ptr<IGame> game = std::make_unique<FakeGame>();
  game->userInput(Left, true);
  GameInfo_t info = game->updateCurrentState();
  EXPECT_EQ(info.score, 7);
  EXPECT_EQ(game->state(), GameState::kMoving);
  EXPECT_FALSE(game->finished());
  auto* fake = static_cast<FakeGame*>(game.get());
  EXPECT_EQ(fake->last_action_, Left);
  EXPECT_TRUE(fake->last_hold_);
}

TEST(HighScoreTest, LoadMissingFileReturnsZero) {
  EXPECT_EQ(s21::HighScore::Load("no_such_high_score_file.txt"), 0);
}

TEST(HighScoreTest, SaveThenLoadRoundTrips) {
  const std::string path = "test_high_score_tmp.txt";
  s21::HighScore::Save(path, 42);
  EXPECT_EQ(s21::HighScore::Load(path), 42);
  std::remove(path.c_str());
}

TEST(HighScoreTest, SaveOverwritesPreviousValue) {
  const std::string path = "test_high_score_tmp2.txt";
  s21::HighScore::Save(path, 5);
  s21::HighScore::Save(path, 9);
  EXPECT_EQ(s21::HighScore::Load(path), 9);
  std::remove(path.c_str());
}

std::chrono::steady_clock::time_point FakeNow(int ms) {
  return std::chrono::steady_clock::time_point{} +
         std::chrono::milliseconds(ms);
}

TEST(TickTimerTest, CountsDownAndFires) {
  int now_ms = 0;
  s21::TickTimer timer([&now_ms] { return FakeNow(now_ms); });
  timer.Reset(300);
  EXPECT_EQ(timer.RemainingMs(), 300);
  EXPECT_FALSE(timer.Due());
  now_ms += 100;
  EXPECT_EQ(timer.RemainingMs(), 200);
  EXPECT_FALSE(timer.Due());
  now_ms += 200;
  EXPECT_TRUE(timer.Due());
}

TEST(TickTimerTest, RemainingIsNeverNegative) {
  int now_ms = 0;
  s21::TickTimer timer([&now_ms] { return FakeNow(now_ms); });
  timer.Reset(50);
  now_ms += 120;
  EXPECT_TRUE(timer.Due());
  EXPECT_EQ(timer.RemainingMs(), 0);
}

TEST(TickTimerTest, ResetStartsNewPeriod) {
  int now_ms = 0;
  s21::TickTimer timer([&now_ms] { return FakeNow(now_ms); });
  timer.Reset(100);
  now_ms += 100;
  ASSERT_TRUE(timer.Due());
  timer.Reset(200);
  EXPECT_FALSE(timer.Due());
  EXPECT_EQ(timer.RemainingMs(), 200);
}

TEST(TickTimerTest, DefaultClockIsUsableImmediately) {
  s21::TickTimer timer;
  timer.Reset(10000);
  EXPECT_FALSE(timer.Due());
  EXPECT_GT(timer.RemainingMs(), 9000);
}

}  // namespace
