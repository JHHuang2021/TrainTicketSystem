#include "include/lru_replacer.h"

#include "include/page.h"

namespace huang {
template <class T>
LRUReplacer<T>::LRUReplacer() {}
template <class T>
LRUReplacer<T>::~LRUReplacer() {}
template <class T>
bool LRUReplacer<T>::Victim(T &value) {
    return false;
}
template <class T>
size_t LRUReplacer<T>::Size() {
    return 0;
}

}  // namespace huang
