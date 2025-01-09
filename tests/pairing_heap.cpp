#include <array>
#include <atlas/pairing_heap.hpp>
#include <doctest.h>

using namespace atlas;

struct TestNode {
  PairingHeapNode<TestNode> hook;
  int value;
};

TEST_SUITE("Pairing Heap") {

  PairingHeap<TestNode, &TestNode::hook,
              [](TestNode *a, TestNode *b) { return a->value < b->value; }>
      heap;
  std::array<TestNode, 10> nodes;

  TEST_CASE("insert") {
    for (int i = 0; i < 10; i++) {
      nodes[i].value = i;
      heap.insert(&nodes[i]);
      CHECK(heap.size() == i + 1);
    }
  }

  TEST_CASE("pop (min-heap)") {
    for (int i = 0; i < 10; i++) {
      auto n = heap.pop();
      CHECK(n);
      CHECK(n.unwrap()->value == i);
      CHECK(heap.size() == 9 - i);
    }
  }

  TEST_CASE("pop (max-heap)") {
    PairingHeap<TestNode, &TestNode::hook,
                [](TestNode *a, TestNode *b) {
                  return (a->value > b->value);
                }>
        heap;

    for (int i = 0; i < 10; i++) {
      nodes[i].value = i;
      heap.insert(&nodes[i]);
    }
    for (int i = 9; i >= 0; i--) {
      auto n = heap.pop();
      CHECK(n);
      CHECK(n.unwrap()->value == i);
      CHECK(heap.size() == i);
    }
  }
}
