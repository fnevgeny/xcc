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
PROG   = xcc

BOBJS  = bxcc.o libexe.o xfile.o
OBJS   = xcc.o libexe.o xfile.o

LOBJS  = libxcc.o

XCCLIB = libxcc.a	

BUNDLE_I = bundle.i 

all: $(PROG)

$(XCCLIB): $(LOBJS)
	$(AR) $@ $(LOBJS)

$(BPROG): $(BOBJS) $(XCCLIB)
	$(CC) $(LDFLAGS) -o $@ $(BOBJS) $(XCCLIB) $(LIBS)
$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(XCCLIB) $(LIBS)

xcc.c:  $(XCC_XCC) $(BPROG)
	./$(BPROG) -i $(XCC_XCC) -o $@

xcc_t.c:  $(XCC_XCC) $(PROG)
	./$(PROG) -i $(XCC_XCC) -o $@

$(BUNDLE_I): xcc.h libxcc.c
	./c2cstr.sh xcc.h libxcc.c > $@

clean:
	rm -f $(BPROG) $(PROG) \
	$(BOBJS) $(OBJS) $(LOBJS) $(XCCLIB) \
	xcc.c xcc_t.c $(BUNDLE_I) tags ChangeLog *~ *.bak

install: $(XCCLIB) $(PROG)
	$(MKINSTALLDIRS) $(bindir)
	$(INSTALL_PROGRAM) -s $(PROG) $(bindir)
	$(MKINSTALLDIRS) $(libdir)
	$(INSTALL_DATA) $(XCCLIB) $(libdir)
	$(MKINSTALLDIRS) $(incdir)
	$(INSTALL_DATA) xcc.h $(incdir)

uninstall:
	rm -f $(bindir)/$(PROG)
	rm -f $(libdir)/$(XCCLIB)
	rm -f $(incdir)/xcc.h

check: xcc_t.c xcc.c
	@echo -n "Testing self-consistency ... "
	@diff -q xcc_t.c xcc.c
	@if test $$? -ne 0; then echo "Failed"; else echo "OK"; fi

tags: bxcc.c xcc.h xccP.h xfile.h libexe.c libxcc.c xfile.c
	ctags bxcc.c xcc.h xccP.h xfile.h libexe.c libxcc.c xfile.c

# Deps
libxcc.o: xccP.h xcc.h
libexe.o: xccP.h xcc.h $(BUNDLE_I) xfile.h
xfile.o: xfile.h
bxcc.o: xccP.h xcc.h
xcc.o: xccP.h xcc.h
