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
#include "plqu.h"
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
	plqu_t q;
	plqu_qid_t i;

	switch (o.typ) {
		btree_val_t *v;

	case TYPE_LMT:
		v = btree_put(c.lmt[o.sid], o.lmt);
		if (UNLIKELY(*v == NULL)) {
			/* ooh, use one of them plqu's */
			*v = make_plqu();
		}
		q = *v;
		break;
	case TYPE_STP:
		v = btree_put(c.stp[o.sid], o.stp);
		if (UNLIKELY(*v == NULL)) {
			/* ooh, use one of them plqu's */
			*v = make_plqu();
		}
		q = *v;
		break;
	case TYPE_MID:
		o.lmt = 0;
		q = c.mid[o.sid];
		break;
	case TYPE_MKT:
		o.lmt = 0;
		q = c.mkt[o.sid];
		break;
	case TYPE_PEG:
		o.lmt = 0;
		q = c.peg[o.sid];
		break;
	}
	i = plqu_add(q, (plqu_val_t){o.vis, o.hid, ++m});
	return (clob_oid_t){o.typ, o.sid, o.lmt, .qid = i};
}

int
clob_del(clob_t c, clob_oid_t o)
{
	plqu_t q;

	switch (o.typ) {
		btree_val_t *v;

	case TYPE_LMT:
		v = btree_get(c.lmt[o.sid], o.prc);
		if (UNLIKELY(*v == NULL)) {
			return -1;
		}
		q = *v;
		break;
	case TYPE_MID:
		q = c.mid[o.sid];
		break;
	}
	return plqu_put(q, o.qid, plqu_val_0);
}

px_t
clob_mid(clob_t c)
{
	btree_key_t b, a;
	(void)btree_top(c.lmt[SIDE_ASK], &a);
	(void)btree_top(c.lmt[SIDE_BID], &b);
	return (a + b) / 2.dd;
}

clob_quo_t
clob_top(clob_t c, clob_type_t typ, clob_side_t sid)
{
	plqu_t q;
	px_t p;
	plqu_val_t r;

	switch (typ) {
		btree_key_t k;
		btree_val_t *v;

	case TYPE_LMT:
		v = btree_top(c.lmt[sid], &k);
		if (UNLIKELY(*v == NULL)) {
			return (clob_quo_t){NANPX, NANQX};
		}
		p = k;
		q = *v;
		break;
	case TYPE_MID:
		p = 0;
		q = c.mid[sid];
		break;
	}
	r = plqu_sum(q);
	return (clob_quo_t){p, r.vis};
}


