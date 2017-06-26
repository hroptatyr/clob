clob
====

A central limit order book designed to support several market models
(uncrossing schemes).  Emphasis is on high throughput.


Red tape
--------

- licensed under [BSD3c][1]
- dependencies: POSIX system, C11 compiler with DFP754 decimal support


Features
--------

- b+tree based central limit order book
- pluggable uncrossing schemes
- order types: market, limit, stop and peg (mid, best)
- market models: auction, continuous trading
- handles ~250 auctions with 10000 orders each per second
- handles ~10M orders per second in continuous trading mode


Motivation
----------

In academia, we want to study crossed books and the orders that caused
them, or gain insight on the effects of walls of stop orders on
liquidity,  or simply study new order types or schemes of uncrossing.

Off-the-shelf products are mostly unsuitable for academic studies.

Not only do they often lack support for a batch mode, they also tightly
couple their order book implementation and uncrossing scheme, a bundle
known as *matching engine*, that allows orders or trades to cut corners
under the label of latency or high performance.

In many cases, it is impossible to extend the functionality of a
matching engine, e.g. to implement the semantics of mid-point orders or
stop orders, or dark liquidity, etc.  Nearly all attempts end up in
changing the existing implementation considerably, or, worse, putting a
second implementation to the side of the existing one with no synergy
between them.

This prompted us to write `clob`, a simple matching engine that's
flexible enough to serve our studies.


  [1]: http://opensource.org/licenses/BSD-3-Clause
