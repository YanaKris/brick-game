#ifndef S21_BRICK_GAME_COMMON_OBSERVER_H_
#define S21_BRICK_GAME_COMMON_OBSERVER_H_

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <utility>

namespace s21 {

// RAII-подписка в стиле scoped_connection из Boost.Signals2:
// move-only, отписывается в деструкторе.
class Subscription {
 public:
  Subscription() = default;
  explicit Subscription(std::function<void()> unsubscribe)
      : unsubscribe_(std::move(unsubscribe)) {}
  ~Subscription() { Reset(); }

  Subscription(Subscription&& other) noexcept
      : unsubscribe_(std::move(other.unsubscribe_)) {
    other.unsubscribe_ = nullptr;
  }
  Subscription& operator=(Subscription&& other) noexcept {
    if (this != &other) {
      Reset();
      unsubscribe_ = std::move(other.unsubscribe_);
      other.unsubscribe_ = nullptr;
    }
    return *this;
  }
  Subscription(const Subscription&) = delete;
  Subscription& operator=(const Subscription&) = delete;

  // Досрочная отписка; повторный вызов — no-op.
  void Reset() {
    if (unsubscribe_) {
      std::function<void()> fn = std::move(unsubscribe_);
      unsubscribe_ = nullptr;
      fn();
    }
  }

  bool active() const { return static_cast<bool>(unsubscribe_); }

 private:
  std::function<void()> unsubscribe_;
};

// Subject уведомляет подписчиков; подписка живёт, пока жив
// возвращённый Subscription. Реестр колбэков — под shared_ptr,
// а Subscription держит weak_ptr, поэтому любой порядок
// разрушения Subject/Subscription безопасен.
template <typename... Args>
class Subject {
 public:
  using Callback = std::function<void(Args...)>;

  Subject() : registry_(std::make_shared<Registry>()) {}
  Subject(const Subject&) = delete;
  Subject& operator=(const Subject&) = delete;

  Subscription Subscribe(Callback callback) {
    const std::size_t id = next_id_++;
    (*registry_)[id] = std::move(callback);
    std::weak_ptr<Registry> weak = registry_;
    return Subscription([weak, id]() {
      if (std::shared_ptr<Registry> reg = weak.lock()) {
        reg->erase(id);
      }
    });
  }

  void Notify(Args... args) const {
    // Копия реестра: колбэк может отписаться прямо во время уведомления.
    Registry snapshot = *registry_;
    for (const auto& [id, callback] : snapshot) {
      callback(args...);
    }
  }

  std::size_t observer_count() const { return registry_->size(); }

 private:
  using Registry = std::map<std::size_t, Callback>;

  std::shared_ptr<Registry> registry_;
  std::size_t next_id_ = 0;
};

}  // namespace s21

#endif  // S21_BRICK_GAME_COMMON_OBSERVER_H_
