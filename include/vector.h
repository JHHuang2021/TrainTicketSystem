#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include <climits>
#include <cstddef>

#include "exceptions.h"

/**
 * @brief A data container like `std::vector`.
 * All undocumented functions act like `std::vector`.
 */
template <typename T>
class vector {
 public:
  class const_iterator;
  class iterator {
    // The following code is written for the C++ type_traits library.
    // Type traits is a C++ feature for describing certain properties of a type.
    // For instance, for an iterator, iterator::value_type is the type that the
    // iterator points to.
    // STL algorithms and containers may use these type_traits (e.g. the following
    // typedef) to work properly. In particular, without the following code,
    // @code{std::sort(iter, iter1);} would not compile.
    // See these websites for more information:
    // https://en.cppreference.com/w/cpp/header/type_traits
    // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
    // About iterator_category: https://en.cppreference.com/w/cpp/iterator
   public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = std::output_iterator_tag;

   private:
    pointer current;
    vector *father;
    explicit iterator(pointer current, vector *father) : current(current), father(father) {}
    friend vector;

   public:
    iterator operator+(const int &n) const { return iterator(current + n, father); }
    iterator operator-(const int &n) const { return iterator(current - n, father); }
    /**
     * @brief Returns the distance between two iterators,
     * if these two iterators point to different vectors, throw `invaild_iterator`.
     */
    int operator-(const iterator &rhs) const {
      if (father != rhs.father) throw invalid_iterator();
      return current - rhs.current;
    }
    iterator &operator+=(const int &n) {
      current += n;
      return *this;
    }
    iterator &operator-=(const int &n) {
      current -= n;
      return *this;
    }
    iterator operator++(int) {
      iterator tmp = *this;
      current++;
      return tmp;
    }
    iterator &operator++() {
      current++;
      return *this;
    }
    iterator operator--(int) {
      iterator tmp = *this;
      current--;
      return tmp;
    }
    iterator &operator--() {
      current--;
      return *this;
    }
    T &operator*() const { return *current; }
    bool operator==(const iterator &rhs) const { return current == rhs.current; }
    bool operator==(const const_iterator &rhs) const { return current == rhs.current; }

    bool operator!=(const iterator &rhs) const { return !operator==(rhs); }
    bool operator!=(const const_iterator &rhs) const { return !operator==(rhs); }
  };

  class const_iterator {
   public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = std::output_iterator_tag;

   private:
    pointer current;
    const vector *father;
    explicit const_iterator(pointer current, const vector *father) : current(current), father(father) {}
    friend vector;

   public:
    const_iterator operator+(const int &n) const { return const_iterator(current + n, father); }
    const_iterator operator-(const int &n) const { return const_iterator(current - n, father); }
    /**
     * @brief Returns the distance between two iterators,
     * if these two iterators point to different vectors, throw `invaild_iterator`.
     */
    int operator-(const iterator &rhs) const {
      if (father != rhs.father) throw invalid_iterator();
      return current - rhs.current;
    }
    const_iterator &operator+=(const int &n) {
      current += n;
      return *this;
    }
    const_iterator &operator-=(const int &n) {
      current -= n;
      return *this;
    }
    const_iterator operator++(int) {
      iterator tmp = *this;
      current++;
      return tmp;
    }
    const_iterator &operator++() {
      current++;
      return *this;
    }
    const_iterator operator--(int) {
      iterator tmp = *this;
      current--;
      return tmp;
    }
    const_iterator &operator--() {
      current--;
      return *this;
    }
    const T &operator*() const { return *current; }
    bool operator==(const iterator &rhs) const { return current == rhs.current; }
    bool operator==(const const_iterator &rhs) const { return current == rhs.current; }
    bool operator!=(const iterator &rhs) const { return !operator==(rhs); }
    bool operator!=(const const_iterator &rhs) const { return !operator==(rhs); }
  };

 public:
  using pointer = T *;

 private:
  pointer start;
  pointer finish;
  pointer end_of_storage;
  pointer ConstructAndCopy(pointer src_begin, pointer src_finish, pointer dst_begin) {
    for (pointer i = src_begin; i != src_finish; ++i) {
      new (dst_begin) T(*i);
      ++dst_begin;
    }
    return dst_begin;
  }
  pointer ConstructAndMove(pointer src_begin, pointer src_finish, pointer dst_begin) {
    for (pointer i = src_begin; i != src_finish; ++i) {
      new (dst_begin) T(std::move(*i));
      i->~T();
      ++dst_begin;
    }
    return dst_begin;
  }
  pointer ReallocInsert(pointer pos, const T &value) {
    size_t size = end_of_storage - start;
    pointer new_start = static_cast<pointer>(::operator new[](2 * size * sizeof(T)));
    pointer new_finish = new_start;
    new_finish = ConstructAndMove(start, pos, new_start);

    pointer new_pos = new_finish;
    new (new_finish) T(value);
    ++new_finish;

    new_finish = ConstructAndMove(pos, finish, new_finish);
    ::operator delete[](start);
    start = new_start;
    finish = new_finish;
    end_of_storage = new_start + 2 * size;
    return new_pos;
  }

