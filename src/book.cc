#include "book.hh"

namespace tagt::xchg::book {

template<order::Direction>
inline std::shared_ptr<const order::Order> best
    (std::shared_ptr<const order::Order> a, std::shared_ptr<const order::Order> b);

template<>
inline std::shared_ptr<const order::Order> best<order::Direction::BUY>
    (std::shared_ptr<const order::Order> a, std::shared_ptr<const order::Order> b) {
  if (a->range().lower <= b->range().lower) {
    return a;
  } else {
    return b;
  }
}

template<>
inline std::shared_ptr<const order::Order> best<order::Direction::SELL>
    (std::shared_ptr<const order::Order> a, std::shared_ptr<const order::Order> b) {
  if (a->range().upper >= b->range().upper) {
    return a;
  } else {
    return b;
  }
}

template<order::Direction>
inline order::Price price(std::shared_ptr<const order::Order> a);

template<>
inline order::Price price<order::Direction::BUY>(std::shared_ptr<const order::Order> a) {
  return a->range().lower;
}

template<>
inline order::Price price<order::Direction::SELL>(std::shared_ptr<const order::Order> a) {
  return a->range().upper;
}

// Returns best price that can be found for an order in the given direction
template<order::Direction dir>
order::Price Book::bbo(order::Ticker ticker) const {
  auto first_id = this->order_ids[0];
  std::shared_ptr<const order::Order> best_order = this->orders.at(first_id);

  for (auto id: this->order_ids) {
    auto order = this->orders.at(id);
    if (order->ticker == ticker && order->direction != dir) {
      best_order = best<dir>(best_order, this->orders.at(id));
    }
  }
  return price<dir>(best_order);
}

// Returns the number of orders of the given direction currently on the book
template<order::Direction dir>
size_t Book::directional_volume(order::Ticker ticker) const {
  size_t count = 0;

  for (auto id: this->order_ids) {
    auto order = this->orders.at(id);
    if (order->ticker == ticker && order->direction == dir) {
      count++;
    }
  }

  return count;
}

// Returns the number of orders currently on the book
size_t Book::volume(order::Ticker ticker) const {
  size_t count = 0;

  for (auto id: this->order_ids) {
    auto order = this->orders.at(id);
    if (order->ticker == ticker) {
      count++;
    }
  }

  return count;
}

}