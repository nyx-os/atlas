#pragma once
#include "alloc.hpp"
#include "base.hpp"
#include "hash.hpp"
#include "map.hpp"
#if 0
#include <bitset>
#include <iostream>
#endif

namespace Atlas {

constexpr size_t BRANCHING_FACTOR = 32;

/// Hash Array Mapped Trie
/// This is used when a good balance between a hashtable and a tree is needed,
/// e.g. when you need good resizing (on deletion) and fast lookups.
template <typename K, typename V, Allocator A = DefaultAllocator,
          typename H = Hash<K>>
class Hamt {

public:
  Hamt(A alloc = A(), H hash = H())
      : root_(nullptr), alloc_(alloc), hash_(hash) {}

  Option<V> get(K key) const {
    auto node = root_;

    if (node == nullptr) {
      return NONE;
    }

    auto hash = hash_(key);

    HashState hash_state{hash, 0, 0, &key, hash_};

    auto result = find_node_rehash(root_, key, hash_state);

    if (result.status == FOUND) {
      return result.value->leaf.value;
    }

    return NONE;
  }

  void insert(K key, V value) {
    auto hash = hash_(key);
    auto node = root_;

    // Build the root hash table
    if (node == nullptr) {
      root_ = reinterpret_cast<Node *>(alloc_.allocate(sizeof(Node)));
      root_->branch.bitmap = 0;
      root_->branch.leafmap = 0;
      root_->branch.ptr = nullptr;
      node = root_;
    }

    HashState hash_state{hash, 0, 0, &key, hash_};

    insert_from_hash(key, value, hash_state);
    return;
  }

  Result<> remove(K key) {
    auto node = root_;

    if (node == nullptr) {
      return Err(Error::Empty);
    }

    auto hash = hash_(key);
    HashState hash_state{hash, 0, 0, &key, hash_};
    auto result = find_node_rehash(root_, key, hash_state);

    if (result.status == NOT_FOUND) {
      return Err(Error::NotFound);
    }

    // Clear bit in parent bitmap
    // Also clear bit in leafmap, just to be sure
    auto hash_index = hash_state.get_index();
    auto prev_bitmap = result.parent->branch.bitmap;

    result.parent->branch.bitmap &= ~(1 << hash_index);
    result.parent->branch.leafmap &= ~(1 << hash_index);

    auto new_size = popcount(result.parent->branch.bitmap);

    // Fold the branch if it only has one other leaf
    if (new_size == 1 && popcount(result.parent->branch.leafmap) == 1) {
      HashState new_state = {hash_state.hash, 0, hash_state.gen, &key, hash_};

      shrink_table_nofree(result.parent, 1, get_index(prev_bitmap, hash_index));

      // The tree depth shouldn't EVER exceed 7, so this is safe
      Node *ancestors[7];
      size_t indices[7];

      size_t n_ancestors = 0;

      // Descend the trie from the root node and find the highest node we can
      // prune. This is probably not the best way to do this, but removal is
      // uncommon enough and this ensures the tree is kept compact...
      auto node = root_;

      while (true) {
        auto index = new_state.get_index();

        ancestors[n_ancestors++] = node;
        indices[n_ancestors - 1] = index;

        if (node->branch.bitmap & (1 << index) &&
            !(node->branch.leafmap & (1 << index))) {
          node = &node->branch.ptr[get_index(node->branch.bitmap, index)];
        }

        else {
          break;
        }

        new_state.next();
      }

      size_t parent_index = n_ancestors - 1;

      // Find the highest ancestor that can be safely pruned
      for (size_t i = n_ancestors - 1; i > 0; i--) {
        auto ancestor = ancestors[i];

        if (popcount(ancestor->branch.bitmap) > 1) {
          break;
        }

        parent_index = i;
      }

      // Not sure how this even happens... but it does
      if (parent_index != 0) {
        auto to_convert = ancestors[parent_index];

        auto parent = ancestors[parent_index - 1];

        // Get the index of the node we want to convert
        auto index = indices[parent_index - 1];

        auto other_node =
            result.parent->branch.ptr[!get_index(prev_bitmap, hash_index)];

        // Set the bit back so it gets properly deallocated
        result.parent->branch.bitmap |= (1 << hash_index);
        result.parent->branch.leafmap |= (1 << hash_index);

        fold_branch(to_convert, parent, index, other_node.leaf);
      } else {
        shrink_table_to_fit(
            result.parent, new_size, new_size + 1,
            get_index(result.parent->branch.bitmap, hash_index));
      }

    } else {
      // Else, shrink the array
      shrink_table_to_fit(result.parent, new_size, new_size + 1,
                          get_index(result.parent->branch.bitmap, hash_index));
    }

    size_--;

    return Ok(NONE);
  }

  void dump() {
#if 0
    dump_rec(root_, false);
#endif
  }

  [[nodiscard]] size_t size() const { return size_; }

