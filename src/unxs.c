/*** unxs.c -- uncrossing schemes
 *
 * Copyright (C) 2016-2021 Sebastian Freundt
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
#if defined WITH_DECIMAL
# include <dfp754_d64.h>
#else
# include <math.h>
#endif
#include "btree.h"
#include "btree_val.h"
#include "plqu.h"
#include "plqu_val.h"
#include "clob.h"
#include "clob_val.h"
#include "quos.h"
#include "unxs.h"
#include "nifty.h"

#define UNXS_INIZ	(8U)

struct _unxs_s {
	unxs_mode_t m;
	size_t n;
	unxs_exe_t *x;
	uint_fast8_t *s;
	clob_oid_t *o;
	qty_t *q;
	size_t z;
};


static qty_t
plqu_qty(plqu_t q)
{
	qty_t sum = qty0;
	for (plqu_iter_t i = {.q = q}; plqu_iter_next(&i);) {
		sum = qty_add(sum, i.v.qty);
	}
	return sum;
}

static inline __attribute__((const, pure)) px_t
sign_side(px_t p, clob_side_t s)
{
/* negate ask prices, leave bid prices */
	return s == CLOB_SIDE_ASK ? -p : p;
}

static int
unxs_add(struct _unxs_s *r, unxs_exe_t x, clob_side_t s, const clob_oid_t o[static 1U], const qty_t q[static 1U])
{
	if (r == NULL) {
		return 0;
	}
	if (UNLIKELY(r->n >= r->z)) {
		/* resize him */
		const size_t nuz = r->z * 2U;
		void *tmpx = realloc(r->x, nuz * sizeof(*r->x));
		void *tmps = realloc(r->s, nuz * sizeof(*r->s));
		void *tmpo = realloc(r->o, nuz * r->m * sizeof(*r->o));
		void *tmpq = realloc(r->q, nuz * r->m * sizeof(*r->q));

		if (UNLIKELY(tmpx == NULL || tmpo == NULL || tmpq == NULL || tmps == NULL)) {
			return -1;
		}
		/* otherwise assign */
		r->x = tmpx;
		r->s = tmps;
		r->o = tmpo;
		r->q = tmpq;
		r->z = nuz;
	}

	r->s[r->n] = (uint_fast8_t)s;
	r->o[r->m * r->n] = o[0U];
	r->q[r->m * r->n] = q[0U];
	if (r->m > 1U) {
		r->o[r->m * r->n + 1U] = o[1U];
		r->q[r->m * r->n + 1U] = q[1U];
	}
	r->x[r->n++] = x;
	return 0;
}

static void
_unxs_auction(unxs_t x, plqu_t q, px_t ref, qx_t max, clob_oid_t proto)
{
	struct _unxs_s *_x = (struct _unxs_s*)x;
	plqu_iter_t i = {.q = q};
	qx_t F = 0.dd;
	qx_t fil;

	for (; plqu_iter_next(&i) && F < max; F += fil) {
		proto.qid = plqu_iter_qid(i);
		proto.usr = i.v.usr;
		fil = qty(i.v.qty);

		if (UNLIKELY(F + fil > max)) {
			/* partial fill the last guy,
			 * we're checking F < max in the for-check first
			 * so we can just finish the loop body as planned */
			fil = max - F;
			i.v.qty = qty_exe(i.v.qty, fil);
			plqu_iter_put(i, i.v);
			/* fill him and out */
			unxs_add(_x, (unxs_exe_t){ref, fil}, proto.sid, &proto, &i.v.qty);
			break;
		}
		/* otherwise fill him fully */
		unxs_add(_x, (unxs_exe_t){ref, fil}, proto.sid, &proto, &qty0);
	}
	plqu_iter_set_top(i);
	return;
}

