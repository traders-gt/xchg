#include "agent.hh"
#include <iostream>

namespace tagt::xchg::agent {

/**
 * Prints out XCHG=> followed by whatever string data is associated with agent ID (uint_16)
 * and the response
 * @param agent something to do with the ID of the agent
 * @param response something about a packet struct
 */
void reply(Id agent, const packet::Packet& response) {
  std::cout << "[XCHG=>" << agent << "]: " << response.serialize() << std::endl;
}

}
