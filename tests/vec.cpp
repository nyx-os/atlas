#include <atlas/array.hpp>
#include <atlas/vec.hpp>
#include <doctest.h>

using namespace Atlas;

TEST_SUITE("Vec") {
  Vec<int> vec{1, 2, 3, 4};

  TEST_CASE("size") { CHECK(vec.size() == 4); }

  TEST_CASE("operator []") {
    CHECK(vec[0] == 1);
    CHECK(vec[1] == 2);
    CHECK(vec[2] == 3);
    CHECK(vec[3] == 4);
    CHECK_THROWS(vec[4]);
  }

  TEST_CASE("push_back") {
    vec.push(5);
    CHECK(vec.size() == 5);
    CHECK(vec[4] == 5);
  }

  TEST_CASE("pop") {
    SUBCASE("Pop on empty") {
      Vec<int> empty;
      CHECK(!empty.pop().is_some());
    }

    auto val = vec.pop();
    CHECK(val == 5);
    CHECK(vec.size() == 4);
    CHECK(vec[3] == 4);
  }

  TEST_CASE("collect iter") {
    Array<int, 4> arr{1, 2, 3, 4};

    auto v = arr.iter().collect<Vec<int>>();
    CHECK(v.size() == 4);
    CHECK(v[0] == 1);
    CHECK(v[1] == 2);
    CHECK(v[2] == 3);
    CHECK(v[3] == 4);
  }

  TEST_CASE("reserve") {
    Vec<int> vec;
    vec.reserve(10);

    CHECK(vec.capacity() >= 10);
    CHECK(vec.size() == 0);

    vec.reserve(4);
    CHECK(vec.capacity() >= 10);
  }
}
