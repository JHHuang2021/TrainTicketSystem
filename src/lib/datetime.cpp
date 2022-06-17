#include "datetime.h"

namespace lin {

namespace {
constexpr const int kDaysPerMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
constexpr const int kSumDays[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
inline void SetNumber(char *s, int num) {
  s[0] = num / 10 + '0';
  s[1] = num % 10 + '0';
}
inline int GetNumber(const char *s) { return (s[0] - '0') * 10 + s[1] - '0'; }
}  // namespace

bool operator==(const Duration &a, const Duration &b) { return a.minutes_ == b.minutes_; }
bool operator<(const Duration &a, const Duration &b) { return a.minutes_ < b.minutes_; }
Duration operator+(const Duration &a, const Duration &b) { return Duration(a.minutes_ + b.minutes_); }

int Time::minutes() const { return minutes_; }

Time::Time(std::string_view str) {
  // str:  hh:mm
  // pos:  01234
  int hour = GetNumber(str.data());
  int min = GetNumber(str.data() + 3);
  minutes_ = hour * 60 + min;
}

std::pair<int, int> Time::GetHourMinute() const {
  int min = minutes_ % 60, hour = min / 60;
  return std::make_pair(hour, min);
}

std::string Time::ToString() const {
  auto [hour, min] = GetHourMinute();
  // char buf[6];
  std::string buf(5, '\0');
  // str:  hh:mm
  // pos:  01234
  SetNumber(buf.data(), hour), buf[2] = ':', SetNumber(buf.data() + 3, min);
  return buf;
}
Time Time::operator+(Duration o) const { return Time(minutes_ + o.minutes_); }
Time &Time::operator+=(Duration o) {
  this->minutes_ += o.minutes_;
  return *this;
}
Duration Time::operator-(const Time &o) const { return Duration(minutes_ - o.minutes_); }

DateDelta Time::GetDays() const { return DateDelta(minutes_ / kOneDay.minutes_ * kOneDay.minutes_); }

std::pair<DateDelta, Time> Time::GetDayTime() const {
  DateDelta days(minutes_ / kOneDay.minutes_ * kOneDay.minutes_);
  return std::make_pair(days, Time(minutes_ - days.minutes_));
}
bool operator<(const Time &a, const Time &b) { return a.minutes_ < b.minutes_; }

Date::Date(std::string_view str) {
  // str:  MM-dd
  // pos:  01234
  int month = GetNumber(str.data());
  int day = GetNumber(str.data() + 3);
  minutes_ = (kSumDays[month - 1] + day) * 24 * 60;
}

std::pair<int, int> Date::GetMonthDay() const {
  int days = minutes_ / (60 * 24);
  int month;
  for (month = 1; month <= 12; ++month) {
    if (days <= kSumDays[month]) {
      days -= kSumDays[month - 1];
      break;
    }
  }
  return std::make_pair(month, days);
}

std::string Date::ToString() const {
  auto [month, day] = GetMonthDay();
  // char buf[6];
  std::string buf(5, '\0');
  // str:  MM-dd
  // pos:  01234
  SetNumber(buf.data(), month), buf[2] = '-', SetNumber(buf.data() + 3, day);
  return buf;
}
DateTime Date::operator+(Time o) const { return DateTime(minutes_ + o.minutes_); }
Date &Date::operator+=(DateDelta o) {
  minutes_ += o.minutes_;
  return *this;
}
Date Date::operator-(DateDelta o) const { return Date(minutes_ - o.minutes_); }
DateTime Date::operator-(Duration o) const { return DateTime(minutes_ - o.minutes_); }
bool operator<(const Date &a, const Date &b) { return a.minutes_ < b.minutes_; }
bool operator==(const Date &a, const Date &b) { return a.minutes_ == b.minutes_; }

DateTime::DateTime(std::string_view str) {
  // str:  MM-dd hh:mm
  // pos:  0123456789
  int month = GetNumber(str.data());
  int day = GetNumber(str.data() + 3);
  int hour = GetNumber(str.data() + 6);
  int min = GetNumber(str.data() + 9);
  minutes_ = ((kSumDays[month - 1] + day) * 24 + hour) * 60 + min;
}

Date DateTime::GetDate() const { return Date(minutes_ / kOneDay.minutes_ * kOneDay.minutes_); }

Time DateTime::GetTime() const { return Time(minutes_ % kOneDay.minutes_); }

std::pair<Date, Time> DateTime::GetDateAndTime() const { return std::make_pair(GetDate(), GetTime()); }

std::pair<int, int> DateTime::GetMonthDay() const {
  int days = minutes_ / (60 * 24);
  // int month = std::lower_bound(kSumDays + 1, kSumDays + 13, days) - kSumDays;
  // days -= kSumDays[month - 1];
  // 只有十二个月，二分查找常数大，反而比枚举慢
  int month;
  for (month = 1; month <= 12; ++month) {
    if (days <= kSumDays[month]) {
      days -= kSumDays[month - 1];
      break;
    }
  }
  return std::make_pair(month, days);
}

std::pair<int, int> DateTime::GetHourMinute() const {
  int min = minutes_ % (60 * 24), hour = min / 60;
  min -= hour * 60;
  return std::make_pair(hour, min);
}

std::string DateTime::ToString() const {
  auto [month, day] = GetMonthDay();
  auto [hour, min] = GetHourMinute();
  // char buf[12];
  std::string buf(11, '\0');
  // str:  MM-dd hh:mm
  // pos:  0123456789
  // SetNumber(buf, month), buf[2] = '-', SetNumber(buf + 3, day);
  SetNumber(buf.data(), month), buf[2] = '-', SetNumber(buf.data() + 3, day);
  buf[5] = ' ';
  // SetNumber(buf + 6, hour), buf[8] = ':', SetNumber(buf + 9, min);
  SetNumber(buf.data() + 6, hour), buf[8] = ':', SetNumber(buf.data() + 9, min);
  return buf;
}

DateTime DateTime::operator+(Duration o) const { return DateTime(minutes_ + o.minutes_); }
DateTime DateTime::operator+(Time o) const { return DateTime(minutes_ + o.minutes_); }
DateTime &DateTime::operator+=(Duration o) {
  this->minutes_ += o.minutes_;
  return *this;
}
Duration DateTime::operator-(const DateTime &o) const { return Duration(minutes_ - o.minutes_); }

bool operator<(const DateTime &a, const DateTime &b) { return a.minutes_ < b.minutes_; }
bool operator<=(const DateTime &a, const DateTime &b) { return a.minutes_ <= b.minutes_; }
bool operator>(const DateTime &a, const DateTime &b) { return a.minutes_ > b.minutes_; }
bool operator>=(const DateTime &a, const DateTime &b) { return a.minutes_ >= b.minutes_; }
bool operator==(const DateTime &a, const DateTime &b) { return a.minutes_ == b.minutes_; }

int Duration::minutes() const { return minutes_; }

int DateDelta::minutes() const { return minutes_; }

}  // namespace lin