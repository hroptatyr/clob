/*** dfp754_d64.c -- _Decimal64 goodness
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
#if defined HAVE_CONFIG_H
# include "config.h"
#endif	/* HAVE_CONFIG_H */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#if defined HAVE_DFP754_H
# include <dfp754.h>
#elif defined HAVE_DFP_STDLIB_H
# include <dfp/stdlib.h>
#else  /* !HAVE_DFP754_H && !HAVE_DFP_STDLIB_H */
extern int isinfd64(_Decimal64);
#endif	/* HAVE_DFP754_H */
#include "dfp754_d64.h"

#define U(x)	(uint_least8_t)((x) - '0')
#define C(x)	(char)((x) + '0')

#if !defined LIKELY
# define LIKELY(_x)	__builtin_expect((_x), 1)
#endif	/* !LIKELY */
#if !defined UNLIKELY
# define UNLIKELY(_x)	__builtin_expect((_x), 0)
#endif	/* UNLIKELY */

#define _paste(x, y)	x ## y
#define paste(x, y)	_paste(x, y)

#if !defined with
# define with(args...)							\
	for (args, *paste(__ep, __LINE__) = (void*)1;			\
	     paste(__ep, __LINE__); paste(__ep, __LINE__)= 0)
#endif	/* !with */

static inline void*
deconst(const void *cp)
{
	union {
		const void *c;
		void *p;
	} tmp = {cp};
	return tmp.p;
}

static inline __attribute__((const, pure)) size_t
min_z(size_t z1, size_t z2)
{
	return z1 <= z2 ? z1 : z2;
}


# if defined HAVE_DFP754_BID_LITERALS
static inline __attribute__((pure, const)) int
sign_bid64(_Decimal64 x)
{
	uint64_t b = bits64(x);
	return (int64_t)b >> 63U;
}

static inline __attribute__((pure, const)) uint_least64_t
mant_bid64(_Decimal64 x)
{
	uint64_t b = bits64(x);
	register uint_least64_t res;

	if (UNLIKELY((b & 0x6000000000000000ULL) == 0x6000000000000000ULL)) {
		/* mantissa is only 51 bits + 0b100 */
		res = 0b100ULL << 51U;
		res ^= (b & 0x7ffffffffffffULL);
	} else {
		/* mantissa is full */
		res = b & 0x1fffffffffffffULL;
	}
	return res;
}

#elif defined HAVE_DFP754_DPD_LITERALS

static inline __attribute__((pure, const)) int
sign_dpd64(_Decimal64 x)
{
	uint64_t b = bits64(x);
	return (int64_t)b >> 63U;
}

static inline __attribute__((pure, const)) uint_least64_t
mant_dpd64(_Decimal64 x)
{
	uint64_t b = bits64(x);
	register uint_least64_t res;

	/* at least the last 50 bits aye */
	res = b & 0x3ffffffffffffULL;
	b >>= 58U;
	if (UNLIKELY((b & 0b11000ULL) == 0b11000ULL)) {
		/* exponent is two more bits, then the high bit */
		res ^= (b & 0b00001ULL | 0b1000ULL) << 50U;
	} else {
		res ^= (b & 0b00111ULL) << 50U;
	}
	return res;
}
#endif	/* HAVE_DFP754_BID_LITERALS || HAVE_DFP754_DPD_LITERALS */

