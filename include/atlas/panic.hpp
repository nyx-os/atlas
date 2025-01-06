#pragma once
#include "impl.hpp"
#include <source_location>

namespace Atlas {

[[noreturn]] inline void
panic(const char *message,
      std::source_location location = std::source_location::current()) {
  Impl::panic(message, location);
  __builtin_unreachable();
}

[[noreturn]] inline void
todo(std::source_location location = std::source_location::current()) {
  panic("Not implemented", location);
}

} // namespace Atlas
