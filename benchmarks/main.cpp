#include "atlas/alloc.hpp"
#include "atlas/hash.hpp"
#include "atlas/hashmap.hpp"
#include "atlas/map.hpp"
#include <absl/container/flat_hash_map.h>
#include <atlas/hamt.hpp>
#include <benchmark/benchmark.h>
#include <frg/hash_map.hpp>
#include <fstream>
#include <parallel_hashmap/phmap.h>
#include <unordered_map>

namespace atlas::impl {
void panic(const char *msg) { throw std::runtime_error(msg); }
} // namespace atlas::impl

// BENCHMARK(frigg_path);
// BENCHMARK(atlas_path);

// BENCHMARK_MAIN();

uint32_t murmur32(const void *key, size_t len, uint32_t seed) {
  const uint8_t *data = (const uint8_t *)key;
  const uint8_t *end = data + len;
  uint32_t h = seed;

  while (data + 4 <= end) {
    uint32_t k = *(uint32_t *)data;
    data += 4;

    // Mix the 4-byte block
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;

    h ^= k;
    h = (h << 13) | (h >> 19);
    h = h * 5 + 0xe6546b64;
  }

  // Handle remaining bytes
  uint32_t k = 0;
  size_t remaining_bytes = end - data;
  if (remaining_bytes > 0) {
    for (size_t i = 0; i < remaining_bytes; i++) {
      k |= (uint32_t)data[i] << (i * 8);
    }

    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;

    h ^= k;
  }

  // Finalization
  h ^= len;
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}

template <typename T> struct MyHash {
  uint32_t operator()(T key, int gen = 0) const { return key ^ gen; }
};

template <> struct MyHash<const char *> {
  uint32_t operator()(const char *key, int gen = 0) const {
    return murmur32(key, strlen(key), gen);
  }
};

// void hamt_bench(benchmark::State &state) {
//   atlas::Hamt<size_t, size_t, atlas::DefaultAllocator, MyHash<size_t>> hamt;

//   for (auto _ : state) {
//     for (size_t i = 0; i < (10UL * 1000); i++) {
//       hamt.insert(i, i);
//     }

//     for (size_t i = 0; i < (10UL * 1000); i++) {
//       if (hamt.get(i) != i) {
//         printf("error %ld\n", i);
//       }
//     }
//   }
// }

// void map_bench(benchmark::State &state) {
//   atlas::Map<size_t, size_t> map;

//   for (auto _ : state) {

//     for (size_t i = 0; i < (10UL * 1000); i++) {
//       map.insert(i, i);
//     }

//     for (size_t i = 0; i < (10UL * 1000); i++) {
//       if (map.get(i) != i) {
//         printf("error %ld\n", i);
//       }
//     }
//   }
// }

// void frg_map_bench(benchmark::State &state) {
//   frg::hash_map<size_t, size_t, frg::hash<size_t>, atlas::DefaultAllocator>
//   map(
//       frg::hash<size_t>{});

//   for (auto _ : state) {
//     for (size_t i = 0; i < (10UL * 1000); i++) {
//       map.insert(i, i);
//     }

//     for (size_t i = 0; i < (10UL * 1000); i++) {
//       if (map[i] != i) {
//         printf("error %ld\n", i);
//       }
//     }
//   }
// }

// void hash_map_bench(benchmark::State &state) {
//   atlas::HashMap<size_t, size_t> map;

//   for (auto _ : state) {

//     for (size_t i = 0; i < (10UL * 1000); i++) {
//       map.insert(i, i);
//     }

//     for (size_t i = 0; i < (10UL * 1000); i++) {
//       if (map[i] != i) {
//         printf("error %ld\n", i);
//       }
//     }
//   }
// }

// BENCHMARK(hamt_bench);
// BENCHMARK(map_bench);
// BENCHMARK(frg_map_bench);
// BENCHMARK(hash_map_bench);
// BENCHMARK_MAIN();

size_t allocated_mem = 0;
size_t alloc_count = 0;

struct TracingAllocator : atlas::DefaultAllocator {
  void *allocate(size_t size) {
    auto ptr = DefaultAllocator::allocate(size);
    allocated_mem += size;
    alloc_count++;
    return ptr;
  }

