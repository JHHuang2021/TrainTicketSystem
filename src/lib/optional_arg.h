#include <iostream>

/**
 * @brief An optional argument of type `std::string_view`
 */
class OptionalArg {
 public:
  OptionalArg() : s_() {}
  template <typename... Args>
  OptionalArg(Args&&... args) : s_(std::forward<Args>(args)...) {}
  operator std::string_view() { return s_; }
  bool has_value() const { return !s_.empty(); }
  OptionalArg& operator=(const std::string_view& s) {
    s_ = s;
    return *this;
  }

 private:
  std::string_view s_;
};

/**
 * @brief An optional int
 */
class OptionalInt {
 public:
  OptionalInt() : engaged_(false) {}
  OptionalInt(int value) : val_(value), engaged_(true) {}
  operator int() { return val_; }
  bool has_value() const { return engaged_; }
  OptionalInt& operator=(int value) {
    val_ = value, engaged_ = true;
    return *this;
  }

 private:
  int val_;
  bool engaged_;
};