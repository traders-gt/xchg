#pragma once
#include <string>

#include "agent.hh"
#include "book.hh"

namespace tagt::xchg::parse {

void parseAndSubmitOrder(const agent::Id agent, book::Book& book, const std::string& data);

}
