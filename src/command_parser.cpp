#include "command_parser.h"

#include <cstring>
#include <iostream>

#include "lib/datetime.h"
#include "lib/vector.h"
#include "train.h"
#include "user.h"

namespace lin {

template <typename T>
vector<T> CommandParser::Split(char *s, const char sep) {
  vector<T> res;
  char *start = s;
  for (char *c = s; *c; ++c) {
    if (*c == sep) {
      *c = '\0';
      if (c != start && *start) res.push_back(start);
      start = c + 1;
    }
  }
  if (*start) res.push_back(start);
  return res;
}
template <typename T>
void CommandParser::Split(char *s, T result[], const char sep) {
  char *start = s;
  int cnt = 0;
  for (char *c = s; *c; ++c) {
    if (*c == sep) {
      *c = '\0';
      if (c != start && *start) result[cnt++] = T(start);
      start = c + 1;
    }
  }
  if (*start) result[cnt++] = T(start);
}
int CommandParser::ParseNumber(const char *s) {
  int res = 0;
  for (const char *c = s; *c; ++c) {
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
    timestamp = 0;
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
    } else if (command == "delete_train") {  // Normal
      res = ParseDeleteTrain();
    } else if (command == "release_train") {  // Normal
      res = ParseReleaseTrain();
    } else if (command == "refund_ticket") {  // Normal
      res = ParseRefundTicket();
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
        throw UnknownParameter();
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
        throw UnknownParameter();
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
        throw UnknownParameter();
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
        throw UnknownParameter();
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
        throw UnknownParameter();
        break;
    }
  }
  return user_manager_->ModifyProfile(cur_username, username, password, name, email, privilege);
}

std::string CommandParser::ParseAddTrain() {
  // 添加<trainID>为-i，<stationNum>为-n，<seatNum>为-m，
  // <stations>为-s，<prices>为-p，<startTime>为-x，<travelTimes>为-t，<stopoverTimes>为-o，<saleDate>为-d，<type>为-y的车次。
  // 由于-s、-p、-t、-o和-d由多个值组成，输入时两个值之间以|隔开（仍是一个不含空格的字符串）。
  Train train;
  // std::string_view train_id;
  // char type;
  // int station_num, seat_num;
  // vector<std::string_view> stations;
  // int prices[100] = {};
  vector<char *> travel_times_string, stop_times_string;
  // Time arrival_times[100], departure_times[100];
  // Date start_sale, end_sale;
  for (int i = 1; i < argv.size(); i += 2) {
    switch (argv[i][1]) {
      case 'i':
        train.id = std::string_view(argv[i + 1]);
        break;
      case 'y':
        train.type = argv[i + 1][0];
        break;
      case 'n':
        train.station_num = ParseNumber(argv[i + 1]);
        break;
      case 'm':
        train.seat_num = ParseNumber(argv[i + 1]);
        break;
      case 's':
        Split<Train::StationName>(argv[i + 1], train.stations, '|');
        break;
      case 'p': {
        auto prices_string = Split(argv[i + 1], '|');
        for (int j = 0; j < prices_string.size(); ++j) {
          train.sum_prices[j + 1] = train.sum_prices[j] + ParseNumber(prices_string[j]);
        }
        break;
      }
      case 'x':
        train.departure_times[0] = Time(argv[i + 1]);
        break;
      case 'd': {
        auto dates = Split(argv[i + 1], '|');
        train.start_sale = Date(dates[0]);
        train.end_sale = Date(dates[1]);
        break;
      }
      case 't':
        travel_times_string = Split(argv[i + 1], '|');
        break;
      case 'o':
        stop_times_string = Split(argv[i + 1], '|');
        break;
      default:
        throw UnknownParameter();
        break;
    }
  }
  for (int j = 0; j < train.station_num - 1; ++j) {
    train.arrival_times[j + 1] = train.departure_times[j] + Duration(ParseNumber(travel_times_string[j]));
    if (j + 1 < train.station_num - 1) {
      train.departure_times[j + 1] = train.arrival_times[j + 1] + Duration(ParseNumber(stop_times_string[j]));
    }
  }
  return train_manager_->AddTrain(train);
}

std::string CommandParser::ParseDeleteTrain() {
  std::string_view train_id;
  for (int i = 1; i < argv.size(); i += 2) {
    switch (argv[i][1]) {
      case 'i':
        train_id = argv[i + 1];
        break;
      default:
        throw UnknownParameter();
        break;
    }
  }
  return train_manager_->DeleteTrain(train_id);
}

