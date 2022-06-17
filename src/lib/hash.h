#pragma once

#include <iostream>  // For string_view

#include "char.h"

namespace lin {

template <class T>
struct Hasher {
  size_t operator()(const T&) const = 0;
};

template <const size_t kSize>
struct Hasher<Char<kSize> > {
  size_t operator()(const Char<kSize>& str) const { return std::_Hash_impl::hash(str.c_str(), str.length()); };
  size_t operator()(std::string_view str) const { return std::_Hash_impl::hash(str.data(), str.length()); };
};

}  // namespace lin