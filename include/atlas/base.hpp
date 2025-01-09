#pragma once
#include <concepts>
#include <cstddef>
#include <cstdint>

namespace atlas {
template <class T, size_t N> constexpr auto array_size(T (&)[N]) { return N; }

template <std::integral T> constexpr T align_up(T value, T alignment) {
  return (value + alignment - 1) & ~(alignment - 1);
}

template <std::integral T> constexpr T align_down(T value, T alignment) {
  return value & ~(alignment - 1);
}

static_assert(align_up(4092, 4096) == 4096);
static_assert(align_down(4092, 4096) == 0);

struct None {};

constexpr inline auto NONE = None{};
} // namespace atlas
