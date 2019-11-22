#pragma once
#include <vector>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include "order.hh"
#include "packet.hh"
#include "response.hh"

namespace tagt::xchg::book {

/* TODO: We can significantly increase performance if we modify the book
   data structure. As a first step, we should divide into multiple books
   where each book corresponds to one specific security. Next, we can
   split these by direction, since we'll never be searching both buy+sell
   orders in the same query. Beyond that, we can sort orders by the best
   price for that direction, while still preserving the queue property of
   being sorted by time. Finally, we can use different buckets for each
   order type to cut down on the time needed for sorting.
   
   In addition, it would be useful to read about how this is done by
   real-world exchanges as it is likely a pretty solved problem already.
*/
struct Book {
  size_t next_order_id { 0 };
  std::vector<order::Id> order_ids;
  std::unordered_map<order::Id, std::shared_ptr<order::Order>> orders;

  template <class OrderType>
  auto match(OrderType& incoming) ->
    std::enable_if_t<std::is_base_of_v<order::Order, OrderType>, void>;
  template<order::Direction dir>
  int bbo() const;
};

template <class OrderType>
auto Book::match(OrderType& incoming) ->
    std::enable_if_t<std::is_base_of_v<order::Order, OrderType>, void> {
  bool has_match = false;
  std::vector<order::Id>::iterator best_match_index;
  std::shared_ptr<order::Order> best_match;
  order::Price best_match_price {0};

  // Shop around for best price
  for (auto id = this->order_ids.begin(); id < this->order_ids.end(); id++) {
    auto other = this->orders[*id];
    if (incoming.match(*other)) {
      // Order on book is always favored, so pick their "best price"
      // FIXME: Market-market match will always return 0 or infinity
      auto match_price = other->get_match_price(incoming);
      if (!has_match) {
        has_match = true;
        best_match_index = id;
        best_match = other;
        best_match_price = match_price;
      } else if ((incoming.direction == order::Direction::BUY && match_price < best_match_price)
              || (incoming.direction == order::Direction::SELL && match_price > best_match_price)) {
        best_match_index = id;
        best_match = other;
        best_match_price = match_price;
      }

    }
  }

  if (has_match) {
    auto exec_quantity = std::min(incoming.size, best_match->size);
    if (exec_quantity < best_match->size) {
      best_match->size -= exec_quantity;
    } else {
      this->orders.erase(best_match->order_id);
      this->order_ids.erase(best_match_index);
      if (exec_quantity < incoming.size) {
        incoming.size -= exec_quantity;
        // Recurse to execute until fill or no more immediate trades possible
        this->match(incoming);
      }
    }
    agent::reply(incoming.agent, response::Trade(
      incoming.order_id,
      best_match->agent,
      exec_quantity,
      incoming.direction,
      best_match_price
    ));
    agent::reply(best_match->agent, response::Trade(
      best_match->order_id,
      incoming.agent,
      exec_quantity,
      best_match->direction,
      best_match_price
    ));
  } else if (!incoming.flags.IOC) {
    // By pushing to the back, this will stay sorted by timestamp
    // which gives preference to earlier orders in the book
    this->order_ids.push_back(incoming.order_id);
    this->orders.emplace(incoming.order_id, std::make_shared<OrderType>(incoming));
    agent::reply(incoming.agent, response::Pending(
      incoming.order_id
    ));
  } else {
    agent::reply(incoming.agent, response::Failure(
      incoming.order_id,
      "IOC order could not be filled"
    ));
  }
}

}