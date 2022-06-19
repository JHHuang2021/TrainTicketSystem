#pragma once

#include "../bpt/bpt.hpp"
#include "../bpt/linked_hashmap.hpp"
#include "../lib/datetime.h"

namespace huang {

template <class T>
struct Hash {
  size_t operator()(const T& t) { return std::hash<T>(t); }
};

template <>
struct Hash<size_t> {
  size_t operator()(const size_t& t) { return t; }
};
template <>
struct Hash<int> {
  size_t operator()(const int& t) { return t; }
};

template <>
struct Hash<lin::Date> {
  size_t operator()(const lin::Date& t) { return t.minutes(); }
};

template <class T, class U>
struct Hash<std::pair<T, U>> {
  size_t operator()(const std::pair<T, U> pair) { return Hash<T>()(pair.first) ^ Hash<U>()(pair.second); }
};

// 在原本 BPlusTree 的基础上加入元素级别的读取缓存
template <class Key, class Value, int kValueBufferSize = 1000, int kInternalSize = 300, int kLeafSize = 100,
    int kInternalBufferSize = 50, int kLeafBufferSize = 50>
class BPTree {
  BPlusTree<Key, Value, kInternalSize, kLeafSize, kInternalSize, kLeafBufferSize> bpt;
  linked_hashmap<Key, Value, Hash<Key>> buffer;

 public:
  BPTree(const std::string& name) : bpt(name) {}

  bool Empty() { return bpt.Empty(); }
  void Insert(const Key& key, const Value& value) { bpt.Insert(key, value); }
  void Remove(const Key& key) {
    bpt.Remove(key);
    auto it = buffer.find(key);
    if (it != buffer.end()) buffer.erase(it);
  }
  std::pair<bool, Value> GetValue(const Key& key) {
    auto it = buffer.find(key);
    if (it != buffer.end()) {
      return std::make_pair(true, it->second);
    } else {
      auto res = bpt.GetValue(key);
      if (res.first) buffer.insert(std::make_pair(key, res.second));
      return res;
    }
  }
  void GetValue(const Key& min_key, const Key& max_key, lin::vector<Value>* ans) {
    return bpt.GetValue(min_key, max_key, ans);
  }
  void Modify(const Key& key, const Value& new_value) {
    bpt.Modify(key, new_value);
    auto it = buffer.find(key);
    if (it != buffer.end()) it->second = new_value;
  }
};

}  // namespace huang