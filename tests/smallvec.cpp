#include <atlas/array.hpp>
#include <atlas/smallvec.hpp>
#include <doctest.h>

using namespace atlas;

TEST_SUITE("SmallVec") {
  SmallVec<int, 5> smallvec{1, 2, 3, 4};

  TEST_CASE("size") { CHECK(smallvec.size() == 4); }

  TEST_CASE("operator []") {
    CHECK(smallvec[0] == 1);
    CHECK(smallvec[1] == 2);
    CHECK(smallvec[2] == 3);
    CHECK(smallvec[3] == 4);
    CHECK_THROWS(smallvec[4]);
  }

  TEST_CASE("push") {
    smallvec.push(5);
    CHECK(smallvec.size() == 5);
    CHECK(smallvec[4] == 5);

    CHECK_THROWS(smallvec.push(6));
  }

  TEST_CASE("pop") {
    SUBCASE("Pop on empty") {
      SmallVec<int, 1> empty;
      CHECK(!empty.pop().is_some());
    }

    auto val = smallvec.pop();
    CHECK(val == 5);
    CHECK(smallvec.size() == 4);
    CHECK(smallvec[3] == 4);
  }

  TEST_CASE("iter") {
    Array<int, 4> arr{1, 2, 3, 4};

    auto v = arr.iter().collect<SmallVec<int, 4>>();
    CHECK(v.size() == 4);
    CHECK(v[0] == 1);
    CHECK(v[1] == 2);
    CHECK(v[2] == 3);
    CHECK(v[3] == 4);

    SmallVec<int, 4> vec{1, 2, 3, 4};
    auto v2 = vec.iter().collect<SmallVec<int, 4>>();
    CHECK(v2.size() == 4);
    CHECK(v2[0] == 1);
    CHECK(v2[1] == 2);
    CHECK(v2[2] == 3);
    CHECK(v2[3] == 4);
  }
}
