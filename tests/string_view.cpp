#include <atlas/string_view.hpp>
#include <doctest.h>

using namespace atlas;

TEST_SUITE("String view") {

  TEST_CASE("[]") {
    StringView view("Hello, World!");

    CHECK(view[0] == 'H');
    CHECK(view[1] == 'e');
    CHECK(view[2] == 'l');
    CHECK(view[3] == 'l');
    CHECK(view[4] == 'o');
    CHECK(view[5] == ',');
    CHECK(view[6] == ' ');
    CHECK(view[7] == 'W');
    CHECK(view[8] == 'o');
    CHECK(view[9] == 'r');
    CHECK(view[10] == 'l');
    CHECK(view[11] == 'd');
    CHECK(view[12] == '!');
  }

  TEST_CASE("==") {
    StringView view1("Hello, World!");
    StringView view2("Hello, World!");
    StringView view3("Hello, World");

    CHECK(view1 == view2);
    CHECK(view1 != view3);
  }

  TEST_CASE("length") {
    StringView view("Hello, World!");

    CHECK(view.length() == 13);
  }

  TEST_CASE("substr") {
    StringView view("Hello, World!");

    CHECK(view.substr(0, 5) == "Hello");
    CHECK(view.substr(7, 5) == "World");
  }

  TEST_CASE("iterator") {

    SUBCASE("begin") {
      StringView view("Hello, World!");

      auto it = view.begin();

      CHECK(*it == 'H');
      CHECK(*it == view[0]);
    }

    SUBCASE("end") {
      StringView view("Hello, World!");

      auto it = view.end();

      CHECK(*it == '\0');
    }
  }
}
