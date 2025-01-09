#include <atlas/formats/dot.hpp>
#include <doctest.h>

using namespace atlas;

struct StringWriter {
  Result<size_t, io::Error> write(Slice<const char> buf) {
    output.append((const char *)buf.data(), buf.size());
    return Ok(buf.size());
  }

  std::string output{};
};

TEST_SUITE("Dot") {

  TEST_CASE("basic output") {
    StringWriter str;
    Dot<> dot;
    dot.add_node("a");
    dot.add_node("b");
    dot.add_edge("a", "b");

    CHECK(dot.output(str));

    CHECK(str.output == "digraph{\"a\";\"b\";\"a\"->\"b\";}");
  }

  TEST_CASE("properties") {
    StringWriter str;
    Dot<> dot;
    dot.add_node("a", {.color = "red",
                       .shape = "circle",
                       .style = "filled",
                       .text_color = "white"});
    dot.add_node("b", {.color = "blue", .shape = "square", .style = "dotted"});
    dot.add_edge("a", "b");

    CHECK(dot.output(str));

    CHECK(str.output ==
          "digraph{\"a\"[color=red,fontcolor=white,shape=circle,style=filled];"
          "\"b\"[color=blue,"
          "shape=square,style=dotted];\"a\"->\"b\";}");
  }
}
