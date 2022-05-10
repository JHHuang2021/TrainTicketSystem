#include "include/buffer_pool_manager.h"

namespace huang {
BufferPoolManager::BufferPoolManager(size_t pool_size,
                                     DiskManager *disk_manager,
                                     Replacer<Page *> *replacer) {
    pool_size_ = pool_size;
    pages_ = new Page[pool_size_];
    disk_manager_ = disk_manager;
    replacer_ = replacer;
    page_table_.clear();
    for (int i = 0; i < pool_size_; i++) free_.push_back(&pages_[i]);
}
BufferPoolManager::~BufferPoolManager() {
    FlushAllPage();
    delete[] pages_;
}
size_t BufferPoolManager::GetPoolSize() { return pool_size_; }
/**
 * 1. search hash table.
 *  1.1 if exist, pin the page and return immediately
 *  1.2 if no exist, find a replacement entry from either free list or lru
 * replacer. (NOTE: always find from free list first)
 * 2. If the entry chosen for replacement is dirty, write it back to disk.
 * 3. Delete the entry for the old page from the hash table and insert an
 * entry for the new page.
 * 4. Update page metadata, read page content from disk file and return page
 * pointer
 */
Page *BufferPoolManager::FetchPage(page_id_t page_id) {
    try {
        Page *ret;
        ret = page_table_.at(page_id);
        ret->is_dirty_ = true;
        return ret;
    } catch (...) {
        Page *targetpage = findUnusedPage();
        disk_manager_->ReadPage(page_id, targetpage->GetData());
        targetpage->page_id_ = page_id;
        targetpage->is_dirty_ = true;
        return targetpage;
    }
}
/*
 * Implementation of unpin page
 * if pin_count>0, decrement it and if it becomes zero, put it back to
 * replacer if pin_count<=0 before this call, return false. is_dirty: set the
 * dirty flag of this page
 */
bool BufferPoolManager::UnpinPage(page_id_t page_id, bool is_dirty){};
/*
 * Used to flush a particular page of the buffer pool to disk. Should call the
 * write_page method of the disk manager
 * if page is not found in page table, return false
 * NOTE: make sure page_id != INVALID_PAGE_ID
 */
bool BufferPoolManager::FlushPage(page_id_t page_id) {
    if (page_id == INVALID_PAGE_ID || !page_table_.count(page_id)) return false;
    Page *page = page_table_[page_id];
    if (page->is_dirty_) {
        page->is_dirty_ = false;
        disk_manager_->WritePage(page_id, page->GetData());
    }
    return true;
};

/**
 * User should call this method if needs to create a new page. This routine
 * will call disk manager to allocate a page.
 * Buffer pool manager should be responsible to choose a victim page either
 * from free list or lru replacer(NOTE: always choose from free list first),
 * update new page's metadata, zero out memory and add corresponding entry
 * into page table. return nullptr if all the pages in pool are pinned
 */
Page *BufferPoolManager::NewPage(page_id_t *page_id) {
    *page_id = disk_manager_->AllocatePage();
    Page *targetpage = findUnusedPage();
    targetpage->page_id_ = *page_id;
    targetpage->is_dirty_ = true;
    page_table_[*page_id] = targetpage;
    return targetpage;
};
/**
 * User should call this method for deleting a page. This routine will call
 * disk manager to deallocate the page. First, if page is found within page
 * table, buffer pool manager should be reponsible for removing this entry out
 * of page table, reseting page metadata and adding back to free list. Second,
 * call disk manager's DeallocatePage() method to delete from disk file. If
 * the page is found within page table, but pin_count != 0, return false
 */
bool BufferPoolManager::DeletePage(page_id_t page_id) {
    auto find = page_table_.find(page_id);
    if (find != page_table_.end()) {
        page_table_.erase(find);
        pages_[page_id].ResetMemory();
        pages_[page_id].page_id_ = INVALID_PAGE_ID;
        pages_[page_id].is_dirty_ = false;
        free_.push_front(&pages_[page_id]);
        return true;
    }
    return false;
};
/**
 * find unused page from free list first than replacer, return null if not
 * enough memory
 */
Page *BufferPoolManager::findUnusedPage() {
    Page *page;
    if (!free_.empty()) {
        // fetch Page from free list first
        page = free_.front();
        free_.pop_front();
    } else {
        // fetch Page from replacer
        if (!replacer_->Victim(page)) {
            return nullptr;
        }
        // write page back to disk
        page_table_.erase(page_table_.find(page->page_id_));
        if (page->is_dirty_) {
            disk_manager_->WritePage(page->page_id_, page->GetData());
            page->is_dirty_ = false;
        }
        page->ResetMemory();
        page->page_id_ = INVALID_PAGE_ID;
    }
    return page;
};

void BufferPoolManager::FlushAllPage() {
    for (int i = 0; i < pool_size_; i++) FlushPage(i);
};

}  // namespace huang