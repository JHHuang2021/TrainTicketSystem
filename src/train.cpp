#include "train.h"

#include <unordered_map>

#include "lib/datetime.h"
#include "lib/hash.h"
#include "lib/tuple.h"
#include "lib/utils.h"

namespace lin {

namespace {
// constexpr const int INT_MAX = std::numeric_limits<int>::max();  // need #include<limits>
constexpr const int INT_MAX = 2147483647;
constexpr const auto kHashMin = 0UL;
constexpr const auto kHashMax = SIZE_MAX;
}  // namespace

int TrainSeats::RangeMin(int l, int r) {
  int res = INT_MAX;
  for (int i = l; i < r; ++i) res = std::min(res, seat_num[i]);
  return res;
}
void TrainSeats::RangeAdd(int l, int r, int x) {
  for (int i = l; i < r; ++i) seat_num[i] += x;
}

bool CompareTime(const Ticket &a, const Ticket &b) {
  if (a.duration == b.duration) return a.train_id < b.train_id;
  return a.duration < b.duration;
}
bool CompareCost(const Ticket &a, const Ticket &b) {
  if (a.cost == b.cost) return a.train_id < b.train_id;
  return a.cost < b.cost;
}

TransferTicket::TransferTicket() : duration(INT_MAX), cost(INT_MAX) {}

bool CompareTime(const TransferTicket &a, const TransferTicket &b) {
  return tie(a.duration, a.cost, a.ticket1.train_id, a.ticket2.train_id) <
         tie(b.duration, b.cost, b.ticket1.train_id, b.ticket2.train_id);
}
bool CompareCost(const TransferTicket &a, const TransferTicket &b) {
  return tie(a.cost, a.duration, a.ticket1.train_id, a.ticket2.train_id) <
         tie(b.cost, b.duration, b.ticket1.train_id, b.ticket2.train_id);
}

std::string TrainManager::AddTrain(const Train &train) {
  auto train_id_hash = TrainIdHasher(train.id);
  trains_.emplace(train_id_hash, train);
  return "0";
}

std::string TrainManager::DeleteTrain(std::string_view train_id) {
  auto train_id_hash = TrainIdHasher(train_id);
  auto it = trains_.find(train_id_hash);
  if (it == trains_.end()) return "-1";  // 车次不存在
  Train &train = it->second;
  if (train.released) return "-1";  // 不能删除已发布的车次
  trains_.erase(it);
  return "0";
}

std::string TrainManager::ReleaseTrain(std::string_view train_id) {
  auto train_id_hash = TrainIdHasher(train_id);
  auto it = trains_.find(train_id_hash);
  if (it == trains_.end()) return "-1";  // 车次不存在
  Train &train = it->second;
  if (train.released) return "-1";  // 不可重复 release
  train.released = true;  // TODO: 将修改写回文件。此处因为使用 std::map 所以可以通过迭代器直接修改，但 B+ 树不能。
  for (int i = 0; i < train.station_num; ++i) {
    station_trains_.emplace(std::make_pair(StationHasher(train.stations[i]), train_id_hash),
        StationTrain(train_id_hash, train.arrival_times[i], train.departure_times[i], train.sum_prices[i], i, train));
  }
  return "0";
}

std::string TrainManager::QueryTrain(std::string_view train_id, Date target_date) {
  auto train_id_hash = TrainIdHasher(train_id);
  auto it = trains_.find(train_id_hash);
  if (it == trains_.end()) return "-1";  // 车次不存在
  Train &train = it->second;
  if (target_date < train.start_sale || train.end_sale < target_date) return "-1";  // 超出日期范围
  std::string ret;
  append(ret, train_id, ' ', train.type, '\n');
  TrainSeats seats(train.seat_num, train.station_num);
  if (train.released) {
    auto it = train_seats_.find(std::make_pair(train_id_hash, target_date));
    if (it != train_seats_.end()) {
      seats = it->second;
    }
  }
  append(ret, train.stations[0].c_str(), " xx-xx xx:xx -> ",  //
      DateTime(target_date, train.departure_times[0]).ToString(), ' ',  //
      std::to_string(train.sum_prices[0]), ' ', std::to_string(seats.seat_num[0]), '\n');
  for (int i = 1; i < train.station_num - 1; ++i) {
    append(ret, train.stations[i].c_str(), ' ',  //
        DateTime(target_date, train.arrival_times[i]).ToString(), " -> ",
        DateTime(target_date, train.departure_times[i]).ToString(), ' ',  //
        std::to_string(train.sum_prices[i]), ' ', std::to_string(seats.seat_num[i]), '\n');
  }
  append(ret, train.stations[train.station_num - 1].c_str(), ' ',
      DateTime(target_date, train.arrival_times[train.station_num - 1]).ToString(), " -> xx-xx xx:xx ",
      std::to_string(train.sum_prices[train.station_num - 1]), " x\n");
  return ret;
}

namespace {
// 仅供 std::map 临时使用，之后换成 B+ 树
template <class Key, class Value>
vector<Value> RangeFind(const std::map<Key, Value> &m, const Key &key1, const Key &key2) {
  auto it = m.lower_bound(key1);
  vector<Value> ret;
  for (; it != m.end() && !(key2 < it->first); ++it) {
    ret.push_back(it->second);
  }
  return ret;
}

// ComparisonOf<T> 表示类型 T 的比较函数指针类型，用于在同名函数中选择一种特定的重载。
template <class T>
using ComparisonOf = bool (*)(const T &, const T &);
}  // namespace

std::string TrainManager::QueryTicket(
    Date date, std::string_view from_station, std::string_view to_station, SortOrder sort_order) {
  auto from_hash = StationHasher(from_station), to_hash = StationHasher(to_station);
  vector<StationTrain> start_trains =
      RangeFind(station_trains_, std::make_pair(from_hash, kHashMin), std::make_pair(from_hash, kHashMax));
  vector<StationTrain> end_trains =
      RangeFind(station_trains_, std::make_pair(to_hash, kHashMin), std::make_pair(to_hash, kHashMax));
  if (start_trains.empty() || end_trains.empty()) return "0";
  vector<Ticket> result;
  for (auto i = start_trains.begin(), j = end_trains.begin(); i != start_trains.end(); ++i) {
    while (j != end_trains.end() && j->train_id_hash < i->train_id_hash) ++j;
    if (j == end_trains.end()) break;
    if (i->train_id != j->train_id) continue;
    if (i->rank >= j->rank) continue;  // 列车运行方向不符
    auto [days, time] = i->departure_time.GetDayTime();
    Date departure_date = date - days;
    if (departure_date < i->start_sale || i->end_sale < departure_date) continue;  // 超出售票日期
    auto seats = GetSeats(i->train_id_hash, date, i->seat_num, i->station_num);
    result.push_back({i->train_id, departure_date + i->departure_time, departure_date + j->arrival_time,
        j->arrival_time - i->departure_time, j->sum_price - i->sum_price, seats.RangeMin(i->rank, j->rank)});
  }
  if (result.empty()) return "0";
  if (sort_order == SortOrder::TIME) {
    Sort(result.begin(), result.end(), ComparisonOf<Ticket>(CompareTime));
  } else {
    Sort(result.begin(), result.end(), ComparisonOf<Ticket>(CompareCost));
  }
  std::string ret = std::to_string(result.size());
  for (auto i : result) {
    append(ret, '\n', i.train_id.c_str(), ' ', from_station, ' ', i.start_time.ToString(), " -> ", to_station, ' ',
        i.end_time.ToString(), ' ', std::to_string(i.cost), ' ', std::to_string(i.seat));
  }
  return ret;
}

std::string TrainManager::QueryTransfer(
    Date date, std::string_view from_station, std::string_view to_station, SortOrder sort_order) {
  auto from_hash{StationHasher(from_station)}, to_hash{StationHasher(to_station)};
  vector<StationTrain> start_trains =
      RangeFind(station_trains_, std::make_pair(from_hash, kHashMin), std::make_pair(from_hash, kHashMax));
  vector<StationTrain> end_trains =
      RangeFind(station_trains_, std::make_pair(to_hash, kHashMin), std::make_pair(to_hash, kHashMax));
  if (start_trains.empty() || end_trains.empty()) return "0";
  
  TransferTicket ans, cur;
  bool is_better = false;

  for (auto i : start_trains) {
    Date i_start_date = date - i.departure_time.GetDays();
    if (i_start_date < i.start_sale || i.end_sale < i_start_date) continue;
    Train i_train = trains_.find(i.train_id_hash)->second, j_train;
    std::unordered_map<StationHash, int> station_rank;
    for (int k = i.rank + 1; k < i_train.station_num; ++k) station_rank[StationHasher(i_train.stations[k])] = k;
    for (auto j : end_trains) {
      if (i.train_id == j.train_id) continue;
      Train j_train = trains_.find(j.train_id_hash)->second;  // TODO: 考虑使用缓存
      for (int k = 0; k < j.rank; ++k) {
        auto &transfer_station = j_train.stations[k];
        auto transfet_station_hash = StationHasher(transfer_station);
        auto iter = station_rank.find(transfet_station_hash);
        if (iter == station_rank.end()) continue;
        DateTime i_arr_datetime = i_start_date + i_train.arrival_times[iter->second];
        auto [i_arr_date, i_arr_time] = i_arr_datetime.GetDateAndTime();
        auto [j_dep_days, j_dep_time] = j_train.departure_times[k].GetDayTime();
        // j_start_date 为 j 车可行的最早发车日期，由于可以在中转站停留任意长的时间，故只需要计算最早发车日期
        Date j_start_date = i_arr_date - j_dep_days;
        if (j_dep_time < i_arr_time) j_start_date += kOneDay;
        j_start_date = std::max(j_start_date, j_train.start_sale);  // 发车日期不能早于开始售票的日期

        DateTime j_dep_datetime(j_start_date, j_dep_time);
        cur.ticket1.cost = i_train.sum_prices[iter->second] - i.sum_price;
        cur.ticket2.cost = j.sum_price - j_train.sum_prices[k];
        cur.cost = cur.ticket1.cost + cur.ticket2.cost;
        cur.duration = (i_train.arrival_times[iter->second] - i_train.departure_times[i.rank]) +
                       (j_train.arrival_times[j.rank] - j_train.departure_times[k]) + (j_dep_datetime - i_arr_datetime);

        if (sort_order == SortOrder::TIME) {
          is_better = CompareTime(cur, ans);
        } else {
          is_better = CompareCost(cur, ans);
        }

        if (is_better) {
          ans = cur;  // TODO: optimize
          ans.transfer_station = transfer_station.str();
          ans.ticket1.start_time = i_start_date + i.departure_time;
          ans.ticket1.end_time = i_arr_datetime;
          auto i_seats = GetSeats(i.train_id_hash, i_start_date, i.seat_num, i.station_num);
          ans.ticket1.seat = i_seats.RangeMin(i.rank, iter->second - 1);

          ans.ticket2.start_time = j_dep_datetime;
          ans.ticket2.end_time = j_start_date + j.arrival_time;
          auto j_seats = GetSeats(j.train_id_hash, j_start_date, j.seat_num, j.station_num);
          ans.ticket2.seat = j_seats.RangeMin(i.rank, iter->second - 1);
        }
      }
    }
  }
  if (!is_better) return "0";
  std::string ret;
  append(ret, ans.ticket1.train_id.c_str(), ' ', from_station, ' ', ans.ticket1.start_time.ToString(), " -> ",
      ans.transfer_station, ' ' + ans.ticket1.end_time.ToString(), ' ', std::to_string(ans.ticket1.cost), ' ',
      std::to_string(ans.ticket1.seat));
  append(ret, '\n', ans.ticket2.train_id.c_str(), ' ', ans.transfer_station, ' ', ans.ticket2.start_time.ToString(),
      " -> ", to_station, ' ' + ans.ticket2.end_time.ToString(), ' ', std::to_string(ans.ticket2.cost), ' ',
      std::to_string(ans.ticket2.seat));
  return ret;
}

TrainSeats TrainManager::GetSeats(Train::IdHash train_id_hash, Date date, int initial_seat_num, int station_num) {
  auto it = train_seats_.find(std::make_pair(train_id_hash, date));
  if (it == train_seats_.end()) return TrainSeats(initial_seat_num, station_num);
  return it->second;
}

}  // namespace lin