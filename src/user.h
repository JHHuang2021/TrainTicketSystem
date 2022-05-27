#pragma once

#include <map>
#include <unordered_map>

#include "lib/char.h"
#include "lib/optional_arg.h"

namespace lin {

struct User {
  using IdType = Char<20>;
  using PasswordType = Char<30>;
  using NameType = Char<20>;  // 为每个汉字预留 4 字节空间以兼容拓展区汉字
  using EmailType = Char<30>;

  IdType username;
  PasswordType password;
  NameType name;
  EmailType email;
  int privilege;
  // User() = default;
  User() {}
  User(IdType username, PasswordType password, NameType name, EmailType email, int privilege)
      : username(username), password(password), name(name), email(email), privilege(privilege) {}
  bool operator<(const User &other) const;
  bool operator<=(const User &other) const;
  bool operator>(const User &other) const;
  bool operator>=(const User &other) const;
};

class UserManager {
 public:
  /**
   * @brief 创建新用户。
   */
  std::string AddUser(std::string_view cur_username, std::string_view username, std::string_view password,
                      std::string_view name, std::string_view email, const int privilege);
  /**
   * @brief 用户登录。
   */
  std::string Login(std::string_view username, std::string_view password);
  /**
   * @brief 用户退出登录。
   */
  std::string Logout(std::string_view username);
  /**
   * @brief 查询用户信息。
   */
  std::string QueryProfile(std::string_view cur_username, std::string_view username);
  /**
   * 已弃用
   * TODO: Delete it in future
   * @brief 表示用户的一项信息，枚举类型 `Type` 指示了信息类型
   * （密码、姓名、邮箱地址或权限），作为 `ModifyProfile` 的参数。
   */
  /* struct ProfileItem {
    enum Type { kPassword, kName, kEmail, kPrivilege };
    Type type;
    std::string_view content;
    int value;
  }; */
  /**
   * @brief 修改用户信息。
   */
  std::string ModifyProfile(std::string_view cur_username, std::string_view username, OptionalArg password,
                            OptionalArg name, OptionalArg email, OptionalInt privilege);

 private:
  std::hash<std::string_view> hasher;
  /// hash of username -> User info
  std::map<size_t, User> user_data_;
  /// Logged-in users, hash of username -> privilege
  std::unordered_map<size_t, int> loggedin_user_;
  static std::string PrintUser(const User &user);
};
}  // namespace lin