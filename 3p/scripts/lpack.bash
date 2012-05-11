#!/bin/bash -e

function build_lpack {
    PACKAGENAME="lpack"

    echo ""
    echo "======================================================================"
    echo "    BUILDING $PACKAGENAME @ "`date`
    echo "======================================================================"

    echo ""
    echo "==> Unpacking $PACKAGENAME."
    tar xvf $BASEPATH/src/$PACKAGENAME.tar.xz --directory=$BASEPATH/tmp

    echo ""
    echo "==> Patching $PACKAGENAME."
    patch --directory=$BASEPATH/tmp/pack -p1 < $BASEPATH/patches/lpack.patch

    echo ""
    echo "==> Making $PACKAGENAME."
    make --directory=$BASEPATH/tmp/pack BASEPATH=$BASEPATH
    cp $BASEPATH/tmp/pack/pack.so $BASEPATH/build/lib
}
