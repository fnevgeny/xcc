#!/bin/sh

VERSION=`cat ../../VERSION`
CODIR=`pwd`
WORKDIR=`mktemp -d $CODIR/xcc-work-XXXXXX`
cd ../..
./configure || exit 1
make DESTDIR=$WORKDIR install || exit 1
cd $CODIR
mv pkginfo pkginfo.in
cat pkginfo.in |sed s/@ARCH@/`uname -p`/ |sed s/@VERSION@/$VERSION/ >pkginfo
rm pkginfo.in
mv pkginfo $WORKDIR/usr/local
mv prototype $WORKDIR/usr/local
cd $WORKDIR/usr/local
pwd
pkgmk -o -r `pwd` -d $CODIR || exit 3
pkgtrans -s $CODIR $CODIR/xcc-$VERSION.pkg Axcc || exit 3
gzip $CODIR/xcc-$VERSION.pkg || exit 3
cd $CODIR
rm -rf $WORKDIR
