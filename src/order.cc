#include "order.hh"
#include <algorithm>

namespace tagt::xchg::order {

/**
 * @return whether this range's lower is actually lower than upper bound
 */
bool Range::is_valid() const {
  return this->lower <= this->upper;
}

/**
 * @param other pointer to another Range object
 * @return a Range object with the highest of the .lower and smallest of the .upper
 */
Range Range::overlap(const Range& other) const {
  return {
    .lower = std::max(this->lower, other.lower),
    .upper = std::min(this->upper, other.upper)
  };
}

/**
 * @return constant Range with the smallest possible price and largest possible price
 */
Range Market::range() const {
  return { .lower = PRICE_MIN, .upper = PRICE_MAX };
}

/**
 * @return a constant range with this order's price as the upper bound for a buy or lowerbound for a sell
 */
Range Limit::range() const {
  switch (this->direction) {
    case Direction::BUY:
      return { .lower = PRICE_MIN, .upper = this->price };
    case Direction::SELL:
    default:
      return { .lower = this->price, .upper = PRICE_MAX };
  }
}

/**
 * @return a constant range with this order's price as the lower bound for a buy or upper bound for a sell
 */
Range StopLoss::range() const {
  switch (this->direction) {
    case Direction::BUY:
      return { .lower = this->price, .upper = PRICE_MAX };
    case Direction::SELL:
    default:
      return { .lower = PRICE_MIN, .upper = this->price };
  }
}

/**
 * @return a constant range with buy:[stop_price, limit_price] or sell[limit,stop]
 */
Range StopLimit::range() const {
  switch (this->direction) {
    case Direction::BUY:
      return { .lower = this->stop_price, .upper = this->limit_price };
    case Direction::SELL:
    default:
      return { .lower = this->limit_price, .upper = this->stop_price };
  }
}

/**
 * @param other pointer to another order
 * @return true if tickers match, directions are different, range overlaps valid, (no AON flag or this size smaller than other)
 */
bool Order::match(const Order& other) const {
  switch (this->direction) {
    case Direction::BUY:
      return this->ticker == other.ticker &&
             other.direction == Direction::SELL &&
             this->range().overlap(other.range()).is_valid() &&
             (!this->flags.AON || this->size <= other.size);
    case Direction::SELL:
    default:
      return this->ticker == other.ticker &&
             other.direction == Direction::BUY &&
             this->range().overlap(other.range()).is_valid() &&
             (!this->flags.AON || this->size <= other.size);
  }
}

/**
 * @param other pointer to another order
 * @return Price of lower of overlap if buy, upper of overlap if sell
 */
Price Order::get_match_price(const Order& other) const {
  auto my_range = this->range();
  auto their_range = other.range();
  auto overlap = my_range.overlap(their_range);

  switch (this->direction) {
    case Direction::BUY:
      return overlap.lower;
    case Direction::SELL:
    default:
      return overlap.upper;
  }
}

}
