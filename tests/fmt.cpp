#include <atlas/fmt.hpp>
#include <atlas/string.hpp>
#include <atlas/tuple.hpp>
#include <atlas/vec.hpp>
#include <doctest.h>

using namespace atlas;

struct MyType {
  int x;
  int y;
};

template <> struct atlas::Formatter<MyType> {
  void format(auto &sink, FormatOptions opts, const MyType &value) {
    atlas::format(sink, "MyType({},{})", value.x, value.y);
  }
};

TEST_SUITE("fmt") {
  String sink;

  TEST_CASE("numbers") {
    sink.clear();
    format(sink, "{}", 1234);
    CHECK(sink == "1234");

    sink.clear();
    format(sink, "{:x}", 1234);
    CHECK(sink == "4d2");

    sink.clear();
    format(sink, "{:X}", 1234);
    CHECK(sink == "4D2");

    sink.clear();

    SUBCASE("padding") {
      format(sink, "{:5}", 1234);
      CHECK(sink == " 1234");

      sink.clear();
      format(sink, "{:05}", 1234);
      CHECK(sink == "01234");

      sink.clear();
      format(sink, "{:05}", -1234);
      CHECK(sink == "-1234");

      sink.clear();
      format(sink, "{:05}", 12345);
      CHECK(sink == "12345");
    }

    SUBCASE("large numbers") {
      sink.clear();
      format(sink, "{}", (2UL << 30UL));
      CHECK(sink == "2147483648");

      sink.clear();
      format(sink, "{:x}", 1234567890);
      CHECK(sink == "499602d2");

      sink.clear();
      format(sink, "{:X}", 1234567890);
      CHECK(sink == "499602D2");
    }
  }

  TEST_CASE("strings") {
    sink.clear();
    format(sink, "{}", "hello");
    CHECK(sink == "hello");
  }

  TEST_CASE("custom types") {
    sink.clear();
    format(sink, "{}", MyType{1, 2});
    CHECK(sink == "MyType(1,2)");
  }

  TEST_CASE("boolean") {
    sink.clear();
    format(sink, "{}", true);
    CHECK(sink == "#t");

    sink.clear();
    format(sink, "{}", false);
    CHECK(sink == "#f");
  }

  TEST_CASE("no arguments") {
    sink.clear();
    format(sink, "hello");
    CHECK(sink == "hello");
  }

  TEST_CASE("option") {
    sink.clear();
    format(sink, "{}", Option<int>{1});
    CHECK(sink == "1");
    sink.clear();
    format(sink, "{}", Option<int>{NONE});
    CHECK(sink == "None");
  }
}
