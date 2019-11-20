#include <sstream>
#include "response.hh"

namespace tagt::xchg::response {

std::string Failure::serialize() const {
  std::stringstream ss;
  ss << "Failure: " << this->msg;
  return ss.str();
}

std::string Trade::serialize() const {
  std::stringstream ss;
  ss << "Trade: {" << "price: " << this->price << ", size: " << this->size << "}";
  return ss.str();
}

std::string Pending::serialize() const {
  std::stringstream ss;
  ss << "Pending";
  return ss.str();
}

}
