#pragma once

#include <cstring>
#include <iostream>

/**
 * @brief `Char<size>` 是对 `char[size + 1]` 的包装，
 * 用来存一个定长的字符串，供文件交互使用。
 * 重载了比较运算符和流提取、流插入运算符。
 */
template <const size_t kSize>
class Char {
  char content_[kSize + 1];

 public:
  /**
   * @brief 构造函数，默认为空字符串。
   */
  Char() { content_[0] = '\0'; }
  /**
   * @brief 从 `std::string` 构造。
   */
  Char(const std::string &s) { strcpy(content_, s.c_str()); }
  /**
   * @brief 从 `std::string_view` 构造。
   */
  Char(std::string_view s) { strcpy(content_, s.data()); }
  /**
   * @brief 从 `char *` 构造。
   */
  Char(const char *cstr) { strcpy(content_, cstr); }
  /**
   * @brief 转化为 `std::string`。
   */
  operator std::string() const { return str(); }
  /**
   * @brief 转化为 `std::string`。
   */
  std::string str() const { return std::string(content_); }
  char *c_str() { return content_; }
  char &operator[](int index) { return content_[index]; }
  char operator[](int index) const { return content_[index]; }
  /**
   * @brief 赋值运算符，使用 \p that 的内容覆盖自身。
   */
  Char &operator=(const Char &that) {
    if (this == &that) return *this;
    strcpy(content_, that.content_);
    return *this;
  }
  /**
   * @brief 赋值运算符，使用 \p s 的内容覆盖自身。
   */
  Char &operator=(std::string_view s) {
    if (s.length() > kSize) throw -1;
    strcpy(content_, s.data());
    return *this;
  }
  /**
   * @brief 返回字符串是否为空。
   */
  bool empty() const { return content_[0] == '\0'; }
  friend bool operator<(const Char<kSize> &a, const Char<kSize> &b) { return strcmp(a.content_, b.content_) < 0; }
  friend bool operator==(const Char<kSize> &a, const Char<kSize> &b) { return strcmp(a.content_, b.content_) == 0; }
  friend bool operator>(const Char<kSize> &a, const Char<kSize> &b) { return strcmp(a.content_, b.content_) > 0; }
  friend bool operator<=(const Char<kSize> &a, const Char<kSize> &b) { return !(operator>(a, b)); }
  friend bool operator>=(const Char<kSize> &a, const Char<kSize> &b) { return !(operator<(a, b)); }
  friend bool operator!=(const Char<kSize> &a, const Char<kSize> &b) { return !(operator==(a, b)); }
  friend std::istream &operator>>(std::istream &is, Char<kSize> &s) { return is >> s.content_; }
  friend std::ostream &operator<<(std::ostream &os, const Char<kSize> &s) { return os << s.content_; }
};