 public:
  vector() {
    start = static_cast<pointer>(::operator new[](10 * sizeof(T)));
    finish = start;
    end_of_storage = start + 10;
  }
  vector(const vector &other) {
    size_t size = other.end_of_storage - other.start;
    start = static_cast<pointer>(::operator new[](size * sizeof(T)));
    finish = std::copy(other.start, other.finish, start);  // TODO
    end_of_storage = start + size;
  }
  ~vector() {
    clear();
    ::operator delete[](start);
  }
  vector &operator=(const vector &other) {
    if (this == &other) return *this;
    clear();
    ::operator delete[](start);
    size_t size = other.end_of_storage - other.start;
    start = static_cast<pointer>(::operator new[](size * sizeof(T)));
    finish = std::copy(other.start, other.finish, start);
    end_of_storage = start + size;
    return *this;
  }
  /**
   * @brief Access specified element with bounds checking,
   * throw `index_out_of_bound` if \p pos is not in [0, size)
   */
  T &at(const size_t &pos) {
    if (pos < 0 || pos >= size()) throw index_out_of_bound();
    return start[pos];
  }
  const T &at(const size_t &pos) const {
    if (pos < 0 || pos >= size()) throw index_out_of_bound();
    return start[pos];
  }
  /**
   * @brief Access specified element with bounds checking,
   * throw `index_out_of_bound` if \p pos is not in [0, size)
   */
  T &operator[](const size_t &pos) {
    if (pos < 0 || pos >= size()) throw index_out_of_bound();
    return start[pos];
  }
  const T &operator[](const size_t &pos) const {
    if (pos < 0 || pos >= size()) throw index_out_of_bound();
    return start[pos];
  }
  /**
   * @brief Access the first element, if container is empty,
   * throw `container_is_empty`.
   */
  const T &front() const {
    if (empty()) throw container_is_empty();
    return *start;
  }
  /**
   * @brief Access the last element, if container is empty,
   * throws `container_is_empty`.
   */
  const T &back() const {
    if (empty()) throw container_is_empty();
    return *(finish - 1);
  }
  iterator begin() { return iterator(start, this); }
  const_iterator cbegin() const { return const_iterator(start, this); }
  iterator end() { return iterator(finish, this); }
  const_iterator cend() const { return const_iterator(finish, this); }
  bool empty() const { return start == finish; }
  size_t size() const { return finish - start; }
  void clear() {
    while (finish != start) {
      --finish;
      finish->~T();
    }
  }
  iterator insert(iterator pos, const T &value) {
    if (finish == end_of_storage) {
      // DoubleStorage();
      return iterator(ReallocInsert(pos.current, value), this);
    } else {
      if (pos.current == finish) {
        new (finish) T(value);
      } else {
        if (start != finish) new (finish) T(*(finish - 1));  // !
        for (pointer i = finish - 1; i > pos.current; --i) {
          *i = *(i - 1);  // TODO: check
        }
        *pos = value;
      }
      ++finish;
      return pos;
    }
  }
  /**
   * @brief Inserts value at index \p ind, after insert, `this->at(ind) == value`.
   * Throws `index_out_of_bound` if `ind > size`.
   */
  iterator insert(const size_t &ind, const T &value) {
    if (ind < 0 || ind > size()) throw index_out_of_bound();
    return insert(iterator(start + ind, this), value);
  }
  iterator erase(iterator pos) {
    for (pointer i = pos.current; i != finish; ++i) {
      // *i = *(i + 1);  // TODO: check performance
      *i = std::move(*(i + 1));
    }
    --finish;
    finish->~T();  // !
    return pos;
  }
  /**
   * @brief Removes the element at index \p ind,
   * returns an iterator pointing to the following element.
   * Throws `index_out_of_bound` if `ind >= size`.
   */
  iterator erase(const size_t &ind) {
    if (ind < 0 || ind >= size()) throw index_out_of_bound();
    return erase(iterator(start + ind, this));
  }
  void push_back(const T &value) {
    if (finish == end_of_storage) {
      ReallocInsert(finish, value);
    } else {
      new (finish) T(value);
      ++finish;
    }
  }
  /**
   * @brief Removes the last element, if container is empty,
   * throws `container_is_empty`.
   */
  void pop_back() {
    if (empty()) throw container_is_empty();
    --finish;
    finish->~T();
  }
};

#endif
