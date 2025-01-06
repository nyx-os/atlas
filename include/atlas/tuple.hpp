#pragma once
#include <utility>

// This code is mostly adapted from
// https://github.com/managarm/frigg/blob/master/include/frg/tuple.hpp

namespace Atlas {

template <typename... Ts> struct TupleStorage_;

template <typename T, typename... Ts> struct TupleStorage_<T, Ts...> {
  constexpr TupleStorage_() = default;

  constexpr TupleStorage_(T item, Ts... tail)
      : item(std::forward<T>(item)), tail(std::forward<Ts>(tail)...) {}

  template <typename... Us>
  constexpr TupleStorage_(const TupleStorage_<Us...> &other)
      : item(other.item), tail(other.tail) {}

  template <typename... Us>
  constexpr TupleStorage_(TupleStorage_<Us...> &&other)
      : item(std::move(other.item)), tail(std::move(other.tail)) {}

  T item;
  TupleStorage_<Ts...> tail;
};

template <> struct TupleStorage_<> {};

template <int n, typename... Ts> struct NthType;

template <int n, typename T, typename... Ts> struct NthType<n, T, Ts...> {
  using type = typename NthType<n - 1, Ts...>::type;
};

template <typename T, typename... Ts> struct NthType<0, T, Ts...> {
  using type = T;
};

template <int n, typename... Ts> struct TupleAccessor_;

template <int n, typename T, typename... Ts>
struct TupleAccessor_<n, T, Ts...> {
  constexpr static auto &get(TupleStorage_<T, Ts...> &storage) {
    return TupleAccessor_<n - 1, Ts...>::get(storage.tail);
  }

  constexpr static const auto &get(const TupleStorage_<T, Ts...> &storage) {
    return TupleAccessor_<n - 1, Ts...>::get(storage.tail);
  }
};

template <typename T, typename... Ts> struct TupleAccessor_<0, T, Ts...> {
  constexpr static T &get(TupleStorage_<T, Ts...> &storage) {
    return storage.item;
  }

  constexpr static const T &get(const TupleStorage_<T, Ts...> &storage) {
    return storage.item;
  }
};

template <typename... Ts> class Tuple {
public:
  Tuple() = default;

  constexpr Tuple(Ts... items) : storage_(std::forward<Ts>(items)...) {}

  template <typename... Us>
  constexpr Tuple(const Tuple<Us...> &other) : storage_(other.storage) {}

  template <typename... Us>
  constexpr Tuple(Tuple<Us...> &&other) : storage_(std::move(other.storage)) {}

  // This constraint gives nicer error messages when trying to access an
  // out-of-bounds element
  template <int n>
    requires(n < sizeof...(Ts))
  constexpr auto &get() {
    return TupleAccessor_<n, Ts...>::get(storage_);
  }

  template <int n>
    requires(n < sizeof...(Ts))
  [[nodiscard]] constexpr const auto &get() const {
    return TupleAccessor_<n, Ts...>::get(storage_);
  }

private:
  TupleStorage_<Ts...> storage_;
};

template <> class Tuple<> {};

static_assert(Tuple<int, int>{10, 0}.get<0>() == 10);
static_assert(Tuple<int, int>{10, 0}.get<1>() == 0);

} // namespace Atlas
