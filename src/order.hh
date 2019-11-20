#pragma once
#include <ctime>
#include <chrono>
#include <cstdint>
#include <climits>
#include "agent.hh"

namespace tagt::xchg::order {

using Ticker = std::string;
using Id = size_t;
using Price = uint32_t;
#define PRICE_MIN 0
#define PRICE_MAX UINT32_MAX

enum class Direction { BUY, SELL };

struct Flags {
  bool AON {false}; // All-or-none
  bool IOC {false}; // Immediate-or-cancel
};

struct Range {
  Price lower, upper;

  bool is_valid() const;
  Range overlap(const Range& other) const;
};

struct Order {
  Order(Id order_id_, agent::Id agent_, Ticker ticker_, Direction direction_,
         size_t size_, Flags flags_ = {},
         std::time_t timestamp_ = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))
    : order_id(order_id_), agent(agent_), ticker(ticker_), direction(direction_),
      size(size_), flags(flags_), timestamp(timestamp_) {}

  Id order_id;
  agent::Id agent;
  Ticker ticker;
  Direction direction;
  size_t size;
  Flags flags;
  std::time_t timestamp;

  virtual Range range() const = 0;
  bool match(const Order& other) const;
  Price get_match_price(const Order& other) const;
};

struct Market : public Order {
  Market(Id order_id_, agent::Id agent_, Ticker ticker_, Direction direction_,
         size_t size_, Flags flags_ = {},
         std::time_t timestamp_ = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))
    : Order(order_id_, agent_, ticker_, direction_, size_, flags_, timestamp_) {}

  Range range() const;
};

struct Limit : public Order {
  Limit(Id order_id_, agent::Id agent_, Ticker ticker_, Direction direction_,
        size_t size_, Price price_, Flags flags_ = {},
        std::time_t timestamp_ =  std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))
    : Order(order_id_, agent_, ticker_, direction_, size_, flags_, timestamp_),
      price(price_) {}
  
  Price price;

  Range range() const;
};

struct StopLoss : public Order {
  StopLoss(Id order_id_, agent::Id agent_, Ticker ticker_, Direction direction_,
           size_t size_, Price price_, Flags flags_ = {},
           std::time_t timestamp_ =  std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))
    : Order(order_id_, agent_, ticker_, direction_, size_, flags_, timestamp_),
      price(price_) {}

  Price price;

  Range range() const;
};

struct StopLimit : public Order {
  StopLimit(Id order_id_, agent::Id agent_, Ticker ticker_, Direction direction_,
         size_t size_, Price stop_price_, Price limit_price_, Flags flags_ = {},
           std::time_t timestamp_ =  std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))
    : Order(order_id_, agent_, ticker_, direction_, size_, flags_, timestamp_),
      stop_price(stop_price_), limit_price(limit_price_) {}

  Price stop_price;
  Price limit_price;

  Range range() const;
};

}
