#include "agent.hh"
#include <iostream>

namespace tagt::xchg::agent {

void reply(Id agent, const packet::Packet& response) {
  std::cout << "[XCHG=>" << agent << "]: " << response.serialize() << std::endl;
}

}
