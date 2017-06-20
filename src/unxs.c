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


static plqu_val_t
plqu_sum(plqu_t q)
{
	plqu_val_t sum = plqu_val_nil;
	for (plqu_iter_t i = {.q = q}; plqu_iter_next(&i);) {
		sum = plqu_val_add(sum, i.v);
	}
	return sum;
}

static size_t
_unxs_plqu2(unxs_exe_t *restrict x, size_t n, plqu_t m1, plqu_t m2, px_t ref)
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
	m1q = plqu_val_tot(m1i.v);
	m2q = plqu_val_tot(m2i.v);
	if (m1q < m2q) {
		/* FULL M1 v PART M2 */
		plqu_iter_put(m2i, m2i.v = plqu_val_exe(m2i.v, m1i.v));
		plqu_iter_put(m1i, plqu_val_nil);
		x[m++] = (unxs_exe_t){ref, m1q};
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
		x[m++] = (unxs_exe_t){ref, m2q};
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
		x[m++] = (unxs_exe_t){ref, m1q};
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

/* auction helper */
static px_t besp;
static px_t besn;
static qx_t besq;
static qx_t bhng;

static void
aucp_init(void)
{
	besq = 0.dd;
	bhng = INFD64;
	besn = 1.dd;
}

static void
aucp_push(px_t p, qx_t b, qx_t a)
{
	qx_t tmp = min(b, a);
	qx_t hng = b - a;

	if (tmp > besq) {
	bang:
		besp = p;
		besq = tmp;
		bhng = hng;
		besn = 1.dd;
	} else if (tmp == besq) {
		/* see who's got less hang */
		qx_t ahng = fabsd64(hng);
		qx_t abhng = fabsd64(bhng);

		if (ahng < abhng) {
			goto bang;
		} else if (ahng == abhng) {
			/* prefer new one if hng < 0 */
			if (hng > 0.dd) {
				goto bang;
			} else if (hng == 0.dd) {
				/* add up for meaning */
				besp += p;
				besq = tmp;
				bhng = hng;
				besn += 1.dd;
			}
		}
	}
	return;
}

static px_t
_unxs_auction_prc(clob_t c)
{
	static px_t *bids;
	static qx_t *bszs;
	static size_t bidz;
	/* calc  */
	btree_iter_t aski;
	btree_iter_t bidi;
	px_t ask, bid;
	qx_t asz, bsz;

	aski = (btree_iter_t){.t = c.lmt[SIDE_ASK]};
	if (UNLIKELY(!btree_iter_next(&aski))) {
		/* no asks */
		return NANPX;
	}
	bidi = (btree_iter_t){.t = c.lmt[SIDE_BID]};
	if (UNLIKELY(!btree_iter_next(&bidi))) {
		/* no bids */
		return NANPX;
	}
	/* track overlap region */
	ask = aski.k;
	bid = bidi.k;

	asz = plqu_val_tot(plqu_sum(c.mkt[SIDE_SHRT]));
	bsz = plqu_val_tot(plqu_sum(c.mkt[SIDE_LONG]));

	/* see if there's an overlap */
	if (LIKELY(ask > bid && !(asz > 0.dd && bsz > 0.dd))) {
		/* no overlap */
		return NANPX;
	}

	/* determine cum vol and keep track of levels */
	size_t bi = 0U;

	do {
		if (UNLIKELY(++bi >= bidz)) {
			/* running out of bid space, resize */
			size_t nuz = (bidz * 2U) ?: 256U;
			bids = realloc(bids, nuz * sizeof(*bids));
			bszs = realloc(bszs, nuz * sizeof(*bszs));
			bidz = nuz;
		}
		bids[bi] = bidi.k;
		bsz += plqu_val_tot(bidi.v->sum);
		bszs[bi] = bsz;
	} while (btree_iter_next(&bidi) && bidi.k >= ask);
	/* set very first element */
	bids[0U] = bids[1U];
	bszs[0U] = bszs[1U];

	/* now cumulate asks and calculate execution */
	size_t ai = bi;

	aucp_init();

	do {
		for (; ai > 0U && aski.k > bids[ai]; ai--) {
			aucp_push(bids[ai], bszs[ai], asz);
		}

		asz += plqu_val_tot(aski.v->sum);

		aucp_push(aski.k, bszs[ai], asz);
	} while (btree_iter_next(&aski) && aski.k <= bid);

	if (ai) {
		do {
			aucp_push(bids[ai], bszs[ai], asz);
		} while (--ai && aski.k > bids[ai]);
	}

	besp = quantizepx(besp / besn, besp);
	printf("ALL @ %f %f %f\n", (double)besp, (double)besq, (double)bhng);
	return besp;
}


size_t
unxs_auction(unxs_exe_t *restrict x, size_t n, clob_t c)
{
	btree_iter_t aski;
	btree_iter_t bidi;
	px_t aucp;
	size_t m = 0U;

	if (isnandpx(aucp = _unxs_auction_prc(c))) {
		puts("NO AUCTION PRICE");
		return 0U;
	}

	aski = (btree_iter_t){.t = c.lmt[SIDE_ASK]};
	bidi = (btree_iter_t){.t = c.lmt[SIDE_BID]};

	btree_iter_next(&aski);
	btree_iter_next(&bidi);

	do {
		plqu_t bq = bidi.v->q;
		plqu_t aq = aski.v->q;

		/* let the plqu matcher do his magic */
		m += _unxs_plqu2(x + m, n - m, bq, aq, aucp);
		/* maintain lmt sum */
		with (plqu_val_t sum = plqu_sum(bq)) {
			if (plqu_val_nil_p(bidi.v->sum = sum)) {
				btree_t bt = c.lmt[SIDE_BID];
				btree_val_t v = btree_rem(bt, bidi.k);
				free_plqu(v.q);
				btree_iter_next(&bidi);
			}
		}
		with (plqu_val_t sum = plqu_sum(aq)) {
			if (plqu_val_nil_p(aski.v->sum = sum)) {
				btree_t at = c.lmt[SIDE_ASK];
				btree_val_t v = btree_rem(at, aski.k);
				free_plqu(v.q);
				btree_iter_next(&aski);
			}
		}
	} while (m < n && bidi.k >= aucp && aski.k <= aucp);
	return m;
}

/* unxs.c ends here */
