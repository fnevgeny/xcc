# Begin config

CC = gcc
CPPFLAGS = -I.
CFLAGS = -g -ansi -pedantic -Wall -W -Wpointer-arith
LDFLAGS =

AR = ar cru

MKINSTALLDIRS = mkdir -p
INSTALL_PROGRAM = install -c
INSTALL_DATA = install -c -m 644

bindir = /usr/local/bin
libdir = /usr/local/lib
incdir = /usr/local/include

LIBS = -lexpat

# End config

XCC_XCC = xcc.xcc

BPROG  = bxcc
B2PROG = b2xcc
PROG   = xcc

BOBJS  = bxcc.o
B2OBJS = b2xcc.o
OBJS   = xcc.o

LOBJS  = libxcc.o

XCCLIB = libxcc.a	

TMP_C = t.c

all: $(PROG)

$(XCCLIB): $(LOBJS)
	$(AR) $@ $(LOBJS)

$(BPROG): $(BOBJS) $(XCCLIB)
	$(CC) $(LDFLAGS) -o $@ $(BOBJS) $(XCCLIB) $(LIBS)
$(B2PROG): $(B2OBJS) $(XCCLIB)
	$(CC) $(LDFLAGS) -o $@ $(B2OBJS) $(XCCLIB) $(LIBS)
$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(XCCLIB) $(LIBS)

b2xcc.c: $(XCC_XCC) $(BPROG)
	./$(BPROG) < $(XCC_XCC) > $(TMP_C) && mv -f $(TMP_C) $@
xcc.c:  $(XCC_XCC) $(B2PROG)
	./$(B2PROG) < $(XCC_XCC) > $(TMP_C) && mv -f $(TMP_C) $@

clean:
	rm -f $(BPROG) $(B2PROG) $(PROG) \
	$(BOBJS) $(B2OBJS) $(OBJS) $(LOBJS) $(XCCLIB) \
	xcc.c b2xcc.c $(TMP_C)

install: $(XCCLIB) $(PROG)
	$(MKINSTALLDIRS) $(bindir)
	$(INSTALL_PROGRAM) -s $(PROG) $(bindir)
	$(MKINSTALLDIRS) $(libdir)
	$(INSTALL_DATA) $(XCCLIB) $(libdir)
	$(MKINSTALLDIRS) $(incdir)
	$(INSTALL_DATA) xcc.h $(incdir)

check: b2xcc.c xcc.c
	@echo -n "Testing self-consistency ... "
	@diff -q b2xcc.c xcc.c
	@if test $$? -ne 0; then echo "Failed"; else echo "OK"; fi

# Deps
libxcc.o: xcc.h
bxcc.o: xcc.h
b2xcc.o: xcc.h
xcc.o: xcc.h