static qty_t
_unxs_order(unxs_t x, clob_ord_t *restrict o, plqu_t q, px_t r, clob_oid_t *ids)
{
/* return the quantity taken off of Q */
	struct _unxs_s *_x = (struct _unxs_s*)x;
	plqu_iter_t qi = {.q = q};
	qx_t oq = qty(o->qty);
	qty_t s = qty0;
	/* calc maker side */
	const clob_side_t ms = clob_contra_side(o->sid);

	for (; plqu_iter_next(&qi) && oq > 0.dd; oq = qty(o->qty)) {
		qty_t remn[2U];
		qx_t mq = qty(qi.v.qty);

		ids[CLOB_SIDE_MAKER].qid = plqu_iter_qid(qi);
		ids[CLOB_SIDE_MAKER].usr = qi.v.usr;
		if (mq <= oq) {
			/* full maker ~ partial taker */
			o->qty = remn[CLOB_SIDE_TAKER] = qty_exe(o->qty, mq);
			remn[CLOB_SIDE_MAKER] = qty0;
			unxs_add(_x, (unxs_exe_t){r, mq}, ms, ids, remn);
			/* add maker quantity to result */
			s = qty_add(s, qi.v.qty);
		} else {
			/* partial maker ~ full taker */
			qty_t prvq = qi.v.qty;

			qi.v.qty = remn[CLOB_SIDE_MAKER] = qty_exe(prvq, oq);
			remn[CLOB_SIDE_TAKER] = qty0;
			unxs_add(_x, (unxs_exe_t){r, oq}, ms, ids, remn);
			/* add residual order quantity to result */
			s = qty_add(s, qty_sub(prvq, qi.v.qty));
			plqu_iter_put(qi, qi.v);
			/* let everyone know there's nothing left */
			o->qty = qty0;
			break;
		}
	}
	plqu_iter_set_top(qi);
	return s;
}


unxs_t
make_unxs(unxs_mode_t m)
{
	struct _unxs_s *r = malloc(sizeof(*r));
	r->m = m;
	r->n = 0U;
	r->x = malloc(UNXS_INIZ * sizeof(*r->x));
	r->s = malloc(UNXS_INIZ * sizeof(*r->s));
	r->o = malloc(UNXS_INIZ * m * sizeof(*r->o));
	r->q = malloc(UNXS_INIZ * m * sizeof(*r->q));
	r->z = UNXS_INIZ;
	return (unxs_t)r;
}

void
free_unxs(unxs_t x)
{
	struct _unxs_s *_x = (struct _unxs_s*)x;
	if (LIKELY(_x->x != NULL)) {
		free(_x->x);
	}
	if (LIKELY(_x->s != NULL)) {
		free(_x->s);
	}
	if (LIKELY(_x->o != NULL)) {
		free(_x->o);
	}
	if (LIKELY(_x->q != NULL)) {
		free(_x->q);
	}
	free(_x);
	return;
}

int
unxs_clr(unxs_t x)
{
	struct _unxs_s *_x = (struct _unxs_s*)x;
	_x->n = 0U;
	return 0;
}

void
unxs_auction(clob_t c, px_t p, qx_t q)
{
	clob_oid_t proto = {};
	qx_t Q;

	if (UNLIKELY(isnanpx(p))) {
		/* no price? */
		return;
	} else if (UNLIKELY(q <= 0.dd)) {
		/* no volume? */
		return;
	}

	Q = 0.dd;
	proto = (clob_oid_t){CLOB_TYPE_MKT, CLOB_SIDE_SHORT, .prc = NANPX};
	/* market orders have the highest priority */
	with (plqu_t pq = c.mkt[proto.sid]) {
		qx_t before = qty(plqu_qty(pq));
		qx_t after;

		/* set the executor free */
		_unxs_auction(c.exe, pq, p, q - Q, proto);
		/* what's left? */
		after = qty(plqu_qty(pq));
		/* up our Q */
		Q += before - after;
	}
	/* and now limit orders */
	proto = (clob_oid_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK};
	for (btree_iter_t i = {.t = (btree_t)c.lmt[proto.sid]};
	     Q < q && btree_iter_next(&i) && (proto.prc = i.k) <= p;) {
		qx_t before = qty(i.v->sum);
		qx_t after;

		/* set the executor free */
		_unxs_auction(c.exe, i.v->q, p, q - Q, proto);
		/* maintain lmt sum */
		with (qty_t sum = plqu_qty(i.v->q)) {
			if (c.quo != NULL) {
				/* publish the change */
				quos_add(c.quo,
					 (quos_msg_t){CLOB_SIDE_ASK, i.k, sum.dis});
			}
			if ((after = qty(i.v->sum = sum)) <= 0.dd) {
				btree_val_t v = btree_rem((btree_t)c.lmt[proto.sid], i.k);
				free_plqu(v.q);
			}
		}
		/* also up our Q */
		Q += before - after;
	}

	Q = 0.dd;
	proto = (clob_oid_t){CLOB_TYPE_MKT, CLOB_SIDE_LONG, .prc = NANPX};
	/* market orders have the highest priority */
	with (plqu_t pq = c.mkt[proto.sid]) {
		qx_t before = qty(plqu_qty(pq));
		qx_t after;

		/* set the executor free */
		_unxs_auction(c.exe, pq, p, q - Q, proto);
		/* what's left? */
		after = qty(plqu_qty(pq));
		/* up the Q */
		Q += before - after;
	}
	/* and limit orders again */
	proto = (clob_oid_t){CLOB_TYPE_LMT, CLOB_SIDE_BID};
	for (btree_iter_t i = {.t = (btree_t)c.lmt[proto.sid]};
	     Q < q && btree_iter_next(&i) && (proto.prc = i.k) >= p;) {
		qx_t before = qty(i.v->sum);
		qx_t after;

		/* set the executor free */
		_unxs_auction(c.exe, i.v->q, p, q - Q, proto);
		/* maintain lmt sum */
		with (qty_t sum = plqu_qty(i.v->q)) {
			if (c.quo != NULL) {
				/* publish the change */
				quos_add(c.quo,
					 (quos_msg_t){CLOB_SIDE_BID, i.k, sum.dis});
			}
			if ((after = qty(i.v->sum = sum)) <= 0.dd) {
				btree_val_t v = btree_rem((btree_t)c.lmt[proto.sid], i.k);
				free_plqu(v.q);
			}
		}
		/* also up our Q */
		Q += before - after;
	}
	return;
}