#include <stdio.h>
void
clob_prnt(clob_t c)
{
	char buf[256];
	size_t len;

	puts("LMT/BID");
	for (btree_iter_t i = {.t = c.lmt[SIDE_BID]}; btree_iter_next(&i);) {
		len = pxtostr(buf, sizeof(buf), i.k);
		buf[len++] = ' ';
		for (plqu_iter_t j = {.q = i.v}; plqu_iter_next(&j);) {
			size_t lem = len;

			lem += qxtostr(buf + lem, sizeof(buf) - lem, j.v.vis);
			buf[lem++] = '+';
			lem += qxtostr(buf + lem, sizeof(buf) - lem, j.v.hid);
			buf[lem++] = ' ';
			lem += snprintf(buf + lem, sizeof(buf) - lem, "%zu", j.v.tim);
			buf[lem++] = '\n';
			fwrite(buf, 1, lem, stdout);
		}
	}
	puts("LMT/ASK");
	for (btree_iter_t i = {.t = c.lmt[SIDE_ASK]}; btree_iter_next(&i);) {
		len = pxtostr(buf, sizeof(buf), i.k);
		buf[len++] = ' ';
		for (plqu_iter_t j = {.q = i.v}; plqu_iter_next(&j);) {
			size_t lem = len;

			lem += qxtostr(buf + lem, sizeof(buf) - lem, j.v.vis);
			buf[lem++] = '+';
			lem += qxtostr(buf + lem, sizeof(buf) - lem, j.v.hid);
			buf[lem++] = ' ';
			lem += snprintf(buf + lem, sizeof(buf) - lem, "%zu", j.v.tim);
			buf[lem++] = '\n';
			fwrite(buf, 1, lem, stdout);
		}
	}
	puts("STP/BID");
	for (btree_iter_t i = {.t = c.stp[SIDE_BID]}; btree_iter_next(&i);) {
		len = pxtostr(buf, sizeof(buf), i.k);
		buf[len++] = ' ';
		for (plqu_iter_t j = {.q = i.v}; plqu_iter_next(&j);) {
			size_t lem = len;

			lem += qxtostr(buf + lem, sizeof(buf) - lem, j.v.vis);
			buf[lem++] = '+';
			lem += qxtostr(buf + lem, sizeof(buf) - lem, j.v.hid);
			buf[lem++] = ' ';
			lem += snprintf(buf + lem, sizeof(buf) - lem, "%zu", j.v.tim);
			buf[lem++] = '\n';
			fwrite(buf, 1, lem, stdout);
		}
	}
	puts("STP/ASK");
	for (btree_iter_t i = {.t = c.stp[SIDE_ASK]}; btree_iter_next(&i);) {
		len = pxtostr(buf, sizeof(buf), i.k);
		buf[len++] = ' ';
		for (plqu_iter_t j = {.q = i.v}; plqu_iter_next(&j);) {
			size_t lem = len;

			lem += qxtostr(buf + lem, sizeof(buf) - lem, j.v.vis);
			buf[lem++] = '+';
			lem += qxtostr(buf + lem, sizeof(buf) - lem, j.v.hid);
			buf[lem++] = ' ';
			lem += snprintf(buf + lem, sizeof(buf) - lem, "%zu", j.v.tim);
			buf[lem++] = '\n';
			fwrite(buf, 1, lem, stdout);
		}
	}
	puts("MKT/BID");
	for (plqu_iter_t i = {.q = c.mkt[SIDE_BID]}; plqu_iter_next(&i);) {
		size_t lem = 0U;
		lem += qxtostr(buf + lem, sizeof(buf) - lem, i.v.vis);
		buf[lem++] = '+';
		lem += qxtostr(buf + lem, sizeof(buf) - lem, i.v.hid);
		buf[lem++] = ' ';
		lem += snprintf(buf + lem, sizeof(buf) - lem, "%zu", i.v.tim);
		buf[lem++] = '\n';
		fwrite(buf, 1, lem, stdout);
	}
	puts("MKT/ASK");
	for (plqu_iter_t i = {.q = c.mkt[SIDE_ASK]}; plqu_iter_next(&i);) {
		size_t lem = 0U;
		lem += qxtostr(buf + lem, sizeof(buf) - lem, i.v.vis);
		buf[lem++] = '+';
		lem += qxtostr(buf + lem, sizeof(buf) - lem, i.v.hid);
		buf[lem++] = ' ';
		lem += snprintf(buf + lem, sizeof(buf) - lem, "%zu", i.v.tim);
		buf[lem++] = '\n';
		fwrite(buf, 1, lem, stdout);
	}
	puts("MID/BID");
	for (plqu_iter_t i = {.q = c.mid[SIDE_BID]}; plqu_iter_next(&i);) {
		size_t lem = 0U;
		lem += qxtostr(buf + lem, sizeof(buf) - lem, i.v.vis);
		buf[lem++] = '+';
		lem += qxtostr(buf + lem, sizeof(buf) - lem, i.v.hid);
		buf[lem++] = ' ';
		lem += snprintf(buf + lem, sizeof(buf) - lem, "%zu", i.v.tim);
		buf[lem++] = '\n';
		fwrite(buf, 1, lem, stdout);
	}
	puts("MID/ASK");
	for (plqu_iter_t i = {.q = c.mid[SIDE_ASK]}; plqu_iter_next(&i);) {
		size_t lem = 0U;
		lem += qxtostr(buf + lem, sizeof(buf) - lem, i.v.vis);
		buf[lem++] = '+';
		lem += qxtostr(buf + lem, sizeof(buf) - lem, i.v.hid);
		buf[lem++] = ' ';
		lem += snprintf(buf + lem, sizeof(buf) - lem, "%zu", i.v.tim);
		buf[lem++] = '\n';
		fwrite(buf, 1, lem, stdout);
	}
	return;
}

/* clob.c ends here */
