#pragma once

#include <queue>
#include <string>
#include <vector>

#include "b_plus_tree_page.hpp"
#include "page.h"
namespace huang {

#define BPLUSTREE_TYPE BPlusTree<KeyType, ValueType>

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
    using BPlusLeafPage = BPlusTreeLeafPage<KeyType, ValueType>;
    using BPlusInternalPage = BPlusTreeInternalPage<KeyType>;

   public:
    explicit BPlusTree(std::string name, BufferPoolManager *buffer_pool_manager)
        : index_name_(name), buffer_pool_manager_(buffer_pool_manager) {
        HeaderPage *header_page = reinterpret_cast<HeaderPage *>(
            buffer_pool_manager_->FetchPage(0)->GetData());
        size_ = header_page->size;
    };

    ~BPlusTree() {
        HeaderPage *header_page = reinterpret_cast<HeaderPage *>(
            buffer_pool_manager_->FetchPage(0)->GetData());
        header_page->size = size_;
    }

    // Returns true if this B+ tree has no keys and values.
    bool IsEmpty() const { return !size_; };

    // Insert a key-value pair into this B+ tree.
    void Insert(const KeyType &key, const ValueType &value) {
        if (IsEmpty())
            StartNewTree(key, value);
        else
            InsertIntoLeaf(key, value);
        size_++;
    };

    // Remove a key and its value from this B+ tree.
    bool Remove(const KeyType &key) {
        HeaderPage *header_page = reinterpret_cast<HeaderPage *>(
            buffer_pool_manager_->FetchPage(0)->GetData());
        BPlusTreePage *b_plus_tree_page = reinterpret_cast<BPlusTreePage *>(
            buffer_pool_manager_->FetchPage(header_page->GetRootId())
                ->GetData());
        BPlusInternalPage *b_plus_int_page =
            reinterpret_cast<BPlusInternalPage *>(b_plus_tree_page);
        BPlusLeafPage *b_plus_leaf_page = nullptr;

        auto de = b_plus_int_page->data_;

        while (!b_plus_tree_page->IsLeafPage()) {
            int i;
            for (i = 0; i < b_plus_tree_page->size_; i++)
                if (key < b_plus_int_page->data_[i].first) {
                    b_plus_tree_page = reinterpret_cast<BPlusTreePage *>(
                        buffer_pool_manager_
                            ->FetchPage(b_plus_int_page->data_[i].second)
                            ->GetData());
                    break;
                }
            if (i == b_plus_tree_page->size_ &&
                !b_plus_tree_page->IsLeafPage()) {
                b_plus_tree_page = reinterpret_cast<BPlusTreePage *>(
                    buffer_pool_manager_
                        ->FetchPage(b_plus_int_page->data_[i].second)
                        ->GetData());
                b_plus_int_page =
                    reinterpret_cast<BPlusInternalPage *>(b_plus_tree_page);
            }
        }

        int i;
        b_plus_leaf_page = reinterpret_cast<BPlusLeafPage *>(b_plus_tree_page);
        de = b_plus_leaf_page->data_;

        for (i = 0; i < b_plus_leaf_page->size_; i++)
            if (key == b_plus_leaf_page->data_[i].first) break;
        if (i == b_plus_leaf_page->size_) return false;

        for (int j = i + 1; j < b_plus_leaf_page->size_; j++)
            b_plus_leaf_page->data_[j - 1] = b_plus_leaf_page->data_[j];
        b_plus_leaf_page->size_--;

        b_plus_tree_page = b_plus_leaf_page;
        while (b_plus_tree_page->size_ < B_PLUS_TREE_MIN_SIZE &&
               (b_plus_tree_page->lst != -1 || b_plus_tree_page->nxt != -1)) {
            CoalesceOrRedistribute(b_plus_tree_page);
            b_plus_tree_page = reinterpret_cast<BPlusTreePage *>(
                buffer_pool_manager_
                    ->FetchPage(b_plus_tree_page->parent_page_id_)
                    ->GetData());
        }
        size_--;

        return true;
    };