  ~Hamt() {
    if (root_ != nullptr) {
      dealloc(root_, false);
      alloc_.deallocate(root_, sizeof(Node));
    }
  }

private:
  struct Node {
    struct Leaf {
      MapKey<K> key;
      V value;
    };

    struct Branch {
      Node *ptr;

      // 32-bit bitmap where each one bit represents the presence of a
      // child node
      uint32_t bitmap;

      // The leafmap is a design decision I've made inspired by clojure's CHAMP
      // optimization, as to avoid the use of tagged pointers and to minimize
      // memory use, now a branch fits neatly into 16 bytes.
      uint32_t leafmap;
    };

    union {
      Leaf leaf;
      Branch branch;
    };
  };

  Node *root_ = nullptr;
  A alloc_;
  H hash_;
  size_t size_ = 0;

  [[nodiscard]] inline size_t popcount(uint32_t x) const {
    return __builtin_popcount(x);
  }

  [[nodiscard]] inline size_t get_index(uint32_t bitmap,
                                        uint32_t sparse_index) const {
    return popcount(bitmap & ((1 << sparse_index) - 1));
  }

  void dealloc(Node *node, bool is_leaf) {
    if (!is_leaf) {
      for (size_t i = 0; i < BRANCHING_FACTOR; i++) {
        if ((node->branch.bitmap & (1 << i)) != 0) {
          dealloc(&node->branch.ptr[get_index(node->branch.bitmap, i)],
                  node->branch.leafmap & (1 << i));
        }
      }

      if (node->branch.ptr) {
        alloc_.deallocate(node->branch.ptr,
                          sizeof(Node) * popcount(node->branch.bitmap));
      }
    }
  }

  struct HashState {
    size_t hash;
    size_t shift;
    size_t gen = 0;
    K *key;
    H hasher_fn;

    inline HashState &next() {
      shift += 5;

      // Hash was exhausted, regenerate
      if (shift > 30) {
        hash = hasher_fn(*key, ++gen);

        shift = 0;
      }
      return *this;
    }

    inline size_t get_index() { return (hash >> shift) & 0x1f; }
  };

  enum SearchStatus { NOT_FOUND, FOUND, COLLISION };

  struct SearchResult {
    Node *parent;
    Node *grandparent;
    Node *value;
    SearchStatus status;
  };

  SearchResult search(Node *node, K key, HashState &state,
                      Node *grandparent) const {
    uint32_t index = state.get_index();

    if (node->branch.bitmap & (1 << index)) {
      auto pos = get_index(node->branch.bitmap, index);

      if (node->branch.leafmap & (1 << index)) {
        auto leaf = &node->branch.ptr[pos];

        if (leaf->leaf.key == key) {
          return {node, grandparent, leaf, FOUND};
        }

        return {node, grandparent, leaf, COLLISION};
      }

      auto grandparent = node;
      node = &node->branch.ptr[get_index(node->branch.bitmap, index)];

      return search(node, key, state.next(), grandparent);
    }

    return {nullptr, nullptr, node, NOT_FOUND};
  }

  void extend_table(Node *branch, size_t prev_size, size_t pos) {
    auto new_table = reinterpret_cast<Node *>(
        alloc_.allocate(sizeof(Node) * (prev_size + 1)));

    if (prev_size > 0) {
      memcpy(new_table, branch->branch.ptr, sizeof(Node) * pos);

      memcpy(&new_table[pos + 1], &branch->branch.ptr[pos],
             sizeof(Node) * (prev_size - pos));

      alloc_.deallocate(branch->branch.ptr, sizeof(Node) * prev_size);
    }

    branch->branch.ptr = new_table;
  }

  void shrink_table_nofree(Node *branch, size_t new_size, size_t pos) {
    memmove(&branch->branch.ptr[pos], &branch->branch.ptr[pos + 1],
            sizeof(Node) * (new_size - pos));
  }

  void shrink_table_to_fit(Node *branch, size_t new_size, size_t prev_size,
                           size_t pos) {
    if (new_size == 0) {
      alloc_.deallocate(branch->branch.ptr, sizeof(Node));
      branch->branch.ptr = nullptr;
    }

    else {
      auto new_table =
          reinterpret_cast<Node *>(alloc_.allocate(sizeof(Node) * new_size));

      if (pos > 0) {
        memcpy(new_table, branch->branch.ptr, sizeof(Node) * pos);
      }

      if (pos < new_size) {
        memcpy(&new_table[pos], &branch->branch.ptr[pos + 1],
               sizeof(Node) * (new_size - pos));
      }

      alloc_.deallocate(branch->branch.ptr, sizeof(Node) * prev_size);
      branch->branch.ptr = new_table;
    }
  }

