/*** plqu.c -- price level queue
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
#include <stdbool.h>
#include "plqu.h"
#include "plqu_val.h"
#include "nifty.h"

#define PLQU_INIZ	(8U)

struct plqu_s {
	/* always a 2-power or a 2-power - 1U if in use */
	size_t z;
	plqu_val_t *a;
	size_t head;
	size_t tail;
};

static struct plqu_s _pool[256U], *pool = _pool;
static size_t zpool = countof(_pool);
/* number of plqus in use */
static size_t npool;
/* lowest returned plqu */
static size_t ipool;

#define Q		pool[q]


plqu_t
make_plqu(void)
{
	if (UNLIKELY(npool >= zpool)) {
		/* resize */
		size_t nuz = zpool * 2U;
		if (UNLIKELY(pool == _pool)) {
			pool = malloc(nuz * sizeof(*pool));
			memcpy(pool, _pool, zpool * sizeof(*_pool));
			memset(pool + zpool, 0, zpool * sizeof(*_pool));
		} else {
			pool = realloc(pool, nuz * sizeof(*pool));
			memset(pool + zpool, 0, zpool * sizeof(*_pool));
		}
		zpool = nuz;
	}
	for (; ipool < zpool && pool[ipool].z & 1U; ipool++);
	/* mark in use */
	pool[ipool].z--;
	/* keep track of used pools */
	npool++;
	return ipool + 1U;
}

void
free_plqu(plqu_t q)
{
	q--;
	Q.head = 0U;
	Q.tail = 0U;
	/* mark free */
	Q.z++;
	npool--;
	ipool = q < ipool ? q : ipool;;
	return;
}

plqu_val_t
plqu_get(plqu_t q, plqu_qid_t i)
{
	q--;
	if (UNLIKELY(!(i > Q.head && i <= Q.tail))) {
		return plqu_val_nil;
	}
	const size_t slot = (i - 1U) & Q.z;
	return Q.a[slot];
}

int
plqu_put(plqu_t q, plqu_qid_t i, plqu_val_t v)
{
	q--;
	if (UNLIKELY(!(i > Q.head && i <= Q.tail))) {
		return -1;
	}
	const size_t slot = (i - 1U) & Q.z;
	Q.a[slot] = v;
	return 0;
}

plqu_qid_t
plqu_add(plqu_t q, plqu_val_t v)
{
	q--;
	if (UNLIKELY(Q.tail - Q.head >= (Q.z + 1U))) {
		/* resize him */
		const size_t nuz = ((Q.z + 1U) * 2U) ?: PLQU_INIZ;
		void *tmp = realloc(Q.a, nuz * sizeof(*Q.a));
		if (UNLIKELY(tmp == NULL)) {
			return 0ULL;
		}
		/* otherwise resize */
		Q.a = tmp;
		/* head is either in the current range or the next range modulo
		 * the new size, move tail or head respectively
		 * to make things easy (and branchless) we simply clone the
		 * whole array */
		memcpy(Q.a + (Q.z + 1U), Q.a, (Q.z + 1U) * sizeof(*Q.a));
		/* keep track of size */
		Q.z = nuz - 1U;
	}
	Q.a[Q.tail++ & Q.z] = v;
	return Q.tail;
}

plqu_val_t
plqu_top(plqu_t q)
{
	plqu_val_t r;

	q--;
	if (LIKELY(Q.head < Q.tail)) {
		r = Q.a[Q.head & Q.z];
	} else {
		r = plqu_val_nil;
	}
	return r;
}

plqu_val_t
plqu_pop(plqu_t q)
{
	plqu_val_t r;

	q--;
	if (LIKELY(Q.head < Q.tail)) {
		const size_t slot = Q.head++ & Q.z;
		r = Q.a[slot];
	} else {
		r = plqu_val_nil;
	}
	return r;
}


bool
plqu_iter_next(plqu_iter_t *iter)
{
	plqu_t q = iter->q;

	if (UNLIKELY(q-- == 0U)) {
		return false;
	} else if (UNLIKELY(iter->i < Q.head)) {
		iter->i = Q.head;
	}
	while (iter->i < Q.tail) {
		const size_t slot = (iter->i++) & Q.z;
		if (LIKELY(!plqu_val_nil_p(Q.a[slot]))) {
			/* good one */
			iter->v = Q.a[slot];
			return true;
		}
	}
	/* set iter past tail */
	iter->i = Q.tail + 1U;
	return false;
}

plqu_qid_t
plqu_iter_qid(plqu_iter_t iter)
{
	plqu_qid_t cand = iter.i;
	plqu_t q = iter.q - 1U;
	return cand <= Q.tail ? cand : 0ULL;
}

int
plqu_iter_put(plqu_iter_t iter, plqu_val_t v)
{
	plqu_t q = iter.q;

	if (UNLIKELY(q-- == 0U)) {
		;
	} else if (UNLIKELY(!iter.i || iter.i > Q.tail)) {
		;
	} else {
		return plqu_put(iter.q, iter.i, v);
	}
	return -1;
}

int
plqu_iter_set_top(plqu_iter_t iter)
{
	plqu_t q = iter.q;

	if (UNLIKELY(q-- == 0U)) {
		return -1;
	} else if (UNLIKELY(!iter.i)) {
		return -1;
	}
	/* otherwise index becomes head */
	Q.head = iter.i - 1U;
	return 0;
}

/* plqu.c ends here */
