#pragma once
#include "assert.hpp"
#include <new>
#include <utility>

namespace atlas {

template <typename T> class Lazy {
public:
  template <typename... Args> void construct(Args &&...args) {
    ASSERT(!constructed_);
    new (buffer_) T(std::forward<Args>(args)...);
    constructed_ = true;
  }

  void destruct() {
    ASSERT(constructed_);
    reinterpret_cast<T *>(buffer_)->~T();
    constructed_ = false;
  }

  T *get() {
    ASSERT(constructed_);
    return std::launder(reinterpret_cast<T *>(buffer_));
  }

  T *operator->() { return get(); }
  T &operator*() { return *get(); }

  explicit operator bool() const { return constructed_; }
  [[nodiscard]] bool valid() const { return constructed_; }

private:
  bool constructed_ = false;
  char buffer_[sizeof(T)];
};

} // namespace atlas
