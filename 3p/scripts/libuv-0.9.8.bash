#!/bin/bash -e

function build_libuv {
    PACKAGENAME="libuv-0.9.8"

    echo ""
    echo "======================================================================"
    echo "    BUILDING $PACKAGENAME @ "`date`
    echo "======================================================================"

    echo ""
    echo "==> Unpacking $PACKAGENAME."
    tar xvf $BASEPATH/src/$PACKAGENAME.tar.gz --directory=$BASEPATH/tmp

    echo ""
    echo "==> Making $PACKAGENAME."
    make --directory=$BASEPATH/tmp/$PACKAGENAME --jobs=12
    cp $BASEPATH/tmp/$PACKAGENAME/libuv.a $BASEPATH/build/lib/libuv.a
    cp -R $BASEPATH/tmp/$PACKAGENAME/include/* $BASEPATH/build/include
}
