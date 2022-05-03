#include <string>

#include "vector.h"

class TrainManager {
 public:
  /**
   * @brief 添加一辆火车。
   */
  void AddTrain(const std::string &train_id, const int station_num, const int total_seat_num,
                vector<std::string> stations, vector<int> price, const std::string &start_time,
                vector<std::string> travel_times, vector<std::string> stopover_times, vector<std::string> sale_date,
                const char type);

  /// 发布火车。发布前的车次，不可发售车票；发布后的车次不可被删除，可发售车票。
  void ReleaseTrain(const std::string &train_id);

  /// 询问符合条件的火车。
  void QueryTrain(TrainIDType train_id, TimeType target_date);

  /// 排序依据
  enum SortOrder { TIME, COST };

  /**
   * @brief 查询指定日期时从 \p start_station 出发，并到达 \p terminal_station 的车票。
   *
   * @note 这里的日期是列车从 \p start_station 出发的日期，不是从列车始发站出发的日期。
   */
  void QueryTicket(TimeType date, std::string &start_station, std::string &terminal_station,
                   SortOrder sort_order = TIME);
  /**
   * @brief 在恰好换乘一次（换乘同一辆车不算恰好换乘一次）的情况下查询符合条件的车次。
   * 仅输出最优解。如果出现多个最优解（排序关键字最小)，则选择在第一辆列车上花费的时间更少的方案。
   *
   * @note 这里的日期是列车从 \p start_station 出发的日期，不是从列车始发站出发的日期。
   */
  void QueryTransfer(TimeType date, std::string &start_station, std::string &terminal_station,
                     SortOrder sort_order = TIME);
};