  void insert_in_branch(Node *branch, HashState hash, K &key, V &value) {
    uint32_t index = hash.get_index();

    auto new_bitmap = branch->branch.bitmap | (1 << index);
    auto pos = get_index(new_bitmap, index);
    auto prev_size = popcount(branch->branch.bitmap);

    // Resize if needed
    extend_table(branch, prev_size, pos);

    branch->branch.bitmap = new_bitmap;
    branch->branch.leafmap |= (1 << index);

    branch->branch.ptr[pos].leaf.key = MapKey<K>{key};
    branch->branch.ptr[pos].leaf.value = value;
  }

  void insert_from_hash(K key, V value, HashState hash) {
    auto node = root_;

    auto result = search(node, key, hash, nullptr);

    if (result.status == FOUND) {
      result.value->leaf.value = value;
    }

    else if (result.status == NOT_FOUND) {
      insert_in_branch(result.value, hash, key, value);
      size_++;
    }

    else if (result.status == COLLISION) {
      convert_to_branch(result.value, result.parent, hash, key, value);
      size_++;
    }
  }

  SearchResult find_node_rehash(Node *start, K key, HashState &state) const {
    auto res = search(start, key, state, nullptr);

    while (res.status == COLLISION) {
      state.hash = hash_(key, ++state.gen);
      state.shift = 0;
      state.gen = 0;
      state.hasher_fn = hash_;

      res = search(root_, key, state, nullptr);
    }

    return res;
  }

  void convert_to_branch(Node *node, Node *parent, HashState hash, K &key,
                         V &value) {

    auto prev_node = *node;

    HashState state = {hash_(prev_node.leaf.key.val), hash.shift, hash.gen,
                       &prev_node.leaf.key.val, hash_};

    ASSERT(find_node_rehash(parent, prev_node.leaf.key.val, state).status ==
           FOUND);

    if (hash.hash == state.hash) {
      hash.hash = (uint32_t)hash_(key, ++hash.gen);
      hash.shift = 0;
      hash.gen = 0;
      hash.hasher_fn = hash_;
      hash.key = &key;

      insert_from_hash(key, value, hash);
      return;
    }

    parent->branch.leafmap &= ~(1 << hash.get_index());

    auto root = node;

    size_t curr_index = hash.next().get_index();
    size_t prev_index = state.next().get_index();

    while (curr_index == prev_index) {
      root->branch.leafmap = 0;
      root->branch.bitmap = 1 << curr_index;
      root->branch.ptr =
          reinterpret_cast<Node *>(alloc_.allocate(sizeof(Node)));

      curr_index = hash.next().get_index();
      prev_index = state.next().get_index();

      root = root->branch.ptr;
    }

    root->branch.ptr =
        reinterpret_cast<Node *>(alloc_.allocate(sizeof(Node) * 2));

    root->branch.bitmap = root->branch.leafmap =
        (1 << curr_index) | (1 << prev_index);

    auto real_prev_index = get_index(root->branch.bitmap, prev_index);
    auto real_curr_index = get_index(root->branch.bitmap, curr_index);

    root->branch.ptr[real_prev_index] = prev_node;
    root->branch.ptr[real_curr_index].leaf.key = MapKey<K>{key};
    root->branch.ptr[real_curr_index].leaf.value = value;
  }

  // 'Fold' a branch, convert it to a leaf node
  void fold_branch(Node *branch, Node *parent, size_t index, Node::Leaf &leaf) {
    // Ensure parent knows we're a leaf node
    parent->branch.leafmap |= (1 << index);

    // Deallocate branch and its children
    dealloc(branch, false);

    branch->leaf = leaf;
  }
#if 0
  void dump_rec(Node *n, bool is_leaf, size_t indent = 0, size_t prefix = 0) {
    if (!n) {
      return;
    }

    std::string indentation(indent, ' ');

    if (!is_leaf) {
      std::cout << indentation << (indent == 0 ? "?---" : "+---")
                << " Branch Node @ " << (void *)n << " (hash prefix " << prefix
                << ")" << std::endl;
      std::cout << indentation
                << "  +- Bitmap: " << std::bitset<32>(n->branch.bitmap)
                << std::endl;
      std::cout << indentation
                << "  +- Leafmap: " << std::bitset<32>(n->branch.leafmap)
                << std::endl;
      std::cout << indentation << "  +- Children:" << std::endl;

      for (size_t i = 0; i < BRANCHING_FACTOR; i++) {
        if (n->branch.bitmap & (1 << i)) {
          dump_rec(&n->branch.ptr[get_index(n->branch.bitmap, i)],
                   n->branch.leafmap & (1 << i), indent + 2, i);
        }
      }
    }

    else {
      std::cout << indentation << "?---"
                << " Leaf Node @ " << (void *)n << ":" << std::endl;
      std::cout << indentation << "  +- Key: " << n->leaf.key.val << std::endl;
      std::cout << indentation << "  +- Value: " << n->leaf.value << std::endl;
    }
  }
#endif
};

} // namespace Atlas
