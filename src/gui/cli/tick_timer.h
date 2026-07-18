#ifndef S21_GUI_CLI_TICK_TIMER_H_
#define S21_GUI_CLI_TICK_TIMER_H_

#include <chrono>
#include <functional>

namespace s21 {

class TickTimer {
 public:
  using TimePoint = std::chrono::steady_clock::time_point;
  using Clock = std::function<TimePoint()>;

  TickTimer();
  explicit TickTimer(Clock clock);

  void Reset(int period_ms);
  int RemainingMs() const;
  bool Due() const;          

 private:
  Clock clock_;
  TimePoint deadline_;
};

}  // namespace s21

#endif  // S21_GUI_CLI_TICK_TIMER_H_
