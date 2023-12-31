AUTOMAKE_OPTIONS = 1.7

SUBDIRS				= . examples

bin_PROGRAMS			= xcc$(EXEEXT)
noinst_PROGRAMS			= bxcc$(EXEEXT) b2xcc$(EXEEXT)
LDADD				= libxcc.la
lib_LTLIBRARIES			= libxcc.la
libxcc_la_SOURCES		= libxcc.c
scripts				= c2cstr.sh

BUILT_SOURCES			= b2xcc.c xcc.c xcc_t.c bundle.i
CLEANFILES			= $(BUILT_SOURCES) *~ 

bxcc_SOURCES			= bxcc.c libexe.c xfile.c xfile.h xccP.h

b2xcc_SOURCES			= b2xcc.c libexe.c xfile.c xfile.h xccP.h

xcc_SOURCES			= xcc.c libexe.c xfile.c xfile.h xccP.h

libexe.$(OBJEXT): bundle.i

b2xcc.c:  xcc.xcc bxcc$(EXEEXT)
	./bxcc$(EXEEXT) -l -i xcc.xcc -o $@

xcc.c:    xcc.xcc b2xcc$(EXEEXT)
	./b2xcc$(EXEEXT) -i xcc.xcc -o $@

xcc_t.c:  xcc.xcc xcc$(EXEEXT)
	./xcc$(EXEEXT) -i xcc.xcc | sed '/^#line/s/stdout/xcc.c/g' > $@

bundle.i: xcc.h libxcc.c
	./c2cstr.sh xcc.h libxcc.c > $@

$(SCHEMA):  xcc.xcc xcc$(EXEEXT)
	./xcc$(EXEEXT) -i xcc.xcc -s -o $@

ChangeLog: configure.ac NEWS
	git2cl > $@
VERSION: configure.ac
	echo $(VERSION) > $@

include version.mak

SCHEMA 		= xcc.xsd
schema_DATA	= $(SCHEMA)
schemadir	= $(pkgdatadir)
doc_DATA 	= README NEWS AUTHORS TODO
docdir  	= $(datadir)/doc/xcc

xcc_h_HEADERS	= xcc.h
xcc_hdir	= $(includedir)

man_MANS	= xcc.1
mandir		= $(prefix)/share/man

EXTRA_DIST	= $(doc_DATA) $(SCHEMA) $(bxcc_SOURCES) $(b2xcc_SOURCES) \
		  $(xcc_SOURCES) xcc.xcc c2cstr.sh $(man_MANS) \
		  LICENSE ChangeLog VERSION \
		  packaging/debian/control \
		  packaging/debian/copyright \
		  packaging/debian/rules \
		  packaging/debian/make-debs.sh \
		  packaging/debian/xcc.docs \
		  packaging/debian/xcc.files \
		  packaging/debian/xcc.manpages \
		  packaging/fedora/make-rpm.sh \
		  packaging/fedora/xcc.spec.in \
		  packaging/fedora/xcc.spec \
		  packaging/freebsd/Makefile.ports \
		  packaging/freebsd/make-freebsd-pkg.sh \
		  packaging/freebsd/pkg-descr \
		  packaging/freebsd/pkg-plist \
		  packaging/openbsd/Makefile.ports \
		  packaging/openbsd/make-openbsd-pkg.sh \
		  packaging/openbsd/pkg/DESCR \
		  packaging/openbsd/pkg/PLIST \
		  packaging/solaris/make-solaris-pkg.sh \
		  packaging/solaris/pkginfo \
		  packaging/solaris/prototype


MAINTAINERCLEANFILES = $(BUILT_SOURCES) $(SCHEMA) bxcc$(EXEEXT) Makefile.in \
			aclocal.m4 config.h.in config.log config.status \
			configure libtool config.guess config.sub depcomp \
			install-sh missing mkinstalldirs ltmain.sh *stamp* \
			ChangeLog* VERSION version.m4 *.tar.gz

all-local: $(SCHEMA)

check-local: xcc_t.c xcc.c
	@echo -n "Testing self-consistency ... "
	@diff -q --strip-trailing-cr xcc_t.c xcc.c
	@if test $$? -ne 0; then echo "Failed"; else echo "OK"; fi
