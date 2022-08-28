/*** mmod-pdo.c -- price discovery orders
 *
 * Copyright (C) 2016-2022 Sebastian Freundt
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
#include <string.h>
#if defined WITH_DECIMAL
# include <dfp754_d64.h>
#endif
#include "btree.h"
#include "btree_val.h"
#include "plqu.h"
#include "plqu_val.h"
#include "clob.h"
#include "clob_val.h"
#include "mmod-pdo.h"
#include "nifty.h"


static inline __attribute__((const, pure)) px_t
sign_side(px_t p, clob_side_t s)
{
/* negate ask prices, leave bid prices */
	return s == CLOB_SIDE_ASK ? -p : p;
}

static qx_t
_pdo(clob_ord_t *restrict o, plqu_t q)
{
/* return the quantity taken off of Q */
	plqu_iter_t qi = {.q = q};
	qx_t oq = qty(o->qty);
	qx_t r = 0.dd;

	for (; plqu_iter_next(&qi) && oq > 0.dd; oq = qty(o->qty)) {
		qx_t mq = qty(qi.v.qty);

		if (mq <= oq) {
			/* full maker ~ partial taker */
			o->qty = qty_exe(o->qty, mq);
			/* add maker quantity to result */
			r += mq;
		} else {
			/* partial maker ~ full taker */
			r += oq;
			/* let everyone know there's nothing left */
			o->qty = qty0;
			break;
		}
	}
	return r;
}


mmod_pdo_t
mmod_pdo(clob_t c, clob_ord_t o)
{
	const clob_side_t contra = clob_contra_side(o.sid);
	mmod_pdo_t a = {.base = 0.dd, .term = 0.dd};

	switch (o.typ) {
		btree_iter_t ti;
		bool lmtp;
		px_t r;

	case CLOB_TYPE_LMT:
		/* execute against contra market first, then contra limit */
		ti = (btree_iter_t){.t = (btree_t)c.lmt[contra]};
		if (LIKELY((lmtp = btree_iter_next(&ti)))) {
			/* market orders act like pegs
			 * so find out about the top bid/ask */
			switch (o.sid) {
			case CLOB_SIDE_ASK:
				r = max(o.lmt, ti.k);
				break;
			case CLOB_SIDE_BID:
				r = min(o.lmt, ti.k);
				break;
			}
		} else {
			/* can't execute against NAN reference price */
			r = o.lmt;
		}
		goto marketable;

	case CLOB_TYPE_MKT:
		/* execute against contra market first, then contra limit */
		ti = (btree_iter_t){.t = (btree_t)c.lmt[contra]};
		if (LIKELY((lmtp = btree_iter_next(&ti)))) {
			/* aaah, reference price we need not */
			r = ti.k;
		} else {
			/* can't execute against NAN reference price */
			break;
		}
		/* turn slippage into absolute limit price
		 * only if slippage is a normal */
		o.lmt = r + sign_side(o.slp, o.sid);
		goto marketable;

	marketable:
		/* get markets ready */
		a.base = _pdo(&o, c.mkt[contra]);
		a.term = a.base * r;
	more:
		if (qty(o.qty) <= 0.dd) {
			/* filled */
			break;
		} else if (!lmtp) {
			/* no more limit orders */
			break;
		} else if (o.sid == CLOB_SIDE_ASK && ti.k < o.lmt) {
			/* we need a price improvement */
			break;
		} else if (o.sid == CLOB_SIDE_BID && ti.k > o.lmt) {
			/* not an improvement */
			break;
		}
		/* otherwise dive into limits */
		with (qx_t q = _pdo(&o, ti.v->q)) {
			/* maintain the sum */
			a.base += q;
			a.term += q * ti.k;
			lmtp = btree_iter_next(&ti);
		}
		goto more;
	}
	/* remainder goes back to requester */
	return a;
}

/* mmod-pdo.c ends here */
