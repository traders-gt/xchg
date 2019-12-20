#include "order.hh"

#include <algorithm>
#include <cassert>

namespace tagt::xchg::order {

bool Range::is_valid() const {
  return this->lower <= this->upper;
}

Range Range::overlap(const Range& other) const {
  return {
    .lower = std::max(this->lower, other.lower),
    .upper = std::min(this->upper, other.upper)
  };
}

Range Market::range() const {
  return { .lower = PRICE_MIN, .upper = PRICE_MAX };
}

Range Limit::range() const {
  switch (this->direction) {
    case Direction::BUY:
      return { .lower = PRICE_MIN, .upper = this->price };
    case Direction::SELL:
      return { .lower = this->price, .upper = PRICE_MAX };
    default:
      assert(false);
  }
}

Range StopLoss::range() const {
  switch (this->direction) {
    case Direction::BUY:
      return { .lower = this->price, .upper = PRICE_MAX };
    case Direction::SELL:
      return { .lower = PRICE_MIN, .upper = this->price };
    default:
      assert(false);
  }
}

Range StopLimit::range() const {
  switch (this->direction) {
    case Direction::BUY:
      return { .lower = this->stop_price, .upper = this->limit_price };
    case Direction::SELL:
    default:
      return { .lower = this->limit_price, .upper = this->stop_price };
  }
}

bool Order::match(const Order& other) const {
  switch (this->direction) {
    case Direction::BUY:
      return this->ticker == other.ticker &&
             other.direction == Direction::SELL &&
             this->range().overlap(other.range()).is_valid() &&
             (!this->flags.AON || this->size <= other.size);
    case Direction::SELL:
      return this->ticker == other.ticker &&
             other.direction == Direction::BUY &&
             this->range().overlap(other.range()).is_valid() &&
             (!this->flags.AON || this->size <= other.size);
    default:
      assert(false);
  }
}

Price Order::get_match_price(const Order& other) const {
  auto my_range = this->range();
  auto their_range = other.range();
  auto overlap = my_range.overlap(their_range);

  switch (this->direction) {
    case Direction::BUY:
      return overlap.lower;
    case Direction::SELL:
      return overlap.upper;
    default:
      assert(false);
  }
}

}
