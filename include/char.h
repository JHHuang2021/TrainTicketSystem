#include <string>

template <const int size>
/**
 * @brief `Char<size>` 是对 `char[size + 1]` 的包装，
 * 用来存一个定长的字符串，供文件交互使用。
 * 重载了比较运算符和流提取、流插入运算符。
 *
 */
class Char {
  char content[size + 1];

 public:
  /**
   * @brief 构造函数，默认为空字符串。
   */
  Char();
  /**
   * @brief 从 `std::string` 构造。
   */
  Char(const std::string &s);
  /**
   * @brief 从 `char *` 构造。
   */
  Char(const char *cstr);
  /**
   * @brief 转化为 `std::string`。
   */
  operator std::string() const;
  /**
   * @brief 转化为 `std::string`。
   */
  std::string str() const;

  /**
   * @brief 赋值运算符，使用 \p that 的内容覆盖自身。
   */
  Char &operator=(const Char &that);
  /**
   * @brief 返回字符串是否为空。
   */
  bool empty() const;

  friend bool operator<(const Char<size> &a, const Char<size> &b);
  friend bool operator==(const Char<size> &a, const Char<size> &b);
  friend bool operator>(const Char<size> &a, const Char<size> &b);
  friend bool operator<=(const Char<size> &a, const Char<size> &b);
  friend bool operator>=(const Char<size> &a, const Char<size> &b);
  friend bool operator!=(const Char<size> &a, const Char<size> &b);
  friend std::istream &operator>>(std::istream &is, Char<size> &s);
  friend std::ostream &operator<<(std::ostream &os, const Char<size> &s);
};