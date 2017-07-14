# Help the Developers and yourself. Just use the C locale and settings
# for the compilation. They can still be overriden by make LANG=<whatever>
# but that is general a not very good idea
include $(top_builddir)/version.mk

LANG = C
LC_ALL = C

AM_CFLAGS = $(EXTRA_CFLAGS)
AM_CPPFLAGS = -D_POSIX_C_SOURCE=200112L -D_XOPEN_SOURCE=600 -D_BSD_SOURCE
AM_CPPFLAGS += -DTEST
AM_CPPFLAGS += $(clob_CFLAGS)

EXTRA_DIST = $(BUILT_SOURCES) $(TESTS)
TESTS =
TEST_EXTENSIONS =
BUILT_SOURCES =
dt_tests =
bin_tests =
batch_tests =
check_PROGRAMS =
CLEANFILES = $(check_PROGRAMS)

clob_CFLAGS = -I$(top_srcdir)/src $(dfp754_CFLAGS)
clob_LIBS = $(top_builddir)/src/libclob.a $(top_builddir)/src/libdfp.a $(dfp754_LIBS)


check_PROGRAMS += clob_01
TESTS += clob_01
clob_01_LDADD = $(clob_LIBS)

check_PROGRAMS += clob_02
TESTS += clob_02
clob_02_LDADD = $(clob_LIBS)

check_PROGRAMS += clob_03
TESTS += clob_03
clob_03_LDADD = $(clob_LIBS)

check_PROGRAMS += clob_04
TESTS += clob_04
clob_04_LDADD = $(clob_LIBS)

check_PROGRAMS += clob_05
TESTS += clob_05
clob_05_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_xetra_01
TESTS += unxs_xetra_01
unxs_xetra_01_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_xetra_02
TESTS += unxs_xetra_02
unxs_xetra_02_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_xetra_03
TESTS += unxs_xetra_03
unxs_xetra_03_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_xetra_04
TESTS += unxs_xetra_04
unxs_xetra_04_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_xetra_05
TESTS += unxs_xetra_05
unxs_xetra_05_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_xetra_06
TESTS += unxs_xetra_06
unxs_xetra_06_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_xetra_07
TESTS += unxs_xetra_07
unxs_xetra_07_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_xetra_08
TESTS += unxs_xetra_08
unxs_xetra_08_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_xetra_09
TESTS += unxs_xetra_09
unxs_xetra_09_LDADD = $(clob_LIBS)


check_PROGRAMS += mmod_auction_xetra_01
TESTS += mmod_auction_xetra_01
mmod_auction_xetra_01_LDADD = $(clob_LIBS)

check_PROGRAMS += mmod_auction_xetra_02
TESTS += mmod_auction_xetra_02
mmod_auction_xetra_02_LDADD = $(clob_LIBS)

check_PROGRAMS += mmod_auction_xetra_03
TESTS += mmod_auction_xetra_03
mmod_auction_xetra_03_LDADD = $(clob_LIBS)

check_PROGRAMS += mmod_auction_xetra_04
TESTS += mmod_auction_xetra_04
mmod_auction_xetra_04_LDADD = $(clob_LIBS)

check_PROGRAMS += mmod_auction_xetra_05
TESTS += mmod_auction_xetra_05
mmod_auction_xetra_05_LDADD = $(clob_LIBS)

check_PROGRAMS += mmod_auction_xetra_06
TESTS += mmod_auction_xetra_06
mmod_auction_xetra_06_LDADD = $(clob_LIBS)

check_PROGRAMS += mmod_auction_xetra_07
TESTS += mmod_auction_xetra_07
mmod_auction_xetra_07_LDADD = $(clob_LIBS)

check_PROGRAMS += mmod_auction_xetra_08
TESTS += mmod_auction_xetra_08
mmod_auction_xetra_08_LDADD = $(clob_LIBS)

check_PROGRAMS += mmod_auction_xetra_09
TESTS += mmod_auction_xetra_09
mmod_auction_xetra_09_LDADD = $(clob_LIBS)


