/*** dfp754_d32.h -- _Decimal32 goodness
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
#if !defined INCLUDED_dfp754_d32_h_
#define INCLUDED_dfp754_d32_h_

#if defined HAVE_CONFIG_H
# include "config.h"
#endif	/* HAVE_CONFIG_H */
#include <stdlib.h>
#include <stdint.h>

#define NAND32_U		(0x7c000000U)
#define INFD32_U		(0x78000000U)

#if !defined __DEC32_MOST_POSITIVE__
# define __DEC32_MOST_POSITIVE__	(__DEC32_MAX__)
#endif	/* !__DEC32_MOST_POSITIVE__ */
#if !defined __DEC32_MOST_NEGATIVE__
# define __DEC32_MOST_NEGATIVE__	(-__DEC32_MOST_POSITIVE__)
#endif	/* !__DEC32_MOST_NEGATIVE__ */

typedef struct {
	uint_least32_t mant;
	int expo;
	int sign;
}  bcd32_t;


extern _Decimal32 strtod32(const char*, char**);

#if defined HAVE_DFP754_BID_LITERALS || defined HAVE_DFP754_DPD_LITERALS
extern int d32tostr(char *restrict buf, size_t bsz, _Decimal32);

/**
 * Round X to the quantum of R. */
extern _Decimal32 quantized32(_Decimal32 x, _Decimal32 r);

/**
 * Return X*10^N. */
extern _Decimal32 scalbnd32(_Decimal32 x, int n);
#endif	/* !HAVE_DFP754_*_LITERALS */

/* non-standard stuff */
/**
 * Decompose x. */
extern bcd32_t decompd32(_Decimal32 x);


inline __attribute__((pure, const)) uint32_t bits32(_Decimal32 x);
inline __attribute__((pure, const)) _Decimal32 bobs32(uint32_t u);
inline __attribute__((pure, const)) int quantexpd32(_Decimal32 x);
#if !defined HAVE_NAND32
inline __attribute__((pure, const)) _Decimal32 nand32(char *__tagp);
#endif	/* !HAVE_NAND32 */
#if !defined HAVE_ISNAND32
# define isnand32		__builtin_isnand32
#endif	/* !HAVE_ISNAND32 */

inline __attribute__((pure, const)) uint32_t
bits32(_Decimal32 x)
{
	return (union {_Decimal32 x; uint32_t u;}){x}.u;
}

inline __attribute__((pure, const)) _Decimal32
bobs32(uint32_t u)
{
	return (union {uint32_t u; _Decimal32 x;}){u}.x;
}

#if defined HAVE_DFP754_BID_LITERALS
inline __attribute__((pure, const)) int quantexpbid32(_Decimal32 x);
inline __attribute__((pure, const)) int
quantexpbid32(_Decimal32 x)
{
	register uint32_t b = bits32(x);
	register int tmp;

	if (b == 0U) {
		return 0;
	} else if ((b & 0x60000000U) != 0x60000000U) {
		tmp = (b >> 23U);
	} else {
		/* exponent starts 2 bits to the left */
		tmp = (b >> 21U);
	}
	return (tmp & 0xffU) - 101;
}
#elif defined HAVE_DFP754_DPD_LITERALS
inline __attribute__((pure, const)) int quantexpdpd32(_Decimal32 x);
inline __attribute__((pure, const)) int
quantexpdpd32(_Decimal32 x)
{
	register uint32_t b = bits32(x);
	register int tmp;

	b >>= 20U;
	if (b == 0U) {
		return 0;
	} else if ((b & 0b11000000000U) != 0b11000000000U) {
		tmp = ((b & 0b11000000000U) >> 3U) | (b & 0b111111U);
	} else {
		/* exponent starts 2 bits to the left */
		tmp = ((b & 0b00110000000U) >> 1U) | (b & 0b111111U);
	}
	return tmp - 101;
}
#endif	/* HAVE_DFP754_DPD_LITERALS || HAVE_DFP754_BID_LITERALS */

#if defined HAVE_DFP754_BID_LITERALS || defined HAVE_DFP754_DPD_LITERALS
inline __attribute__((pure, const)) int
quantexpd32(_Decimal32 x)
{
#if defined HAVE_DFP754_BID_LITERALS
	return quantexpbid32(x);
#elif defined HAVE_DFP754_DPD_LITERALS
	return quantexpdpd32(x);
#endif	/* HAVE_DFP754_*_LITERALS */
}
#endif	/* !HAVE_DFP754_*_LITERALS */

#if defined HAVE_BUILTIN_NAND32
# define NAND32		__builtin_nand32("")
#elif defined HAVE_BUILTIN_NAN_FOR_NAND32
# define NAND32		((_Decimal32)__builtin_nan(""))
#else
# define NAND32		((union {uint32_t u; _Decimal32 x;}){NAND32_U}.x)
#endif
#if defined HAVE_BUILTIN_INFD32
# define INFD32		__builtin_infd32()
#elif defined HAVE_BUILTIN_INF_FOR_INFD32
# define INFD32		((_Decimal32)__builtin_inf())
#else
# define INFD32		(((union {uint32_t u; _Decimal32 x;}){INFD32_U}).x)
#endif

#if !defined HAVE_NAND32
inline __attribute__((pure, const)) _Decimal32
nand32(char *__tagp __attribute__((unused)))
{
	return NAND32;
}
#endif	/* !HAVE_NAND32 */

#if !defined HAVE_INFD32
inline __attribute__((pure, const)) _Decimal32
infd32(void)
{
	return INFD32;
}
#endif	/* !HAVE_INFD32 */

#endif	/* INCLUDED_dfp754_d32_h_ */
