/*** plqu.h -- price level queue
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
#if !defined INCLUDED_plqu_h_
#define INCLUDED_plqu_h_
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
/* defines plqu_val_t, hopefully */
#include "plqu_val.h"

typedef uintptr_t plqu_qid_t;
typedef uintptr_t plqu_t;

typedef struct {
	plqu_t q;
	size_t i;
	plqu_val_t v;
} plqu_iter_t;


extern plqu_t make_plqu(void);
extern void free_plqu(plqu_t);

extern plqu_val_t plqu_get(plqu_t, plqu_qid_t);
extern int plqu_put(plqu_t, plqu_qid_t, plqu_val_t);
extern plqu_qid_t plqu_add(plqu_t, plqu_val_t);
extern plqu_val_t plqu_top(plqu_t);
extern plqu_val_t plqu_pop(plqu_t);

extern bool plqu_iter_next(plqu_iter_t*);
extern plqu_qid_t plqu_iter_qid(plqu_iter_t);
extern int plqu_iter_put(plqu_iter_t, plqu_val_t);
extern int plqu_iter_set_top(plqu_iter_t);

#endif	/* INCLUDED_plqu_h_ */
