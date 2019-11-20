#pragma once
#include <string>

namespace tagt::xchg::packet {

struct Packet {
  virtual std::string serialize() const = 0;
};

}
