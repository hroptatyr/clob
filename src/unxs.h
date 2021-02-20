/*** unxs.h -- uncrossing schemes
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
#if !defined INCLUDED_unxs_h_
#define INCLUDED_unxs_h_
#include <stdint.h>
#include "clob.h"
#include "clob_val.h"

typedef enum {
	/** don't keep track of parties */
	MODE_NO = 0U,
	/** uncross with a single contra firm */
	MODE_SC = 1U,
	/** uncross bilaterally */
	MODE_BI = 2U,
} unxs_mode_t;

typedef struct {
	px_t prc;
	qx_t qty;
} unxs_exe_t;

typedef struct {
	qx_t base;
	qx_t term;
} unxs_exa_t;

typedef struct unxs_s {
	/** mode used for uncrossing */
	const unxs_mode_t m;
	/** number of executions */
	const size_t n;
	/** executions */
	const unxs_exe_t *x;
	/** sides of executions from the maker point of view */
	const uint_fast8_t *s;
	/** orders taking part in this match, (unxs_mode_t)M  times N */
	const clob_oid_t *o;
	/** remaining quantities, same dimension as O */
	const qty_t *q;
} *unxs_t;


/**
 * Instantiate an uncrossing stream using uncrossing mode M. */
extern unxs_t make_unxs(unxs_mode_t);

/**
 * Free resources associated with uncrossing stream. */
extern void free_unxs(unxs_t);

/**
 * Clear execution stream. */
extern int unxs_clr(unxs_t);

/**
 * Try crossing order O with book C at reference price R.
 * Executions will be tracked in C's exe-list.
 * The remainder of the order is returned. */
extern clob_ord_t
unxs_order(clob_t c, clob_ord_t o, px_t r);

/**
 * Uncross the book C at price P for a quantity of at most Q
 * and assuming a single contra firm. */
extern void
unxs_auction(clob_t c, px_t p, qx_t q);


/* convenience */
static inline __attribute__((pure, const)) unxs_exa_t
unxs_exa(unxs_exe_t x, clob_side_t s)
{
	switch (s) {
	case CLOB_SIDE_SELLER:
		/* maker is a seller */
		return (unxs_exa_t){-x.qty, x.prc * x.qty};
	case CLOB_SIDE_BUYER:
		/* maker is a buyer */
		return (unxs_exa_t){x.qty, -x.prc * x.qty};
	default:
		break;
	}
	return (unxs_exa_t){};
}

#endif	/* INCLUDED_unxs_h_ */
