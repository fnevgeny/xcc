#!/bin/sh

VERSION=`cat ../../VERSION`
CODIR=`pwd`
WORKDIR=`mktemp -d $CODIR/xcc-work-XXXXXX`
cd ../..
./configure || exit 1
make DESTDIR=$WORKDIR install || exit 1
if test "x$RUN_TESTS" = "x1"; then
    make check || exit 2
fi
cd $WORKDIR
touch COMMENT
pkg_create -A `uname -m`  -c COMMENT -d $CODIR/pkg/DESCR -f $CODIR/pkg/PLIST -B $WORKDIR -p /usr/local xcc-$VERSION || exit 3
mv xcc-$VERSION $CODIR/xcc-$VERSION.tgz
cd $CODIR
rm -rf $WORKDIR

