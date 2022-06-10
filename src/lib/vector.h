#pragma once

#include <iostream>

#include "exception.h"

namespace lin {
/**
 * @brief A data container like `std::vector`.
 * All undocumented functions act like `std::vector`.
 */
template <typename T>
class vector {
 public:
  class const_iterator;
  class iterator {
   public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = std::output_iterator_tag;

   private:
    pointer ptr_;
    vector *container_;
    explicit iterator(pointer ptr_, vector *container_) : ptr_(ptr_), container_(container_) {}
    friend vector;

   public:
    iterator operator+(const int &n) const { return iterator(ptr_ + n, container_); }
    iterator operator-(const int &n) const { return iterator(ptr_ - n, container_); }
    /**
     * @brief Returns the distance between two iterators,
     * if these two iterators point to different vectors, throws `InvalidIterator`.
     */
    int operator-(const iterator &rhs) const {
      if (container_ != rhs.container_) throw InvalidIterator();
      return ptr_ - rhs.ptr_;
    }
    iterator &operator+=(const int &n) {
      ptr_ += n;
      return *this;
    }
    iterator &operator-=(const int &n) {
      ptr_ -= n;
      return *this;
    }
    iterator operator++(int) {
      iterator tmp = *this;
      ptr_++;
      return tmp;
    }
    iterator &operator++() {
      ptr_++;
      return *this;
    }
    iterator operator--(int) {
      iterator tmp = *this;
      ptr_--;
      return tmp;
    }
    iterator &operator--() {
      ptr_--;
      return *this;
    }
    T &operator*() const { return *ptr_; }
    T *operator->() const { return ptr_; }
    bool operator==(const iterator &rhs) const { return ptr_ == rhs.ptr_; }
    bool operator==(const const_iterator &rhs) const { return ptr_ == rhs.ptr_; }

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
    pointer ptr_;
    const vector *container_;
    explicit const_iterator(pointer ptr_, const vector *container_) : ptr_(ptr_), container_(container_) {}
    friend vector;

   public:
    const_iterator operator+(const int &n) const { return const_iterator(ptr_ + n, container_); }
    const_iterator operator-(const int &n) const { return const_iterator(ptr_ - n, container_); }
    /**
     * @brief Returns the distance between two iterators,
     * if these two iterators point to different vectors, throws `InvalidIterator`.
     */
    int operator-(const iterator &rhs) const {
      if (container_ != rhs.container_) throw InvalidIterator();
      return ptr_ - rhs.ptr_;
    }
    const_iterator &operator+=(const int &n) {
      ptr_ += n;
      return *this;
    }
    const_iterator &operator-=(const int &n) {
      ptr_ -= n;
      return *this;
    }
    const_iterator operator++(int) {
      iterator tmp = *this;
      ptr_++;
      return tmp;
    }
    const_iterator &operator++() {
      ptr_++;
      return *this;
    }
    const_iterator operator--(int) {
      iterator tmp = *this;
      ptr_--;
      return tmp;
    }
    const_iterator &operator--() {
      ptr_--;
      return *this;
    }
    const T &operator*() const { return *ptr_; }
    const T *operator->() const { return ptr_; }
    bool operator==(const iterator &rhs) const { return ptr_ == rhs.ptr_; }
    bool operator==(const const_iterator &rhs) const { return ptr_ == rhs.ptr_; }
    bool operator!=(const iterator &rhs) const { return !operator==(rhs); }
    bool operator!=(const const_iterator &rhs) const { return !operator==(rhs); }
  };

 public:
  using pointer = T *;

 private:
  pointer start_;
  pointer finish_;
  pointer end_of_storage_;
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
    size_t size = end_of_storage_ - start_;
    pointer new_start = static_cast<pointer>(::operator new[](2 * size * sizeof(T)));
    pointer new_finish = new_start;
    new_finish = ConstructAndMove(start_, pos, new_start);

    pointer new_pos = new_finish;
    new (new_finish) T(value);
    ++new_finish;

    new_finish = ConstructAndMove(pos, finish_, new_finish);
    ::operator delete[](start_);
    start_ = new_start;
    finish_ = new_finish;
    end_of_storage_ = new_start + 2 * size;
    return new_pos;
  }

