#include "tick_timer.h"

#include <algorithm>
#include <utility>

namespace s21 {

TickTimer::TickTimer() : TickTimer(Clock(&std::chrono::steady_clock::now)) {}

TickTimer::TickTimer(Clock clock)
    : clock_(std::move(clock)), deadline_(clock_()) {}

void TickTimer::Reset(int period_ms) {
  deadline_ = clock_() + std::chrono::milliseconds(period_ms);
}

int TickTimer::RemainingMs() const {
  const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
      deadline_ - clock_());
  return std::max(0, static_cast<int>(remaining.count()));
}

bool TickTimer::Due() const { return clock_() >= deadline_; }

}  // namespace s21