  void deallocate(void *ptr, size_t size) {
    DefaultAllocator::deallocate(ptr, size);
    allocated_mem -= size;
  }
};

template <typename T> struct AbseilHash {
  size_t operator()(T key, size_t gen = 0) const {
    if (gen)
      return atlas::Hash<T>()(key, gen);

    return absl::Hash<T>()(key) ^ gen;
  }
};

void hamt_benchmark(benchmark::State &state) {

  std::fstream file("words.txt");

  std::string word;
  std::vector<const char *> words;

  while (file >> word) {
    const char *new_str = new char[word.size() + 1];
    strcpy((char *)new_str, word.c_str());

    words.push_back(new_str);
  }

  atlas::Hamt<const char *, size_t, atlas::DefaultAllocator,
              AbseilHash<const char *>>
      hamt;

  for (auto _ : state) {
    for (auto word : words) {
      hamt.insert(word, strlen(word));
    }
    for (auto word : words) {
      (void)hamt.get(word);
    }
  }
}

// same thing as hamt_benchmark but with frg::hash_map
void frg_map_benchmark(benchmark::State &state) {

  std::fstream file("words.txt");

  std::string word;
  std::vector<const char *> words;

  while (file >> word) {
    const char *new_str = new char[word.size() + 1];
    strcpy((char *)new_str, word.c_str());
    words.push_back(new_str);
  }

  frg::hash_map<const char *, size_t, AbseilHash<const char *>,
                atlas::DefaultAllocator>
      ctrie{AbseilHash<const char *>{}};

  for (auto _ : state) {

    for (auto word : words) {
      ctrie.insert(word, strlen(word));
    }

    for (auto word : words) {
      ctrie.get(word);
    }
  }
}

void absl_map_benchmark(benchmark::State &state) {

  std::fstream file("words.txt");

  std::string word;
  std::vector<const char *> words;

  while (file >> word) {
    const char *new_str = new char[word.size() + 1];
    strcpy((char *)new_str, word.c_str());
    words.push_back(new_str);
  }

  absl::flat_hash_map<const char *, size_t, AbseilHash<const char *>> map;

  for (auto _ : state) {
    for (auto word : words) {
      map.insert({word, strlen(word)});
    }

    for (auto word : words) {
      map.at(word);
    }
  }
}

void phashmap_benchmark(benchmark::State &state) {

  std::fstream file("words.txt");

  std::string word;
  std::vector<const char *> words;

  while (file >> word) {
    const char *new_str = new char[word.size() + 1];
    strcpy((char *)new_str, word.c_str());
    words.push_back(new_str);
  }

  phmap::flat_hash_map<const char *, size_t, AbseilHash<const char *>> ctrie;
  for (auto _ : state) {
    for (auto word : words) {
      ctrie.insert({word, strlen(word)});
    }

    for (auto word : words) {
      ctrie.at(word);
    }
  }
}
#if 0
BENCHMARK(hamt_benchmark);
BENCHMARK(frg_map_benchmark);
BENCHMARK(absl_map_benchmark);
BENCHMARK(phashmap_benchmark);
BENCHMARK_MAIN();
#endif

int main() {
  // frg::hash_map<const char *, size_t, MyHash<const char *>,
  // TracingAllocator>
  //  ctrie{MyHash<const char *>{}};

  atlas::Hamt<const char *, size_t, TracingAllocator> hamt;

  std::fstream file("words.txt");

  std::string word;
  std::vector<const char *> words;

  while (file >> word) {
    const char *new_str = new char[word.size() + 1];
    strcpy((char *)new_str, word.c_str());
    hamt.insert(new_str, word.size());
    words.push_back(new_str);
  }

  // Check if the words are in the ctrie
  file.clear();
  file.seekg(0);

  while (file >> word) {
    auto value = hamt.get(word.c_str());
  }

  std::cout << "Allocated memory: " << allocated_mem << std::endl;
  std::cout << "Alloc count: " << alloc_count << std::endl;

  for (auto word : words) {
    hamt.remove(word).unwrap();
    delete[] word;
  }

  std::cout << "Allocated memory: " << allocated_mem << std::endl;

  return 0;
}
