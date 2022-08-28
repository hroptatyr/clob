/*** clob_val.h -- basic types
 *
 * Copyright (C) 2016-2022 Sebastian Freundt
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
#if !defined INCLUDED_clob_val_h_
#define INCLUDED_clob_val_h_
#include <stdlib.h>
#include "clob_type.h"

/* metronome type */
typedef size_t metr_t;

typedef struct {
	/** display quantity */
	qx_t dis;
	/** hidden quantity */
	qx_t hid;
} qty_t;

#define qty0		((qty_t){0.dd, 0.dd})


static inline __attribute__((pure, const)) qx_t
qty(qty_t v)
{
	return v.dis + v.hid;
}

static inline __attribute__((pure, const)) qty_t
qty_add(qty_t v1, qty_t v2)
{
	return (qty_t){v1.dis + v2.dis, v1.hid + v2.hid};
}

static inline __attribute__((pure, const)) qty_t
qty_sub(qty_t v1, qty_t v2)
{
	return (qty_t){v1.dis - v2.dis, v1.hid - v2.hid};
}

static inline __attribute__((pure, const)) qty_t
qty_exe(qty_t v, qx_t q)
{
/* V is executed against quantity Q, use up hidden liquidity of V first */
	qx_t nuh = v.hid <= q ? v.hid : q;
	qx_t nuv = v.hid <= q ? q - v.hid : 0.dd;
	return (qty_t){v.dis - nuv, v.hid - nuh};
}

#endif	/* INCLUDED_clob_val_h_ */
