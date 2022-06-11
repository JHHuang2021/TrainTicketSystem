#pragma once

#include <iostream>  // for size_t, compare

namespace lin {

// 实现一个简陋的 tuple。
// 重载了小于号，使得多关键字比较更为方便。
// 参考了 https://codereview.stackexchange.com/a/44832 和 GCC。

// helpers
template <typename T>
struct id {
  using type = T;
};

template <typename T>
using type_of = typename T::type;

template <size_t... N>
struct sizes : id<sizes<N...> > {};

// choose N-th element in list <T...>
template <size_t N, typename... T>
struct Choose;

template <size_t N, typename H, typename... T>
struct Choose<N, H, T...> : Choose<N - 1, T...> {};

template <typename H, typename... T>
struct Choose<0, H, T...> : id<H> {};

template <size_t N, typename... T>
using choose = type_of<Choose<N, T...> >;

// given L>=0, generate sequence <0, ..., L-1>
template <size_t L, size_t I = 0, typename S = sizes<> >
struct Range;

template <size_t L, size_t I, size_t... N>
struct Range<L, I, sizes<N...> > : Range<L, I + 1, sizes<N..., I> > {};

template <size_t L, size_t... N>
struct Range<L, L, sizes<N...> > : sizes<N...> {};

template <size_t L>
using range = type_of<Range<L> >;

// single tuple element
template <size_t N, typename T>
class TupleElem {
  T elem;

 public:
  template <typename... Args>
  TupleElem(Args... args) : elem(std::forward<Args>(args)...) {}

  T& get() { return elem; }
  const T& get() const { return elem; }
};

// tuple implementation
template <typename N, typename... T>
class TupleImpl;

template <size_t... N, typename... T>
class TupleImpl<sizes<N...>, T...> : TupleElem<N, T>... {
  template <size_t M>
  using pick = choose<M, T...>;
  template <size_t M>
  using elem = TupleElem<M, pick<M> >;

 public:
  TupleImpl() = default;
  TupleImpl(T... vals) : TupleElem<N, T>(vals)... {}

  template <size_t M>
  pick<M>& get() {
    return elem<M>::get();
  }

  template <size_t M>
  const pick<M>& get() const {
    return elem<M>::get();
  }
};

namespace {
template <typename T, typename U>
constexpr bool __tuple_cmp(const T& t, const U& u, sizes<>) {
  return 0;
}

template <typename T, typename U, size_t _Idx0, size_t... _Idxs>
constexpr bool __tuple_cmp(const T& t, const U& u, sizes<_Idx0, _Idxs...>) {
  auto c = t.template get<_Idx0>() < u.template get<_Idx0>();
  if (c != 0) return c;
  return __tuple_cmp(t, u, sizes<_Idxs...>());
}
}  // namespace

template <typename... T>
struct Tuple : TupleImpl<range<sizeof...(T)>, T...> {
  Tuple() = default;
  Tuple(T... vals) : TupleImpl<range<sizeof...(T)>, T...>(vals...) {}
  static constexpr std::size_t size() { return sizeof...(T); }

  template <typename... Tps, typename... Ups>
  friend constexpr auto operator<(const Tuple<Tps...>& t, const Tuple<Ups...>& u) {
    return __tuple_cmp(t, u, range<sizeof...(Tps)>());
  }
};

template <typename... _Elements>
constexpr Tuple<_Elements&...> tie(_Elements&... __args) noexcept {
  return Tuple<_Elements&...>(__args...);
}

}  // namespace lin
