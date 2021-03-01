/*** btree.c -- simple b+tree impl
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
#if defined HAVE_DFP754_H
# include <dfp754.h>
#elif defined HAVE_DFP_STDLIB_H
# include <dfp/stdlib.h>
#elif defined HAVE_DECIMAL_H
# include <decimal.h>
#endif	/* DFP754_H || HAVE_DFP_STDLIB_H || HAVE_DECIMAL_H */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "btree.h"
#include "btree_val.h"
#include "dfp754_d64.h"
#include "nifty.h"

typedef union {
	btree_val_t v;
	btree_t t;
} btree_ual_t;

struct btree_s {
	uint32_t n;
	uint32_t innerp:1;
	uint32_t descp:1;
	uint32_t splitp:1;
	btree_key_t key[63U + 1U/*spare*/];
	btree_ual_t val[64U];
	btree_t next;
};


static bool
node_free_p(btree_t t)
{
/* check if btree cell can be pruned
 * and we say a btree cell can be pruned if all of its children can be pruned */
	size_t nul;
	if (!t->innerp) {
		for (nul = 0U; nul < t->n &&
			     btree_val_nil_p(t->val[nul].v); nul++);
		return nul >= t->n;
	}
	/* otherwise recur */
	for (nul = 0U; nul <= t->n && node_free_p(t->val[nul].t); nul++);
	return nul > t->n;
}

static void
root_split(btree_t root)
{
	/* root got split, bollocks */
	const btree_t left = make_btree(root->descp);
	const btree_t rght = make_btree(root->descp);
	const size_t piv = countof(root->key) / 2U - 1U;

	/* T will become the new root so push stuff to LEFT ... */
	memcpy(left->key, root->key, (piv + 1U) * sizeof(*root->key));
	memcpy(left->val, root->val, (piv + 1U) * sizeof(*root->val));
	left->innerp = root->innerp;
	left->n = piv + !root->innerp;
	left->next = rght;
	/* ... and RGHT */
	memcpy(rght->key, root->key + piv + 1U, (piv + 0U) * sizeof(*root->key));
	memcpy(rght->val, root->val + piv + 1U, (piv + 1U) * sizeof(*root->val));
	rght->innerp = root->innerp;
	rght->n = piv;
	rght->next = NULL;
	/* and now massage T */
	root->key[0U] = root->key[piv];
	memset(root->key + 1U, -1, sizeof(root->key) - sizeof(*root->key));
	root->val[0U].t = left;
	root->val[1U].t = rght;
	root->n = 1U;
	root->innerp = 1U;
	root->next = NULL;
	return;
}

static void
node_split(btree_t prnt, size_t idx)
{
/* PaReNT's IDX-th child will be split */
	const btree_t chld = prnt->val[idx].t;
	const size_t piv = countof(chld->key) / 2U - 1U;
	btree_t rght;
	size_t nul;

	/* do a scan to see if we have spare items */
	for (nul = 0U; nul <= prnt->n && !node_free_p(prnt->val[nul].t); nul++);

	if (nul > prnt->n) {
		/* no cell to prune, create one */
		rght = make_btree(chld->descp);
	} else {
		/* hijack the value cell */
		rght = prnt->val[nul].t;
		/* adjust next pointers */
		if (nul) {
			prnt->val[nul - 1U].t->next = rght->next;
		}
	}

	if (nul > idx) {
		/* spare item is far to the right */
		memmove(prnt->key + idx + 1U,
			prnt->key + idx + 0U,
			(nul - idx) * sizeof(*prnt->key));
		memmove(prnt->val + idx + 1U,
			prnt->val + idx + 0U,
			(nul - idx) * sizeof(*prnt->val));
	} else if (nul < idx) {
		/* spare item to the left, good job */
		memmove(prnt->key + nul + 0U,
			prnt->key + nul + 1U,
			(idx - nul) * sizeof(*prnt->key));
		memmove(prnt->val + nul + 0U,
			prnt->val + nul + 1U,
			(idx - nul) * sizeof(*prnt->val));
		/* whole to the left, adjust index */
		idx--;
	}

	/* massage PaReNT */
	prnt->n += nul > prnt->n;
	prnt->key[idx + 0U] = chld->key[piv];
	prnt->val[idx + 1U].t = rght;

	/* then shift things to RGHT */
	memcpy(rght->key, chld->key + piv + 1U, (piv + 0U) * sizeof(*chld->key));
	memcpy(rght->val, chld->val + piv + 1U, (piv + 1U) * sizeof(*chld->val));
	memset(rght->key + piv, -1,
	       (countof(rght->key) - piv) * sizeof(*rght->key));
	rght->innerp = chld->innerp;
	rght->next = chld->next;
	rght->n = piv;
	/* and CHLD (the left one) */
	chld->n = piv + !chld->innerp;
	chld->next = rght;
	memset(chld->key + chld->n, -1,
	       (countof(chld->key) - chld->n) * sizeof(*chld->key));
	return;
}


static btree_val_t*
leaf_get(btree_t t, btree_key_t k)
{
	size_t i;

	switch (t->descp) {
	case 0U:
		for (i = 0U; i < t->n && k > t->key[i]; i++);
		break;
	case 1U:
		for (i = 0U; i < t->n && k < t->key[i]; i++);
		break;
	}

	if (k != t->key[i]) {
		/* key isn't home today */
		return NULL;
	}
	return &t->val[i].v;
}

