
#pragma once

#include <iostream>

#include "buffer_pool_manager.h"

namespace huang {

#define INDEX_TEMPLATE_ARGUMENTS template <typename KeyType, typename ValueType>

class HeaderPage {
   private:
    page_id_t root;

   public:
    void ModifyRoot(const page_id_t &root_id) { root = root_id; };

    page_id_t GetRootId() { return root; };
};

// define page type enum
enum IndexPageType { INVALID_INDEX_PAGE = 0, LEAF_PAGE, INTERNAL_PAGE };

#define B_PLUS_TREE_SIZE 11
#define B_PLUS_TREE_MAX_SIZE 9
#define B_PLUS_TREE_MIN_SIZE 4

class BPlusTreePage {
   public:
    BPlusTreePage() {
        page_type_ = INVALID_INDEX_PAGE;
        is_root = false;
        parent_page_id_ = -1;
        page_id_ = -1;
        nxt = -1, lst = -1;
    };
    bool IsLeafPage() const { return page_type_ == LEAF_PAGE; };
    bool IsRootPage() const { return is_root; };

    IndexPageType page_type_ = INVALID_INDEX_PAGE;
    int size_;
    bool is_root = false;
    page_id_t parent_page_id_ = -1;
    page_id_t page_id_ = -1;
    page_id_t nxt = -1, lst = -1;
};

template <class KeyType>
class BPlusTreeInternalPage : public BPlusTreePage {
   public:
    std::pair<KeyType, page_id_t> data_[B_PLUS_TREE_SIZE];

    void Clear() {
        this->is_root = false;
        this->lst = -1;
        this->nxt = -1;
        this->page_id_ = -1;
        this->parent_page_id_ = -1;
        this->page_type_ = INVALID_INDEX_PAGE;
        this->size_ = 0;
        memset(this->data_, 0, sizeof(this->data_));
    }

    void PageSplit(BPlusTreePage *new_page) {
        BPlusTreeInternalPage *int_new_page =
            reinterpret_cast<BPlusTreeInternalPage *>(new_page);
        new_page->page_type_ = this->page_type_;
        new_page->parent_page_id_ = this->parent_page_id_;
        new_page->size_ = this->size_ / 2;
        this->size_ -= new_page->size_;

        // this->nxt = new_page->page_id_;
        // new_page->lst = this->page_id_;
        // new_page->nxt=-1;

        for (int i = this->size_; i < this->size_ + new_page->size_; i++)
            int_new_page->data_[i - this->size_ - 1] = this->data_[i];
    };
    void Merge(BPlusTreePage *rhs_page) {
        BPlusTreeInternalPage *internal_page =
            reinterpret_cast<BPlusTreeInternalPage *>(rhs_page);
        for (int i = size_; i < size_ + rhs_page->size_; i++)
            data_[i] = internal_page->data_[i - size_];

        // this->nxt = rhs_page->nxt;
        size_ += rhs_page->size_;
        internal_page->Clear();
    };
    void MoveRhsFirst(BPlusTreePage *parent_page, BPlusTreePage *rhs_page,
                      int neighbor_node_index) {
        BPlusTreeInternalPage *p = reinterpret_cast<BPlusTreeInternalPage *>(
                                  parent_page),
                              *r = reinterpret_cast<BPlusTreeInternalPage *>(
                                  rhs_page);
        this->data_[this->size_++] = r->data_[0];
        p->data_[neighbor_node_index - 1].first = r->data_[1].first;
        for (int i = 1; i < r->size_; i++) r->data_[i - 1] = r->data_[i];
        r->size_--;
    };
    void MoveLhsLast(BPlusTreePage *parent_page, BPlusTreePage *rhs_page,
                     int neighbor_node_index) {
        BPlusTreeInternalPage *p = reinterpret_cast<BPlusTreeInternalPage *>(
                                  parent_page),
                              *r = reinterpret_cast<BPlusTreeInternalPage *>(
                                  rhs_page);
        for (int i = r->size_ - 1; i >= 0; i--) r->data_[i + 1] = r->data_[i];
        r->size_++;
        r->data_[0] = this->data_[this->size_ - 1];
        p->data_[neighbor_node_index - 1].first =
            this->data_[--this->size_].first;
    };
};

template <class KeyType, class ValueType>
class BPlusTreeLeafPage : public BPlusTreePage {
   public:
    std::pair<KeyType, ValueType> data_[B_PLUS_TREE_SIZE];

    void Clear() {
        this->is_root = false;
        this->lst = -1;
        this->nxt = -1;
        this->page_id_ = -1;
        this->parent_page_id_ = -1;
        this->page_type_ = INVALID_INDEX_PAGE;
        this->size_ = 0;
        memset(this->data_, 0, sizeof(this->data_));
    }

    void PageSplit(BPlusTreePage *new_page) {
        BPlusTreeLeafPage *int_new_page =
            reinterpret_cast<BPlusTreeLeafPage *>(new_page);
        new_page->page_type_ = this->page_type_;
        new_page->parent_page_id_ = this->parent_page_id_;
        new_page->size_ = this->size_ / 2;
        this->size_ -= new_page->size_;

        // this->nxt = new_page->page_id_;
        // new_page->lst = this->page_id_;
        // new_page->nxt = -1;

        for (int i = this->size_; i < this->size_ + new_page->size_; i++)
            int_new_page->data_[i - this->size_] = this->data_[i];
    };
    void Merge(BPlusTreePage *rhs_page) {
        BPlusTreeLeafPage *leaf_page =
            reinterpret_cast<BPlusTreeLeafPage *>(rhs_page);
        for (int i = size_; i < size_ + rhs_page->size_; i++)
            data_[i] = leaf_page->data_[i - size_];

        // this->nxt = rhs_page->nxt;
        size_ += rhs_page->size_;
        leaf_page->Clear();
    };
    void MoveRhsFirst(BPlusTreePage *parent_page, BPlusTreePage *rhs_page,
                      int neighbor_node_index) {
        BPlusTreeInternalPage<KeyType> *p =
            reinterpret_cast<BPlusTreeInternalPage<KeyType> *>(parent_page);
        BPlusTreeLeafPage *r = reinterpret_cast<BPlusTreeLeafPage *>(rhs_page);
        this->data_[this->size_++] = r->data_[0];
        p->data_[neighbor_node_index - 1].first = r->data_[1].first;
        for (int i = 1; i < r->size_; i++) r->data_[i - 1] = r->data_[i];
        r->size_--;
    };
    void MoveLhsLast(BPlusTreePage *parent_page, BPlusTreePage *rhs_page,
                     int neighbor_node_index) {
        BPlusTreeInternalPage<KeyType> *p =
            reinterpret_cast<BPlusTreeInternalPage<KeyType> *>(parent_page);
        BPlusTreeLeafPage *r = reinterpret_cast<BPlusTreeLeafPage *>(rhs_page);
        auto de = this->data_;
        auto dep = p->data_;
        auto der = r->data_;
        for (int i = r->size_ - 1; i >= 0; i--) r->data_[i + 1] = r->data_[i];
        r->size_++;
        r->data_[0] = this->data_[this->size_ - 1];
        p->data_[neighbor_node_index - 1].first =
            this->data_[--this->size_].first;
    };
};

}  // namespace huang
