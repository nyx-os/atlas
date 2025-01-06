#include <atlas/rbtree.hpp>
#include <doctest.h>
#include <vector>

using namespace Atlas;

struct MyStruct {
  int val;

  RBTreeNode<MyStruct> hook;
};

TEST_SUITE("Red-black tree") {
  RBTree<MyStruct, &MyStruct::hook, int, &MyStruct::val> tree;
  MyStruct elem_a{13};
  MyStruct elem_b{8};
  MyStruct elem_c{17};
  MyStruct elem_d{15};

  TEST_CASE("insert") {

#define INSERT(X) tree.insert(&(X))

    INSERT(elem_a);

    CHECK(tree.root() == &elem_a);
    CHECK(elem_a.hook.color == RBColor::Black);

    INSERT(elem_b);

    CHECK(tree.root() == &elem_a);
    CHECK(elem_a.hook.color == RBColor::Black);
    CHECK(elem_b.hook.color == RBColor::Red);

    INSERT(elem_c);

    CHECK(tree.root() == &elem_a);
    CHECK(elem_a.hook.color == RBColor::Black);
    CHECK(elem_b.hook.color == RBColor::Red);
    CHECK(elem_c.hook.color == RBColor::Red);

    INSERT(elem_d);

    CHECK(tree.root() == &elem_a);
    CHECK(elem_a.hook.color == RBColor::Black);
    CHECK(elem_b.hook.color == RBColor::Black);
    CHECK(elem_c.hook.color == RBColor::Black);
    CHECK(elem_d.hook.color == RBColor::Red);
  }

  TEST_CASE("remove") {
    tree.remove(&elem_a);

    CHECK(tree.root() == &elem_d);
    CHECK(elem_c.hook.color == RBColor::Black);
    CHECK(elem_b.hook.color == RBColor::Black);
    CHECK(elem_d.hook.color == RBColor::Black);
  }

  TEST_CASE("find") {
    CHECK(tree.find(13).is_none());

    CHECK(tree.find(8).is_some());
    CHECK(tree.find(17).is_some());
    CHECK(tree.find(15).is_some());
  }

  TEST_CASE("iter") {
    std::vector<int> expected{8, 15, 17};
    std::vector<int> rev_expected{17, 15, 8};

    auto iter = tree.iter();

    for (int &i : expected) {
      CHECK(iter.next().unwrap()->val == i);
    }

    CHECK_FALSE(iter.next().is_some());

    auto rev_iter = tree.iter().rev();

    for (int &i : rev_expected) {
      CHECK(rev_iter.next().unwrap()->val == i);
    }

    CHECK_FALSE(rev_iter.next().is_some());
  }
}