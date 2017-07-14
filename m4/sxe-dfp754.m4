dnl sxe-dfp754.m4 --- decimal32 goodies
dnl
dnl Copyright (C) 2013-2017 Sebastian Freundt
dnl
dnl Author: Sebastian Freundt <hroptatyr@sxemacs.org>
dnl
dnl Redistribution and use in source and binary forms, with or without
dnl modification, are permitted provided that the following conditions
dnl are met:
dnl
dnl 1. Redistributions of source code must retain the above copyright
dnl    notice, this list of conditions and the following disclaimer.
dnl
dnl 2. Redistributions in binary form must reproduce the above copyright
dnl    notice, this list of conditions and the following disclaimer in the
dnl    documentation and/or other materials provided with the distribution.
dnl
dnl 3. Neither the name of the author nor the names of any contributors
dnl    may be used to endorse or promote products derived from this
dnl    software without specific prior written permission.
dnl
dnl THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR
dnl IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
dnl WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
dnl DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
dnl FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
dnl CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
dnl SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
dnl BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
dnl WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
dnl OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
dnl IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
dnl
dnl This file is part of truffle.

AC_DEFUN([_SXE_CHECK_DFP754_LITERAL_FLAVOUR], [dnl
	AC_MSG_CHECKING([for dfp754 flavour of float literals])

	save_CPPFLAGS="${CPPFLAGS}"
	save_LDFLAGS="${LDFLAGS}"
	CPPFLAGS="${CPPFLAGS} ${dfp754_CFLAGS}"
	LDFLAGS="${LDFLAGS} ${dfp754_LIBS}"

	AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <stdint.h>
#include <stdio.h>
]], [[
		_Decimal32 naught = 0.df;
		uint32_t x = (union {_Decimal32 x; uint32_t u;}){naught}.u;

		switch (x) {
		default:
			printf("%x  ", x);
			return 0;
		case 0b00110010100000000000000000000000U:
			/* bid 0.df */
			return 96;
		case 0b00110010000000000000000000000000U:
			/* bid 0.0df */
			return 97;
		case 0b00100010010100000000000000000000U:
			/* dpd 0.df */
			return 98;
		case 0b00100010010000000000000000000000U:
			/* dpd 0.0df */
			return 99;
		}
]])], [
		sxe_cv_feat_dfp754_literal_flavour="unknown"
		$2
	], [
		case "$?" in
		(96)
			sxe_cv_feat_dfp754_literal_flavour="bid"
			;;
		(97)
			sxe_cv_feat_dfp754_literal_flavour="bid"
			;;
		(98)
			sxe_cv_feat_dfp754_literal_flavour="dpd"
			;;
		(99)
			sxe_cv_feat_dfp754_literal_flavour="dpd"
			;;
		(*)
			sxe_cv_feat_dfp754_literal_flavour="unknown"
			;;
		esac
		$1
	])

	CPPFLAGS="${save_CPPFLAGS}"
	LDFLAGS="${save_LDFLAGS}"

	case "${sxe_cv_feat_dfp754_literal_flavour}" in
	("bid")
		AC_DEFINE([HAVE_DFP754_BID_LITERALS], [1],
			[Whether dfloat literals are bid])
		;;
	("dpd")
		AC_DEFINE([HAVE_DFP754_DPD_LITERALS], [1],
			[Whether dfloat literals are dpd])
		;;
	(*)
		sxe_cv_feat_dfp754_literal_flavour="unknown"
		;;
	esac

	AC_MSG_RESULT([${sxe_cv_feat_dfp754_literal_flavour}])
])dnl _SXE_CHECK_DFP754_LITERAL_FLAVOUR

