#include <iostream>
#include <chrono>
#include "book.hh"

using namespace tagt::xchg;

int main(int argc, char **argv) {
  agent::Id a { 0 }, b { 1 };
  book::Book book;

  order::Market order0(book.next_order_id++, a, "ABCD", order::Direction::BUY, 5);
  order::Limit order1(book.next_order_id++, b, "ABCD", order::Direction::SELL, 3, 100);
  order::Limit order2(book.next_order_id++, b, "ABCD", order::Direction::SELL, 2, 150);

  book.match(order1);
  book.match(order2);
  book.match(order0);

  return 0;
}
