#pragma once
#include <string>
#include "agent.hh"
#include "order.hh"
#include "packet.hh"

namespace tagt::xchg::response {

struct Failure : public packet::Packet {
  Failure(order::Id _order_id, std::string _msg)
    : order_id(_order_id), msg(_msg) {}

  order::Id order_id;
  std::string msg;

  std::string serialize() const;
};

struct Trade : public packet::Packet {
  Trade(order::Id _order_id, agent::Id _other, size_t _size,
        order::Direction _dir, order::Price _price)
    : order_id(_order_id), other(_other), size(_size), dir(_dir),
      price(_price) {}

  order::Id order_id;
  agent::Id other;
  size_t size;
  order::Direction dir;
  order::Price price;

  std::string serialize() const;
};

struct Pending : public packet::Packet {
  Pending(order::Id _order_id) : order_id(_order_id) {}

  order::Id order_id;

  std::string serialize() const;
};

}
