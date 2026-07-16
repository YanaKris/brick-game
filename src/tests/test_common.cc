#include <gtest/gtest.h>

#include <memory>
#include <utility>
#include <vector>

#include "../brick_game/common/fsm.h"
#include "../brick_game/common/game_base.h"
#include "../brick_game/common/observer.h"

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

// Табличный тест: цепочка (событие -> ожидаемое состояние).
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
  // из kStart перехода по kResume нет — состояние не меняется
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

// --- Observer / RAII Subscription ---

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
  }  // sub разрушена — подписка должна сняться
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
  }  // inner разрушена, но подписка переехала в outer
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
  subject.reset();  // Subject умер раньше подписки
  sub.Reset();      // не должно упасть
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

}  // namespace