static unsigned int
pack_declet(unsigned int x)
{
/* consider X to be 12bit BCD of 3 digits, return the declet
 * X == BCD(d2, d1, d0) */
	unsigned int res;
	unsigned int d0;
	unsigned int d1;
	unsigned int d2;
	unsigned int c = 0U;

	if (UNLIKELY((d0 = x & 0xfU) >= 8U)) {
		/* d0 is large, 8 or 9, 0b1000 or 0b1001, store only low bit */
		d0 &= 0x1U;
		c |= 0b001U;
	}
	x >>= 4U;

	if (UNLIKELY((d1 = x & 0xfU) >= 8U)) {
		/* d1 is large, 8 or 9, 0b1000 or 0b1001, store only low bit */
		d1 &= 0x1U;
		c |= 0b010U;
	}
	x >>= 4U;

	if (UNLIKELY((d2 = x & 0xfU) >= 8U)) {
		/* d2 is large, 8 or 9, 0b1000 or 0b1001, store only low bit */
		d2 &= 0x1U;
		c |= 0b100U;
	}

	/* generally the low bits in the large case coincide */
	res = (d2 << 7U) | (d1 << 4U) | d0;

	switch (c) {
	case 0b000U:
		break;
	case 0b001U:
		/* we need d2d1(100)d0 */
		res |= 0b100U << 1U;
		break;
	case 0b010U:
		/* this is d2(gh)d1(101)(i) for d0 = ghi */
		res &= ~0b1110U;
		res |= (0b101U << 1U) | ((d0 & 0b110U) << 4U);
		break;
	case 0b011U:
		/* this goes to d2(10)d1(111)d0 */
		res |= (0b100U << 4U) | (0b111U << 1U);
		break;
	case 0b100U:
		/* this goes to (gh)d2d1(110)(i) for d0 = ghi */
		res &= ~0b1110U;
		res |= ((d0 & 0b110U) << 7U) | (0b110U << 1U);
		break;
	case 0b101U:
		/* this will be (de)d2(01)f(111)d0 for d1 = def */
		res &= ~0b1100000U;
		res |= ((d1 & 0b110U) << 7U) | (0b010U << 4U) | (0b111U << 1U);
		break;
	case 0b110U:
		/* goes to (gh)d2(00)d1(111)(i) for d0 = ghi */
		res &= ~0b1110U;
		res |= ((d0 & 0b110U) << 7U) | (0b111U << 1U);
		break;
	case 0b111U:
		/* goes to (xx)d2(11)d1(111)d0 */
		res |= (0b110U << 04U) | (0b111U << 1U);
		break;
	default:
		res = 0U;
		break;
	}
	return res;
}