check_PROGRAMS += unxs_order_01
TESTS += unxs_order_01
unxs_order_01_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_order_02
TESTS += unxs_order_02
unxs_order_02_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_order_03
TESTS += unxs_order_03
unxs_order_03_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_order_04
TESTS += unxs_order_04
unxs_order_04_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_order_05
TESTS += unxs_order_05
unxs_order_05_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_order_06
TESTS += unxs_order_06
unxs_order_06_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_order_07
TESTS += unxs_order_07
unxs_order_07_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_order_11
TESTS += unxs_order_11
unxs_order_11_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_order_12
TESTS += unxs_order_12
unxs_order_12_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_order_13
TESTS += unxs_order_13
unxs_order_13_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_order_14
TESTS += unxs_order_14
unxs_order_14_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_order_15
TESTS += unxs_order_15
unxs_order_15_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_order_16
TESTS += unxs_order_16
unxs_order_16_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_order_17
TESTS += unxs_order_17
unxs_order_17_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_order_18
TESTS += unxs_order_18
unxs_order_18_LDADD = $(clob_LIBS)

check_PROGRAMS += unxs_order_19
TESTS += unxs_order_19
unxs_order_19_LDADD = $(clob_LIBS)


check_PROGRAMS += plqu_01
TESTS += plqu_01
plqu_01_LDADD = $(clob_LIBS)


check_PROGRAMS += clob_stress_01
clob_stress_01_LDADD = $(clob_LIBS)

check_PROGRAMS += clob_stress_02
clob_stress_02_LDADD = $(clob_LIBS)

check_PROGRAMS += clob_stress_03
clob_stress_03_LDADD = $(clob_LIBS)

check_PROGRAMS += clob_stress_04
clob_stress_04_LDADD = $(clob_LIBS)

check_PROGRAMS += clob_stress_05
clob_stress_05_LDADD = $(clob_LIBS)

check_PROGRAMS += clob_stress_06
clob_stress_06_LDADD = $(clob_LIBS)

check_PROGRAMS += cloe_01
TESTS += cloe_01
cloe_01_LDADD = $(clob_LIBS)

check_PROGRAMS += cloe_02
TESTS += cloe_02
cloe_02_LDADD = $(clob_LIBS)

check_PROGRAMS += cloe_03
TESTS += cloe_03
cloe_03_LDADD = $(clob_LIBS)

check_PROGRAMS += cloe_04
TESTS += cloe_04
cloe_04_LDADD = $(clob_LIBS)

check_PROGRAMS += cloe_05
TESTS += cloe_05
cloe_05_LDADD = $(clob_LIBS)

check_PROGRAMS += cloe_06
TESTS += cloe_06
cloe_06_LDADD = $(clob_LIBS)

check_PROGRAMS += cloe_07
TESTS += cloe_07
cloe_07_LDADD = $(clob_LIBS)

check_PROGRAMS += cloe_08
TESTS += cloe_08
cloe_08_LDADD = $(clob_LIBS)

check_PROGRAMS += cloe_09
TESTS += cloe_09
cloe_09_LDADD = $(clob_LIBS)

check_PROGRAMS += cloe_10
TESTS += cloe_10
cloe_10_LDADD = $(clob_LIBS)

check_PROGRAMS += cloe_11
TESTS += cloe_11
cloe_11_LDADD = $(clob_LIBS)

check_PROGRAMS += cloe_12
TESTS += cloe_12
cloe_12_LDADD = $(clob_LIBS)

check_PROGRAMS += cloe_13
TESTS += cloe_13
cloe_13_LDADD = $(clob_LIBS)

check_PROGRAMS += cloe_14
TESTS += cloe_14
cloe_14_LDADD = $(clob_LIBS)

check_PROGRAMS += cloe_15
TESTS += cloe_15
cloe_15_LDADD = $(clob_LIBS)

## Makefile.am ends here
