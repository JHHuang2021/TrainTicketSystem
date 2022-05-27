#include "command_parser.h"

#include <cstring>
#include <iostream>

#include "lib/vector.h"
#include "user.h"

namespace lin {
/**
 * @brief Splits a C-style string by blank character.
 * Note that the original string WILL BE MODIFIED.
 */
vector<std::string_view> CommandParser::Split(char *s) {
  vector<std::string_view> res;
  char *start = s;
  for (char *c = s; *c; ++c) {
    if (isblank(*c)) {
      *c = '\0';
      if (c != start && *start) res.push_back(start);
      start = c + 1;
    }
  }
  if (*start) res.push_back(start);
  return res;
}
int CommandParser::ParseNumber(std::string_view s) {
  int res = 0;
  for (const char *c = s.data(); *c; ++c) {
    res = res * 10 + *c - '0';
  }
  return res;
}

namespace {
constexpr const size_t kInputBufferSize = 10240;
}

void CommandParser::Run() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(NULL);
  char input[kInputBufferSize];
  while (std::cin.getline(input, kInputBufferSize)) {
    int timestamp = 0;
    char *c = input + 1;
    while ('0' <= *c && *c <= '9') timestamp = timestamp * 10 + *(c++) - '0';
    ++c;
    while (isblank(*c)) ++c;
    this->argv = Split(c);
    std::string_view command = argv[0];
    std::string res;
    if (command[0] == 'q') {  // Super Frequent: query_profile, query_ticket
      if (command == "query_ticket") {
        res = ParseQueryTicket();
      } else if (command == "query_profile") {
        res = ParseQueryProfile();
      } else if (command == "query_order") {  // Frequent
        res = ParseQueryOrder();
      } else if (command == "query_train") {  // Normal
        res = ParseQueryTrain();
      } else if (command == "query_transfer") {  // Normal
        res = ParseQueryTransfer();
      }
    } else if (command == "buy_ticket") {  // Super Frequent: buy_ticket
      res = ParseBuyTicket();
    } else if (command[0] == 'l') {  // Frequent: login, logout
      if (command == "login") {
        res = ParseLogin();
      } else if (command == "logout") {
        res = ParseLogout();
      }
    } else if (command == "modify_profile") {  // Frequent: modify_profile
      res = ParseModifyProfile();
    } else if (command[0] == 'a') {  // Normal: add_user, add_train
      if (command == "add_user") {
        res = ParseAddUser();
      } else if (command == "add_train") {
        res = ParseAddTrain();
      }
    } else if (command == "delete_train") {
      res = ParseDeleteTrain();
    } else if (command == "release_train") {
      res = ParseReleaseTrain();
    } else if (command == "rollback") {  // Rare
      res = ParseRollback();
    } else if (command == "clean") {
      res = ParseClean();
    } else if (command == "exit") {
      res = "bye";
      std::cout << '[' << timestamp << "] " << res << '\n';
      break;
    }
    std::cout << '[' << timestamp << "] " << res << '\n';
  }
}

std::string CommandParser::ParseAddUser() {
  std::string_view cur_username, username, password, name, email;
  int privilege = 10;
  for (int i = 1; i < argv.size(); i += 2) {
    switch (argv[i][1]) {
      case 'c':
        cur_username = argv[i + 1];
        break;
      case 'u':
        username = argv[i + 1];
        break;
      case 'p':
        password = argv[i + 1];
        break;
      case 'n':
        name = argv[i + 1];
        break;
      case 'm':
        email = argv[i + 1];
        break;
      case 'g':
        privilege = ParseNumber(argv[i + 1]);
        break;
      default:
        break;
    }
  }
  return user_manager_->AddUser(cur_username, username, password, name, email, privilege);
}
std::string CommandParser::ParseLogin() {
  std::string_view username, password;
  for (int i = 1; i < argv.size(); i += 2) {
    switch (argv[i][1]) {
      case 'u':
        username = argv[i + 1];
        break;
      case 'p':
        password = argv[i + 1];
        break;
      default:
        break;
    }
  }
  return user_manager_->Login(username, password);
}
std::string CommandParser::ParseLogout() {
  std::string_view username;
  for (int i = 1; i < argv.size(); i += 2) {
    switch (argv[i][1]) {
      case 'u':
        username = argv[i + 1];
        break;
      default:
        break;
    }
  }
  return user_manager_->Logout(username);
}
std::string CommandParser::ParseQueryProfile() {
  std::string_view cur_username, username;
  for (int i = 1; i < argv.size(); i += 2) {
    switch (argv[i][1]) {
      case 'c':
        cur_username = argv[i + 1];
        break;
      case 'u':
        username = argv[i + 1];
        break;
      default:
        break;
    }
  }
  return user_manager_->QueryProfile(cur_username, username);
}
std::string CommandParser::ParseModifyProfile() {
  std::string_view cur_username, username;
  OptionalArg password, name, email;
  OptionalInt privilege;
  for (int i = 1; i < argv.size(); i += 2) {
    switch (argv[i][1]) {
      case 'c':
        cur_username = argv[i + 1];
        break;
      case 'u':
        username = argv[i + 1];
        break;
      case 'p':
        password = argv[i + 1];
        break;
      case 'n':
        name = argv[i + 1];
        break;
      case 'm':
        email = argv[i + 1];
        break;
      case 'g':
        privilege = ParseNumber(argv[i + 1]);
        break;
      default:
        break;
    }
  }
  return user_manager_->ModifyProfile(cur_username, username, password, name, email, privilege);
}

std::string CommandParser::ParseAddTrain() { ; }

std::string CommandParser::ParseDeleteTrain() { ; }

std::string CommandParser::ParseReleaseTrain() { ; }

std::string CommandParser::ParseQueryTrain() { ; }

std::string CommandParser::ParseQueryTicket() { ; }

std::string CommandParser::ParseQueryTransfer() { ; }

std::string CommandParser::ParseBuyTicket() { ; }

std::string CommandParser::ParseQueryOrder() { ; }

std::string CommandParser::ParseRefundTicket() { ; }

std::string CommandParser::ParseRollback() { ; }

std::string CommandParser::ParseClean() { ; }

}  // namespace lin