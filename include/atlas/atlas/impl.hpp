#pragma once
#include <source_location>

namespace Atlas::Impl {

[[noreturn]] void panic(const char *message,
                        const std::source_location location);

}
