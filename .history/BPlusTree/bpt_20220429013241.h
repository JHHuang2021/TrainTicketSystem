#pragma once
#include <iostream>
#include <vector>
namespace huang {
#define BPLUSTREE_TYPE \
    BPlusTree<class KeyType, class ValueType, class KeyComparator>
#define INDEX_TEMPLATE_ARGUMENTS \
    template <class KeyType, class ValueType, class KeyComparator>
INDEX_TEMPLATE_ARGUMENTS
class BPlusTree {
    // using InternalPage =
    //     BPlusTreeInternalPage<KeyType, page_id_t, KeyComparator>;
    // using LeafPage = BPlusTreeLeafPage<KeyType, ValueType, KeyComparator>;

   public:
    explicit BPlusTree(std::string name, BufferPoolManager *buffer_pool_manager,
                       const KeyComparator &comparator,
                       int leaf_max_size = LEAF_PAGE_SIZE,
                       int internal_max_size = INTERNAL_PAGE_SIZE);

    bool IsEmpty() const;

    bool Insert(const KeyType &key, const ValueType &value);

    void Remove(const KeyType &key);

    bool GetValue(const KeyType &key, std::vector<ValueType> *result);

    // index iterator
    INDEXITERATOR_TYPE Begin();
    INDEXITERATOR_TYPE Begin(const KeyType &key);
    INDEXITERATOR_TYPE End();

    // read data from file and insert one by one
    void InsertFromFile(const std::string &file_name);

    // read data from file and remove one by one
    void RemoveFromFile(const std::string &file_name);

   private:
    void StartNewTree(const KeyType &key, const ValueType &value);

    bool InsertIntoLeaf(const KeyType &key, const ValueType &value);

    void InsertIntoParent(BPlusTreePage *old_node, const KeyType &key,
                          BPlusTreePage *new_node);

    template <typename N>
    N *Split(N *node);

    template <typename N>
    void Redistribute(N *neighbor_node, N *node, int index);

    bool AdjustRoot(BPlusTreePage *node);

    void UpdateRootPageId(int insert_record = 0);

    // member variable
    std::string index_name_;
    page_id_t root_page_id_;
    BufferPoolManager *buffer_pool_manager_;
    KeyComparator comparator_;
    int leaf_max_size_;
    int internal_max_size_;
};
}  // namespace huang