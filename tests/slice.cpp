#include <atlas/slice.hpp>
#include <doctest.h>
#include <vector>

using namespace atlas;

TEST_SUITE("Slice") {
  int data[] = {1, 2, 3, 4};
  Slice<int> s(data, sizeof(data) / sizeof(*data));

  TEST_CASE("size") { CHECK(s.size() == 4); }
  TEST_CASE("operator []") {
    CHECK(s[0] == 1);
    CHECK(s[1] == 2);
    CHECK(s[2] == 3);
    CHECK(s[3] == 4);
    CHECK(s.data()[0] == 1);
    CHECK_THROWS(s[4]);
  }

  TEST_CASE("sub_slice") {
    auto s2o = s.sub_slice(1, 3);
    CHECK(s2o.is_some());

    auto s2 = *s2o;

    CHECK(s2.size() == 2);
    CHECK(s2[0] == 2);
    CHECK(s2[1] == 3);

    CHECK(!s.sub_slice(1, 0).is_some());
  }

  TEST_CASE("iter") {
    auto v = s.iter().collect<std::vector<int>>();
    CHECK(v == std::vector<int>{1, 2, 3, 4});

    auto v2 = s.iter().rev().collect<std::vector<int>>();
    CHECK(v2 == std::vector<int>{4, 3, 2, 1});

    CHECK(s.begin() == data);
    CHECK(s.end() == data + sizeof(data) / sizeof(*data));
  }

  TEST_CASE("default") {
    Slice<int> s;
    CHECK(s.size() == 0);
    CHECK(s.data() == nullptr);
  }

  TEST_CASE("equality") {
    int data2[] = {1, 2, 3, 4};
    Slice<int> s2(data2, sizeof(data2) / sizeof(*data2));

    CHECK(s == s2);
  }

  TEST_CASE("comparison") {
    int data2[] = {1, 2, 3, 4};
    Slice<int> s2(data2, sizeof(data2) / sizeof(*data2));

    CHECK(s == s2);

    int data3[] = {1, 2, 3, 5};
    Slice<int> s3(data3, sizeof(data3) / sizeof(*data3));

    CHECK(s2 < s3);

    int data4[] = {1, 2, 3};
    Slice<int> s4(data4, sizeof(data4) / sizeof(*data4));

    CHECK(s4 < s);
  }
}