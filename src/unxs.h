/*** unxs.h -- uncrossing schemes
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
#if !defined INCLUDED_unxs_h_
#define INCLUDED_unxs_h_
#include <stdlib.h>
#include "clob.h"
#include "clob_val.h"

typedef struct {
	px_t prc;
	qx_t qty;
} unxs_exe_t;

/** single contra firm execution */
typedef struct {
	unxs_exe_t x;
	clob_oid_t o;
} unxs_exsc_t;

/** maker/taker execution */
typedef struct {
	unxs_exe_t x;
	clob_oid_t o[NSIDES];
} unxs_exbi_t;

/**
 * Try crossing order O with book C at reference price R.
 * At most N executions are stored in X.
 * Lateral sides are maker/taker.
 * If the order is accepted the first object in X will hold the residual
 * quantity left in the order book along, if > 0, with the order id on
 * the MAKER side. */
extern size_t
unxs_order(unxs_exbi_t *restrict x, size_t n, clob_t c, clob_ord_t o, px_t r);

/**
 * Uncross the book C at price P for a quantity of at most Q
 * and assuming a single contra firm.
 * At most N executions are stored in X. */
extern size_t
unxs_mass_sc(unxs_exsc_t *restrict x, size_t n, clob_t c, px_t p, qx_t q);

#endif	/* INCLUDED_unxs_h_ */
