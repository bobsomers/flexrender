#!/bin/bash -e

PACKAGENAME="libuv-0.6"

function build_libuv() {
    echo ""
    echo "======================================================================"
    echo "    BUILDING $PACKAGENAME @ "`date`
    echo "======================================================================"

    echo ""
    echo "==> Unpacking $PACKAGENAME."
    tar xvf $BASEPATH/src/$PACKAGENAME.tar.xz --directory=$BASEPATH/tmp

    echo ""
    echo "==> Patching $PACKAGENAME."
    patch --directory=$BASEPATH/tmp/$PACKAGENAME -p1 < $BASEPATH/patches/libuv-work-priority.patch

    echo ""
    echo "==> Making $PACKAGENAME."
    make --directory=$BASEPATH/tmp/$PACKAGENAME --jobs=4
    cp $BASEPATH/tmp/$PACKAGENAME/uv.a $BASEPATH/build/lib/libuv.a
    cp -R $BASEPATH/tmp/$PACKAGENAME/include/* $BASEPATH/build/include
}
