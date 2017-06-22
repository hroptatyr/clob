/*** clob.c -- central limit order book
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
#include <dfp754_d64.h>
#include "btree.h"
#include "btree_val.h"
#include "plqu.h"
#include "plqu_val.h"
#include "clob.h"
#include "nifty.h"

#define pxtostr		d64tostr
#define qxtostr		d64tostr


clob_t
make_clob(void)
{
	clob_t r = {
		.lmt[SIDE_ASK] = make_btree(false),
		.lmt[SIDE_BID] = make_btree(true),
		.stp[SIDE_ASK] = make_btree(false),
		.stp[SIDE_BID] = make_btree(true),
		.mid[SIDE_ASK] = make_plqu(),
		.mid[SIDE_BID] = make_plqu(),
		.mkt[SIDE_ASK] = make_plqu(),
		.mkt[SIDE_BID] = make_plqu(),
		.peg[SIDE_ASK] = make_plqu(),
		.peg[SIDE_BID] = make_plqu(),
	};
	return r;
}

void
free_clob(clob_t c)
{
	free_btree(c.lmt[SIDE_ASK]);
	free_btree(c.lmt[SIDE_BID]);
	free_btree(c.stp[SIDE_ASK]);
	free_btree(c.stp[SIDE_BID]);
	free_plqu(c.mid[SIDE_ASK]);
	free_plqu(c.mid[SIDE_BID]);
	free_plqu(c.mkt[SIDE_ASK]);
	free_plqu(c.mkt[SIDE_BID]);
	free_plqu(c.peg[SIDE_ASK]);
	free_plqu(c.peg[SIDE_BID]);
	return;
}

clob_oid_t
clob_add(clob_t c, clob_ord_t o)
{
	static metr_t m;
	plqu_qid_t i;
	px_t p;

	switch (o.typ) {
		btree_t t;
		plqu_t q;

	case TYPE_LMT:
		p = o.lmt;
		t = c.lmt[o.sid];
		goto addv;
	case TYPE_STP:
		p = o.stp;
		t = c.stp[o.sid];
		goto addv;

	case TYPE_MID:
		p = 0;
		q = c.mid[o.sid];
		goto addq;
	case TYPE_MKT:
		p = 0;
		q = c.mkt[o.sid];
		goto addq;
	case TYPE_PEG:
		p = 0;
		q = c.peg[o.sid];
		goto addq;

	addv:
		with (btree_val_t *v = btree_put(t, p)) {
			if (UNLIKELY(btree_val_nil_p(*v))) {
				/* ooh, use one of them plqu's */
				*v = (btree_val_t){make_plqu(), qty0};
			}
			/* maintain the sum */
			v->sum = qty_add(v->sum, (qty_t)o.qty);
			/* set queue variable for plqu stuff */
			q = v->q;
		}
	addq:
		i = plqu_add(q, (plqu_val_t){o.qty, ++m});
		break;
	}
	return (clob_oid_t){o.typ, o.sid, p, .qid = i};
}

int
clob_del(clob_t c, clob_oid_t o)
{
	plqu_t q;

	switch (o.typ) {
		btree_t t;

	case TYPE_LMT:
		t = c.lmt[o.sid];
		goto delt;
	case TYPE_STP:
		t = c.stp[o.sid];
		goto delt;

	delt:
		with (btree_val_t *v = btree_get(t, o.prc)) {
			if (UNLIKELY(btree_val_nil_p(*v))) {
				return -1;
			}
			q = v->q;

			/* maintain the sum */
			with (plqu_val_t w = plqu_get(q, o.qid)) {
				v->sum = qty_sub(v->sum, w.qty);
			}
		}
		break;

	case TYPE_MID:
		q = c.mid[o.sid];
		break;
	case TYPE_MKT:
		q = c.mid[o.sid];
		break;
	case TYPE_PEG:
		q = c.mid[o.sid];
		break;
	}
	return plqu_put(q, o.qid, plqu_val_nil);
}

px_t
clob_mid(clob_t c)
{
	btree_key_t b, a;
	btree_val_t *B, *A;

	A = btree_top(c.lmt[SIDE_ASK], &a);
	B = btree_top(c.lmt[SIDE_BID], &b);

	if (UNLIKELY(A == NULL && B == NULL)) {
		return NANPX;
	} else if (UNLIKELY(A == NULL)) {
		return b;
	} else if (UNLIKELY(B == NULL)) {
		return a;
	}
	return (a + b) / 2.dd;
}


#include <stdio.h>
#include <string.h>

static void
_prnt_btree(btree_t t)
{
	char buf[256U];

	for (btree_iter_t i = {.t = t}; btree_iter_next(&i);) {
		size_t lem, len;

		len = pxtostr(buf, sizeof(buf), i.k);
		buf[len++] = ' ';
		lem = len;
		lem += qxtostr(buf + lem, sizeof(buf) - lem, i.v->sum.dis);
		buf[lem++] = '+';
		lem += qxtostr(buf + lem, sizeof(buf) - lem, i.v->sum.hid);
		buf[lem++] = '\n';
		fwrite(buf, 1, lem, stdout);

		memset(buf, ' ', len);
		for (plqu_iter_t j = {.q = i.v->q}; plqu_iter_next(&j);) {
			lem = len;
			lem += qxtostr(buf + lem, sizeof(buf) - lem, j.v.qty.dis);
			buf[lem++] = '+';
			lem += qxtostr(buf + lem, sizeof(buf) - lem, j.v.qty.hid);
			buf[lem++] = ' ';
			lem += snprintf(buf + lem, sizeof(buf) - lem, "%zu", j.v.tim);
			buf[lem++] = '\n';
			fwrite(buf, 1, lem, stdout);
		}
	}
}

static void
_prnt_plqu(plqu_t q)
{
	char buf[256U];

	for (plqu_iter_t i = {.q = q}; plqu_iter_next(&i);) {
		size_t lem = 0U;
		lem += qxtostr(buf + lem, sizeof(buf) - lem, i.v.qty.dis);
		buf[lem++] = '+';
		lem += qxtostr(buf + lem, sizeof(buf) - lem, i.v.qty.hid);
		buf[lem++] = ' ';
		lem += snprintf(buf + lem, sizeof(buf) - lem, "%zu", i.v.tim);
		buf[lem++] = '\n';
		fwrite(buf, 1, lem, stdout);
	}
	return;
}

void
clob_prnt(clob_t c)
{
	puts("LMT/BID");
	_prnt_btree(c.lmt[SIDE_BID]);
	puts("LMT/ASK");
	_prnt_btree(c.lmt[SIDE_ASK]);
	puts("STP/BID");
	_prnt_btree(c.stp[SIDE_BID]);
	puts("STP/ASK");
	_prnt_btree(c.stp[SIDE_ASK]);
	puts("MKT/BID");
	_prnt_plqu(c.mkt[SIDE_BID]);
	puts("MKT/ASK");
	_prnt_plqu(c.mkt[SIDE_ASK]);
	puts("MID/BID");
	_prnt_plqu(c.mid[SIDE_BID]);
	puts("MID/ASK");
	_prnt_plqu(c.mid[SIDE_ASK]);
	puts("PEG/BID");
	_prnt_plqu(c.peg[SIDE_BID]);
	puts("PEG/ASK");
	_prnt_plqu(c.peg[SIDE_ASK]);
	return;
}

/* clob.c ends here */
