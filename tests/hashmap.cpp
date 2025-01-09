#include <atlas/hashmap.hpp>
#include <doctest.h>

using namespace atlas;

TEST_SUITE("HashMap") {
  HashMap<int, int> hashmap;

  TEST_CASE("insert") {
    CHECK(hashmap.insert(1, 2));
    CHECK(hashmap.insert(2, 3));
    CHECK(hashmap.insert(3, 4));

    CHECK(hashmap.size() == 3);
  }

  TEST_CASE("get") {
    CHECK(hashmap.get(1).unwrap() == 2);
    CHECK(hashmap.get(2).unwrap() == 3);
    CHECK(hashmap.get(3).unwrap() == 4);
    CHECK_FALSE(hashmap.get(4).is_some());

    CHECK(hashmap[1] == 2);
    CHECK(hashmap[2] == 3);
    CHECK(hashmap[3] == 4);
    CHECK_THROWS((void)hashmap[4]);
  }

  TEST_CASE("insert duplicate") {
    CHECK_FALSE(hashmap.insert(1, 3));
    CHECK(hashmap.size() == 3);
    CHECK(hashmap.get(1).unwrap() == 2);
  }

  TEST_CASE("remove") {
    CHECK(hashmap.remove(1));
    CHECK(hashmap.size() == 2);
    CHECK_FALSE(hashmap.get(1).is_some());
    CHECK(hashmap.remove(2));
    CHECK(hashmap.size() == 1);
    CHECK_FALSE(hashmap.get(2).is_some());
    CHECK(hashmap.remove(3));
    CHECK(hashmap.size() == 0);
    CHECK_FALSE(hashmap.get(3).is_some());

    SUBCASE("Remove non-existent") { CHECK_FALSE(hashmap.remove(4)); }
  }

  TEST_CASE("String hashmap") {

    HashMap<StringView, int> hashmap;

    CHECK(hashmap.insert("hello"_sv, 1));
    CHECK(hashmap.insert("world"_sv, 2));

    CHECK(hashmap.size() == 2);
    CHECK(hashmap.get("hello"_sv).unwrap() == 1);
    CHECK(hashmap.get("world"_sv).unwrap() == 2);
  }

  TEST_CASE("big hashmap") {
    HashMap<int, int> hashmap(1);

    for (int i = 0; i < 100; i++) {
      CHECK(hashmap.insert(i, i));
    }

    CHECK(hashmap.size() == 100);

    for (int i = 0; i < 100; i++) {
      CHECK(hashmap.get(i).unwrap() == i);
    }
  }
}
