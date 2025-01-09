#include <atlas/bitmap.hpp>
#include <doctest.h>

using namespace atlas;

TEST_SUITE("Bitmap") {
  uint8_t *buf = new uint8_t[128];
  Bitmap bitmap(Slice<uint8_t>{buf, 128});

  TEST_CASE("set") {
    for (size_t i = 0; i < 128; i++) {
      CHECK(bitmap.set(i, i % 2));
    }
  }

  TEST_CASE("get") {
    for (size_t i = 0; i < 128; i++) {
      CHECK(bitmap.get(i) == i % 2);
    }
  }

  TEST_CASE("iterator") {
    for (auto [i, bit] : bitmap.iter().enumerate()) {
      CHECK(bit == i % 2);
    }

    for (auto [i, bit] : bitmap.iter().rev().enumerate()) {
      CHECK_FALSE(bit == i % 2);
    }
  }
}
