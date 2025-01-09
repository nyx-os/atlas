#include "atlas/alloc.hpp"
#include <atlas/hamt.hpp>
#include <atlas/hashmap.hpp>
#include <csignal>
#include <doctest.h>
#include <iostream>
#include <unordered_map>

using namespace atlas;

size_t allocated_mem = 0;
size_t alloc_count = 0;

std::unordered_map<uintptr_t, size_t> alloc_map;

struct TracingAllocator : atlas::DefaultAllocator {
  void *allocate(size_t size) {
    auto ptr = DefaultAllocator::allocate(size);
    allocated_mem += size;
    alloc_count++;
    alloc_map[(uintptr_t)ptr] = size;
    return ptr;
  }

  void deallocate(void *ptr, size_t size) {
    DefaultAllocator::deallocate(ptr, size);
    allocated_mem -= size;

    if (alloc_map.find((uintptr_t)ptr) == alloc_map.end()) {
      std::cout << "Trying to free unallocated memory: " << ptr << std::endl;
      std::raise(SIGABRT);
    }

    if (alloc_map[(uintptr_t)ptr] != size) {
      std::cout << "Trying to free memory with wrong size: " << ptr
                << " previous size was " << alloc_map[(uintptr_t)ptr]
                << " new size is " << size << std::endl;
      std::raise(SIGABRT);
    }

    alloc_map.erase((uintptr_t)ptr);
  }
};

TEST_SUITE("ctrie") {
  Hamt<size_t, size_t> hamt;

  TEST_CASE("insert/get") {
    for (int i = 0; i < 100; i++) {
      hamt.insert(i, i);
    }

    CHECK(hamt.size() == 100);

    for (int i = 0; i < 100; i++) {
      auto res = hamt.get(i);
      CHECK(res.is_some());
      CHECK(*res == i);
    }

    SUBCASE("Replacing an existing value") {
      hamt.insert(1, 7);
      auto res = hamt.get(1);
      CHECK(res.is_some());
      CHECK(*res == 7);
      CHECK(hamt.size() == 100);
    }

    SUBCASE("Getting an invalid value") {
      auto res = hamt.get(100);

      CHECK_FALSE(res.is_some());
      CHECK_THROWS(*res);
    }
  }

  TEST_CASE("remove") {
    Hamt<size_t, size_t, TracingAllocator> other;
    other.insert(78, 1);
    other.insert(954, 1);
    other.insert(32, 1);
    other.insert(5460, 1);
    other.insert(1812219, 1);
    other.insert(69879428, 1);
    other.insert(772812588, 1);

    CHECK(other.remove(1812219).is_ok());
    CHECK_FALSE(other.get(1812219).is_some());

    CHECK(other.remove(5460));
    CHECK_FALSE(other.get(5460).is_some());
    CHECK(other.remove(78));
    CHECK_FALSE(other.get(78).is_some());
    CHECK(other.remove(69879428));
    CHECK_FALSE(other.get(69879428).is_some());

    CHECK(other.remove(32));
    CHECK_FALSE(other.get(32).is_some());
    CHECK(other.remove(954));
    CHECK_FALSE(other.get(954).is_some());
    CHECK(other.remove(772812588));
    CHECK_FALSE(other.get(772812588).is_some());
    CHECK(allocated_mem == 16);
    other.insert(78, 1);
    CHECK(allocated_mem == 32);

    CHECK(other.get(5460).is_none());
    CHECK_FALSE(other.get(1812219).is_some());
  }
}
