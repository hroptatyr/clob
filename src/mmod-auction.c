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


static plqu_val_t
plqu_sum(plqu_t q)
{
	plqu_val_t sum = plqu_val_nil;
	for (plqu_iter_t i = {.q = q}; plqu_iter_next(&i);) {
		sum = plqu_val_add(sum, i.v);
	}
	return sum;
}

static void
_init(void)
{
	besq = 0.dd;
	bimb = INFD64;
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
		qx_t aimb = fabsd64(imb);
		qx_t abimb = fabsd64(bimb);

		if (aimb < abimb) {
			goto bang;
		} else if (aimb == abimb) {
			if (imb == 0.dd || -imb == bimb) {
				/* add up for meaning */
				besp += p;
				besq = tmp;
				bimb = 0;
				besn += 1;
			} else if (imb > 0.dd) {
				goto bang;
			}
		}
	}
	return;
}


mmod_auc_t
mmod_auction(clob_t c)
{
	btree_iter_t aski;
	btree_iter_t bidi;
	px_t ask, bid;
	qx_t asz, bsz;

	aski = (btree_iter_t){.t = c.lmt[SIDE_ASK]};
	if (UNLIKELY(!btree_iter_next(&aski))) {
		/* no asks */
		return (mmod_auc_t){NANPX};
	}
	bidi = (btree_iter_t){.t = c.lmt[SIDE_BID]};
	if (UNLIKELY(!btree_iter_next(&bidi))) {
		/* no bids */
		return (mmod_auc_t){NANPX};
	}
	/* track overlap region */
	ask = aski.k;
	bid = bidi.k;

	asz = plqu_val_tot(plqu_sum(c.mkt[SIDE_SHRT]));
	bsz = plqu_val_tot(plqu_sum(c.mkt[SIDE_LONG]));

	/* see if there's an overlap */
	if (LIKELY(ask > bid && !(asz > 0.dd && bsz > 0.dd))) {
		/* no overlap */
		return (mmod_auc_t){NANPX};
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

	_init();

	do {
		for (; ai > 0U && aski.k > bids[ai]; ai--) {
			_push(bids[ai], bszs[ai], asz);
		}

		asz += plqu_val_tot(aski.v->sum);

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
