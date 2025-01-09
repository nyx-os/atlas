#include <atlas/arc.hpp>
#include <doctest.h>

using namespace atlas;

TEST_SUITE("Arc") {
  auto ptr = Arc<int>::make(0);

  TEST_CASE("initial ref") { CHECK(ptr.ref() == 1); }

  TEST_CASE("operator bool") { CHECK(ptr); }

  TEST_CASE("copy") {
    auto b = ptr;
    CHECK(ptr.ref() == 2);
    CHECK(b.ref() == 2);
    CHECK(b.as_pointer() == ptr.as_pointer());

    b.~Arc();

    CHECK(ptr.ref() == 1);

    b = ptr;

    CHECK(ptr.ref() == 2);
  }

  TEST_CASE("move") {
    auto b = std::move(ptr);

    CHECK(b.ref() == 1);

    CHECK(!ptr);
  }

  TEST_CASE("operator *") {
    auto b = Arc<int>::make(5);

    CHECK(*b == 5);
  }
}
