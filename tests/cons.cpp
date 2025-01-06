#include <atlas/cons.hpp>
#include <doctest.h>

using namespace Atlas;

TEST_SUITE("Cons") {
  Cons<int, int> c(1, 2);

  TEST_CASE("first and second") {
    CHECK(c.first() == 1);
    CHECK(c.second() == 2);
  }

  TEST_CASE("get") {
    CHECK(c.get<0>() == 1);
    CHECK(c.get<1>() == 2);

    SUBCASE("Structured bindings") {
      auto [a, b] = c;
      CHECK(a == 1);
      CHECK(b == 2);
    }
  }

  TEST_CASE("operators") {
    Cons<int, int> c2(1, 2);
    CHECK(c == c2);
    CHECK(c != Cons<int, int>(2, 1));
  }
}