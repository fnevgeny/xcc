#!/bin/sh
DEBDIR="../../debian"
(
    RELEASE_DATE=`date "+%a, %d %b %Y %T %z"`
    VERSION=`cat ../../VERSION`
    echo "xcc ($VERSION) unstable; urgency=low" >changelog
    echo "" >>changelog
    echo "  * upstream release. See ChangeLog for deatils." >>changelog
    echo "" >>changelog
    echo " -- Botond Botyanszki <boti@andrews.hu>  $RELEASE_DATE" >>changelog
    echo "" >>changelog
    
    cd ../..
    ln -s -f packaging/debian debian
    dpkg-buildpackage -rfakeroot || exit 2
    if test "x$RUN_TESTS" = "x1"; then
	make check || exit 2
    fi
)
rm -f $DEBDIR
