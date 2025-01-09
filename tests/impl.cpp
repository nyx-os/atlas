#include <atlas/impl.hpp>
#include <source_location>
#include <stdexcept>

namespace atlas::impl {

[[noreturn]] void panic(const char *msg, std::source_location loc) {
  throw std::runtime_error(std::string(msg) + " at " + loc.file_name() + ":" +
                           std::to_string(loc.line()) + ":" +
                           std::to_string(loc.column()));
}

} // namespace atlas::impl
