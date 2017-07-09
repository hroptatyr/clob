/*** mmod-auction.c -- auction market model
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
#include <dfp754_d64.h>
#include "btree.h"
#include "btree_val.h"
#include "plqu.h"
#include "plqu_val.h"
#include "clob.h"
#include "clob_val.h"
#include "mmod-auction.h"
#include "nifty.h"

/* auction helper */
static px_t besp;
static px_t besn;
static qx_t besq;
static qx_t bimb;

/* temporary store of bids to do traversals in the opposite direction */
static px_t *bids;
static qx_t *bszs;
static size_t bidz;


static qx_t
plqu_qx(plqu_t q)
{
	qty_t sum = qty0;
	for (plqu_iter_t i = {.q = q}; plqu_iter_next(&i);) {
		sum = qty_add(sum, i.v.qty);
	}
	return qty(sum);
}

static void
_init(void)
{
	besq = 0.dd;
	bimb = MAXQX;
	besn = 1.dd;
}

static void
_push(px_t p, qx_t b, qx_t a)
{
	qx_t tmp = min(b, a);
	qx_t imb = b - a;

	if (tmp > besq) {
	bang:
		besp = p;
		besq = tmp;
		bimb = imb;
		besn = 1.dd;
	} else if (tmp == besq) {
		/* see who's got less hang */
		qx_t aimb = fabsqx(imb);
		qx_t abimb = fabsqx(bimb);

		if (aimb < abimb) {
			goto bang;
		} else if (aimb == abimb) {
			if (imb == 0.dd || -imb == bimb) {
				/* add up for meaning */
				besp += p;
				besq = tmp;
				bimb = 0.dd;
				besn += 1.dd;
			} else if (imb > 0.dd) {
				goto bang;
			}
		}
	}
	return;
}

static mmod_auc_t
_mmod_unwind(btree_iter_t i, clob_side_t is, qx_t m)
{
/* helper routine for markets with limit orders on only one side
 * unwind orders of at most M quantities from I
 * this is the obvious unwind operation as would be done in
 * continuous trading, however we won't touch the book
 * the iterator is assumed to have stepped already
 * IS is the side of I. */
	qx_t iq = 0.dd;
	px_t ik;

	do {
		/* keep track of auction price because we mustn't
		 * access the iterator after he's run out of steam
		 * (and invalidates its key slot along the way) */
		ik = i.k;
		iq += qty(i.v->sum);
	} while (iq < m && btree_iter_next(&i));
	return (mmod_auc_t){ik, min(iq, m), (iq - m) * ((qx_t)(2 * is) - 1.dd)};
}


mmod_auc_t
mmod_auction(clob_t c)
{
/* here we line up the cumsum of bid quantities against the
 * cumsum of ask quantities, like xetra we stop at the edge
 * points of the overlap
 * it would look like this
 *
 *  sum(bq) bq  p  aq  sum(aq)
 *        x  x  M
 *        x  x  P  x   x
 *        x  x  P      x
 *              P  x   x
 *              P      x
 *              M  x   x
 *
 * Obviously because there's no way to traverse the btree
 * backwards we have to temporarily write one of bids and
 * asks into an array and traverse that one backwards
 * with the opposite side traversing forwards.
 *
 * At each point we check for the highest turnover, and
 * if there's no improvement on turnover for the lowest
 * imbalance. */
	btree_iter_t aski, bidi;
	bool askp, bidp;
	px_t ask, bid;
	qx_t asz, bsz;

	aski = (btree_iter_t){.t = c.lmt[SIDE_ASK]};
	bidi = (btree_iter_t){.t = c.lmt[SIDE_BID]};
	askp = btree_iter_next(&aski);
	bidp = btree_iter_next(&bidi);
	if (UNLIKELY(!askp && !bidp)) {
		/* no bids nor asks */
		return (mmod_auc_t){NANPX};
	}

	/* track overlap region */
	ask = aski.k;
	bid = bidi.k;

	asz = plqu_qx(c.mkt[SIDE_SHORT]);
	bsz = plqu_qx(c.mkt[SIDE_LONG]);

	/* see if there's an overlap */
	if (0) {
		;
	} else if (!bidp) {
		/* unwind bid market against asks */
		return _mmod_unwind(aski, SIDE_ASK, bsz);
	} else if (!askp) {
		/* unwind ask market against bids */
		return _mmod_unwind(bidi, SIDE_BID, asz);
	} else if (LIKELY(ask > bid && asz <= 0.dd && bsz <= 0.dd)) {
		/* no overlap */
		return (mmod_auc_t){NANPX};
	}

	/* determine cum vol and keep track of levels */
	size_t bi = 0U;
	qx_t mb = bsz;

	do {
		if (UNLIKELY(++bi >= bidz)) {
			/* running out of bid space, resize */
			size_t nuz = (bidz * 2U) ?: 256U;
			bids = realloc(bids, nuz * sizeof(*bids));
			bszs = realloc(bszs, nuz * sizeof(*bszs));
			bidz = nuz;
		}
		bids[bi] = bidi.k;
		bsz += qty(bidi.v->sum);
		bszs[bi] = bsz;
	} while (btree_iter_next(&bidi) && bidi.k >= ask);
	/* set very first element */
	bids[0U] = bids[1U];
	bszs[0U] = mb;

	/* now cumulate asks and calculate execution */
	size_t ai = bi;

	_init();

	do {
		for (; ai > 0U && aski.k > bids[ai]; ai--) {
			_push(bids[ai], bszs[ai], asz);
		}

		asz += qty(aski.v->sum);

		_push(aski.k, bszs[ai], asz);

		/* prevent double push in the next iteration */
		ai -= ai > 0U && bids[ai] == aski.k;
	} while (btree_iter_next(&aski) && aski.k <= bid);

	if (ai) {
		do {
			_push(bids[ai], bszs[ai], asz);
		} while (--ai && aski.k > bids[ai]);
	}

	besp = quantizepx(besp / besn, besp);
	return (mmod_auc_t){besp, besq, bimb};
}

/* mmod-auction.c ends here */