static unsigned int
unpack_declet(unsigned int x)
{
/* go from dpd to bcd, here's the dpd box again:
 * abc def 0ghi
 * abc def 100i
 * abc ghf 101i
 * ghc def 110i
 * abc 10f 111i
 * dec 01f 111i
 * ghc 00f 111i
 * ??c 11f 111i
 */
	unsigned int res = 0U;

	/* check for the easiest case first */
	if (!(x & 0b1000U)) {
		goto trivial;
	} else {
		switch ((x >> 1U) & 0b111U) {
		case 0b100U:
		trivial:
			res |= x & 0b1111U;
			res |= (x & 0b1110000U);
			res |= (x & 0b1110000000U) << 1U;
			break;
		case 0b101U:
			/* ghf -> 100f ghi */
			res |= (x & 0b1110000000U) << 1U;
			res |= (0b1000U << 4U) | (x & 0b0010000U);
			res |= ((x & 0b1100000U) >> 4U) | (x & 0b1U);
			break;
		case 0b110U:
			/* ghc -> 100c ghi */
			res |= (0b1000U << 8U) | ((x & 0b0010000000U) << 1U);
			res |= (x & 0b1110000U);
			res |= ((x & 0b1100000000U) >> 7U) | (x & 0b1U);
			break;
		case 0b111U:
			/* grrr */
			switch ((x >> 5U) & 0b11U) {
			case 0b10U:
				res |= (x & 0b1110000000U) << 1U;
				res |= (0b1000U << 4U) | (x & 0b0000010000U);
				res |= (0b1000U << 0U) | (x & 0b0000000001U);
				break;
			case 0b01U:
				res |= (0b1000U << 8U) |
					((x & 0b0010000000U) << 1U);
				res |= ((x & 0b1100000000U) >> 3U) |
					(x & 0b0000010000U);
				res |= (0b1000U << 0U) | (x & 0b0000000001U);
				break;
			case 0b00U:
				res |= (0b1000U << 8U) |
					((x & 0b0010000000U) << 1U);
				res |= (0b1000U << 4U) | (x & 0b0000010000U);
				res |= ((x & 0b1100000000U) >> 7U) | (x & 0b1U);
				break;
			case 0b11U:
				res |= (0b1000U << 8U) |
					((x & 0b0010000000U) << 1U);
				res |= (0b1000U << 4U) | (x & 0b0000010000U);
				res |= (0b1000U << 0U) | (x & 0b0000000001U);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
	return res;
}

static _Decimal64
assemble_bid(uint64_t m, uint64_t ex, uint64_t s)
{
	uint64_t u = s << 63U;

	/* check if 54th bit of mantissa is set */
	if (UNLIKELY(m & (1ULL << 53U))) {
		u ^= 0b11ULL << 61U;
		u ^= ex << 51U;
		/* just use 51 bits of the mantissa */
		m &= 0x7ffffffffffffULL;
	} else {
		u ^= ex << 53U;
		/* use all 53 bits */
		m &= 0x1fffffffffffffULL;
	}
	u ^= m;
	return bobs64(u);
}

static _Decimal64
bcd64tobid(bcd64_t b)
{
	uint64_t m = 0U;

	/* massage the mantissa, first mirror it, so the most significant
	 * nibble is the lowest */
	b.mant = (b.mant & 0xffffffff00000000ULL) >> 32U ^
		(b.mant & 0x00000000ffffffffULL) << 32U;
	b.mant = (b.mant & 0xffff0000ffff0000ULL) >> 16U ^
		(b.mant & 0x0000ffff0000ffffULL) << 16U;
	b.mant = (b.mant & 0xff00ff00ff00ff00ULL) >> 8U ^
		(b.mant & 0x00ff00ff00ff00ffULL) << 8U;
	b.mant = (b.mant & 0xf0f0f0f0f0f0f0f0ULL) >> 4U ^
		(b.mant & 0x0f0f0f0f0f0f0f0fULL) << 4U;
	b.mant >>= 4U;
	for (size_t i = 15U; i > 0U; b.mant >>= 4U, i--) {
		m *= 10U;
		m += b.mant & 0b1111ULL;
	}

	return assemble_bid(m, b.expo + 398, b.sign);
}

static _Decimal64
bcd64todpd(bcd64_t b)
{
	uint64_t m = b.mant;
	uint64_t u = (uint64_t)b.sign << 63U;
	unsigned int rex = b.expo + 398;

        /* assemble the d64 */
	u ^= (uint64_t)pack_declet(m & 0xfffU);
	m >>= 12U;
	u ^= (uint64_t)pack_declet(m & 0xfffU) << 10U;
	m >>= 12U;
	u ^= (uint64_t)pack_declet(m & 0xfffU) << 20U;
	m >>= 12U;
	u ^= (uint64_t)pack_declet(m & 0xfffU) << 30U;
	m >>= 12U;
	u ^= (uint64_t)pack_declet(m & 0xfffU) << 40U;
	if (UNLIKELY((m >>= 12U) >= 8U)) {
		rex = (rex & 0b1100000000U) << 1U ^ (rex & 0b0011111111U);
		rex ^= 0b1100000000000U;
	} else {
		rex = (rex & 0b1100000000U) << 3U ^ (rex & 0b0011111111U);
	}
	/* the beef bits from the expo */
	u ^= (uint64_t)rex << 50U;
	/* the TTT bits (or T) */
	u ^= (m & 0x7ULL) << 58U;
	return bobs64(u);
}

static uint_least64_t
round_bcd64(uint_least64_t mant, int roff)
{
	register uint_least64_t x;
	unsigned int sh = 0U;

	while (UNLIKELY((x = (mant & 0b1111U) + roff) > 9)) {
		mant >>= 4U;
		sh += 4U;
	}
	mant = (mant & ~0b1111U) ^ x;
	mant <<= sh;
	return mant;
}

static bcd64_t
strtobcd64(const char *src, char **on)
{
	const char *sp = src;
	uint_least64_t mant = 0U;
	int expo = 0;
	int sign = 0;
	int roff = 0;
	unsigned int nd;

	if (UNLIKELY(*sp == '-')) {
		sign = 1;
		sp++;
	} else if (UNLIKELY(*sp == '+')) {
		sign = 0;
		sp++;
	}
	/* skip leading zeros innit? */
	for (; *sp == '0'; sp++);
	/* pick up some digits, not more than 15 though */
	for (nd = 15U; *sp >= '0' && *sp <= '9' && nd > 0; sp++, nd--) {
		mant <<= 4U;
		mant |= U(*sp);
	}
	/* just pick up digits, don't fiddle with the mantissa though
	 * here we determine the round off digit really */
	roff += (*sp >= '5' && *sp <= '9');
	for (; *sp >= '0' && *sp <= '9'; sp++, expo++);
	if (*sp == '.' && nd > 0) {
		/* less than 15 digits, read more from the right side */
		for (sp++;
		     *sp >= '0' && *sp <= '9' && nd > 0; sp++, expo--, nd--) {
			mant <<= 4U;
			mant |= U(*sp);
		}
		/* pick up trailing digits for the word */
		roff += (*sp >= '5' && *sp <= '9');
		for (; *sp >= '0' && *sp <= '9'; sp++);
	} else if (*sp == '.') {
		/* more than 15 digits already, just consume */
		roff += (*sp >= '5' && *sp <= '9');
		for (sp++; *sp >= '0' && *sp <= '9'; sp++);
	}

	if (UNLIKELY(roff)) {
		mant = round_bcd64(mant, sign ? -1 : 1);
	}

	if (LIKELY(on != NULL)) {
		*on = deconst(sp);
	}
	return (bcd64_t){mant, expo, sign};
}

static size_t
bcd64tostr(char *restrict buf, size_t bsz, bcd64_t b)
{
	char *restrict bp = buf;
	const char *const ep = buf + bsz;

	/* write the right-of-point side first */
	for (; b.expo < 0 && bp < ep; b.expo++, b.mant >>= 4U) {
		*bp++ = C(b.mant & 0b1111U);
	}
	/* write point now */
	if (bp > buf && bp < ep) {
		*bp++ = '.';
	}
	/* write trailing 0s for left-of-point side */
	for (; b.expo > 0 && bp < ep; b.expo--) {
		*bp++ = '0';
	}
	/* now write the rest of the mantissa */
	if (LIKELY(b.mant)) {
		for (; b.mant && bp < ep; b.mant >>= 4U) {
			*bp++ = C(b.mant & 0b1111U);
		}
	} else if (bp < ep) {
		/* put a leading 0 */
		*bp++ = '0';
	}
	if (b.sign && bp < ep) {
		*bp++ = '-';
	}
	if (bp < ep) {
		*bp = '\0';
	}
	/* reverse the string */
	for (char *ip = buf, *jp = bp - 1; ip < jp; ip++, jp--) {
		char tmp = *ip;
		*ip = *jp;
		*jp = tmp;
	}
	return bp - buf;
}

#if defined HAVE_DFP754_BID_LITERALS
static _Decimal64
quantizebid64(_Decimal64 x, _Decimal64 r)
{
/* d64s look like s??eeeeeeee mm..53..mm
 * and the decimal is (-1 * s) * m * 10^(e - 101),
 * this implementation is very minimal serving only the cattle use cases */
	int er;
	int ex;
	uint_least64_t m;

	/* get the exponents of x and r */
	er = quantexpbid64(r);
	ex = quantexpbid64(x);

	m = mant_bid64(x);

	/* truncate */
	for (; ex < er; ex++) {
		m = m / 10U + ((m % 10U) >= 5U);
	}
	/* expand (only if we don't exceed the range) */
	for (; m < 100000000000000ULL && ex > er; ex--) {
		m *= 10U;
	}

	/* assemble the bid64 */
	return assemble_bid(m, ex + 398, sign_bid64(x));
}
#elif defined HAVE_DFP754_DPD_LITERALS
static _Decimal64
quantizedpd64(_Decimal64 x, _Decimal64 r)
{
/* d64s dpds look like s??ttteeeeeeee mm..50..mm
 * this implementation is very minimal serving only the cattle use cases */
	int er;
	int ex;
	uint_least64_t m;
	uint_least64_t b;

	/* get the exponents of x and r */
	er = quantexpdpd64(r);
	ex = quantexpdpd64(x);

	/* get the mantissa TTT MH ML, with MH, ML being declets */
	m = mant_dpd64(x);
	/* unpack the declets and TTT, TTT first, then MH, then ML */
	b = (m & 0x3c000000000000ULL);
	b >>= 8U;
	b ^= unpack_declet((m >> 40U) & 0x3ffU);
	b <<= 12U;
	b ^= unpack_declet((m >> 30U) & 0x3ffU);
	b <<= 12U;
	b ^= unpack_declet((m >> 20U) & 0x3ffU);
	b <<= 12U;
	b ^= unpack_declet((m >> 10U) & 0x3ffU);
	b <<= 12U;
	b ^= unpack_declet((m >> 0U) & 0x3ffU);

	/* truncate (on bcd) */
	for (; ex < er; ex++) {
		b = (b >> 4U) + ((b & 0xfU) >= 5U);
	}
	/* the lowest 4 bits could be 0xa, so flip them over to 0 */
	with (size_t i) {
		for (i = 0U; (b & 0xfU) >= 10U; b >>= 4U, b++, i++);
		/* fix them up by shifting by 4i */
		b <<= 4U * i;
	}

	/* expand (only if we don't exceed the range) */
	for (; b < 0x100000000000000ULL && ex > er; ex--) {
		b <<= 4U;
	}

	/* assemble the d64 */
	return bcd64todpd((bcd64_t){b, ex, sign_dpd64(x)});
}
#endif	/* HAVE_DFP754_*_LITERALS */

#if !defined HAVE_SCALBND64
# if defined HAVE_DFP754_BID_LITERALS
static _Decimal64
scalbnbid64(_Decimal64 x, int n)
{
	/* just fiddle with the exponent of X then */
	with (uint64_t b = bits64(x), u) {
		/* the idea is to xor the current expo with the new expo
		 * and shift the result to the right position and xor again */
		if (UNLIKELY((b & 0x6000000000000000ULL) ==
			     0x6000000000000000ULL)) {
			/* 54th bit of mantissa is set, special expo */
			u = (b >> 51U) & 0x3ffU;
			u ^= u + n;
			u &= 0x3ffU;
			u <<= 51U;
		} else {
			u = (b >> 53U) & 0x3ffU;
			u ^= u + n;
			u &= 0x3ffU;
			u <<= 53U;
		}
		b ^= u;
		x = bobs64(b);
	}
	return x;
}
# elif defined HAVE_DFP754_DPD_LITERALS
static _Decimal64
scalbndpd64(_Decimal64 x, int n)
{
	/* just fiddle with the exponent of X then */
	with (uint64_t b = bits64(x), u) {
		/* the idea is to xor the current expo with the new expo
		 * and shift the result to the right position and xor again */
		if (UNLIKELY((b & 0x6000000000000000ULL) ==
			     0x6000000000000000ULL)) {
			/* 24th bit of mantissa is set, special expo
			 * 11ee T (ee)eeeeeeee mmm... */
			u = (b >> 50U) & 0xffULL;
			u ^= (b >> 51U) & 0x300ULL;
			u ^= u + n;
			/* move the top-2 bits out by 1 bit again */
			u = (u & 0xffULL) ^ ((u & 0x300ULL) << 1U);
			u <<= 50U;
		} else {
			/* ee TTT (ee)eeeeeeee mmm... */
			u = (b >> 50U) & 0xffULL;
			u ^= (b >> 53U) & 0x300ULL;
			u ^= u + n;
			/* move the top-2 bits out by 3 bits again */
			u = (u & 0xffULL) ^ ((u & 0x300ULL) << 3U);
			u <<= 50U;
		}
		b ^= u;
		x = bobs64(b);
	}
	return x;
}
# endif	/* HAVE_DFP754_*_LITERALS */
#endif	/* !HAVE_SCALBND64 */


#if defined HAVE_DFP754_BID_LITERALS
static _Decimal64
strtobid64(const char *src, char **on)
{
/* d64s look like s??eeeeee mm..23..mm
 * and the decimal is (-1 * s) * m * 10^(e - 101),
 * this implementation is very minimal serving only the cattle use cases */
	bcd64_t b = strtobcd64(src, on);
	return bcd64tobid(b);
}
#elif defined HAVE_DFP754_DPD_LITERALS
static _Decimal64
strtodpd64(const char *src, char **on)
{
/* d64s look like s??eeeeee mm..23..mm
 * and the decimal is (-1 * s) * m * 10^(e - 101),
 * this implementation is very minimal serving only the cattle use cases */
	bcd64_t b = strtobcd64(src, on);
	return bcd64todpd(b);
}
#endif	/* HAVE_DFP754_BID_LITERALS || HAVE_DFP754_DPD_LITERALS */

#if !defined HAVE_STRTOD64 || !defined HAVE_CLEAN_STRTOD64
_Decimal64
strtod64(const char *src, char **on)
{
# if defined HAVE_DFP754_BID_LITERALS
	return strtobid64(src, on);
# elif defined HAVE_DFP754_DPD_LITERALS
	return strtodpd64(src, on);
# endif	 /* HAVE_DFP754_*_LITERALS */
}
#endif	/* !HAVE_STRTOD64 || !HAVE_CLEAN_STRTOD64 */


#if defined HAVE_DFP754_BID_LITERALS
static int
bid64tostr(char *restrict buf, size_t bsz, _Decimal64 x)
{
/* d64s look like s??eeeeeeee mm..53..mm
 * and the decimal is (-1 * s) * m * 10^(e - 398),
 * this implementation is very minimal serving only the cattle use cases */
	int e;
	int s;
	uint_least64_t m;

	/* get the exponent, sign and mantissa */
	e = quantexpbid64(x);
	m = mant_bid64(x);
	s = m ? sign_bid64(x) : 0/*no stinking signed naughts*/;

	/* reencode m as bcd */
	with (uint_least64_t bcdm = 0U) {
		for (size_t i = 0; i < 16U; i++) {
			uint_least64_t digit;

			bcdm >>= 4U;
			digit = m % 10U, m /= 10U;
			bcdm ^= digit << 60U;
		}
		m = bcdm;
	}
	return (int)bcd64tostr(buf, bsz, (bcd64_t){m, e, s});
}
#elif defined HAVE_DFP754_DPD_LITERALS
static int
dpd64tostr(char *restrict buf, size_t bsz, _Decimal64 x)
{
/* d64s look like s??eeeeeeee mm..53..mm
 * and the decimal is (-1 * s) * m * 10^(e - 398),
 * this implementation is very minimal serving only the cattle use cases */
	int e;
	int s;
	uint_least64_t m;

	/* get the exponent, sign and mantissa */
	e = quantexpdpd64(x);
	if (LIKELY((m = mant_dpd64(x)))) {
		uint_least64_t b;

		s = sign_dpd64(x);
		/* get us a proper bcd version of M */
		b = (m >> 50U);
		b <<= 12U;
		b ^= unpack_declet((m >> 40U) & 0x3ffU);
		b <<= 12U;
		b ^= unpack_declet((m >> 30U) & 0x3ffU);
		b <<= 12U;
		b ^= unpack_declet((m >> 20U) & 0x3ffU);
		b <<= 12U;
		b ^= unpack_declet((m >> 10U) & 0x3ffU);
		b <<= 12U;
		b ^= unpack_declet((m >> 0U) & 0x3ffU);
		m = b;
	} else {
		/* no stinking signed 0s and m is in bcd form already */
		s = 0;
	}
	return bcd64tostr(buf, bsz, (bcd64_t){m, e, s});
}
#endif	/* HAVE_DFP754_BID_LITERALS || HAVE_DFP754_DPD_LITERALS */

int
d64tostr(char *restrict buf, size_t bsz, _Decimal64 x)
{
	if (UNLIKELY(isnand64(x))) {
		const size_t z = min_z(3U, bsz);
		memcpy(buf, "nan", z);
		return z;
	} else if (UNLIKELY(isinfd64(x))) {
		const size_t z = min_z(3U + (x < 0.df), bsz);
		buf[0U] = '-';
		memcpy(buf + (x < 0.df), "inf", z);
		return z;
	}
#if defined HAVE_DFP754_BID_LITERALS
	return bid64tostr(buf, bsz, x);
#elif defined HAVE_DFP754_DPD_LITERALS
	return dpd64tostr(buf, bsz, x);
#endif	 /* HAVE_DFP754_*_LITERALS */
}

/* always use our own version,
 * the official version would return NAN in case the significand's
 * capacity is exceeded, we do fuckall in that case. */
_Decimal64
quantized64(_Decimal64 x, _Decimal64 r)
{
#if defined HAVE_DFP754_BID_LITERALS
	return quantizebid64(x, r);
#elif defined HAVE_DFP754_DPD_LITERALS
	/* this one's missing */
	return quantizedpd64(x, r);
#endif	 /* HAVE_DFP754_*_LITERALS */
}

#if !defined HAVE_SCALBND64
_Decimal64
scalbnd64(_Decimal64 x, int n)
{
# if defined HAVE_DFP754_BID_LITERALS
	return scalbnbid64(x, n);
# elif defined HAVE_DFP754_DPD_LITERALS
	return scalbndpd64(x, n);
# endif	 /* HAVE_DFP754_*_LITERALS */
}
#endif	/* !HAVE_SCALBND64 */


/* non-standard stuff */
bcd64_t
decompd64(_Decimal64 x)
{
#if defined HAVE_DFP754_BID_LITERALS
	return (bcd64_t){mant_bid64(x), quantexpbid64(x), sign_bid64(x)};
#elif defined HAVE_DFP754_DPD_LITERALS
	return (bcd64_t){mant_dpd64(x), quantexpdpd64(x), sign_dpd64(x)};
#endif	/* HAVE_DFP754_*_LITERALS */
}

/* dfp754_d64.c ends here */
