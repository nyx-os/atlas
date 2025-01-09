#pragma once
#include "impl.hpp"
#include <source_location>

namespace atlas {

[[noreturn]] inline void
panic(const char *message,
      std::source_location location = std::source_location::current()) {
  impl::panic(message, location);
  __builtin_unreachable();
}

[[noreturn]] inline void
todo(std::source_location location = std::source_location::current()) {
  panic("Not implemented", location);
}

} // namespace atlas
