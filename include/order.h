#include <string>

class OrderManager {
 public:
  /**
   * @brief 查询用户 \p username 的所有订单信息，按照交易时间顺序从新到旧排序。
   * （候补订单即使补票成功，交易时间也以下单时刻为准。）
   */
  void QueryOrder(const std::string username);

  /**
   * @brief 买票。
   *
   * @param pending 为真时表示在余票不足的情况下愿意接受候补购票，当有余票时立即视为此用户购买了车票。
   *
   * @note 这里的日期是列车从 \p start_station 出发的日期，不是从列车始发站出发的日期。
   */
  void BuyTicket(const std::string &username, const std::string &train_id, TimeType date, const int number,
                 const std::string &start_station, const std::string &terminal_station, const bool pending);
  /**
   * @brief 用户 \p username 退订从新到旧（即 query_order 的返回顺序）第 \p number 个（1-base）订单。
   */
  void RefundTicket(const std::string &username, const int number = 1);
};