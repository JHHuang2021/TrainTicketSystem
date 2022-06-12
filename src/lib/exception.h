#pragma once

#include<iostream>

namespace lin {

class Exception : public std::exception {
  const char *const what_ = "an exception";

 public:
  Exception() = default;
  Exception(const char *what) : what_(what) {}
  virtual ~Exception() = default;
  virtual const char *what() const noexcept { return what_; }
};

class InvalidIterator : public Exception {
 public:
  InvalidIterator() : Exception("invalid iterator operation") {}
};

class IndexOutOfBound : public Exception {
 public:
  IndexOutOfBound() : Exception("index out of bound") {}
};

class UnknownParameter : public Exception {
 public:
  UnknownParameter() : Exception("Unknown Parameter") {}
};
}  // namespace lin