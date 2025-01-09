#include <atlas/option.hpp>
#include <doctest.h>

using namespace atlas;

TEST_SUITE("Option") {
  TEST_CASE("Some") {
    Option<int> some(1);
    CHECK(some.is_some());
    CHECK(some.unwrap() == 1);
  }

  TEST_CASE("None") {
    Option<int> none(NONE);
    CHECK_FALSE(none.is_some());
    CHECK(none.is_none());
  }

  TEST_CASE("Dereference with star") {
    Option<int> some(1);
    CHECK(*some == 1);

    Option<int> none(NONE);
    CHECK_THROWS(*none);
  }

  TEST_CASE("Dereference with arrow") {
    struct Foo {
      int value;
    };
    Option<Foo> some(Foo{1});
    CHECK(some->value == 1);

    Option<Foo> none(NONE);
    CHECK_THROWS((void)none->value);
  }

  TEST_CASE("Unwrap") {
    Option<int> some(1);
    CHECK(some.unwrap() == 1);

    Option<int> none(NONE);
    CHECK_THROWS(none.unwrap());
  }

  TEST_CASE("take") {
    Option<int> some(1);
    CHECK(some.take() == 1);
    CHECK_FALSE(some.is_some());

    Option<int> none(NONE);
    CHECK_THROWS(none.take());
  }

  TEST_CASE("default") {
    Option<int> o;
    CHECK_FALSE(o.is_some());
    CHECK(o.is_none());
  }
}
