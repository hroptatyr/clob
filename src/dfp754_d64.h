/*** dfp754_d64.h -- _Decimal64 goodness
 *
 * Copyright (C) 2013-2016 Sebastian Freundt
 *
 * Author:  Sebastian Freundt <freundt@ga-group.nl>
 *
 * This file is part of dfp754.
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
#if !defined INCLUDED_dfp754_d64_h_
#define INCLUDED_dfp754_d64_h_

#if defined HAVE_CONFIG_H
# include "config.h"
#endif	/* HAVE_CONFIG_H */
#include <stdlib.h>
#include <stdint.h>

#define NAND64_U		(0x7c00000000000000U)
#define INFD64_U		(0x7800000000000000U)

#if !defined __DEC64_MOST_POSITIVE__
# define __DEC64_MOST_POSITIVE__	(__DEC64_MAX__)
#endif	/* !__DEC64_MOST_POSITIVE__ */
#if !defined __DEC64_MOST_NEGATIVE__
# define __DEC64_MOST_NEGATIVE__	(-__DEC64_MOST_POSITIVE__)
#endif	/* !__DEC64_MOST_NEGATIVE__ */

typedef struct {
	uint_least64_t mant;
	int expo;
	int sign;
}  bcd64_t;


extern _Decimal64 strtod64(const char*, char**);

#if defined HAVE_DFP754_BID_LITERALS || defined HAVE_DFP754_DPD_LITERALS
extern int d64tostr(char *restrict buf, size_t bsz, _Decimal64);

/**
 * Round X to the quantum of R. */
extern _Decimal64 quantized64(_Decimal64 x, _Decimal64 r);

/**
 * Return X*10^N. */
extern _Decimal64 scalbnd64(_Decimal64 x, int n);
#endif	/* !HAVE_DFP754_*_LITERALS */

/* non-standard stuff */
/**
 * Decompose x. */
extern bcd64_t decompd64(_Decimal64 x);


inline __attribute__((pure, const)) uint64_t bits64(_Decimal64 x);
inline __attribute__((pure, const)) _Decimal64 bobs64(uint64_t u);
inline __attribute__((pure, const)) int quantexpd64(_Decimal64 x);
#if !defined HAVE_NAND64
inline __attribute__((pure, const)) _Decimal64 nand64(char *__tagp);
#endif	/* !HAVE_NAND64 */
#if !defined HAVE_ISNAND64
# define isnand64		__builtin_isnand64
#endif	/* !HAVE_ISNAND64 */

inline __attribute__((pure, const)) uint64_t
bits64(_Decimal64 x)
{
	return (union {_Decimal64 x; uint64_t u;}){x}.u;
}

inline __attribute__((pure, const)) _Decimal64
bobs64(uint64_t u)
{
	return (union {uint64_t u; _Decimal64 x;}){u}.x;
}

#if defined HAVE_DFP754_BID_LITERALS
inline __attribute__((pure, const)) int quantexpbid64(_Decimal64 x);
inline __attribute__((pure, const)) int
quantexpbid64(_Decimal64 x)
{
	register uint64_t b = bits64(x);
	register int tmp;

	if (b == 0U) {
		return 0;
	} else if ((b & 0x6000000000000000ULL) != 0x6000000000000000ULL) {
		tmp = (b >> 53U);
	} else {
		/* exponent starts 2 bits to the left */
		tmp = (b >> 51U);
	}
	return (tmp & 0x3ffU) - 398;
}
#elif defined HAVE_DFP754_DPD_LITERALS
inline __attribute__((pure, const)) int quantexpdpd64(_Decimal64 x);
inline __attribute__((pure, const)) int
quantexpdpd64(_Decimal64 x)
{
	register uint64_t b = bits64(x);
	register int tmp;

	b >>= 50U;
	if (b == 0U) {
		return 0;
	} else if ((b & 0b1100000000000ULL) != 0b1100000000000ULL) {
		tmp = ((b & 0b1100000000000ULL) >> 3U) | (b & 0b11111111ULL);
	} else {
		/* exponent starts 2 bits to the left */
		tmp = ((b & 0b001100000000ULL) >> 1U) | (b & 0b11111111ULL);
	}
	return tmp - 398;
}
#endif	/* HAVE_DFP754_DPD_LITERALS || HAVE_DFP754_BID_LITERALS */

#if defined HAVE_DFP754_BID_LITERALS || defined HAVE_DFP754_DPD_LITERALS
inline __attribute__((pure, const)) int
quantexpd64(_Decimal64 x)
{
#if defined HAVE_DFP754_BID_LITERALS
	return quantexpbid64(x);
#elif defined HAVE_DFP754_DPD_LITERALS
	return quantexpdpd64(x);
#endif	/* HAVE_DFP754_*_LITERALS */
}
#endif	/* !HAVE_DFP754_*_LITERALS */

#if defined HAVE_BUILTIN_NAND64
# define NAND64		__builtin_nand64("")
#elif defined HAVE_BUILTIN_NAN_FOR_NAND64
# define NAND64		((_Decimal64)__builtin_nan(""))
#else
# define NAND64		((union {uint64_t u; _Decimal64 x;}){NAND64_U}.x)
#endif
#if defined HAVE_BUILTIN_INFD64
# define INFD64		__builtin_infd64()
#elif defined HAVE_BUILTIN_INF_FOR_INFD64
# define INFD64		((_Decimal64)__builtin_inf())
#else
# define INFD64		(((union {uint64_t u; _Decimal64 x;}){INFD64_U}).x)
#endif

#if !defined HAVE_NAND64
inline __attribute__((pure, const)) _Decimal64
nand64(char *__tagp __attribute__((unused)))
{
	return NAND64;
}
#endif	/* !HAVE_NAND64 */

#if !defined HAVE_INFD64
inline __attribute__((pure, const)) _Decimal64
infd64(void)
{
	return INFD64;
}
#endif	/* !HAVE_INFD64 */

#endif	/* INCLUDED_dfp754_d64_h_ */
