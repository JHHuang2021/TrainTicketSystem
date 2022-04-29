
#pragma once

#include <iostream>

#include "buffer_pool_manager.h"

namespace huang {

#define MappingType std::pair<KeyType, ValueType>

#define INDEX_TEMPLATE_ARGUMENTS \
    template <typename KeyType, typename ValueType, typename KeyComparator>

// define page type enum
enum class IndexPageType { INVALID_INDEX_PAGE = 0, LEAF_PAGE, INTERNAL_PAGE };

/**
 * Both internal and leaf page are inherited from this page.
 *
 * It actually serves as a header part for each B+ tree page and
 * contains information shared by both leaf page and internal page.
 *
 * Header format (size in byte, 24 bytes in total):
 * ----------------------------------------------------------------------------
 * | PageType (4) | LSN (4) | CurrentSize (4) | MaxSize (4) |
 * ----------------------------------------------------------------------------
 * | ParentPageId (4) | PageId(4) |
 * ----------------------------------------------------------------------------
 */
class BPlusTreePage {
   public:
    bool IsLeafPage() const;
    bool IsRootPage() const;
    void SetPageType(IndexPageType page_type);

    int GetSize() const;
    void SetSize(int size);
    void IncreaseSize(int amount);

    int GetMaxSize() const;
    void SetMaxSize(int max_size);
    int GetMinSize() const;

    size_t GetParentPageId() const;
    void SetParentPageId(size_t parent_page_id);

    size_t GetPageId() const;
    void SetPageId(size_t page_id);

   private:
};

#define B_PLUS_TREE_LEAF_PAGE_TYPE \
    BPlusTreeLeafPage<KeyType, ValueType, KeyComparator>
#define LEAF_PAGE_HEADER_SIZE 28
#define LEAF_PAGE_SIZE \
    ((PAGE_SIZE - LEAF_PAGE_HEADER_SIZE) / sizeof(MappingType))

INDEX_TEMPLATE_ARGUMENTS
class BPlusTreeLeafPage : public BPlusTreePage {
   public:
    // After creating a new leaf page from buffer pool, must call initialize
    // method to set default values
    void Init(size_t page_id, size_t parent_id = INVALID_PAGE_ID,
              int max_size = LEAF_PAGE_SIZE);
    // helper methods
    size_t GetNextPageId() const;
    void SetNextPageId(size_t next_page_id);
    KeyType KeyAt(int index) const;
    int KeyIndex(const KeyType &key, const KeyComparator &comparator) const;
    const MappingType &GetItem(int index);

    // insert and delete methods
    int Insert(const KeyType &key, const ValueType &value,
               const KeyComparator &comparator);
    bool Lookup(const KeyType &key, ValueType *value,
                const KeyComparator &comparator) const;
    int RemoveAndDeleteRecord(const KeyType &key,
                              const KeyComparator &comparator);

    // Split and Merge utility methods
    void MoveHalfTo(BPlusTreeLeafPage *recipient);
    void MoveAllTo(BPlusTreeLeafPage *recipient);
    void MoveFirstToEndOf(BPlusTreeLeafPage *recipient);
    void MoveLastToFrontOf(BPlusTreeLeafPage *recipient);

   private:
    void CopyNFrom(MappingType *items, int size);
    void CopyLastFrom(const MappingType &item);
    void CopyFirstFrom(const MappingType &item);
    size_t next_page_id_;
    MappingType array_[0];
};

#define B_PLUS_TREE_INTERNAL_PAGE_TYPE \
    BPlusTreeInternalPage<KeyType, ValueType, KeyComparator>
#define INTERNAL_PAGE_HEADER_SIZE 24
#define INTERNAL_PAGE_SIZE \
    ((PAGE_SIZE - INTERNAL_PAGE_HEADER_SIZE) / (sizeof(MappingType)))

INDEX_TEMPLATE_ARGUMENTS
class BPlusTreeInternalPage : public BPlusTreePage {
   public:
    // must call initialize method after "create" a new node
    void Init(page_id_t page_id, page_id_t parent_id = INVALID_PAGE_ID,
              int max_size = INTERNAL_PAGE_SIZE);

    KeyType KeyAt(int index) const;
    void SetKeyAt(int index, const KeyType &key);
    int ValueIndex(const ValueType &value) const;
    ValueType ValueAt(int index) const;

    ValueType Lookup(const KeyType &key, const KeyComparator &comparator) const;
    void PopulateNewRoot(const ValueType &old_value, const KeyType &new_key,
                         const ValueType &new_value);
    int InsertNodeAfter(const ValueType &old_value, const KeyType &new_key,
                        const ValueType &new_value);
    void Remove(int index);
    ValueType RemoveAndReturnOnlyChild();

    // Split and Merge utility methods
    void MoveAllTo(BPlusTreeInternalPage *recipient, const KeyType &middle_key,
                   BufferPoolManager *buffer_pool_manager);
    void MoveHalfTo(BPlusTreeInternalPage *recipient,
                    BufferPoolManager *buffer_pool_manager);
    void MoveFirstToEndOf(BPlusTreeInternalPage *recipient,
                          const KeyType &middle_key,
                          BufferPoolManager *buffer_pool_manager);
    void MoveLastToFrontOf(BPlusTreeInternalPage *recipient,
                           const KeyType &middle_key,
                           BufferPoolManager *buffer_pool_manager);

   private:
    void CopyNFrom(MappingType *items, int size,
                   BufferPoolManager *buffer_pool_manager);
    void CopyLastFrom(const MappingType &pair,
                      BufferPoolManager *buffer_pool_manager);
    void CopyFirstFrom(const MappingType &pair,
                       BufferPoolManager *buffer_pool_manager);
    MappingType array_[0];
};

}  // namespace huang
