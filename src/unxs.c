/*** unxs.c -- uncrossing schemes
 *
 * Copyright (C) 2016-2017 Sebastian Freundt
 *
 * Author:  Sebastian Freundt <freundt@ga-group.nl>
 *
 * This file is part of clob.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the author nor the names of any contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **/
#if defined HAVE_CONFIG_H
# include "config.h"
#endif	/* HAVE_CONFIG_H */
#include <stdlib.h>
#include <stdio.h>
#if defined HAVE_DFP754_H
# include <dfp754.h>
#elif defined HAVE_DFP_STDLIB_H
# include <dfp/stdlib.h>
#elif defined HAVE_DECIMAL_H
# include <decimal.h>
#endif
#include "btree.h"
#include "btree_val.h"
#include "plqu.h"
#include "plqu_val.h"
#include "clob.h"
#include "clob_val.h"
#include "unxs.h"
#include "nifty.h"


static qty_t
plqu_qty(plqu_t q)
{
	qty_t sum = qty0;
	for (plqu_iter_t i = {.q = q}; plqu_iter_next(&i);) {
		sum = qty_add(sum, i.v.qty);
	}
	return sum;
}

static size_t
_unxs_plqu_sc(
	unxs_exsc_t *restrict x, size_t n, plqu_t q, px_t ref, qx_t max,
	clob_oid_t proto)
{
	plqu_iter_t i = {.q = q};
	size_t m = 0U;
	qx_t F = 0.dd;
	qx_t fil;

	for (; plqu_iter_next(&i) && m < n && F < max; m++, F += fil) {
		proto.qid = plqu_iter_qid(i);
		fil = qty(i.v.qty);

		if (UNLIKELY(F + fil > max)) {
			/* partial fill the last guy,
			 * we're checking F < max in the for-check first
			 * so we can just finish the loop body as planned */
			fil = max - F;
			i.v.qty = qty_exe(i.v.qty, fil);
			plqu_iter_put(i, i.v);
			/* fill him and out */
			x[m++] = (unxs_exsc_t){{ref, fil}, proto};
			break;
		}
		/* otherwise fill him fully */
		x[m] = (unxs_exsc_t){{ref, fil}, proto};
	}
	plqu_iter_set_top(i);
	return m;
}

static size_t
_unxs_order(
	unxs_exbi_t *restrict x, size_t n, clob_ord_t *restrict o,
	plqu_t q, px_t r, clob_oid_t maker, clob_oid_t taker)
{
	plqu_iter_t qi = {.q = q};
	qx_t oq = qty(o->qty);
	size_t m = 0U;

	for (; plqu_iter_next(&qi) && m < n && oq > 0.dd; oq = qty(o->qty)) {
		qx_t mq = qty(qi.v.qty);

		maker.qid = plqu_iter_qid(qi);
		if (mq <= oq) {
			/* full maker ~ partial taker */
			x[m++] = (unxs_exbi_t){{r, mq}, {maker, taker}};
			o->qty = qty_exe(o->qty, mq);
		} else {
			/* partial maker ~ full taker */
			x[m++] = (unxs_exbi_t){{r, oq}, {maker, taker}};
			qi.v.qty = qty_exe(qi.v.qty, oq);
			plqu_iter_put(qi, qi.v);
			/* let everyone know there's nothing left */
			o->qty = qty0;
			break;
		}
	}
	plqu_iter_set_top(qi);
	return m;
}