AC_DEFUN([_SXE_CHECK_DFP754_CAST_FLAVOUR], [dnl
	AC_MSG_CHECKING([for dfp754 flavour of float casts])

	save_CPPFLAGS="${CPPFLAGS}"
	save_LDFLAGS="${LDFLAGS}"
	CPPFLAGS="${CPPFLAGS} ${dfp754_CFLAGS}"
	LDFLAGS="${LDFLAGS} ${dfp754_LIBS}"

	AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <stdint.h>
#include <stdio.h>
]], [[
		_Decimal32 half = (_Decimal32)0.5f;
		uint32_t x = (union {_Decimal32 x; uint32_t u;}){half}.u;

		switch (x) {
		default:
			printf("%x  ", x);
			return 0;
		case 0b00110010000000000000000000000101U:
			/* bid 0.5df */
			return 96;
		case 0b00110001100000000000000000110010U:
			/* bid 0.50df */
			return 97;
		case 0b00100010010000000000000000000101U:
			/* dpd 0.5df */
			return 98;
		case 0b00100010001100000000000001010000U:
			/* dpd 0.50df */
			return 99;
		}
]])], [
		sxe_cv_feat_dfp754_cast_flavour="unknown"
		$2
	], [
		case "$?" in
		(96)
			sxe_cv_feat_dfp754_cast_flavour="bid"
			;;
		(97)
			sxe_cv_feat_dfp754_cast_flavour="bid"
			;;
		(98)
			sxe_cv_feat_dfp754_cast_flavour="dpd"
			;;
		(99)
			sxe_cv_feat_dfp754_cast_flavour="dpd"
			;;
		(*)
			sxe_cv_feat_dfp754_cast_flavour="unknown"
			;;
		esac
		$1
	])

	CPPFLAGS="${save_CPPFLAGS}"
	LDFLAGS="${save_LDFLAGS}"

	case "${sxe_cv_feat_dfp754_cast_flavour}" in
	("bid")
		AC_DEFINE([HAVE_DFP754_BID_CAST], [1],
			[Whether dfloat casts are bid])
		;;
	("dpd")
		AC_DEFINE([HAVE_DFP754_DPD_CAST], [1],
			[Whether dfloat casts are dpd])
		;;
	(*)
		sxe_cv_feat_dfp754_cast_flavour="unknown"
		;;
	esac

	AC_MSG_RESULT([${sxe_cv_feat_dfp754_cast_flavour}])
])dnl _SXE_CHECK_DFP754_CAST_FLAVOUR

AC_DEFUN([_SXE_CHECK_DFP754_ARITH_FLAVOUR], [dnl
	AC_MSG_CHECKING([for dfp754 flavour of float arithmetic])

	save_CPPFLAGS="${CPPFLAGS}"
	save_LDFLAGS="${LDFLAGS}"
	CPPFLAGS="${CPPFLAGS} ${dfp754_CFLAGS}"
	LDFLAGS="${LDFLAGS} ${dfp754_LIBS}"

	AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <stdint.h>
#include <stdio.h>
]], [[
		_Decimal32 naught = (_Decimal32)0.f;
		_Decimal32 one = 1.df;
		_Decimal32 sum = naught + one;
		uint32_t x = (union {_Decimal32 x; uint32_t u;}){sum}.u;

		switch (x) {
		default:
			printf("%x  ", x);
			return 0;
		case 0b00110010100000000000000000000001U:
			/* bid 1.df */
			return 96;
		case 0b00110010000000000000000000001010U:
			/* bid 1.0df */
			return 97;
		case 0b00100010010100000000000000000001U:
			/* dpd 1.df */
			return 98;
		case 0b00100010010000000000000000010000U:
			/* dpd 1.0df */
			return 99;
		}
]])], [
		sxe_cv_feat_dfp754_arith_flavour="unknown"
		$2
	], [
		case "$?" in
		(96)
			sxe_cv_feat_dfp754_arith_flavour="bid"
			;;
		(97)
			sxe_cv_feat_dfp754_arith_flavour="bid"
			;;
		(98)
			sxe_cv_feat_dfp754_arith_flavour="dpd"
			;;
		(99)
			sxe_cv_feat_dfp754_arith_flavour="dpd"
			;;
		(*)
			sxe_cv_feat_dfp754_arith_flavour="unknown"
			;;
		esac
		$1
	])

	CPPFLAGS="${save_CPPFLAGS}"
	LDFLAGS="${save_LDFLAGS}"

	case "${sxe_cv_feat_dfp754_arith_flavour}" in
	("bid")
		AC_DEFINE([HAVE_DFP754_BID_ARITH], [1],
			[Whether dfloat arithmetic is bid])
		;;
	("dpd")
		AC_DEFINE([HAVE_DFP754_DPD_ARITH], [1],
			[Whether dfloat arithmetic is dpd])
		;;
	(*)
		sxe_cv_feat_dfp754_arith_flavour="unknown"
		;;
	esac

	AC_MSG_RESULT([${sxe_cv_feat_dfp754_arith_flavour}])
])dnl _SXE_CHECK_DFP754_ARITH_FLAVOUR

