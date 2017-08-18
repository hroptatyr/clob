/*** clob.h -- central limit order book
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
#if !defined INCLUDED_clob_h_
#define INCLUDED_clob_h_
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "clob_val.h"

typedef size_t clob_qid_t;

typedef enum {
	CLOB_TYPE_LMT,
	CLOB_TYPE_MKT,
} clob_type_t;

typedef enum {
	CLOB_SIDE_ASK = 0U,
	CLOB_SIDE_SHORT = CLOB_SIDE_ASK,
	CLOB_SIDE_SELLER = CLOB_SIDE_ASK,
	CLOB_SIDE_MAKER = CLOB_SIDE_ASK,
	CLOB_SIDE_BID = 1U,
	CLOB_SIDE_LONG = CLOB_SIDE_BID,
	CLOB_SIDE_BUYER = CLOB_SIDE_BID,
	CLOB_SIDE_TAKER = CLOB_SIDE_BID,
	NCLOB_SIDES,
} clob_side_t;

_Static_assert(NCLOB_SIDES == 2U, "more than 2 sides");

typedef struct {
	void *lmt[NCLOB_SIDES];
	void *mkt[NCLOB_SIDES];
	struct quos_s *quo;
	struct unxs_s *exe;
} clob_t;

typedef struct {
	clob_type_t typ;
	clob_side_t sid;
	qty_t qty;
	px_t lmt;
	px_t stp;
	uintptr_t user;
} clob_ord_t;

typedef struct {
	clob_type_t typ;
	clob_side_t sid;
	px_t prc;
	clob_qid_t qid;
	uintptr_t user;
} clob_oid_t;

_Static_assert(sizeof(clob_oid_t) == 32U, "clob_oid_t of odd size");
_Static_assert(sizeof(clob_ord_t) == 48U, "clob_ord_t of odd size");


/**
 * Instantiate central limit order book.
 * DESCP indicates whether to sort descendingly. */
extern clob_t make_clob(void);

/**
 * Deinstantiate clob object and free associated resources. */
extern void free_clob(clob_t);

/**
 * Add order to clob_t object and return an order id object.
 * If a quote-stream is attached any touched price levels will
 * be published there. */
extern clob_oid_t clob_add(clob_t, clob_ord_t);

/**
 * Delete order from clob_t object.
 * If a quote-stream is attached any touched price levels will
 * be published there. */
extern int clob_del(clob_t, clob_oid_t);

extern px_t clob_mid(clob_t);

extern void clob_prnt(clob_t c);

extern void clob_lvl2(clob_t c);


/* convenience */
static inline __attribute__((pure, const)) clob_side_t
clob_contra_side(clob_side_t s)
{
	return (clob_side_t)((unsigned int)s ^ 1U);
}

#endif	/* INCLUDED_clob_h_ */
