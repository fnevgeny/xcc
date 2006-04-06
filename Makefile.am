AUTOMAKE_OPTIONS = 1.7

SUBDIRS				= . examples

bin_PROGRAMS			= xcc
noinst_PROGRAMS			= bxcc b2xcc
LDADD				= libxcc.la
lib_LTLIBRARIES			= libxcc.la
libxcc_la_SOURCES		= libxcc.c
scripts				= c2cstr.sh

BUILT_SOURCES			= b2xcc.c xcc.c xcc_t.c bundle.i
CLEANFILES			= *~

bxcc_SOURCES			= bxcc.c libexe.c xfile.c xfile.h xccP.h
bxcc.c:				 bundle.i

b2xcc_SOURCES			= b2xcc.c libexe.c xfile.c xfile.h xccP.h
b2xcc.c:  xcc.xcc bxcc
	./bxcc -l -i xcc.xcc -o $@

xcc_SOURCES			= xcc.c libexe.c xfile.c xfile.h xccP.h
xcc.c:  xcc.xcc b2xcc
	./b2xcc -i xcc.xcc -o $@

xcc_t.c:  xcc.xcc xcc
	./xcc -i xcc.xcc | sed '/^#line/s/stdout/xcc.c/g' > $@

bundle.i: xcc.h libxcc.c
	./c2cstr.sh xcc.h libxcc.c > $@

$(SCHEMA):  xcc.xcc xcc
	./xcc -i xcc.xcc -s -o $@

ChangeLog : configure.in
	cvs2cl

SCHEMA 		= xcc.xsd
schema_DATA	= $(SCHEMA)
schemadir	= $(pkgdatadir)
doc_DATA 	= README NEWS AUTHORS TODO
docdir  	= $(datadir)/doc/xcc

xcc_h_HEADERS	= xcc.h
xcc_hdir	= $(includedir)

man_MANS	= xcc.1

EXTRA_DIST	= $(doc_DATA) $(SCHEMA) $(bxcc_SOURCES) $(b2xcc_SOURCES) \
		  $(xcc_SOURCES) xcc.xcc c2cstr.sh bundle.i $(man_MANS) \
		  ChangeLog

MAINTAINERCLEANFILES = $(BUILT_SOURCES) $(SCHEMA) bxcc Makefile.in aclocal.m4 \
			config.h.in config.log config.status configure libtool \
			config.guess config.sub depcomp install-sh missing \
			mkinstalldirs ltmain.sh stamp-h* \
			ChangeLog*

all-local: $(SCHEMA)

check-local: xcc_t.c xcc.c
	@echo -n "Testing self-consistency ... "
	@diff -q xcc_t.c xcc.c
	@if test $$? -ne 0; then echo "Failed"; else echo "OK"; fi
