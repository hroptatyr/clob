/*** nifty.h -- generally handy macroes
 *
 * Copyright (C) 2009-2017 Sebastian Freundt
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
 ***/
#if !defined INCLUDED_nifty_h_
#define INCLUDED_nifty_h_

#if !defined LIKELY
# define LIKELY(_x)	__builtin_expect((_x), 1)
#endif	/* !LIKELY */
#if !defined UNLIKELY
# define UNLIKELY(_x)	__builtin_expect((_x), 0)
#endif	/* UNLIKELY */

#if !defined UNUSED
# define UNUSED(_x)	_x __attribute__((unused))
#endif	/* !UNUSED */

#if !defined ALGN
# define ALGN(_x, to)	_x __attribute__((aligned(to)))
#endif	/* !ALGN */

#if !defined countof
# define countof(x)	(sizeof(x) / sizeof(*x))
#endif	/* !countof */

#if !defined strlenof
# define strlenof(lit)	(sizeof(lit) - 1U)
#endif	/* !strlenof */

#define _paste(x, y)	x ## y
#define paste(x, y)	_paste(x, y)

#if !defined with
# define with(args...)							\
	for (args, *paste(__ep, __LINE__) = (void*)1;			\
	     paste(__ep, __LINE__); paste(__ep, __LINE__)= 0)
#endif	/* !with */

#if !defined if_with
# define if_with(init, args...)					\
	for (init, *paste(__ep, __LINE__) = (void*)1;			\
	     paste(__ep, __LINE__) && (args); paste(__ep, __LINE__)= 0)
#endif	/* !if_with */

#define once					\
	static int paste(__, __LINE__);		\
	if (!paste(__, __LINE__)++)
#define but_first				\
	static int paste(__, __LINE__);		\
	if (paste(__, __LINE__)++)

#define save_errno				\
	for (int paste(__er, __LINE__) = errno, paste(__ep, __LINE__) = 1; \
	     paste(__ep, __LINE__);					\
	     errno = paste(__er, __LINE__), paste(__ep, __LINE__) = 0)

static inline void*
deconst(const void *cp)
{
	union {
		const void *c;
		void *p;
	} tmp = {cp};
	return tmp.p;
}

#define max(x, y)	_Generic((x) + (y), default: (x) >= (y) ? (x) : (y))
#define min(x, y)	_Generic((x) + (y), default: (x) <= (y) ? (x) : (y))

#endif	/* INCLUDED_nifty_h_ */
