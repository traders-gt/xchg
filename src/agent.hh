#pragma once
#include <cstdint>
#include "packet.hh"

namespace tagt::xchg::agent {

// Id = port
using Id = uint16_t;

void reply(Id agent, const packet::Packet& response);

}
