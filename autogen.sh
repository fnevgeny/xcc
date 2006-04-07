#!/bin/sh

make -f version.mak version.m4						&& \
libtoolize --copy --automake --force					&& \
aclocal -I .								&& \
autoheader								&& \
autoconf								&& \
automake --include-deps --add-missing --copy --foreign --no-force	&& \
./configure "$@"
