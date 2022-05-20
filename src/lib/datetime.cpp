#include "datetime.h"

namespace lin {

namespace {
constexpr int kDaysPerMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
constexpr int kSumDays[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
inline void SetNumber(char *s, int num) {
  s[0] = num / 10 + '0';
  s[1] = num % 10 + '0';
}
}  // namespace

DateTime::DateTime(const std::string &str) {
  // str:  MM-dd hh:mm
  // pos:  0123456789
  int month = std::stoi(str.substr(0, 2));
  int day = std::stoi(str.substr(3, 2));
  int hour = std::stoi(str.substr(6, 2));
  int min = std::stoi(str.substr(9, 2));
  minutes_ = ((kSumDays[month - 1] + day) * 24 + hour) * 60 + min;
}

std::pair<int, int> DateTime::GetDate() const {
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

std::pair<int, int> DateTime::GetTime() const {
  int min = minutes_ % (60 * 24), hour = min / 60;
  min -= hour * 60;
  return std::make_pair(hour, min);
}

std::string DateTime::ToString() const {
  auto [month, day] = GetDate();
  auto [hour, min] = GetTime();
  char buf[11];
  // str:  MM-dd hh:mm
  // pos:  0123456789
  SetNumber(buf, month), buf[2] = '-', SetNumber(buf + 3, day);
  buf[5] = ' ';
  SetNumber(buf + 6, hour), buf[8] = ':', SetNumber(buf + 9, min);
  buf[11] = '\0';
  return buf;
}

DateTime DateTime::operator+(Duration o) const { return DateTime(minutes_ + o.minutes_); }
DateTime &DateTime::operator+=(Duration o) {
  this->minutes_ += o.minutes_;
  return *this;
}
Duration DateTime::operator-(const DateTime &o) const { return Duration(minutes_ - o.minutes_); }

bool DateTime::operator<(const DateTime &o) const { return minutes_ < o.minutes_; }
bool DateTime::operator<=(const DateTime &o) const { return minutes_ <= o.minutes_; }
bool DateTime::operator>(const DateTime &o) const { return minutes_ > o.minutes_; }
bool DateTime::operator>=(const DateTime &o) const { return minutes_ >= o.minutes_; }
bool DateTime::operator==(const DateTime &o) const { return minutes_ == o.minutes_; }

int Duration::minutes() const { return minutes_; }

}  // namespace lin