    // return the value according to the key provided
    std::pair<ValueType *, bool> GetValue(const KeyType &key) {
        HeaderPage *header_page = reinterpret_cast<HeaderPage *>(
            buffer_pool_manager_->FetchPage(0)->GetData());
        BPlusTreePage *b_plus_tree_page = reinterpret_cast<BPlusTreePage *>(
            buffer_pool_manager_->FetchPage(header_page->GetRootId())
                ->GetData());
        BPlusInternalPage *b_plus_int_page =
            reinterpret_cast<BPlusInternalPage *>(b_plus_tree_page);
        BPlusLeafPage *b_plus_leaf_page = nullptr;

        while (!b_plus_tree_page->IsLeafPage()) {
            int i;
            for (i = 0; i < b_plus_tree_page->size_; i++)
                if (key < b_plus_int_page->data_[i].first) {
                    b_plus_tree_page = reinterpret_cast<BPlusTreePage *>(
                        buffer_pool_manager_
                            ->FetchPage(b_plus_int_page->data_[i].second)
                            ->GetData());
                    b_plus_int_page =
                        reinterpret_cast<BPlusInternalPage *>(b_plus_tree_page);
                    break;
                }
            if (i == b_plus_tree_page->size_ &&
                !b_plus_tree_page->IsLeafPage()) {
                b_plus_tree_page = reinterpret_cast<BPlusTreePage *>(
                    buffer_pool_manager_
                        ->FetchPage(b_plus_int_page->data_[i].second)
                        ->GetData());
                b_plus_int_page =
                    reinterpret_cast<BPlusInternalPage *>(b_plus_tree_page);
            }
        }
        b_plus_leaf_page = reinterpret_cast<BPlusLeafPage *>(b_plus_tree_page);
        auto de = b_plus_leaf_page->data_;
        for (int i = 0; i < b_plus_leaf_page->size_; i++)
            if (b_plus_leaf_page->data_[i].first == key)
                return {&b_plus_leaf_page->data_[i].second, true};
        return {nullptr, false};
    };

    // return the value between two keys
    void GetValue(const KeyType &first_key, const KeyType &last_key,
                  std::vector<ValueType> *result){};

    void Debug() {
        HeaderPage *header_page = reinterpret_cast<HeaderPage *>(
            buffer_pool_manager_->FetchPage(0)->GetData());
        for (int i = 1; i < size_; i++) {
            BPlusLeafPage *leaf_page = reinterpret_cast<BPlusLeafPage *>(
                buffer_pool_manager_->FetchPage(i)->GetData());
            BPlusInternalPage *int_page =
                reinterpret_cast<BPlusInternalPage *>(leaf_page);
            auto la = leaf_page->page_id_;
            auto lb = leaf_page->page_type_;
            auto lc = leaf_page->parent_page_id_;
            auto ld = leaf_page->lst;
            auto lf = leaf_page->nxt;
            auto ia = int_page->page_id_;
            auto ib = int_page->page_type_;
            auto ic = int_page->parent_page_id_;
            auto id = int_page->lst;
            auto ie = int_page->nxt;

            auto de_leaf = leaf_page->data_;
            auto de_int = int_page->data_;
            int j;
        }
    }

   private:
    void StartNewTree(const KeyType &key, const ValueType &value) {
        page_id_t header_page_id;
        HeaderPage *header_page = reinterpret_cast<HeaderPage *>(
            buffer_pool_manager_->NewPage(&header_page_id)->GetData());
        header_page->ModifyRoot(1);

        BPlusLeafPage *root_page = reinterpret_cast<BPlusLeafPage *>(
            buffer_pool_manager_->NewPage(&header_page_id)->GetData());
        root_page->page_id_ = header_page_id;
        root_page->lst = root_page->nxt = root_page->parent_page_id_ = -1;
        root_page->page_type_ = LEAF_PAGE;
        root_page->is_root = true;
        InsertIntoLeaf(key, value);
    };

