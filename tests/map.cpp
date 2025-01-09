#include <atlas/map.hpp>
#include <doctest.h>

using namespace atlas;

TEST_SUITE("Map") {
  Map<const char *, int> map;

  TEST_CASE("insert") {
    CHECK(map.insert("hello", 1));
    CHECK(map.insert("world", 2));

    CHECK(map.size() == 2);
  }

  TEST_CASE("get") {
    CHECK(map.get("hello").is_some());
    CHECK(map.get("hello").unwrap() == 1);

    CHECK(map.get("world").is_some());
    CHECK(map.get("world").unwrap() == 2);

    CHECK(!map.get("foo").is_some());
  }

  TEST_CASE("insert duplicate") {
    CHECK_FALSE(map.insert("hello", 3));
    CHECK(map.size() == 2);
    CHECK(map.get("hello").unwrap() == 1);
  }

  TEST_CASE("iter") {
    auto iter = map.iter();
    auto first = iter.next();
    auto second = iter.next();

    CHECK(first.is_some());
    CHECK(first.unwrap().first() == "hello");
    CHECK(first.unwrap().second() == 1);

    CHECK(second.is_some());
    CHECK(second.unwrap().first() == "world");
    CHECK(second.unwrap().second() == 2);

    CHECK_FALSE(iter.next().is_some());
  }

  TEST_CASE("operator[]") {
    CHECK(map["hello"] == 1);
    CHECK(map["world"] == 2);
    CHECK_THROWS((void)map["foo"]);
  }

  TEST_CASE("remove") {
    CHECK(map.remove("hello"));
    CHECK(map.size() == 1);
    CHECK_FALSE(map.get("hello").is_some());
    CHECK(map.remove("world"));
    CHECK(map.size() == 0);
    CHECK_FALSE(map.get("world").is_some());
  }
}