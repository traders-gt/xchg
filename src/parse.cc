#include "parse.hh"
#include "order.hh"

#include <cassert>
#include <sstream>

namespace tagt::xchg::parse {

// FIXME: This does 0 validation. We can reject on any kind of error.

// Format: TM{B,S}{T,F}{T,F}<TICKER>.<QUANTITY>
// trade, market, direction, aon, ioc, ticker, quantity
order::Market parseMarket(const order::Id order, const agent::Id agent, const std::string& data) {
  auto dir = data[2] == 'B' ? order::Direction::BUY : order::Direction::SELL;
  auto AON = data[3] == 'T' ? true : false;
  auto IOC = data[4] == 'T' ? true : false;

  size_t ticker_end_index {5};
  std::stringstream ts;
  while (data[ticker_end_index] != '.')
    ts << data[ticker_end_index++];

  size_t quantity_end_index {ticker_end_index + 1}, quantity {0};
  while (quantity_end_index < data.length())
    quantity = (quantity * 10) + (data[quantity_end_index++] - '0');

  return order::Market(order, agent, ts.str(), dir, quantity, { .AON = AON, .IOC = IOC });
}

// Format: TL{B,S}{T,F}{T,F}<TICKER>.<QUANTITY>.<PRICE>
// trade, limit, direction, aon, ioc, ticker, quantity, price
order::Limit parseLimit(const order::Id order, const agent::Id agent, const std::string& data) {
  auto dir = data[2] == 'B' ? order::Direction::BUY : order::Direction::SELL;
  auto AON = data[3] == 'T' ? true : false;
  auto IOC = data[4] == 'T' ? true : false;

  size_t ticker_end_index {5};
  std::stringstream ts;
  while (data[ticker_end_index] != '.')
    ts << data[ticker_end_index++];

  size_t quantity_end_index {ticker_end_index + 1}, quantity {0};
  while (data[quantity_end_index] != '.')
    quantity = (quantity * 10) + (data[quantity_end_index++] - '0');
  
  size_t price_end_index {quantity_end_index + 1}, price {0};
  while (price_end_index < data.length())
    price = (price * 10) + (data[price_end_index++] - '0');

  return order::Limit(order, agent, ts.str(), dir, quantity, price, { .AON = AON, .IOC = IOC });
}

// Format: Ts{B,S}{T,F}{T,F}<TICKER>.<QUANTITY>.<PRICE>
// trade, stop-loss, direction, aon, ioc, ticker, quantity, price
order::StopLoss parseStopLoss(const order::Id order, const agent::Id agent, const std::string& data) {
  auto dir = data[2] == 'B' ? order::Direction::BUY : order::Direction::SELL;
  auto AON = data[3] == 'T' ? true : false;
  auto IOC = data[4] == 'T' ? true : false;

  size_t ticker_end_index {5};
  std::stringstream ts;
  while (data[ticker_end_index] != '.')
    ts << data[ticker_end_index++];

  size_t quantity_end_index {ticker_end_index + 1}, quantity {0};
  while (data[quantity_end_index] != '.')
    quantity = (quantity * 10) + (data[quantity_end_index++] - '0');
  
  size_t price_end_index {quantity_end_index + 1}, price {0};
  while (price_end_index < data.length())
    price = (price * 10) + (data[price_end_index++] - '0');

  return order::StopLoss(order, agent, ts.str(), dir, quantity, price, { .AON = AON, .IOC = IOC });
}

// Format: TS{B,S}{T,F}{T,F}<TICKER>.<QUANTITY>.<STOP>.<LIMIT>
// trade, stop-limit, direction, aon, ioc, ticker, quantity, stop price, limit price
order::StopLimit parseStopLimit(const order::Id order, const agent::Id agent, const std::string& data) {
  auto dir = data[2] == 'B' ? order::Direction::BUY : order::Direction::SELL;
  auto AON = data[3] == 'T' ? true : false;
  auto IOC = data[4] == 'T' ? true : false;

  size_t ticker_end_index {5};
  std::stringstream ts;
  while (data[ticker_end_index] != '.')
    ts << data[ticker_end_index++];

  size_t quantity_end_index {ticker_end_index + 1}, quantity {0};
  while (data[quantity_end_index] != '.')
    quantity = (quantity * 10) + (data[quantity_end_index++] - '0');
  
  size_t stop_end_index {quantity_end_index + 1}, stop {0};
  while (data[stop_end_index] != '.')
    stop = (stop * 10) + (data[stop_end_index++] - '0');

  size_t limit_end_index {stop_end_index + 1}, limit {0};
  while (limit_end_index < data.length())
    limit = (limit * 10) + (data[limit_end_index++] - '0');

  return order::StopLimit(order, agent, ts.str(), dir, quantity, stop, limit, { .AON = AON, .IOC = IOC });
}

void parseAndSubmitOrder(const agent::Id agent, book::Book& book, const std::string& data) {
  // T = "trade"
  if (data[0] != 'T')
    return;
  switch (data[1]) {
    case 'M': { // Market
      auto market = parseMarket(book.next_order_id++, agent, data);
      book.match(market);
      break;
    }
    case 'L': { // Limit
      auto limit = parseLimit(book.next_order_id++, agent, data);
      book.match(limit);
      break;
    }
    case 's': { // Stop-loss
      auto stopLoss = parseStopLoss(book.next_order_id++, agent, data);
      book.match(stopLoss);
      break;
    }
    case 'S': { // Stop-limit
      auto stopLimit = parseStopLimit(book.next_order_id++, agent, data);
      book.match(stopLimit);
      break;
    }
    default:
      assert(false);
  }
}

}