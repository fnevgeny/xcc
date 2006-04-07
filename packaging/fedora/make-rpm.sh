#!/bin/bash

NAME=xcc
VER=`cat ../../VERSION`
REL=1
ARCH=i386

PWD=`pwd`
if ! RPM_BASE=`mktemp -d $PWD/temp.MakeRpm.sh.XXXXXX`
then
    echo "failed to create tempdir"
    exit -1
fi

echo "using RPM_BASE=$RPM_BASE";
echo

MKDIRLIST="$RPM_BASE/BUILD/$NAME-root $RPM_BASE/RPMS $RPM_BASE/SOURCES $RPM_BASE/SPECS $RPM_BASE/SRPMS"

for dirn in $MKDIRLIST
do
	mkdir -p $dirn
done

PKG_VER="$NAME-$VER"

TARBALL="$RPM_BASE/SOURCES/$PKG_VER.tar.gz"
SPEC_FILE="$RPM_BASE/SPECS/$NAME.spec"
BINARY_RPM="$RPM_BASE/RPMS/$ARCH/$PKG_VER-$REL.$ARCH.rpm"
(
    cp  $NAME.spec $SPEC_FILE
    cd ../../..
    tar -c $PKG_VER |gzip > $RPM_BASE/SOURCES/$PKG_VER.tar.gz || exit 1
)

(
    cd "$RPM_BASE/SPECS/"
    echo "Building Binary RPM"
    rpmbuild -bb --define="_topdir $RPM_BASE" --buildroot=$RPM_BASE/BUILD/$NAME-root --target $ARCH $SPEC_FILE || exit 1
)

echo "getting rpm"
mv $BINARY_RPM . || cp $RPM_BASE/RPMS/$ARCH/$PKG_VER-$REL.$ARCH.rpm .

rm -r $RPM_BASE

