#include <atlas/array.hpp>
#include <doctest.h>
#include <vector>

using namespace Atlas;

TEST_SUITE("Array") {
  Array<int, 4> a{1, 2, 3, 4};

  TEST_CASE("operator []") {
    CHECK(a[0] == 1);
    CHECK(a[1] == 2);
    CHECK(a[2] == 3);
    CHECK(a[3] == 4);
    CHECK_THROWS(a[4]);

    CHECK(a.data()[0] == 1);
  }

  TEST_CASE("size") { CHECK(a.size() == 4); }

  TEST_CASE("iterator") {
    auto v = a.iter().collect<std::vector<int>>();
    CHECK(v == std::vector<int>{1, 2, 3, 4});

    auto v2 = a.iter().rev().collect<std::vector<int>>();
    CHECK(v2 == std::vector<int>{4, 3, 2, 1});
  }
}
