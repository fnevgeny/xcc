# Top-level directory
TOP = .

include $(TOP)/Make.conf

bindir = $(PREFIX)/bin
libdir = $(PREFIX)/lib
incdir = $(PREFIX)/include

CPPFLAGS = -I. $(EXPAT_INC)
CFLAGS = $(DBG_CFLAGS) $(OPT_CFLAGS) $(LNT_CFLAGS)
LDFLAGS =

LIBS = $(EXPAT_LIB)

XCC_XCC = xcc.xcc

BPROG  = bxcc
B2PROG = b2xcc
PROG   = xcc

BOBJS  = bxcc.o libexe.o
B2OBJS = b2xcc.o libexe.o
OBJS   = xcc.o libexe.o

LOBJS  = libxcc.o

XCCLIB = libxcc.a	

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
	./$(BPROG) -i $(XCC_XCC) -o $@
xcc.c:  $(XCC_XCC) $(B2PROG)
	./$(B2PROG) -i $(XCC_XCC) -o $@

clean:
	rm -f $(BPROG) $(B2PROG) $(PROG) \
	$(BOBJS) $(B2OBJS) $(OBJS) $(LOBJS) $(XCCLIB) \
	xcc.c b2xcc.c

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

tags: bxcc.c xcc.h xccP.h libexe.c libxcc.c
	ctags bxcc.c xcc.h xccP.h libexe.c libxcc.c

# Deps
libxcc.o: xccP.h xcc.h
libexe.o: xccP.h xcc.h
bxcc.o: xccP.h xcc.h
b2xcc.o: xccP.h xcc.h
xcc.o: xccP.h xcc.h
