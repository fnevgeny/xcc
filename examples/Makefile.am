noinst_PROGRAMS		= books
books_LDADD		= $(top_builddir)/libxcc.la
XCC 			= $(top_builddir)/xcc
SCHEMA 			= books.xsd
BUILT_SOURCES		= books.c
CLEANFILES		= *~
MAINTAINERCLEANFILES	= $(BUILT_SOURCES) $(SCHEMA) Makefile.in
books_SOURCES		= books.c libbooks.c books.h
books.c: books.xcc $(XCC)
	$(XCC) -i books.xcc -o $@

$(SCHEMA): books.xcc $(XCC)
	$(XCC) -i books.xcc -s -o $@

EXTRA_DIST		= books.xcc $(BUILT_SOURCES)

all-local: $(SCHEMA)
