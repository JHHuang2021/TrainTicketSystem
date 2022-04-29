#pragma once

#include <list>
#include <mutex>  // NOLINT
#include <vector>

namespace huang {

/**
 * LRUReplacer implements the Least Recently Used replacement policy.
 */
template <typename T>
class Replacer {
   public:
    explicit Replacer();
    ~Replacer();
    void Insert(const T &value);
    bool Victim(T &value);
    bool Erase(const T &value);
    size_t Size();
    /**
     * Create a new LRUReplacer.
     * @param num_pages the maximum number of pages the LRUReplacer will be
     * required to store
     */

    /**
     * Destroys the LRUReplacer.
     */

   private:
    // TODO(student): implement me!
};

}  // namespace huang