static btree_val_t*
twig_get(btree_t t, btree_key_t k)
{
	btree_val_t *vp;
	btree_t c;
	size_t i;

	switch (t->descp) {
	case 0U:
		for (i = 0U; i < t->n && k > t->key[i]; i++);
		break;
	case 1U:
		for (i = 0U; i < t->n && k < t->key[i]; i++);
		break;
	}

	/* descent */
	c = t->val[i].t;

	if (!c->innerp) {
		/* oh, we're in the leaves again */
		vp = leaf_get(c, k);
	} else {
		/* got to go deeper, isn't it? */
		vp = twig_get(c, k);
	}
	return vp;
}


static btree_val_t*
leaf_add(btree_t t, btree_key_t k, bool *splitp)
{
	size_t nul;
	size_t i;

	switch (t->descp) {
	case 0U:
		for (i = 0U; i < t->n && k > t->key[i]; i++);
		break;
	case 1U:
		for (i = 0U; i < t->n && k < t->key[i]; i++);
		break;
	}

	if (k == t->key[i]) {
		/* got him */
		goto out;
	}
	/* otherwise do a scan to see if we have spare items */
	for (nul = 0U; nul < t->n && !btree_val_nil_p(t->val[nul].v); nul++);

	if (nul > i) {
		/* spare item is far to the right */
		memmove(t->key + i + 1U,
			t->key + i + 0U,
			(nul - i) * sizeof(*t->key));
		memmove(t->val + i + 1U,
			t->val + i + 0U,
			(nul - i) * sizeof(*t->val));
	} else if (nul < i) {
		/* spare item to the left, good job
		 * go down with the index as the hole will be to our left */
		i--;
		memmove(t->key + nul + 0U,
			t->key + nul + 1U,
			(i - nul) * sizeof(*t->key));
		memmove(t->val + nul + 0U,
			t->val + nul + 1U,
			(i - nul) * sizeof(*t->val));
	}
	t->n += !(nul < t->n);
	t->key[i] = k;
	t->val[i].v = btree_val_nil;
out:
	*splitp = t->n >= countof(t->key) - 1U;
	return &t->val[i].v;
}

static btree_val_t*
twig_add(btree_t t, btree_key_t k, bool *splitp)
{
	btree_val_t *r;
	btree_t c;
	size_t i;

	switch (t->descp) {
	case 0U:
		for (i = 0U; i < t->n && k > t->key[i]; i++);
		break;
	case 1U:
		for (i = 0U; i < t->n && k < t->key[i]; i++);
		break;
	}

	/* descent */
	c = t->val[i].t;

	if (!c->innerp) {
		/* oh, we're in the leaves again */
		r = leaf_add(c, k, splitp);
	} else {
		/* got to go deeper, isn't it? */
		r = twig_add(c, k, splitp);
	}

	if (UNLIKELY(*splitp)) {
		/* C needs splitting, not again */
		node_split(t, i);
	}
	*splitp = t->n >= countof(t->key) - 1U;
	return r;
}


btree_t
make_btree(bool descp)
{
	btree_t r = calloc(1U, sizeof(*r));

	r->descp = descp;
	memset(r->key, -1, sizeof(r->key));
	return r;
}

void
free_btree(btree_t t)
{
	if (t->innerp) {
		/* descend and free */
		for (size_t i = 0U; i <= t->n; i++) {
			/* descend */
			free_btree(t->val[i].t);
		}
	} else {
		/* free values */
		for (size_t i = 0U; i < t->n; i++) {
			if (!btree_val_nil_p(t->val[i].v)) {
				free_btree_val(t->val[i].v);
			}
		}
	}
	free(t);
	return;
}

btree_val_t*
btree_get(btree_t t, btree_key_t k)
{
	btree_val_t *vp;

	if (!t->innerp) {
		vp = leaf_get(t, k);
	} else {
		vp = twig_get(t, k);
	}
	return vp;
}

btree_val_t*
btree_put(btree_t t, btree_key_t k)
{
	btree_val_t *vp;
	bool splitp;

	if (UNLIKELY(t->splitp)) {
		/* root got split, bollocks */
		root_split(t);
	}

	/* check if root has leaves */
	if (!t->innerp) {
		vp = leaf_add(t, k, &splitp);
	} else {
		vp = twig_add(t, k, &splitp);
	}

	t->splitp = splitp;
	return vp;
}

btree_val_t
btree_rem(btree_t t, btree_key_t k)
{
	btree_val_t *vp, w;

	if ((vp = btree_get(t, k)) != NULL) {
		w = *vp;
		*vp = btree_val_nil;
	} else {
		w = btree_val_nil;
	}
	return w;
}

btree_val_t*
btree_top(btree_t t, btree_key_t *k)
{
	/* go down them levels */
	for (; t->innerp; t = t->val->t);
	do {
		for (size_t i = 0U; i < t->n; i++) {
			if (LIKELY(!btree_val_nil_p(t->val[i].v))) {
				/* good one */
				*k = t->key[i];
				return &t->val[i].v;
			}
		}
	} while ((t = t->next));
	return NULL;
}

bool
btree_iter_next(btree_iter_t *iter)
{
	if (UNLIKELY(iter->t == NULL)) {
		goto inv;
	}
	for (; iter->t->innerp; iter->t = iter->t->val->t, iter->i = 0U);
	do {
		for (size_t i = iter->i; i < iter->t->n; i++) {
			if (LIKELY(!btree_val_nil_p(iter->t->val[i].v))) {
				/* good one */
				iter->k = iter->t->key[i];
				iter->v = &iter->t->val[i].v;
				iter->i = i + 1U;
				return true;
			}
		}
		/* reset index */
		iter->i = 0U;
	} while ((iter->t = iter->t->next));
inv:
	/* invalidate */
	iter->v = NULL;
	iter->k = NANPX;
	return false;
}

/* btree.c ends here */
