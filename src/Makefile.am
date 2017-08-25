### Makefile.am
include $(top_builddir)/version.mk

AM_CFLAGS = $(EXTRA_CFLAGS)
AM_CPPFLAGS = -D_POSIX_C_SOURCE=201001L -D_XOPEN_SOURCE=700 -D_BSD_SOURCE
AM_CPPFLAGS += -D_HAVE_VERSION_H
AM_LDFLAGS = $(XCCLDFLAGS)

bin_PROGRAMS =
noinst_PROGRAMS =
lib_LIBRARIES =
noinst_LIBRARIES =
noinst_HEADERS =
BUILT_SOURCES =
EXTRA_DIST = $(BUILT_SOURCES)
CLEANFILES = 
SUFFIXES =

CLEANFILES += version.c
EXTRA_DIST += version.c.in

noinst_LIBRARIES += libdfp.a
libdfp_a_SOURCES =
libdfp_a_SOURCES += dfp754_d32.c dfp754_d32.h
libdfp_a_SOURCES += dfp754_d64.c dfp754_d64.h
libdfp_a_CPPFLAGS = $(AM_CPPFLAGS)
libdfp_a_CPPFLAGS += $(dfp754_CFLAGS)

lib_LIBRARIES += libclob.a
libclob_a_SOURCES =
libclob_a_SOURCES += btree.c btree.h btree_val.h
libclob_a_SOURCES += plqu.c plqu.h plqu_val.h
libclob_a_SOURCES += clob.c clob.h clob_val.h
libclob_a_SOURCES += unxs.c unxs.h
libclob_a_SOURCES += mmod-auction.c mmod-auction.h
libclob_a_SOURCES += mmod-fok.c mmod-fok.h
libclob_a_SOURCES += mmod-ioc.c mmod-ioc.h
libclob_a_SOURCES += quos.c quos.h
libclob_a_SOURCES += nifty.h
libclob_a_CPPFLAGS = $(AM_CPPFLAGS)
libclob_a_CPPFLAGS += $(dfp754_CFLAGS)
libclob_a_LIBADD = $(dfp754_LIBS)
libclob_a_LIBADD += libdfp.a

pkginclude_HEADERS =
pkginclude_HEADERS += clob.h clob_val.h
pkginclude_HEADERS += unxs.h
pkginclude_HEADERS += mmod-auction.h
pkginclude_HEADERS += quos.h

bin_PROGRAMS += cloe
cloe_SOURCES = cloe.c cloe.yuck
cloe_CPPFLAGS = $(AM_CPPFLAGS)
cloe_CPPFLAGS += $(dfp754_CFLAGS)
cloe_LDFLAGS = $(AM_LDFLAGS)
cloe_LDFLAGS += $(dfp754_LIBS)
cloe_LDADD = libclob.a libdfp.a
BUILT_SOURCES = cloe.yucc


## version rules
version.c: version.c.in $(top_builddir)/.version
	$(AM_V_GEN) PATH="$(top_builddir)/build-aux:$${PATH}" \
		yuck$(EXEEXT) scmver --ignore-noscm --force -o $@ \
			--use-reference --reference $(top_builddir)/.version $<

## yuck rule
SUFFIXES += .yuck
SUFFIXES += .yucc
.yuck.yucc:
	$(AM_V_GEN) PATH=$(top_builddir)/build-aux:"$${PATH}" \
		yuck$(EXEEXT) gen -o $@ $<

## Create preprocessor output (debugging purposes only)
.c.i:
	$(COMPILE) -E -o $@ $<

## Create assembler output (debugging purposes only)
.c.s:
	$(COMPILE) -S -c $(AM_CFLAGS) $<
