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
cat $CODIR/pkg-descr | head -1 > COMMENT
pkg_create -c COMMENT -d $CODIR/pkg-descr -f $CODIR/pkg-plist -S $WORKDIR -p /usr/local xcc-$VERSION || exit 3
mv xcc-$VERSION.tgz $CODIR/xcc-$VERSION.tgz
cd $CODIR
rm -rf $WORKDIR

