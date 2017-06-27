/*** quos.c -- quote stream attachment to central limit order book
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
#include "quos.h"
#include "nifty.h"

#define QUOS_INIZ	(8U)

struct _quos_s {
	size_t n;
	quos_msg_t *a;
	size_t z;
};


quos_t
make_quos(void)
{
	struct _quos_s *r = malloc(sizeof(*r));
	r->n = 0U;
	r->a = malloc(QUOS_INIZ * sizeof(*r->a));
	r->z = QUOS_INIZ;
	return (quos_t)r;
}

void
free_quos(quos_t q)
{
	struct _quos_s *_q = (struct _quos_s*)q;
	if (LIKELY(_q->a != NULL)) {
		free(_q->a);
	}
	free(q);
	return;
}

int
quos_add(quos_t q, quos_msg_t m)
{
	struct _quos_s *_q = (struct _quos_s*)q;
	if (UNLIKELY(_q->n >= _q->z)) {
		/* resize him */
		const size_t nuz = _q->z * 2U;
		void *tmp = realloc(_q->a, nuz * sizeof(*_q->a));
		if (UNLIKELY(tmp == NULL)) {
			return -1;
		}
		/* otherwise assign */
		_q->a = tmp;
		_q->z = nuz;
	}
	_q->a[_q->n++] = m;
	return 0;
}

int
quos_clr(quos_t q)
{
	struct _quos_s *_q = (struct _quos_s*)q;
	_q->n = 0U;
	return 0;
}

/*  quos.c ends here*/
