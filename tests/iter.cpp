#include <atlas/iter.hpp>
#include <doctest.h>
#include <vector>

using namespace Atlas;

template <typename T = int> struct Test {
  Test(std::vector<T> a) : vec(std::move(a)) {}

  auto iter() {
    return make_iterator(vec.begin(), vec.end(), vec.rbegin(), vec.rend());
  }

  auto iter_no_rev() { return make_iterator(vec.begin(), vec.end()); }

  std::vector<T> vec;
};

TEST_SUITE("Iter") {
  std::vector<int> v{1, 2, 3, 4};

  TEST_CASE("map") {
    Test test(v);

    std::vector<int> expected;

    auto r = test.iter().map([](const int &a) -> int { return a + 1; });

    for (auto value : r) {
      expected.push_back(value);
    }
    CHECK(expected == std::vector<int>{2, 3, 4, 5});
  }

  TEST_CASE("for_each") {
    Test test(v);
    std::vector<int> expected;

    test.iter().for_each([&expected](int a) { expected.push_back(a); });

    CHECK(expected == v);
  }

  TEST_CASE("filter") {
    Test test(v);
    std::vector<int> expected;

    auto r =
        test.iter().filter([](const int &a) -> bool { return a % 2 == 0; });

    for (auto value : r) {
      expected.push_back(value);
    }

    CHECK(expected == std::vector<int>{2, 4});
  }

  TEST_CASE("find") {
    Test test(v);
    auto found = test.iter().find([](int a) -> bool { return a == 3; });
    CHECK(found == 3);

    found = test.iter().find([](int a) -> bool { return a == 5; });
    CHECK(found == NONE);
  }

  TEST_CASE("all") {
    Test test(v);

    auto all = test.iter().all([](int a) -> bool { return a > 0; });
    CHECK(all == true);

    all = test.iter().all([](int a) -> bool { return a > 2; });
    CHECK(all == false);
  }

  TEST_CASE("any") {
    Test test(v);

    auto any = test.iter().any([](int a) -> bool { return a > 2; });
    CHECK(any == true);

    any = test.iter().any([](int a) -> bool { return a > 4; });
    CHECK(any == false);
  }

  TEST_CASE("step_by") {
    Test test(v);
    std::vector<int> expected;

    for (auto value : test.iter().step_by(2)) {
      expected.push_back(value);
    }

    CHECK(expected == std::vector<int>{1, 3});

    expected.clear();

    for (auto value : test.iter().step_by(3)) {
      expected.push_back(value);
    }

    CHECK(expected == std::vector<int>{1, 4});

    CHECK_THROWS(test.iter().step_by(0));
  }

  TEST_CASE("enumerate") {
    Test test(v);
    std::vector<std::pair<int, int>> expected;

    for (auto [i, value] : test.iter().enumerate()) {
      expected.push_back({i, value});
    }

    CHECK(expected ==
          std::vector<std::pair<int, int>>{{0, 1}, {1, 2}, {2, 3}, {3, 4}});
  }

  TEST_CASE("count") {
    Test test(v);

    auto count = test.iter().count();
    CHECK(count == 4);
  }

  TEST_CASE("nth") {
    Test test(v);

    auto nth = test.iter().nth(2);
    CHECK(nth == 3);

    auto nth2 = test.iter().nth(5);
    CHECK(nth2 == NONE);
  }

  TEST_CASE("last") {
    Test test(v);

    auto last = test.iter().last();
    CHECK(last == 4);
  }

  TEST_CASE("fold") {
    Test test(v);

    auto sum = test.iter().fold(0, [](int a, int b) -> int { return a + b; });
    CHECK(sum == 10);

    sum = test.iter().fold(10, [](int a, int b) -> int { return a + b; });
    CHECK(sum == 20);
  }

  TEST_CASE("reduce") {
    Test test(v);

    auto sum = test.iter().reduce([](int a, int b) -> int { return a + b; });
    CHECK(sum == 10);

    Test empty({});

    CHECK(empty.iter().reduce([](int a, int b) -> int { return a + b; }) ==
          NONE);
  }

  TEST_CASE("C++ iterator") {
    Test test(v);
    std::vector<int> expected;

    for (auto value : test.iter()) {
      expected.push_back(value);
    }

    CHECK(expected == v);
  }

  TEST_CASE("collect") {
    Test test(v);
    auto vec = test.iter().collect<std::vector<int>>();
    CHECK(vec == v);
  }

  TEST_CASE("rev") {
    Test test(v);
    auto expected = test.iter().rev().collect<std::vector<int>>();

    CHECK(expected == std::vector<int>{4, 3, 2, 1});

    CHECK_THROWS(test.iter_no_rev().rev());
  }

  TEST_CASE("next") {
    Test test(v);
    auto iter = test.iter();

    CHECK(iter.next() == 1);
    CHECK(iter.next() == 2);
    CHECK(iter.next() == 3);
    CHECK(iter.next() == 4);
    CHECK(iter.next() == NONE);
  }

  TEST_CASE("composition") {
    Test test(v);

    auto my_map([](auto iterator) {
      auto next_func = [iterator]() mutable -> Option<int> {
        auto a = iterator.next();
        if (!a) {
          return NONE;
        }
        return *a + 1;
      };
      return Iterator<decltype(next_func)>(next_func);
    });

    auto expected = (test.iter() | my_map).collect<std::vector<int>>();

    CHECK(expected == std::vector<int>{2, 3, 4, 5});
  }
}
