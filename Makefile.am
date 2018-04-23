include $(top_builddir)/version.mk

LANG = C
LC_ALL = C

ACLOCAL_AMFLAGS = -I m4

SUBDIRS =
EXTRA_DIST = $(DISTCLEANFILES) $(doc_DATA)
DISTCLEANFILES =
CLEANFILES =
doc_DATA =

pkgconfigdir = @libdir@/pkgconfig
pkgconfig_DATA =
pkgconfig_DATA += clob.pc
DISTCLEANFILES += $(pkgconfig_DATA)
EXTRA_DIST += clob.pc.in

SUBDIRS += build-aux
SUBDIRS += src
SUBDIRS += test

DISTCLEANFILES += version.mk
DISTCLEANFILES += .version
EXTRA_DIST += version.mk.in

## make sure .version is read-only in the dist
dist-hook:
	chmod ugo-w $(distdir)/.version

clob.pc: .version clob.pc.in
	$(AM_V_GEN) PATH="$(top_builddir)/build-aux:$${PATH}" \
		yuck$(EXEEXT) scmver -n \
			--reference .version $(srcdir)/clob.pc.in | \
		$(SHELL) "$(builddir)/config.status" --file=- > $@

## Makefile.am ends here
