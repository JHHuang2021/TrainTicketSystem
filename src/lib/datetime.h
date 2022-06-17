#pragma once

#include <ostream>
#include <string>

namespace lin {
class Time;
class Date;
class DateTime;
/**
 * @brief 表示一段时间，精确到分钟。
 */
class Duration {
 private:
  friend Time;
  friend Date;
  friend DateTime;
  int minutes_;

 public:
  Duration() : minutes_(0) {}
  explicit Duration(int minutes) : minutes_(minutes) {}
  int minutes() const;
  // friend auto operator<=>(const Duration &, const Duration &) = default;
  friend bool operator==(const Duration &, const Duration &);
  friend bool operator<(const Duration &, const Duration &);
  friend Duration operator+(const Duration &, const Duration &);
  friend std::ostream &operator<<(std::ostream &os, const Duration &duration) {
    return os << duration.minutes_;
  }
};
/**
 * @brief 表示日期之间相差了几天。
 */
class DateDelta {
 private:
  friend Time;
  friend Date;
  friend DateTime;
  int minutes_;

 public:
  constexpr explicit DateDelta(int minutes) : minutes_(minutes) {}
  int minutes() const;
  // friend auto operator<=>(const DateDelta &, const DateDelta &) = default;
};
constexpr const DateDelta kOneDay(24 * 60);

/**
 * @brief 表示一天中的时间（几时几分）。
 * 注意，为了实现方便，Time 表示的时间可能超过一天。
 */
class Time {
 private:
  int minutes_;
  friend Date;
  friend DateTime;

 public:
  Time() : minutes_(0) {}
  constexpr explicit Time(int minutes) : minutes_(minutes) {}
  int minutes() const;
  /**
   * @brief 从格式为「hh:mm」的字符串解析时间。
   */
  explicit Time(std::string_view str);
  std::pair<int, int> GetHourMinute() const;
  /**
   * @brief 转化为「hh:mm」格式的字符串。
   */
  std::string ToString() const;
  Time operator+(Duration o) const;
  Time &operator+=(Duration o);
  Duration operator-(const Time &o) const;
  DateDelta GetDays() const;
  std::pair<DateDelta, Time> GetDayTime() const;
  // friend auto operator<=>(const Time &, const Time &) = default;
  friend bool operator<(const Time &, const Time &);
};

/**
 * @brief 表示一个日期（几月几日）。
 */
class Date {
 private:
  int minutes_;
  friend DateTime;

 public:
  Date() : minutes_(0) {}
  explicit Date(int minutes) : minutes_(minutes) {}
  /**
   * @brief 从格式为「MM-dd」的字符串解析日期。
   */
  explicit Date(std::string_view str);
  std::pair<int, int> GetMonthDay() const;
  /**
   * @brief 转化为「MM-dd」格式的字符串。
   */
  std::string ToString() const;
  DateTime operator+(Time o) const;
  DateTime &operator+=(Time o);
  Date &operator+=(DateDelta o);
  Date operator-(DateDelta o) const;
  DateTime operator-(Duration o) const;
  // friend auto operator<=>(const Date &, const Date &) = default;
  friend bool operator<(const Date &, const Date &);
  friend bool operator==(const Date &, const Date &);
};
/**
 * @brief 简陋的时间类型，精确到分钟，只能正确处理 2021 年 6 月至 8 月内的时间。
 */
class DateTime {
 private:
  int minutes_;

 public:
  DateTime() : minutes_(0) {}
  DateTime(Date date, Time time) : minutes_(date.minutes_ + time.minutes_) {}
  explicit DateTime(int minutes) : minutes_(minutes) {}
  /**
   * @brief 从格式为「MM-dd hh:mm」的字符串解析时间。
   */
  explicit DateTime(std::string_view str);
  Date GetDate() const;
  Time GetTime() const;
  std::pair<Date, Time> GetDateAndTime() const;
  std::pair<int, int> GetMonthDay() const;
  std::pair<int, int> GetHourMinute() const;
  /**
   * @brief 转化为「MM-dd hh:mm」格式的字符串。
   */
  std::string ToString() const;
  DateTime operator+(Duration o) const;
  DateTime operator+(Time o) const;
  DateTime &operator+=(Duration o);
  Duration operator-(const DateTime &o) const;
  friend bool operator<(const DateTime &, const DateTime &);
  friend bool operator<=(const DateTime &, const DateTime &);
  friend bool operator>(const DateTime &, const DateTime &);
  friend bool operator>=(const DateTime &, const DateTime &);
  friend bool operator==(const DateTime &, const DateTime &o);
};
}  // namespace lin