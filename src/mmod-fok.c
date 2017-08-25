/*** mmod-fok.c -- decide on FOK orders
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
#include <string.h>
#if defined HAVE_DFP754_H
# include <dfp754.h>
#elif defined HAVE_DFP_STDLIB_H
# include <dfp/stdlib.h>
#elif defined HAVE_DECIMAL_H
# include <decimal.h>
#endif
#include <dfp754_d64.h>
#include "btree.h"
#include "btree_val.h"
#include "plqu.h"
#include "plqu_val.h"
#include "clob.h"
#include "clob_val.h"
#include "mmod-fok.h"
#include "nifty.h"


static inline __attribute__((const, pure)) px_t
sign_side(px_t p, clob_side_t s)
{
/* negate ask prices, leave bid prices */
	return s == CLOB_SIDE_ASK ? -p : p;
}

static void
_fok(clob_ord_t *restrict o, plqu_t q)
{
/* return the quantity taken off of Q */
	plqu_iter_t qi = {.q = q};
	qx_t oq = qty(o->qty);

	for (; plqu_iter_next(&qi) && oq > 0.dd; oq = qty(o->qty)) {
		qx_t mq = qty(qi.v.qty);

		if (mq <= oq) {
			/* full maker ~ partial taker */
			o->qty = qty_exe(o->qty, mq);
		} else {
			/* partial maker ~ full taker */
			qi.v.qty = qty_exe(qi.v.qty, oq);
			/* let everyone know there's nothing left */
			o->qty = qty0;
			break;
		}
	}
	return;
}


int
mmod_fok(clob_t c, clob_ord_t o)
{
	const clob_side_t contra = clob_contra_side(o.sid);

	switch (o.typ) {
		btree_iter_t ti;
		bool lmtp;

	case CLOB_TYPE_LMT:
		/* execute against contra market first, then contra limit */
		ti = (btree_iter_t){.t = (btree_t)c.lmt[contra]};
		lmtp = btree_iter_next(&ti);
		goto marketable;

	case CLOB_TYPE_MKT:
		/* execute against contra market first, then contra limit */
		ti = (btree_iter_t){.t = (btree_t)c.lmt[contra]};
		if (LIKELY((lmtp = btree_iter_next(&ti)))) {
			/* aaah, reference price, use to imply a limit */
			o.lmt = ti.k + sign_side(o.slp, o.sid);
		} else {
			/* no price means no execution */
			goto kill;
		}
		goto marketable;

	marketable:
		/* get markets ready */
		_fok(&o, c.mkt[contra]);
	more:
		if (qty(o.qty) <= 0.dd) {
			goto fill;
		} else if (!lmtp) {
			goto kill;
		} else if (o.sid == CLOB_SIDE_ASK && ti.k < o.lmt) {
			/* we need an improvement over R */
			goto kill;
		} else if (o.sid == CLOB_SIDE_BID && ti.k > o.lmt) {
			/* not an improvement */
			goto kill;
		}
		/* otherwise dive into limits */
		_fok(&o, ti.v->q);
		/* climb up/down the ladder */
		lmtp = btree_iter_next(&ti);
		goto more;
	}
fill:
	/* filled */
	return 1;

kill:
	/* killed */
	return 0;
}

/* mmod-fok.c ends here */