AC_DEFUN([_SXE_CHECK_DFP754_LITERALS], [dnl
	AC_MSG_CHECKING([whether dfp754 literals work])

	save_CPPFLAGS="${CPPFLAGS}"
	save_LDFLAGS="${LDFLAGS}"
	CPPFLAGS="${CPPFLAGS} ${dfp754_CFLAGS}"
	LDFLAGS="${LDFLAGS} ${dfp754_LIBS}"

	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <stdint.h>
#include <stdio.h>
]], [[
	_Decimal32 one = 1.df;
	_Decimal32 xxx = 12.9df;

	if (one + xxx == 13.9df) {
		return 0;
	}
	return 1;
]])], [
	sxe_cv_feat_dfp754_literals="yes"
	$2
], [
	sxe_cv_feat_dfp754_literals="no"
	$3
])

	CPPFLAGS="${save_CPPFLAGS}"
	LDFLAGS="${save_LDFLAGS}"

	AC_MSG_RESULT([${sxe_cv_feat_dfp754_literals}])
])dnl _SXE_CHECK_DFP754_LITERALS

AC_DEFUN([_SXE_CHECK_DFP754_FLAGS], [dnl
## checks for defines and libs needed to get the dfp stuff going
## defines dfp754_CFLAGS and dfp754_LIBS
	AC_ARG_VAR([dfp754_CFLAGS], [C compiler flags for _Decimal32 support])
	AC_ARG_VAR([dfp754_LIBS], [linker flags for _Decimal32 support])

	AC_CHECK_HEADERS([math.h])
	AC_CHECK_HEADERS([dfp754.h])
	AC_CHECK_HEADERS([dfp/stdlib.h])

	if test "${ac_cv_env_dfp754_CFLAGS_set}" = "set"; then
		:
	else
		dfp754_CFLAGS="-D__STDC_WANT_DEC_FP__"
	fi
])dnl _SXE_CHECK_SOURCE_DEFS

AC_DEFUN([_SXE_CHECK_DFP754_STRTOD], [dnl
	pushdef([strtod], [$1])

	AC_REQUIRE([_SXE_CHECK_DFP754_HEADERS])

	save_CPPFLAGS="${CPPFLAGS}"
	save_LDFLAGS="${LDFLAGS}"
	CPPFLAGS="${CPPFLAGS} ${dfp754_CFLAGS}"
	LDFLAGS="${LDFLAGS} ${dfp754_LIBS}"

	AC_MSG_CHECKING([whether ]strtod[ is clean])
	AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <stdint.h>
#include <stdio.h>
#if defined HAVE_DFP754_H
# include <dfp754.h>
#endif
#if defined HAVE_DFP_STDLIB_H
# include <dfp/stdlib.h>
#endif
#if defined HAVE_DECIMAL_H
# include <decimal.h>
#endif
]], [[
	static char test[] = "0xxx";
	char *endptr;

	(void)]strtod[(test, &endptr);
	if (endptr != test + 1U) {
		return 1;
	}
]])], [
	sxe_cv_func_]strtod[_clean="yes"
	AC_DEFINE(AS_TR_CPP([HAVE_CLEAN_]strtod), [1],
		[Whether ]strtod[ has endptr pointing to an element of nptr])
	$2
], [
	sxe_cv_func_]strtod[_clean="no"
	$3
])
	CPPFLAGS="${save_CPPFLAGS}"
	LDFLAGS="${save_LDFLAGS}"

	AC_MSG_RESULT([${sxe_cv_func_]strtod[_clean}])
	popdef([strtod])
])dnl _SXE_CHECK_DFP754_STRTOD

