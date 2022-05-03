#include "char.h"
#include "vector.h"

class UserManager {
 public:
  /**
   * @brief 创建新用户。
   */
  void AddUser(const std::string &username, const std::string &password, const std::string &name,
               const std::string &mail_addr, const int privilege);
  /**
   * @brief 用户登录。
   */
  void Login(const std::string &username, const std::string &password);
  /**
   * @brief 用户退出登录。
   */
  void Logout(const std::string &username);
  /**
   * @brief 查询用户信息。
   */
  void QueryProfile(const std::string &username);
  /**
   * @brief 修改用户信息。
   */
  void ModifyProfile(const std::string &username, const vector<ProfileItem> &args);
  /**
   * @brief 表示用户的一项信息，枚举类型 `Type` 指示了信息类型
   * （密码、姓名、邮箱地址或权限），作为 `ModifyProfile` 的参数。
   */
  struct ProfileItem {
    enum Type { Password, Name, MailAddr, Privilege };
    Type type;
    const std::string &content;
    int value;
  };
};