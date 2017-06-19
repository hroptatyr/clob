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

clob_CFLAGS = -I$(top_srcdir)/src
clob_LIBS = $(top_builddir)/src/libclob.a $(top_builddir)/src/libdfp.a


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

check_PROGRAMS += plqu_01
TESTS += plqu_01
plqu_01_LDADD = $(clob_LIBS)

## Makefile.am ends here
