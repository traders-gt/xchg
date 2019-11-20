#include <iostream>
#include <chrono>
#include "book.hh"

using namespace tagt::xchg;

int main(int argc, char **argv) {
  agent::Id a { 0 };
  book::Book book;

  order::Market order0(book.next_order_id++, a, "ABCD", order::Direction::BUY, 5);
  order::Limit order1(book.next_order_id++, a, "ABCD", order::Direction::SELL, 3, 100);
  order::Limit order2(book.next_order_id++, a, "ABCD", order::Direction::SELL, 2, 150);

  auto resp0 = book.match(order0);
  std::cout << resp0->serialize() << std::endl;
  auto resp1 = book.match(order1);
  std::cout << resp1->serialize() << std::endl;
  auto resp2 = book.match(order2);
  std::cout << resp2->serialize() << std::endl;

  return 0;
}
