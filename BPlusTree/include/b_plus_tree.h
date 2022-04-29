#pragma once

#include <queue>
#include <string>
#include <vector>

#include "b_plus_tree_page.h"
#include "index_iterator.h"
namespace huang {

#define BPLUSTREE_TYPE BPlusTree<KeyType, ValueType, KeyComparator>

/**
 * Main class providing the API for the Interactive B+ Tree.
 *
 * Implementation of simple b+ tree data structure where internal pages direct
 * the search and leaf pages contain actual data.
 * (1) We only support unique key
 * (2) support insert & remove
 * (3) The structure should shrink and grow dynamically
 * (4) Implement index iterator for range scan
 */
INDEX_TEMPLATE_ARGUMENTS
class BPlusTree {
    using InternalPage =
        BPlusTreeInternalPage<KeyType, page_id_t, KeyComparator>;
    using LeafPage = BPlusTreeLeafPage<KeyType, ValueType, KeyComparator>;

   public:
    explicit BPlusTree(std::string name, BufferPoolManager *buffer_pool_manager,
                       const KeyComparator &comparator,
                       int leaf_max_size = LEAF_PAGE_SIZE,
                       int internal_max_size = INTERNAL_PAGE_SIZE);

    // Returns true if this B+ tree has no keys and values.
    bool IsEmpty() const;

    // Insert a key-value pair into this B+ tree.
    bool Insert(const KeyType &key, const ValueType &value);

    // Remove a key and its value from this B+ tree.
    void Remove(const KeyType &key);

    // return the value associated with a given key
    bool GetValue(const KeyType &key, std::vector<ValueType> *result);

    // index iterator
    INDEXITERATOR_TYPE Begin();
    INDEXITERATOR_TYPE Begin(const KeyType &key);
    INDEXITERATOR_TYPE End();

    // read data from file and insert one by one
    void InsertFromFile(const std::string &file_name);

    // read data from file and remove one by one
    void RemoveFromFile(const std::string &file_name);
    // expose for test purpose
    Page *FindLeafPage(const KeyType &key, bool leftMost = false);

   private:
    void StartNewTree(const KeyType &key, const ValueType &value);

    bool InsertIntoLeaf(const KeyType &key, const ValueType &value);

    void InsertIntoParent(BPlusTreePage *old_node, const KeyType &key,
                          BPlusTreePage *new_node);

    template <typename N>
    N *Split(N *node);

    bool AdjustRoot(BPlusTreePage *node);

    void UpdateRootPageId(int insert_record = 0);

    // member variable
    std::string index_name_;
    size_t root_page_id_;
    BufferPoolManager *buffer_pool_manager_;
    KeyComparator comparator_;
    int leaf_max_size_;
    int internal_max_size_;
};

}  // namespace huang
