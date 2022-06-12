#include "user.h"

#include <iostream>
namespace lin {
bool User::operator<(const User &other) const { return username < other.username; }
bool User::operator<=(const User &other) const { return username <= other.username; }
bool User::operator>(const User &other) const { return username > other.username; }
bool User::operator>=(const User &other) const { return username >= other.username; }

std::string UserManager::AddUser(std::string_view cur_username, std::string_view username, std::string_view password,
                                 std::string_view name, std::string_view email, int privilege) {
  auto username_hash = hasher(username);
  if (user_data_.find(username_hash) != user_data_.end()) return "-1";  // 如果 username 已经存在则注册失败
  auto it = loggedin_user_.find(hasher(cur_username));
  if (it == loggedin_user_.end()) {
    if (!user_data_.empty()) return "-1";  // cur_username 未登录
    privilege = 10;  // 创建第一个用户时，新用户权限为 10，无视权限规则的约束。
  } else {
    if (it->second <= privilege) return "-1";  // 新用户的权限需要低于当前用户的权限
  }
  User new_user{username, password, name, email, privilege};
  user_data_.emplace(username_hash, new_user);
  return "0";
}

std::string UserManager::Login(std::string_view username, std::string_view password) {
  auto username_hash = hasher(username);
  if (loggedin_user_.find(username_hash) != loggedin_user_.end()) return "-1";  // 用户已经登录
  auto it = user_data_.find(username_hash);
  if (it == user_data_.end()) return "-1";  // 若用户不存在
  if (it->second.password != password) return "-1";  // 密码错误
  loggedin_user_.emplace(username_hash, it->second.privilege);
  return "0";
}

std::string UserManager::Logout(std::string_view username) {
  auto username_hash = hasher(username);
  auto it = loggedin_user_.find(username_hash);
  if (it == loggedin_user_.end()) return "-1";  // 用户未登录
  loggedin_user_.erase(it);
  return "0";
}

std::string UserManager::PrintUser(const User &user) {
  // 返回用户信息字符串，依次列出被查询用户的 username，name，mailAddr 和 privilege，用一个空格隔开。
  std::string res;
  res.reserve(75);  // username<20> + name<20> + email<30> + privilege<2> + ' '<1>*3
  res += user.username.c_str();
  res.push_back(' ');
  res += user.name.c_str();
  res.push_back(' ');
  res += user.email.c_str();
  res.push_back(' ');
  res += std::to_string(user.privilege);
  return res;
}

std::string UserManager::QueryProfile(std::string_view cur_username, std::string_view username) {
  auto it_cur = loggedin_user_.find(hasher(cur_username));
  if (it_cur == loggedin_user_.end()) return "-1";  // 用户未登录
  auto it = user_data_.find(hasher(username));
  if (it == user_data_.end()) return "-1";  // 查询用户不存在
  if (it_cur->second <= it->second.privilege) {
    if (cur_username != username) return "-1";  // 权限不足
  }
  return PrintUser(it->second);
}

std::string UserManager::ModifyProfile(std::string_view cur_username, std::string_view username, OptionalArg password,
                                       OptionalArg name, OptionalArg email, OptionalInt privilege) {
  auto it_cur = loggedin_user_.find(hasher(cur_username));
  if (it_cur == loggedin_user_.end()) return "-1";  // 用户未登录
  auto username_hash = hasher(username);
  auto it = user_data_.find(username_hash);
  if (it == user_data_.end()) return "-1";  // 查询用户不存在
  auto &user = it->second;
  if (it_cur->second <= user.privilege) {
    if (cur_username != username) return "-1";  // 权限不足
  }
  if (privilege.has_value()) {
    if (it_cur->second <= privilege) return "-1";
    user.privilege = privilege;
  }
  if (password.has_value()) user.password = password;
  if (name.has_value()) user.name = name;
  if (email.has_value()) user.email = email;
  // TODO: 将修改写回文件。此处因为使用 std::map 所以可以通过迭代器直接修改，但 B+ 树不能。
  return PrintUser(user);
}

}  // namespace lin