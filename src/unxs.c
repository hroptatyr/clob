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
_unxs_plqu_bi(unxs_exbi_t *restrict x, size_t n, plqu_t m1, plqu_t m2, px_t ref)
{
/* match up order from M1 and M2 assuming they're opposite sides */
	plqu_iter_t m1i = {.q = m1};
	plqu_iter_t m2i = {.q = m2};
	qx_t m1q, m2q;
	size_t m = 0U;

rwnd:
	if (UNLIKELY(!plqu_iter_next(&m1i))) {
		/* plqu1 is empty */
		goto out;
	} else if (UNLIKELY(!plqu_iter_next(&m2i))) {
		/* plqu2 is empty */
		goto out;
	}

redo:
	m1q = qty(m1i.v.qty);
	m2q = qty(m2i.v.qty);
	if (m1q < m2q) {
		/* FULL M1 v PART M2 */
		plqu_iter_put(m2i, m2i.v = plqu_val_exe(m2i.v, m1i.v));
		plqu_iter_put(m1i, plqu_val_nil);
		x[m++] = (unxs_exbi_t){{ref, m1q}};
		if (UNLIKELY(m >= n)) {
			/* queue is full, sorry */
			goto out;
		}
		if (plqu_iter_next(&m1i)) {
			goto redo;
		}
	} else if (m1q > m2q) {
		/* PART M1 v FULL M2 */
		plqu_iter_put(m1i, m1i.v = plqu_val_exe(m1i.v, m2i.v));
		plqu_iter_put(m2i, plqu_val_nil);
		x[m++] = (unxs_exbi_t){{ref, m2q}};
		if (UNLIKELY(m >= n)) {
			/* queue is full, sorry */
			goto out;
		}
		if (plqu_iter_next(&m2i)) {
			goto redo;
		}
	} else {
		/* FULL v FULL, how lucky can we get */
		plqu_iter_put(m1i, plqu_val_nil);
		plqu_iter_put(m2i, plqu_val_nil);
		x[m++] = (unxs_exbi_t){{ref, m1q}};
		if (UNLIKELY(m >= n)) {
			/* queue is full, sorry */
			goto out;
		}
		goto rwnd;
	}
out:
	plqu_iter_set_top(m1i);
	plqu_iter_set_top(m2i);
	return m;
}

static size_t
_unxs_plqu_sc(unxs_exsc_t *restrict x, size_t n, plqu_t q, px_t ref, qx_t max)
{
	plqu_iter_t i = {.q = q};
	size_t m = 0U;
	qx_t F = 0.dd;
	qx_t fil;

	for (; plqu_iter_next(&i) && m < n && F < max; m++, F += fil) {
		fil = qty(i.v.qty);

		if (UNLIKELY(F + fil > max)) {
			/* partial fill the last guy,
			 * we're checking F < max in the for-check first
			 * so we can just finish the loop body as planned */
			fil = max - F;
			i.v = plqu_val_exe(i.v, (plqu_val_t){fil, 0.dd});
			plqu_iter_put(i, i.v);
			/* fill him and out */
			x[m++] = (unxs_exsc_t){{ref, fil}};
			break;
		}
		/* otherwise fill him fully */
		x[m] = (unxs_exsc_t){{ref, fil}};
	}
	plqu_iter_set_top(i);
	return m;
}


size_t
unxs_mass_bi(unxs_exbi_t *restrict x, size_t n, clob_t c, px_t p, qx_t q)
{
	btree_iter_t aski;
	btree_iter_t bidi;
	size_t m = 0U;

	if (UNLIKELY(isnandpx(p))) {
		/* no price? */
		return 0U;
	} else if (UNLIKELY(q <= 0.dd)) {
		/* no volume? */
		return 0U;
	}

	/* bilateral mode */
	aski = (btree_iter_t){.t = c.lmt[SIDE_ASK]};
	bidi = (btree_iter_t){.t = c.lmt[SIDE_BID]};

	btree_iter_next(&aski);
	btree_iter_next(&bidi);

	do {
		plqu_t bq = bidi.v->q;
		plqu_t aq = aski.v->q;

		/* let the plqu matcher do his magic */
		m += _unxs_plqu_bi(x + m, n - m, bq, aq, p);
		/* maintain lmt sum */
		with (qty_t sum = plqu_qty(bq)) {
			if (qty(bidi.v->sum = sum) <= 0.dd) {
				btree_t bt = c.lmt[SIDE_BID];
				btree_val_t v = btree_rem(bt, bidi.k);
				free_plqu(v.q);
				btree_iter_next(&bidi);
			}
		}
		with (qty_t sum = plqu_qty(aq)) {
			if (qty(aski.v->sum = sum) <= 0.dd) {
				btree_t at = c.lmt[SIDE_ASK];
				btree_val_t v = btree_rem(at, aski.k);
				free_plqu(v.q);
				btree_iter_next(&aski);
			}
		}
	} while (m < n && bidi.k >= p && aski.k <= p);
	return m;
}

size_t
unxs_mass_sc(unxs_exsc_t *restrict x, size_t n, clob_t c, px_t p, qx_t q)
{
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
	for (btree_iter_t i = {.t = c.lmt[SIDE_ASK]};
	     m < n && Q < q && btree_iter_next(&i) && i.k <= p;) {
		qx_t before = qty(i.v->sum);
		qx_t after;

		/* set the executor free */
		m += _unxs_plqu_sc(x + m, n - m, i.v->q, p, q - Q);
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
	for (btree_iter_t i = {.t = c.lmt[SIDE_BID]};
	     m < n && Q < q && btree_iter_next(&i) && i.k >= p;) {
		qx_t before = qty(i.v->sum);
		qx_t after;

		/* set the executor free */
		m += _unxs_plqu_sc(x + m, n - m, i.v->q, p, q - Q);
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

/* unxs.c ends here */