    void InsertIntoLeaf(const KeyType &key, const ValueType &value) {
        HeaderPage *header_page = reinterpret_cast<HeaderPage *>(
            buffer_pool_manager_->FetchPage(0)->GetData());
        BPlusTreePage *b_plus_tree_page = reinterpret_cast<BPlusTreePage *>(
            buffer_pool_manager_->FetchPage(header_page->GetRootId())
                ->GetData());
        BPlusInternalPage *b_plus_int_page =
            reinterpret_cast<BPlusInternalPage *>(b_plus_tree_page);
        BPlusLeafPage *b_plus_leaf_page = nullptr;

        while (!b_plus_tree_page->IsLeafPage()) {
            int i;
            auto de = b_plus_int_page->data_;
            for (i = 0; i < b_plus_tree_page->size_; i++)
                if (key < b_plus_int_page->data_[i].first) {
                    b_plus_tree_page = reinterpret_cast<BPlusTreePage *>(
                        buffer_pool_manager_
                            ->FetchPage(b_plus_int_page->data_[i].second)
                            ->GetData());
                    b_plus_int_page =
                        reinterpret_cast<BPlusInternalPage *>(b_plus_tree_page);
                    break;
                }
            if (i == b_plus_tree_page->size_ &&
                !b_plus_tree_page->IsLeafPage()) {
                b_plus_tree_page = reinterpret_cast<BPlusTreePage *>(
                    buffer_pool_manager_
                        ->FetchPage(b_plus_int_page->data_[i].second)
                        ->GetData());
                b_plus_int_page =
                    reinterpret_cast<BPlusInternalPage *>(b_plus_tree_page);
            }
        }

        // should move the element in data_
        int i;
        b_plus_leaf_page = reinterpret_cast<BPlusLeafPage *>(b_plus_tree_page);
        for (i = 0; i < b_plus_tree_page->size_; i++)
            if (key < b_plus_leaf_page->data_[i].first) break;
        for (int j = b_plus_leaf_page->size_ - 1; j >= i; j--)
            b_plus_leaf_page->data_[j + 1] = b_plus_leaf_page->data_[j];
        b_plus_leaf_page->data_[i] = std::make_pair(key, value);
        b_plus_leaf_page->size_++;

        b_plus_tree_page = b_plus_leaf_page;
        while (b_plus_tree_page->size_ > B_PLUS_TREE_MAX_SIZE) {
            BPlusTreePage *tmp_page = Split(b_plus_leaf_page);

            b_plus_tree_page = reinterpret_cast<BPlusTreePage *>(
                buffer_pool_manager_
                    ->FetchPage(b_plus_tree_page->parent_page_id_)
                    ->GetData());
        }
    };

    void InsertIntoParent(BPlusTreePage *old_node, BPlusTreePage *new_node) {
        BPlusInternalPage *parent_page = reinterpret_cast<BPlusInternalPage *>(
            buffer_pool_manager_->FetchPage(old_node->parent_page_id_)
                ->GetData());
        int i;
        KeyType key;
        if (new_node->IsLeafPage()) {
            BPlusLeafPage *tmp_page =
                reinterpret_cast<BPlusLeafPage *>(new_node);
            key = tmp_page->data_[0].first;
        } else {
            BPlusInternalPage *tmp_page =
                reinterpret_cast<BPlusInternalPage *>(new_node);
            key = tmp_page->data_[0].first;
        }

        auto de = parent_page->data_;
        for (i = 0; i < parent_page->size_; i++)
            if (key < parent_page->data_[i].first) break;
        parent_page->data_[i].second = new_node->page_id_;
        for (int j = parent_page->size_; j >= i; j--)
            parent_page->data_[j + 1] = parent_page->data_[j];
        parent_page->data_[i] = {key, old_node->page_id_};
        parent_page->size_++;
    };

    BPlusTreePage *Split(BPlusTreePage *node) {
        page_id_t new_page_id;
        BPlusTreePage *new_page = reinterpret_cast<BPlusTreePage *>(
            buffer_pool_manager_->NewPage(&new_page_id)->GetData());
        new_page->page_type_ = node->page_type_;
        new_page->page_id_ = new_page_id;

        if (node->nxt != -1) {
            BPlusTreePage *nxt_page = reinterpret_cast<BPlusTreePage *>(
                buffer_pool_manager_->FetchPage(node->nxt)->GetData());
            nxt_page->lst = new_page_id;
        }
        new_page->nxt = node->nxt;
        node->nxt = new_page_id;
        new_page->lst = node->page_id_;

        if (node->IsLeafPage()) {
            BPlusLeafPage *tmp_page = reinterpret_cast<BPlusLeafPage *>(node);
            tmp_page->PageSplit(new_page);
        } else {
            BPlusInternalPage *tmp_page =
                reinterpret_cast<BPlusInternalPage *>(node);
            tmp_page->PageSplit(new_page);
        }

        if (!node->IsRootPage())
            InsertIntoParent(node, new_page);
        else {
            HeaderPage *header_page = reinterpret_cast<HeaderPage *>(
                buffer_pool_manager_->FetchPage(0)->GetData());

            page_id_t root_page_id;
            BPlusInternalPage *root_page =
                reinterpret_cast<BPlusInternalPage *>(
                    buffer_pool_manager_->NewPage(&root_page_id)->GetData());
            BPlusLeafPage *leaf_new_page =
                reinterpret_cast<BPlusLeafPage *>(new_page);

            auto del = leaf_new_page->data_;
            auto der = root_page->data_;

            header_page->ModifyRoot(root_page_id);
            node->is_root = false;
            root_page->is_root = true;
            root_page->page_id_ = root_page_id;
            root_page->page_type_ = INTERNAL_PAGE;
            root_page->parent_page_id_ = -1;
            root_page->lst = root_page->nxt = -1;

            node->parent_page_id_ = root_page_id;
            new_page->parent_page_id_ = root_page_id;
            root_page->data_[0] = {leaf_new_page->data_[0].first,
                                   node->page_id_};
            root_page->data_[1] = {KeyType(), new_page_id};
            root_page->size_ = 1;
        }

        return new_page;
    };

