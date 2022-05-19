
#pragma once

#include <iostream>

#include "buffer_pool_manager.h"

namespace huang {

#define INDEX_TEMPLATE_ARGUMENTS template <typename KeyType, typename ValueType>

class HeaderPage {
   public:
    page_id_t root = 0;
    page_id_t allocator = 0;
    int size;
};

// define page type enum
enum IndexPageType { INVALID_INDEX_PAGE = 0, LEAF_PAGE, INTERNAL_PAGE };

#define B_PLUS_TREE_SIZE 500
#define B_PLUS_TREE_MAX_SIZE 450
#define B_PLUS_TREE_MIN_SIZE 200
template <class KeyType, class ValueType>
class BPlusTreePage {
   public:
    BPlusTreePage() {
        page_type_ = INVALID_INDEX_PAGE;
        is_root_ = false;
        parent_page_id_ = -1;
        page_id_ = -1;
        nxt = -1;
    };
    bool IsLeafPage() const { return page_type_ == LEAF_PAGE; };
    void Init(page_id_t page_id = 0, IndexPageType page_type = LEAF_PAGE,
              bool is_root = false, size_t size = 0) {
        page_id_ = page_id;
        nxt = parent_page_id_ = -1;
        page_type_ = page_type;
        is_root_ = is_root;
        size_ = size;
    }
    void Insert(const KeyType& key, const ValueType& value) {
        int i;
        for (i = 0; i < size_; i++)
            if (key < data_[i].first) break;
        for (int j = size_ - 1; j >= i; j--) data_[j + 1] = data_[j];
        data_[i] = {key, value};
        size_++;
    }
    std::pair<KeyType, ValueType> data_[B_PLUS_TREE_SIZE];

    IndexPageType page_type_ = INVALID_INDEX_PAGE;
    size_t size_;
    bool is_root_ = false;
    page_id_t parent_page_id_ = -1;
    page_id_t page_id_ = -1;
    page_id_t nxt = -1;
};

}  // namespace huang