std::string CommandParser::ParseReleaseTrain() {
  std::string_view train_id;
  for (int i = 1; i < argv.size(); i += 2) {
    switch (argv[i][1]) {
      case 'i':
        train_id = argv[i + 1];
        break;
      default:
        throw UnknownParameter();
        break;
    }
  }
  return train_manager_->ReleaseTrain(train_id);
}

std::string CommandParser::ParseQueryTrain() {
  std::string_view train_id;
  Date date;
  for (int i = 1; i < argv.size(); i += 2) {
    switch (argv[i][1]) {
      case 'i':
        train_id = argv[i + 1];
        break;
      case 'd':
        date = Date(argv[i + 1]);
        break;
      default:
        throw UnknownParameter();
        break;
    }
  }
  return train_manager_->QueryTrain(train_id, date);
}

std::string CommandParser::ParseQueryTicket() {
  std::string_view start_station, terminal_station;
  Date date;
  TrainManager::SortOrder sort_order = TrainManager::SortOrder::TIME;
  for (int i = 1; i < argv.size(); i += 2) {
    switch (argv[i][1]) {
      case 'd':
        date = Date(argv[i + 1]);
        break;
      case 's':
        start_station = argv[i + 1];
        break;
      case 't':
        terminal_station = argv[i + 1];
        break;
      case 'p':
        if (strcmp(argv[i + 1], "time") == 0) {
          sort_order = TrainManager::SortOrder::TIME;
        } else {
          sort_order = TrainManager::SortOrder::COST;
        }
        break;
      default:
        throw UnknownParameter();
        break;
    }
  }
  return train_manager_->QueryTicket(date, start_station, terminal_station, sort_order);
}

std::string CommandParser::ParseQueryTransfer() {
  std::string_view start_station, terminal_station;
  Date date;
  TrainManager::SortOrder sort_order = TrainManager::SortOrder::TIME;
  for (int i = 1; i < argv.size(); i += 2) {
    switch (argv[i][1]) {
      case 'd':
        date = Date(argv[i + 1]);
        break;
      case 's':
        start_station = argv[i + 1];
        break;
      case 't':
        terminal_station = argv[i + 1];
        break;
      case 'p':
        if (strcmp(argv[i + 1], "time") == 0) {
          sort_order = TrainManager::SortOrder::TIME;
        } else {
          sort_order = TrainManager::SortOrder::COST;
        }
        break;
      default:
        throw UnknownParameter();
        break;
    }
  }
  return train_manager_->QueryTransfer(date, start_station, terminal_station, sort_order);
}

std::string CommandParser::ParseBuyTicket() {
  std::string_view username, train_id, from_station, to_station;
  Date date;
  int num;
  bool pending = false;
  for (int i = 1; i < argv.size(); i += 2) {
    switch (argv[i][1]) {
      case 'u':
        username = argv[i + 1];
        break;
      case 'i':
        train_id = argv[i + 1];
        break;
      case 'd':
        date = Date(argv[i + 1]);
        break;
      case 'f':
        from_station = argv[i + 1];
        break;
      case 't':
        to_station = argv[i + 1];
        break;
      case 'n':
        num = ParseNumber(argv[i + 1]);
        break;
      case 'q':
        pending = strcmp(argv[i + 1], "true") == 0;
        break;
      default:
        throw UnknownParameter();
        break;
    }
  }
  if (!user_manager_->IsLoggedIn(username)) return "-1";
  return train_manager_->BuyTicket(timestamp, username, train_id, date, num, from_station, to_station, pending);
}

std::string CommandParser::ParseQueryOrder() {
  std::string_view username;
  for (int i = 1; i < argv.size(); i += 2) {
    switch (argv[i][1]) {
      case 'u':
        username = argv[i + 1];
        break;
      default:
        throw UnknownParameter();
        break;
    }
  }
  if (!user_manager_->IsLoggedIn(username)) return "-1";
  return train_manager_->QueryOrder(username);
}

std::string CommandParser::ParseRefundTicket() {
  std::string_view username;
  int num = 1;
  for (int i = 1; i < argv.size(); i += 2) {
    switch (argv[i][1]) {
      case 'u':
        username = argv[i + 1];
        break;
      case 'n':
        num = ParseNumber(argv[i + 1]);
        break;
      default:
        throw UnknownParameter();
        break;
    }
  }
  if (!user_manager_->IsLoggedIn(username)) return "-1";
  return train_manager_->RefundTicket(username, num);
}

std::string CommandParser::ParseRollback() { ; }

std::string CommandParser::ParseClean() { ; }

}  // namespace lin