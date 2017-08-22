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
- market models: auction, continuous trading
- IEEE-754 decimals
- handles ~250 auctions with 10000 orders each per second
- handles ~1.8M orders per second in continuous trading mode
- twice the performance when double instead of _Decimal64 is used


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


Patterns
--------

A book for continuous auctions:

    while (1) {
            next = next_auction();

            do {
                    clob_ord_t ord = read_order();
                    clob_oid_t oid = clob_add(book, ord);
                    send_confirmation(oid);
            } while (time(NULL) < next);

            mmod_auc_t A = mmod_auction(book);
            unxs_auction(book, A.prc, A.qty);
            send_executions(book);
    }

Routines `next_auction`, `read_order`, `send_confirmation` and
`send_executions` would have to be written.

A book for continuous trading:

    while (1) {
            clob_ord_t ord = read_order();

            /* see if order crosses book */
            ord = unxs_order(book, ord);

            if (ord.qty.dis + ord.qty.hid > 0.dd) {
                    /* put remainder of order into book */
                    clob_oid_t oid = clob_add(book, ord);
                    send_confirmation(oid);
            }

            send_executions(book);
            send_quotes(book);
    }

where additionally `send_quotes` would have to be written to
disseminate updated quotes.  Continuous trading and continuous auction
can freely be mixed as well.

  [1]: http://opensource.org/licenses/BSD-3-Clause
