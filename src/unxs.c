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
#include "btree.h"
#include "btree_val.h"
#include "plqu.h"
#include "plqu_val.h"
#include "clob.h"
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

static void
_unx_mkt_mkt(plqu_t m1, plqu_t m2, px_t ref)
{
	plqu_iter_t m1i = {.q = m1};
	plqu_iter_t m2i = {.q = m2};
	qx_t m1q, m2q;

rwnd:
	if (UNLIKELY(!plqu_iter_next(&m1i))) {
		puts("NO PLQU1");
		goto out;
	} else if (UNLIKELY(!plqu_iter_next(&m2i))) {
		puts("NO PLQU2");
		goto out;
	}

redo:
	m1q = m1i.v.vis + m1i.v.hid;
	m2q = m2i.v.vis + m2i.v.hid;
	if (m1q < m2q) {
		/* fill market order fully */
		printf("FULL %f v PART %f  @%f\n", (double)m1q, (double)m2q, (double)ref);
		plqu_iter_put(m1i, plqu_val_nil);
		plqu_iter_put(m2i, m2i.v = plqu_val_exe(m2i.v, m1i.v));
		if (plqu_iter_next(&m1i)) {
			goto redo;
		}
	} else if (m1q > m2q) {
		/* fill limit order fully */
		printf("PART %f v FULL %f  @%f\n", (double)m1q, (double)m2q, (double)ref);
		plqu_iter_put(m1i, m1i.v = plqu_val_exe(m1i.v, m2i.v));
		plqu_iter_put(m2i, plqu_val_nil);
		if (plqu_iter_next(&m2i)) {
			goto redo;
		}
	} else {
		/* fill both fully, yay */
		printf("FULL %f v FULL %f  @%f\n", (double)m1q, (double)m2q, (double)ref);
		plqu_iter_put(m1i, plqu_val_nil);
		plqu_iter_put(m2i, plqu_val_nil);
		goto rwnd;
	}
out:
	plqu_iter_set_top(m1i);
	plqu_iter_set_top(m2i);
	return;
}

static void
_unx_mkt_lmt(plqu_t mkt, btree_t lmt)
{
	btree_iter_t lmti;

redo:
	lmti = (btree_iter_t){.t = lmt};
	if (UNLIKELY(!btree_iter_next(&lmti))) {
		puts("NO BTREE");
		return;
	}
	/* use MKTvMKT with reference price set to the limit price */
	_unx_mkt_mkt(mkt, lmti.v->q, lmti.k);
	/* maintain lmt sum */
	with (plqu_val_t sum = plqu_sum(lmti.v->q)) {
		if (plqu_val_nil_p(lmti.v->sum = sum)) {
			btree_val_t v = btree_rem(lmt, lmti.k);
			free_plqu(v.q);

			if (!plqu_val_nil_p(plqu_sum(mkt))) {
				/* there's more */
				goto redo;
			}
		}
	}
	return;
}


void
clob_unx_mkt_lmt(clob_t c)
{
	_unx_mkt_lmt(c.mkt[SIDE_ASK], c.lmt[SIDE_BID]);
	_unx_mkt_lmt(c.mkt[SIDE_BID], c.lmt[SIDE_ASK]);
	return;
}

void
clob_unx_lmt_lmt(clob_t c)
{
	return;
}

void
clob_unx_mkt_mid(clob_t c)
{
	px_t mid = clob_mid(c);
	_unx_mkt_mkt(c.mkt[SIDE_ASK], c.mid[SIDE_BID], mid);
	_unx_mkt_mkt(c.mkt[SIDE_BID], c.mid[SIDE_ASK], mid);
	return;
}

void
clob_unx_mid_mid(clob_t c)
{
	px_t mid = clob_mid(c);
	_unx_mkt_mkt(c.mid[SIDE_ASK], c.mid[SIDE_BID], mid);
	return;
}

/* unxs ends here */
