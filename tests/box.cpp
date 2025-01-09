#include <atlas/box.hpp>
#include <doctest.h>
#include <utility>

using namespace atlas;

TEST_SUITE("Box") {
  auto box = Box<int>::make(5);

  TEST_CASE("operator*") { CHECK(*box == 5); }
  TEST_CASE("as_pointer") { CHECK(box.as_pointer() != nullptr); }

  TEST_CASE("move") {
    auto b1 = Box<int>::make(7);
    auto b2 = std::move(b1);
    CHECK(*b2 == 7);
    CHECK_THROWS(*b1);
  }
}