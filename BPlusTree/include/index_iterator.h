#pragma once
#include "b_plus_tree_page.h"

namespace huang {

#define INDEXITERATOR_TYPE IndexIterator<KeyType, ValueType, KeyComparator>

INDEX_TEMPLATE_ARGUMENTS
class IndexIterator {
 public:
  // you may define your own constructor based on your member variables
  IndexIterator();
  ~IndexIterator();

  bool IsEnd();

  const MappingType &operator*();

  IndexIterator &operator++();

  bool operator==(const IndexIterator &itr) const { throw std::runtime_error("unimplemented"); }

  bool operator!=(const IndexIterator &itr) const { throw std::runtime_error("unimplemented"); }

 private:
  // add your own private member variables here
};

}  // namespace huang
