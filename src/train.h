#pragma once

#include <iostream>
#include <map>

#include "lib/char.h"
#include "lib/datetime.h"
#include "lib/hash.h"
#include "lib/vector.h"

namespace lin {

struct Train {
  using IdType = Char<20>;
  using IdHash = size_t;
  using IdHasher = Hasher<Train::IdType>;
  using StationName = Char<40>;
  static constexpr const int kMaxStationNum = 100;
  IdType id;
  char type;
  int seat_num, station_num;
  StationName stations[kMaxStationNum];
  int sum_prices[kMaxStationNum] = {};  // 累计价格
  Date start_sale, end_sale;
  Time arrival_times[kMaxStationNum] = {};
  Time departure_times[kMaxStationNum] = {};
  bool released = false;
};

struct StationTrain {
  Train::IdHash train_id_hash;
  Time arrival_time, departure_time;
  int sum_price;  // 累计价格
  int rank;  // 储存此车站是该列车途径的第几个车站，用于在查票时判断列车运行方向
  Train::IdType train_id;
  Date start_sale, end_sale;
  int seat_num, station_num;
  StationTrain(Train::IdHash train_id_hash_, Time arrival_time_, Time departure_time_, int sum_price_, int rank_,
      const Train &train)
      : train_id_hash(train_id_hash_),
        arrival_time(arrival_time_),
        departure_time(departure_time_),
        sum_price(sum_price_),
        rank(rank_),
        train_id(train.id),
        start_sale(train.start_sale),
        end_sale(train.end_sale) {}
};

struct TrainSeats {
  int seat_num[Train::kMaxStationNum];  // seat_num[i] 表示从 stations[i] 到 stations[i + 1] 的剩余座位数
  TrainSeats(int initial_seat_num, int station_num) { std::fill(seat_num, seat_num + station_num, initial_seat_num); }
  // 左闭右开
  int RangeMin(int l, int r);
  // 左闭右开
  void RangeAdd(int l, int r, int x);
};

struct Ticket {
  Train::IdType train_id;
  DateTime start_time, end_time;
  Duration duration;
  int cost, seat;
  friend bool CompareTime(const Ticket &a, const Ticket &b);
  friend bool CompareCost(const Ticket &a, const Ticket &b);
};

struct TransferTicket {
  Duration duration;
  int cost;
  Ticket ticket1, ticket2;
  std::string transfer_station;
  TransferTicket();
  friend bool CompareTime(const TransferTicket &a, const TransferTicket &b);
  friend bool CompareCost(const TransferTicket &a, const TransferTicket &b);
};

class TrainManager {
 public:
  /**
   * @brief 添加一辆火车。
   */
  std::string AddTrain(const Train &train);
  // std::string AddTrain(std::string_view train_id, const int station_num, const int total_seat_num,
  //               vector<std::string_view> stations, vector<int> price, std::string_view start_time,
  //               vector<std::string> travel_times, vector<std::string> stopover_times, vector<std::string> sale_date,
  //               const char type);

  /// 删除指定 train_id 的车次，删除车次必须保证未发布。
  std::string DeleteTrain(std::string_view train_id);

  /**
   * @brief 发布火车。发布前的车次，不可发售车票，无法被 query_ticket 和 query_transfer 操作所查询到；
   * 发布后的车次不可被删除，可发售车票。
   */
  std::string ReleaseTrain(std::string_view train_id);

  /// 询问符合条件的火车。
  std::string QueryTrain(std::string_view train_id, Date target_date);

  /// 排序依据
  enum SortOrder { TIME, COST };

  /**
   * @brief 查询指定日期时从 \p from_station 出发，并到达 \p to_station 的车票。
   *
   * @note 这里的日期是列车从 \p from_station 出发的日期，不是从列车始发站出发的日期。
   */
  std::string QueryTicket(
      Date date, std::string_view from_station, std::string_view to_station, SortOrder sort_order = TIME);
  /**
   * @brief 在恰好换乘一次（换乘同一辆车不算恰好换乘一次）的情况下查询符合条件的车次。
   * 仅输出最优解。如果出现多个最优解（排序关键字最小)，则选择在第一辆列车上花费的时间更少的方案。
   *
   * @note 这里的日期是列车从 \p from_station 出发的日期，不是从列车始发站出发的日期。
   */
  std::string QueryTransfer(
      Date date, std::string_view from_station, std::string_view to_station, SortOrder sort_order = TIME);

 private:
  Train::IdHasher TrainIdHasher;
  using StationHash = size_t;
  Hasher<Train::StationName> StationHasher;
  std::map<Train::IdHash, Train> trains_;
  // std::map<Train::IdHash, TrainDate> train_dates_;
  std::map<std::pair<Train::IdHash, Date>, TrainSeats> train_seats_;
  std::map<std::pair<StationHash, Train::IdHash>, StationTrain> station_trains_;

  TrainSeats GetSeats(Train::IdHash train_id_hash, Date date, int initial_seat_num, int station_num);
};

}  // namespace lin