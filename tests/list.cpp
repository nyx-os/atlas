#include <atlas/list.hpp>
#include <doctest.h>

using namespace Atlas;

struct MyStruct {
  int val;
  ListNode<MyStruct> hook;
};

TEST_SUITE("List") {

  TEST_CASE("insert_tail") {
    List<MyStruct, &MyStruct::hook> list;
    MyStruct a;
    MyStruct b{2};
    MyStruct c{3};

    CHECK(list.insert_tail(&a));
    CHECK(list.insert_tail(&b));
    CHECK(list.insert_tail(&c));

    CHECK(list.head() == &a);
    CHECK(list.tail() == &c);
  }

  TEST_CASE("insert_head") {
    List<MyStruct, &MyStruct::hook> list;
    MyStruct a{1};
    MyStruct b{2};
    MyStruct c{3};

    CHECK(list.insert_head(&a));
    CHECK(list.insert_head(&b));
    CHECK(list.insert_head(&c));

    CHECK(list.head() == &c);
    CHECK(list.tail() == &a);
  }

  TEST_CASE("remove") {
    List<MyStruct, &MyStruct::hook> list;
    MyStruct a{1};
    MyStruct b{2};
    MyStruct c{3};

    CHECK(list.insert_tail(&a));
    CHECK(list.insert_tail(&b));
    CHECK(list.insert_tail(&c));

    CHECK(list.remove(&b));
    CHECK(list.head() == &a);
    CHECK(list.tail() == &c);

    CHECK(list.remove(&a));
    CHECK(list.head() == &c);
    CHECK(list.tail() == &c);

    CHECK(list.remove(&c));
    CHECK(list.head() == nullptr);
    CHECK(list.tail() == nullptr);
  }

  TEST_CASE("remove_head") {
    List<MyStruct, &MyStruct::hook> list;
    MyStruct a{1};
    MyStruct b{2};
    MyStruct c{3};

    CHECK(list.insert_tail(&a));
    CHECK(list.insert_tail(&b));
    CHECK(list.insert_tail(&c));

    CHECK(list.remove_head());
    CHECK(list.head() == &b);
    CHECK(list.tail() == &c);

    CHECK(list.remove_head());
    CHECK(list.head() == &c);
    CHECK(list.tail() == &c);

    CHECK(list.remove_head());
    CHECK(list.head() == nullptr);
    CHECK(list.tail() == nullptr);
  }

  TEST_CASE("remove_tail") {
    List<MyStruct, &MyStruct::hook> list;
    MyStruct a{1};
    MyStruct b{2};
    MyStruct c{3};

    CHECK(list.insert_tail(&a));
    CHECK(list.insert_tail(&b));
    CHECK(list.insert_tail(&c));

    CHECK(list.remove_tail());
    CHECK(list.head() == &a);
    CHECK(list.tail() == &b);

    CHECK(list.remove_tail());
    CHECK(list.head() == &a);
    CHECK(list.tail() == &a);

    CHECK(list.remove_tail());
    CHECK(list.head() == nullptr);
    CHECK(list.tail() == nullptr);
  }

  TEST_CASE("length") {
    List<MyStruct, &MyStruct::hook> list;
    MyStruct a{1};
    MyStruct b{2};
    MyStruct c{3};

    CHECK(list.length() == 0);

    CHECK(list.insert_tail(&a));
    CHECK(list.length() == 1);

    CHECK(list.insert_tail(&b));
    CHECK(list.length() == 2);

    CHECK(list.insert_tail(&c));
    CHECK(list.length() == 3);

    CHECK(list.remove_tail());
    CHECK(list.length() == 2);

    CHECK(list.remove_tail());
    CHECK(list.length() == 1);

    CHECK(list.remove_tail());
    CHECK(list.length() == 0);
  }

  TEST_CASE("iter") {
    List<MyStruct, &MyStruct::hook> list;
    MyStruct a{1};
    MyStruct b{2};
    MyStruct c{3};

    CHECK(list.insert_tail(&a));
    CHECK(list.insert_tail(&b));
    CHECK(list.insert_tail(&c));

    int i = 1;
    for (auto node : list.iter()) {
      CHECK(node->val == i);
      i++;
    }
  }
}
