#pragma once

#include <string>

namespace lin {
class DateTime;
/**
 * @brief 表示一段时间，精确到分钟。
 */
class Duration {
 private:
  friend DateTime;
  int minutes_;

 public:
  Duration() : minutes_(0) {}
  explicit Duration(int minutes) : minutes_(minutes) {}
  int minutes() const;
};
/**
 * @brief 简陋的时间类型，精确到分钟，只能正确处理 2021 年 6 月至 8 月内的时间。
 */
class DateTime {
 private:
  int minutes_;

 public:
  DateTime() : minutes_(0) {}
  explicit DateTime(int minutes) : minutes_(minutes) {}
  /**
   * @brief 从格式为「MM-dd hh:mm」的字符串解析时间。
   */
  explicit DateTime(const std::string &str);
  std::pair<int, int> GetDate() const;
  std::pair<int, int> GetTime() const;
  std::string ToString() const;
  DateTime operator+(Duration o) const;
  DateTime &operator+=(Duration o);
  Duration operator-(const DateTime &o) const;
  bool operator<(const DateTime &o) const;
  bool operator<=(const DateTime &o) const;
  bool operator>(const DateTime &o) const;
  bool operator>=(const DateTime &o) const;
  bool operator==(const DateTime &o) const;
};
}  // namespace lin