AC_DEFUN([_SXE_CHECK_DFP754_BUILTIN], [dnl
	pushdef([fun], [$1])
	pushdef([typ], [$2])
	pushdef([arg], m4_ifblank([$3], [()], [$3]))

	save_CPPFLAGS="${CPPFLAGS}"
	save_LDFLAGS="${LDFLAGS}"
	CPPFLAGS="${CPPFLAGS} ${dfp754_CFLAGS}"
	LDFLAGS="${LDFLAGS} ${dfp754_LIBS}"

	AC_MSG_CHECKING([for __builtin_]fun)
	AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <stdint.h>
#include <stdio.h>
#if defined HAVE_DFP754_H
# include <dfp754.h>
#endif
#if defined HAVE_DFP_STDLIB_H
# include <dfp/stdlib.h>
#endif
#if defined HAVE_DECIMAL_H
# include <decimal.h>
#endif
]], [[
	static ]typ[ X = __builtin_]fun[]arg[;
	(void)X;
]])], [
	sxe_cv_builtin_]fun[="yes"
	AC_MSG_RESULT([${sxe_cv_builtin_]fun[}])
	AC_DEFINE(AS_TR_CPP([HAVE_BUILTIN_]fun), [1],
		[Whether __builtin_]fun[ works and can initialise globals])
	$4
], [
	sxe_cv_builtin_]fun[="no"
	AC_MSG_RESULT([${sxe_cv_builtin_]fun[}])
	$5
])

	CPPFLAGS="${save_CPPFLAGS}"
	LDFLAGS="${save_LDFLAGS}"

	popdef([fun])
	popdef([typ])
	popdef([arg])
])dnl _SXE_CHECK_DFP754_BUILTIN

AC_DEFUN([_SXE_CHECK_DFP754_BUILTIN_BANG], [dnl
	pushdef([fun], [$1])
	pushdef([typ], [$2])
	pushdef([bng], [$3])
	pushdef([arg], m4_ifblank([$4], [()], [$4]))

	save_CPPFLAGS="${CPPFLAGS}"
	save_LDFLAGS="${LDFLAGS}"
	CPPFLAGS="${CPPFLAGS} ${dfp754_CFLAGS}"
	LDFLAGS="${LDFLAGS} ${dfp754_LIBS}"

	AC_MSG_CHECKING([whether __builtin_]bng[ can be used to mimic __builtin_]fun[])
	AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <stdint.h>
#include <stdio.h>
#if defined HAVE_DFP754_H
# include <dfp754.h>
#endif
#if defined HAVE_DFP_STDLIB_H
# include <dfp/stdlib.h>
#endif
#if defined HAVE_DECIMAL_H
# include <decimal.h>
#endif
]], [[
	static ]typ[ X = (]typ[)__builtin_]bng[]arg[;
	if (!is]fun[(X)) {
		return 1;
	}
]])], [
	sxe_cv_builtin_]bng[_for_]fun[="yes"
	AC_MSG_RESULT([${sxe_cv_builtin_]bng[_for_]fun[}])
	AC_DEFINE(AS_TR_CPP([HAVE_BUILTIN_]bng[_FOR_]fun), [1],
		[Whether __builtin_]bng[ can be used to initialise ]typ[ globals])
	$5
], [
	sxe_cv_builtin_]bng[_for_]fun[="no"
	AC_MSG_RESULT([${sxe_cv_builtin_]bng[_for_]fun[}])
	$6
])

	CPPFLAGS="${save_CPPFLAGS}"
	LDFLAGS="${save_LDFLAGS}"

	popdef([fun])
	popdef([typ])
	popdef([bng])
	popdef([arg])
])dnl _SXE_CHECK_DFP754_BUILTIN_BANG

