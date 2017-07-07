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
AM_LDFLAGS = $(clob_LIBS)

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

check_PROGRAMS += clob_02
TESTS += clob_02

check_PROGRAMS += clob_03
TESTS += clob_03

check_PROGRAMS += clob_04
TESTS += clob_04

check_PROGRAMS += unxs_xetra_01
TESTS += unxs_xetra_01

check_PROGRAMS += unxs_xetra_02
TESTS += unxs_xetra_02

check_PROGRAMS += unxs_xetra_03
TESTS += unxs_xetra_03

check_PROGRAMS += unxs_xetra_04
TESTS += unxs_xetra_04

check_PROGRAMS += unxs_xetra_05
TESTS += unxs_xetra_05

check_PROGRAMS += unxs_xetra_06
TESTS += unxs_xetra_06

check_PROGRAMS += unxs_xetra_07
TESTS += unxs_xetra_07

check_PROGRAMS += unxs_xetra_08
TESTS += unxs_xetra_08

check_PROGRAMS += unxs_xetra_09
TESTS += unxs_xetra_09


check_PROGRAMS += mmod_auction_xetra_01
TESTS += mmod_auction_xetra_01

check_PROGRAMS += mmod_auction_xetra_02
TESTS += mmod_auction_xetra_02

check_PROGRAMS += mmod_auction_xetra_03
TESTS += mmod_auction_xetra_03

check_PROGRAMS += mmod_auction_xetra_04
TESTS += mmod_auction_xetra_04

check_PROGRAMS += mmod_auction_xetra_05
TESTS += mmod_auction_xetra_05

check_PROGRAMS += mmod_auction_xetra_06
TESTS += mmod_auction_xetra_06

check_PROGRAMS += mmod_auction_xetra_07
TESTS += mmod_auction_xetra_07

check_PROGRAMS += mmod_auction_xetra_08
TESTS += mmod_auction_xetra_08

check_PROGRAMS += mmod_auction_xetra_09
TESTS += mmod_auction_xetra_09


check_PROGRAMS += unxs_order_01
TESTS += unxs_order_01

check_PROGRAMS += unxs_order_02
TESTS += unxs_order_02

check_PROGRAMS += unxs_order_03
TESTS += unxs_order_03

check_PROGRAMS += unxs_order_04
TESTS += unxs_order_04

check_PROGRAMS += unxs_order_05
TESTS += unxs_order_05

check_PROGRAMS += unxs_order_06
TESTS += unxs_order_06

check_PROGRAMS += unxs_order_07
TESTS += unxs_order_07

check_PROGRAMS += unxs_order_11
TESTS += unxs_order_11

check_PROGRAMS += unxs_order_12
TESTS += unxs_order_12

check_PROGRAMS += unxs_order_13
TESTS += unxs_order_13

check_PROGRAMS += unxs_order_14
TESTS += unxs_order_14

check_PROGRAMS += unxs_order_15
TESTS += unxs_order_15

check_PROGRAMS += unxs_order_16
TESTS += unxs_order_16

check_PROGRAMS += unxs_order_17
TESTS += unxs_order_17

check_PROGRAMS += unxs_order_18
TESTS += unxs_order_18

check_PROGRAMS += unxs_order_19
TESTS += unxs_order_19


check_PROGRAMS += plqu_01
TESTS += plqu_01


check_PROGRAMS += clob_stress_01

check_PROGRAMS += clob_stress_02

check_PROGRAMS += clob_stress_03

check_PROGRAMS += clob_stress_04

check_PROGRAMS += clob_stress_05

check_PROGRAMS += clob_stress_06

check_PROGRAMS += cloe_01
TESTS += cloe_01

check_PROGRAMS += cloe_02
TESTS += cloe_02

check_PROGRAMS += cloe_03
TESTS += cloe_03

check_PROGRAMS += cloe_04
TESTS += cloe_04

check_PROGRAMS += cloe_05
TESTS += cloe_05

check_PROGRAMS += cloe_06
TESTS += cloe_06

check_PROGRAMS += cloe_07
TESTS += cloe_07

## Makefile.am ends here
