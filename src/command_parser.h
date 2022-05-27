#pragma once

#include "lib/vector.h"

namespace lin {
class UserManager;
class TrainManager;
class OrderManager;
/**
 * @brief 解析输入指令，并调用相应的函数。
 */
class CommandParser {
 public:
  CommandParser(UserManager *user_manager): user_manager_(user_manager) {};
  /**
   * @brief 循环读入指令并解析，直到遇到 exit。
   */
  void Run();

 private:
  UserManager *user_manager_;
  TrainManager *train_manager_;
  OrderManager *order_manager_;
  int timestamp;
  vector<std::string_view> argv;
  static vector<std::string_view> Split(char *s);
  static int ParseNumber(std::string_view s);
  std::string ParseAddUser();
  std::string ParseLogin();
  std::string ParseLogout();
  std::string ParseQueryProfile();
  std::string ParseModifyProfile();
  std::string ParseAddTrain();
  std::string ParseDeleteTrain();
  std::string ParseReleaseTrain();
  std::string ParseQueryTrain();
  std::string ParseQueryTicket();
  std::string ParseQueryTransfer();
  std::string ParseBuyTicket();
  std::string ParseQueryOrder();
  std::string ParseRefundTicket();
  std::string ParseRollback();
  std::string ParseClean();
};
}  // namespace lin