AC_DEFUN([_SXE_CHECK_DFP754_HEADERS], [dnl
	AC_CHECK_HEADERS([dfp754.h])
	AC_CHECK_HEADERS([dfp/stdlib.h])
	AC_CHECK_HEADERS([decimal.h])
])dnl _SXE_CHECK_DFP754_HEADERS


AC_DEFUN([_SXE_CHECK_DFP754_SYMBOLS], [dnl
	AC_CHECK_FUNCS([strtod32])
	AC_CHECK_FUNCS([quantized32])
	AC_CHECK_FUNCS([scalbnd32])

	AC_CHECK_FUNCS([strtod64])
	AC_CHECK_FUNCS([quantized64])
	AC_CHECK_FUNCS([scalbnd64])

	## see if strtod64/strtod32 are clean
	if test "${ac_cv_func_strtod32}" = "yes"; then
		_SXE_CHECK_DFP754_STRTOD([strtod32])
	fi
	if test "${ac_cv_func_strtod64}" = "yes"; then
		_SXE_CHECK_DFP754_STRTOD([strtod64])
	fi

	save_LDFLAGS="${LDFLAGS}"
	LDFLAGS="${LDFLAGS} -lm"
	AC_CHECK_FUNCS([nand32])
	AC_CHECK_FUNCS([isnand32])

	AC_CHECK_FUNCS([nand64])
	AC_CHECK_FUNCS([isnand64])
	LDFLAGS="${save_LDFLAGS}"
])dnl _SXE_CHECK_DFP754_SYMBOLS

AC_DEFUN([_SXE_CHECK_DFP754_BUILTINS], [dnl
	_SXE_CHECK_DFP754_BUILTIN([infd32], [_Decimal32], [], [:], [
		_SXE_CHECK_DFP754_BUILTIN_BANG([infd32], [_Decimal32], [inf])])
	_SXE_CHECK_DFP754_BUILTIN([infd64], [_Decimal64], [], [:], [
		_SXE_CHECK_DFP754_BUILTIN_BANG([infd64], [_Decimal64], [inf])])
	_SXE_CHECK_DFP754_BUILTIN([nand32], [_Decimal32], [("")], [:], [
		_SXE_CHECK_DFP754_BUILTIN_BANG([nand32], [_Decimal32], [nan], [("")])])
	_SXE_CHECK_DFP754_BUILTIN([nand64], [_Decimal64], [("")], [:], [
		_SXE_CHECK_DFP754_BUILTIN_BANG([nand64], [_Decimal64], [nan], [("")])])
])dnl _SXE_CHECK_DFP754_BUILTINS

AC_DEFUN([SXE_CHECK_DFP754], [dnl
	AC_REQUIRE([_SXE_CHECK_DFP754_FLAGS])
	AC_REQUIRE([_SXE_CHECK_DFP754_LITERALS])
	AC_REQUIRE([_SXE_CHECK_DFP754_LITERAL_FLAVOUR])
	AC_REQUIRE([_SXE_CHECK_DFP754_CAST_FLAVOUR])
	AC_REQUIRE([_SXE_CHECK_DFP754_ARITH_FLAVOUR])

	AC_REQUIRE([_SXE_CHECK_DFP754_HEADERS])
	AC_REQUIRE([_SXE_CHECK_DFP754_SYMBOLS])
	AC_REQUIRE([_SXE_CHECK_DFP754_BUILTINS])
])dnl SXE_CHECK_DFP754

dnl sxe-dfp754.m4 ends here