clob_ord_t
unxs_order(clob_t c, clob_ord_t o, px_t r)
{
	const clob_side_t contra = clob_contra_side(o.sid);
	clob_oid_t oids[] = {
		[CLOB_SIDE_MAKER] = {.sid = contra, .prc = NANPX},
		[CLOB_SIDE_TAKER] = {o.typ, o.sid, .prc = o.lmt, .usr = o.usr},
	};

	switch (o.typ) {
		btree_iter_t ti;
		bool lmtp;

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
		} else if (UNLIKELY(isnanpx(r))) {
			/* can't execute against NAN reference price */
			r = o.lmt;
		} else if (o.sid == CLOB_SIDE_ASK && r < o.lmt) {
			/* we need an improvement over R */
			goto rest;
		} else if (o.sid == CLOB_SIDE_BID && r > o.lmt) {
			/* not an improvement */
			goto rest;
		}
		goto marketable;

	case CLOB_TYPE_MKT:
		/* execute against contra market first, then contra limit */
		ti = (btree_iter_t){.t = (btree_t)c.lmt[contra]};
		if (LIKELY((lmtp = btree_iter_next(&ti)))) {
			/* aaah, reference price we need not */
			r = ti.k;
		} else if (UNLIKELY(isnanpx(r))) {
			/* can't execute against NAN reference price */
			goto rest;
		}
		/* turn slippage into absolute limit price
		 * only if slippage is a normal */
		o.lmt = r + sign_side(o.slp, o.sid);
		goto marketable;

	marketable:
		/* get markets ready */
		oids[CLOB_SIDE_MAKER].typ = CLOB_TYPE_MKT;
		(void)_unxs_order(c.exe, &o, c.mkt[contra], r, oids);
	more:
		if (qty(o.qty) <= 0.dd) {
			/* filled */
			break;
		} else if (!lmtp) {
			goto rest;
		} else if (o.sid == CLOB_SIDE_ASK && ti.k < o.lmt) {
			/* we need an improvement over R */
			goto rest;
		} else if (o.sid == CLOB_SIDE_BID && ti.k > o.lmt) {
			/* not an improvement */
			goto rest;
		}
		/* otherwise dive into limits */
		oids[CLOB_SIDE_MAKER].typ = CLOB_TYPE_LMT;
		oids[CLOB_SIDE_MAKER].prc = ti.k;
		with (qty_t sum = _unxs_order(c.exe, &o, ti.v->q, ti.k, oids)) {
			/* maintain the sum */
			sum = ti.v->sum = qty_sub(ti.v->sum, sum);

			if (c.quo != NULL) {
				quos_add(c.quo,
					 (quos_msg_t){contra, ti.k, sum.dis});
			}
			if (qty(ti.v->sum) <= 0.dd) {
				btree_val_t v = btree_rem((btree_t)c.lmt[contra], ti.k);
				free_plqu(v.q);
				lmtp = btree_iter_next(&ti);
			}
		}
		goto more;
	}
rest:
	/* remainder goes back to requester */
	return o;
}

/* unxs.c ends here */
