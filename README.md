XCHG
----

An order-matching and exchange system for testing trading bots. Part of the software suite to be used by
Trading @ Georgia Tech club.

### Features

* Support for market, limit, stop-loss, and stop-limit orders
* Support for IOC (immediate-or-cancel) and AON (all-or-nothing) trades
* Ability to query book for BBO, volume, etc.
* Support for 2^16 simultaneous trading agents
* Listing by ticker

### Ideas

* Book stratification by ticker, direction, and order-type (how to best stratify on order-type?)
* TCP-based communication to each port, with option for local or remote trading bots
* Atomic execution with an order queue -- we need to be fast for this to work
* UI to view book + orders in real-time (TUI or web server + HTML)

### Bugs/issues

* Should Book::bbo really return the best price from the *opposite* side of the book?
* Should Book::{bbo,directional_volume} really be template functions?
* We can simplify book operations by making add/remove/update methods instead of directly operating on map + vector
* We probably don't need an unordered_map for the book (but this makes deleting orders slower!)

Test write access
