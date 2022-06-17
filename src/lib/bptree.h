#pragma once

#include "../b_plus_tree/include/b_plus_tree.hpp"

namespace huang {

template <class KeyType, class ValueType>
class BPTree : public huang::BPlusTree<KeyType, ValueType> {
  huang::DiskManager dm;
  huang::Replacer<huang::Page *> rep;
  huang::BufferPoolManager bpm;

 public:
  BPTree(const std::string &db_filename, size_t buffer_pool_size)
      : dm(db_filename), bpm(buffer_pool_size, &dm, &rep), huang::BPlusTree<KeyType, ValueType>(&bpm) {}
};

}  // namespace huang