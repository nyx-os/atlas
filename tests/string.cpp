#include <atlas/string.hpp>
#include <doctest.h>
#include <utility>

using namespace atlas;

TEST_SUITE("String") {

  TEST_CASE("length") {
    CHECK(String("Hello").length() == 5);
    CHECK(String("Hello World").length() == 11);
    CHECK(String("").length() == 0);
  }

  TEST_CASE("equals") {
    CHECK(String("Hello") == "Hello");
    CHECK(String("Hello") == StringView("Hello"));
    CHECK(String("Hello") == String("Hello"));
    CHECK(String("Hello") != "Hello World");
  }

  TEST_CASE("at") {
    auto str = String("Hello");
    CHECK(str.at(0).is_some());
    CHECK(str.at(0).unwrap() == 'H');

    CHECK_FALSE(str.at(100).is_some());
    CHECK_FALSE(str.at(1000).is_some());
  }

  TEST_CASE("Large string") {
    auto str = String("abcdefghijklmnopqrstuvwxyz");
    CHECK(str.length() == 26);
    CHECK(str == "abcdefghijklmnopqrstuvwxyz");
    CHECK(str != "obcdefghijklmnopqrstuvwxyz");

    auto str2 = std::move(str);

    CHECK(str2 == "abcdefghijklmnopqrstuvwxyz");
    CHECK(str2 != "obcdefghijklmnopqrstuvwxyz");
    CHECK(str2.length() == 26);
  }

  TEST_CASE("resize") {
    auto str = String("Hello");
    str.resize(10);
    CHECK(str.length() == 10);
    CHECK(str.data() == StringView("Hello"));

    str.resize(30);
    CHECK(str.length() == 30);

    CHECK(str.data() == StringView("Hello"));

    str.resize(40);
    CHECK(str.length() == 40);
    CHECK(str.data() == StringView("Hello"));

    str.resize(10);
    CHECK(str.length() == 10);
    CHECK(str.data() == StringView("Hello"));

    str.resize(3);
    CHECK(str.length() == 3);
    CHECK(str.data() == StringView("Hel"));
  }

  TEST_CASE("push") {
    auto str = String("Hello");
    str.push('c');
    CHECK(str == "Helloc");
    CHECK(str.length() == 6);

    str.resize(4);
    str.push('c');
    CHECK(str == "Hellc");
    CHECK(str.length() == 5);
  }
}