size_t
unxs_mass_sc(unxs_exsc_t *restrict x, size_t n, clob_t c, px_t p, qx_t q)
{
	clob_oid_t proto = {};
	size_t m = 0U;
	qx_t Q;

	if (UNLIKELY(isnandpx(p))) {
		/* no price? */
		return 0U;
	} else if (UNLIKELY(q <= 0.dd)) {
		/* no volume? */
		return 0U;
	}

	Q = 0.dd;
	proto = (clob_oid_t){TYPE_MKT, SIDE_SHORT, .prc = NANPX};
	/* market orders have the highest priority */
	with (plqu_t pq = c.mkt[proto.sid]) {
		qx_t before = qty(plqu_qty(pq));
		qx_t after;

		/* set the executor free */
		m += _unxs_plqu_sc(x + m, n - m, pq, p, q - Q, proto);
		/* what's left? */
		after = qty(plqu_qty(pq));
		/* up our Q */
		Q += before - after;
	}
	/* and now limit orders */
	proto = (clob_oid_t){TYPE_LMT, SIDE_ASK};
	for (btree_iter_t i = {.t = c.lmt[proto.sid]};
	     m < n && Q < q && btree_iter_next(&i) && (proto.prc = i.k) <= p;) {
		qx_t before = qty(i.v->sum);
		qx_t after;

		/* set the executor free */
		m += _unxs_plqu_sc(x + m, n - m, i.v->q, p, q - Q, proto);
		/* maintain lmt sum */
		with (qty_t sum = plqu_qty(i.v->q)) {
			if ((after = qty(i.v->sum = sum)) <= 0.dd) {
				btree_val_t v = btree_rem(i.t, i.k);
				free_plqu(v.q);
			}
		}
		/* also up our Q */
		Q += before - after;
	}

	Q = 0.dd;
	proto = (clob_oid_t){TYPE_MKT, SIDE_LONG};
	/* market orders have the highest priority */
	with (plqu_t pq = c.mkt[proto.sid]) {
		qx_t before = qty(plqu_qty(pq));
		qx_t after;

		/* set the executor free */
		m += _unxs_plqu_sc(x + m, n - m, pq, p, q - Q, proto);
		/* what's left? */
		after = qty(plqu_qty(pq));
		/* up the Q */
		Q += before - after;
	}
	/* and limit orders again */
	proto = (clob_oid_t){TYPE_LMT, SIDE_LONG};
	for (btree_iter_t i = {.t = c.lmt[proto.sid]};
	     m < n && Q < q && btree_iter_next(&i) && (proto.prc = i.k) >= p;) {
		qx_t before = qty(i.v->sum);
		qx_t after;

		/* set the executor free */
		m += _unxs_plqu_sc(x + m, n - m, i.v->q, p, q - Q, proto);
		/* maintain lmt sum */
		with (qty_t sum = plqu_qty(i.v->q)) {
			if ((after = qty(i.v->sum = sum)) <= 0.dd) {
				btree_val_t v = btree_rem(i.t, i.k);
				free_plqu(v.q);
			}
		}
		/* also up our Q */
		Q += before - after;
	}
	return m;
}

size_t
unxs_order(unxs_exbi_t *restrict x, size_t n, clob_t c, clob_ord_t o, px_t r)
{
	clob_oid_t maker, taker;
	size_t m = 1U;

	maker = (clob_oid_t){.sid = clob_contra_side(o.sid), .prc = NANPX};
	taker = (clob_oid_t){o.typ, o.sid, .prc = NANPX};

	switch (o.typ) {
		btree_iter_t ti;
		plqu_iter_t qi;
		bool lmtp;

	case TYPE_LMT:

	case TYPE_MKT:
		/* execute against contra market first, then contra limit */
		ti = (btree_iter_t){.t = c.lmt[maker.sid]};
		if (LIKELY((lmtp = btree_iter_next(&ti)))) {
			/* aaah, reference price we need not */
			r = ti.k;
		} else if (UNLIKELY(isnandpx(r))) {
			/* can't execute against NAN reference price */
			return 0U;
		}
		/* get markets ready */
		maker.typ = TYPE_MKT;
		m += _unxs_order(
			x + m, n - m, &o, c.mkt[maker.sid], r, maker, taker);
	more:
		if (qty(o.qty) <= 0.dd) {
			goto fill;
		} else if (!lmtp) {
			goto rest;
		}
		/* otherwise dive into limits */
		maker.typ = TYPE_LMT;
		m += _unxs_order(x + m, n - m, &o, ti.v->q, r, maker, taker);
		/* maintain the sum */
		with (qty_t sum = plqu_qty(qi.q)) {
			if (qty(ti.v->sum = sum) <= 0.dd) {
				btree_val_t v = btree_rem(ti.t, ti.k);
				free_plqu(v.q);
				lmtp = btree_iter_next(&ti);
			}
		}
		goto more;
	}
rest:
	/* put the rest on the queue and bugger off*/
	maker = clob_add(c, o);
	/* let the user know about the order in the book */
	x[0U] = (unxs_exbi_t){{NANPX, qty(o.qty)}, {maker}};
	return m;
fill:
	x[0U] = (unxs_exbi_t){{NANPX, 0.dd}};
	return m;
}

/* unxs.c ends here */
