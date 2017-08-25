/*** mmod-ioc.c -- decide on IOC orders
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
#include "mmod-ioc.h"
#include "nifty.h"


static inline __attribute__((const, pure)) px_t
sign_side(px_t p, clob_side_t s)
{
/* negate ask prices, leave bid prices */
	return s == CLOB_SIDE_ASK ? -p : p;
}


int
mmod_ioc(clob_t c, clob_ord_t o)
{
	const clob_side_t contra = clob_contra_side(o.sid);

	switch (o.typ) {
		btree_iter_t ti;
		plqu_iter_t qi;
		bool lmtp;

	case CLOB_TYPE_LMT:
		qi = (plqu_iter_t){.q = c.mkt[contra]};
		if (plqu_iter_next(&qi)) {
			/* anything goes against a market order */
			break;
		}
		/* otherwise prep for some serious limit checking */
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
		if (!lmtp) {
			goto kill;
		} else if (o.sid == CLOB_SIDE_ASK && ti.k < o.lmt) {
			/* we need an improvement over R */
			goto kill;
		} else if (o.sid == CLOB_SIDE_BID && ti.k > o.lmt) {
			/* not an improvement */
			goto kill;
		}
		/* otherwise there would be at least some sort of fill */
		break;
	}
	/* filled */
	return 1;

kill:
	/* killed */
	return 0;
}

/* mmod-ioc.c ends here */