    void CoalesceOrRedistribute(BPlusTreePage *&node) {
        BPlusInternalPage *parent_page = reinterpret_cast<BPlusInternalPage *>(
            buffer_pool_manager_->FetchPage(node->parent_page_id_)->GetData());
        // always insure that node < sibling_page
        BPlusTreePage *sibling_page = nullptr;
        if (node->nxt != -1)
            sibling_page = reinterpret_cast<BPlusTreePage *>(
                buffer_pool_manager_->FetchPage(node->nxt)->GetData());
        else {
            sibling_page = node;
            node = reinterpret_cast<BPlusTreePage *>(
                buffer_pool_manager_->FetchPage(node->lst)->GetData());
        }
        int index;
        for (index = 0; index < parent_page->size_; index++)
            if (parent_page->data_[index].second == sibling_page->page_id_)
                break;

        if (sibling_page->size_ + node->size_ > B_PLUS_TREE_MAX_SIZE)
            Redistribute(sibling_page, node, parent_page, index);
        else
            Coalesce(sibling_page, node, parent_page, index);
    };

    void Coalesce(BPlusTreePage *neighbor_node, BPlusTreePage *node,
                  BPlusInternalPage *parent, int neighbor_node_index) {
        if (neighbor_node->nxt != -1) {
            BPlusTreePage *nxt_page = reinterpret_cast<BPlusTreePage *>(
                buffer_pool_manager_->FetchPage(neighbor_node->nxt)->GetData());
            nxt_page->lst = node->page_id_;
        }
        node->nxt = neighbor_node->nxt;

        if (node->IsLeafPage()) {
            BPlusLeafPage *leaf_page = reinterpret_cast<BPlusLeafPage *>(node);
            leaf_page->Merge(neighbor_node);
        } else {
            BPlusInternalPage *int_page =
                reinterpret_cast<BPlusInternalPage *>(node);
            int_page->Merge(neighbor_node);
        }
        for (int i = neighbor_node_index; i <= parent->size_; i++)
            parent->data_[i] = parent->data_[i + 1];
        KeyType *new_key = nullptr;
        BPlusTreePage *tmp_page = reinterpret_cast<BPlusTreePage *>(
            buffer_pool_manager_
                ->FetchPage(parent->data_[neighbor_node_index].second)
                ->GetData());
        if (tmp_page->IsLeafPage()) {
            BPlusLeafPage *leaf_page =
                reinterpret_cast<BPlusLeafPage *>(tmp_page);
            new_key = &leaf_page->data_[0].first;
        } else {
            BPlusInternalPage *int_page =
                reinterpret_cast<BPlusInternalPage *>(tmp_page);
            new_key = &int_page->data_[0].first;
        }
        parent->data_[neighbor_node_index - 1].first = *new_key;

        parent->size_--;
    };

    void Redistribute(BPlusTreePage *neighbor_node, BPlusTreePage *node,
                      BPlusInternalPage *parent, int neighbor_node_index) {
        if (node->size_ < B_PLUS_TREE_MAX_SIZE)
            if (node->IsLeafPage()) {
                BPlusLeafPage *leaf_page =
                    reinterpret_cast<BPlusLeafPage *>(node);
                leaf_page->MoveRhsFirst(parent, neighbor_node,
                                        neighbor_node_index);
            } else {
                BPlusInternalPage *int_page =
                    reinterpret_cast<BPlusInternalPage *>(node);
                int_page->MoveRhsFirst(parent, neighbor_node,
                                       neighbor_node_index);
            }
        else if (node->IsLeafPage()) {
            BPlusLeafPage *leaf_page = reinterpret_cast<BPlusLeafPage *>(node);
            leaf_page->MoveLhsLast(parent, neighbor_node, neighbor_node_index);
        } else {
            BPlusInternalPage *int_page =
                reinterpret_cast<BPlusInternalPage *>(node);
            int_page->MoveLhsLast(parent, neighbor_node, neighbor_node_index);
        }
    };

    // member variable
    std::string index_name_;
    BufferPoolManager *buffer_pool_manager_;
    size_t size_;
};

}  // namespace huang
