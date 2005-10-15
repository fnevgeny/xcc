# Top-level directory
TOP = ..

include $(TOP)/Make.conf

CPPFLAGS = -I. -I.. $(EXPAT_INC)
CFLAGS = $(DBG_CFLAGS) $(OPT_CFLAGS) $(LNT_CFLAGS)
LDFLAGS =

XCCLIB = $(TOP)/libxcc.a
LIBS = $(XCCLIB) $(EXPAT_LIB)

XCC = $(TOP)/xcc

BOOKS  = books

BOOKSOBJS = books.o libbooks.o

all: $(BOOKS)

books.c: books.xcc $(XCC)
	$(XCC) -i $< -o $@

$(BOOKS): $(BOOKSOBJS) $(XCCLIB)
	$(CC) $(LDFLAGS) -o $@ $(BOOKSOBJS) $(LIBS)

clean:
	rm -f $(BOOKS) $(BOOKSOBJS) books.c

install: 

uninstall:

check: 

# Deps
libbooks.o: books.h $(TOP)/xcc.h
books.o: books.h $(TOP)/xcc.h
