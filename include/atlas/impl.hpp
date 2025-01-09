#pragma once
#include <source_location>

namespace atlas::impl {

[[noreturn]] void panic(const char *message,
                        const std::source_location location);

}