 public:
  vector() {
    start_ = static_cast<pointer>(::operator new[](10 * sizeof(T)));
    finish_ = start_;
    end_of_storage_ = start_ + 10;
  }
  vector(const vector &other) {
    size_t size = other.end_of_storage_ - other.start_;
    start_ = static_cast<pointer>(::operator new[](size * sizeof(T)));
    finish_ = std::copy(other.start_, other.finish_, start_);  // TODO
    end_of_storage_ = start_ + size;
  }
  vector(vector &&other) noexcept {
    start_ = other.start_;
    finish_ = other.finish_;
    end_of_storage_ = other.end_of_storage_;
    other.start_ = other.finish_ = other.end_of_storage_ = nullptr;
  }
  ~vector() {
    clear();
    if (start_) ::operator delete[](start_);
  }
  vector &operator=(const vector &other) {
    if (this == &other) return *this;
    clear();
    ::operator delete[](start_);
    size_t size = other.end_of_storage_ - other.start_;
    start_ = static_cast<pointer>(::operator new[](size * sizeof(T)));
    finish_ = std::copy(other.start_, other.finish_, start_);
    end_of_storage_ = start_ + size;
    return *this;
  }
  vector &operator=(vector &&other) noexcept {
    if (this == &other) return *this;
    clear();
    ::operator delete[](start_);
    start_ = other.start_;
    finish_ = other.finish_;
    end_of_storage_ = other.end_of_storage_;
    other.start_ = other.finish_ = other.end_of_storage_ = nullptr;
    return *this;
  }
  /**
   * @brief Access specified element with bounds checking,
   * throw `IndexOutOfBound` if \p pos is not in [0, size)
   */
  T &at(const size_t &pos) {
    if (pos < 0 || pos >= size()) throw IndexOutOfBound();
    return start_[pos];
  }
  const T &at(const size_t &pos) const {
    if (pos < 0 || pos >= size()) throw IndexOutOfBound();
    return start_[pos];
  }
  /**
   * @brief Access specified element with bounds checking,
   * throw `IndexOutOfBound` if \p pos is not in [0, size)
   */
  T &operator[](const size_t &pos) {
    if (pos < 0 || pos >= size()) throw IndexOutOfBound();
    return start_[pos];
  }
  const T &operator[](const size_t &pos) const {
    if (pos < 0 || pos >= size()) throw IndexOutOfBound();
    return start_[pos];
  }
  /**
   * @brief Access the first element, throw exception if container is empty.
   */
  const T &front() const {
    if (empty()) throw Exception("container is empty");
    return *start_;
  }
  /**
   * @brief Access the last element, throw exception if
   * container is empty.
   */
  const T &back() const {
    if (empty()) throw Exception("container is empty");
    return *(finish_ - 1);
  }
  iterator begin() { return iterator(start_, this); }
  const_iterator cbegin() const { return const_iterator(start_, this); }
  iterator end() { return iterator(finish_, this); }
  const_iterator cend() const { return const_iterator(finish_, this); }
  bool empty() const { return start_ == finish_; }
  size_t size() const { return finish_ - start_; }
  void clear() {
    while (finish_ != start_) {
      --finish_;
      finish_->~T();
    }
  }
  iterator insert(iterator pos, const T &value) {
    if (finish_ == end_of_storage_) {
      // DoubleStorage();
      return iterator(ReallocInsert(pos.ptr_, value), this);
    } else {
      if (pos.ptr_ == finish_) {
        new (finish_) T(value);
      } else {
        if (start_ != finish_) new (finish_) T(*(finish_ - 1));  // !
        for (pointer i = finish_ - 1; i > pos.ptr_; --i) {
          *i = *(i - 1);  // TODO: check
        }
        *pos = value;
      }
      ++finish_;
      return pos;
    }
  }
  /**
   * @brief Inserts value at index \p ind, after insert, `this->at(ind) == value`.
   * Throws `IndexOutOfBound` if `ind > size`.
   */
  iterator insert(const size_t &ind, const T &value) {
    if (ind < 0 || ind > size()) throw IndexOutOfBound();
    return insert(iterator(start_ + ind, this), value);
  }
  iterator erase(iterator pos) {
    for (pointer i = pos.ptr_; i != finish_; ++i) {
      // *i = *(i + 1);  // TODO: check performance
      *i = std::move(*(i + 1));
    }
    --finish_;
    finish_->~T();  // !
    return pos;
  }
  /**
   * @brief Removes the element at index \p ind,
   * returns an iterator pointing to the following element.
   * Throws `IndexOutOfBound` if `ind >= size`.
   */
  iterator erase(const size_t &ind) {
    if (ind < 0 || ind >= size()) throw IndexOutOfBound();
    return erase(iterator(start_ + ind, this));
  }
  void push_back(const T &value) {
    if (finish_ == end_of_storage_) {
      ReallocInsert(finish_, value);
    } else {
      new (finish_) T(value);
      ++finish_;
    }
  }
  /**
   * @brief Removes the last element, throws exception if
   * container is empty.
   */
  void pop_back() {
    if (empty()) throw Exception("container is empty");
    --finish_;
    finish_->~T();
  }
};
}  // namespace lin