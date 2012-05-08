#!/bin/bash -e

function build_openexr {
    PACKAGENAME="openexr-1.7.0"

    echo ""
    echo "======================================================================"
    echo "    BUILDING $PACKAGENAME @ "`date`
    echo "======================================================================"

    echo ""
    echo "==> Unpacking $PACKAGENAME."
    tar xvf $BASEPATH/src/$PACKAGENAME.tar.xz --directory=$BASEPATH/tmp

    echo ""
    echo "==> Patching $PACKAGENAME."
    patch --directory=$BASEPATH/tmp/$PACKAGENAME -p1 < $BASEPATH/patches/openexr-memcpy-fix.patch

    echo ""
    echo "==> Configuring $PACKAGENAME."
    pushd $BASEPATH/tmp/$PACKAGENAME
    PKG_CONFIG_PATH=$BASEPATH/build/lib/pkgconfig ./configure --prefix=$BASEPATH/build --enable-static=yes --enable-shared=no --disable-ilmbasetest
    popd

    echo ""
    echo "==> Making $PACKAGENAME."
    make --directory=$BASEPATH/tmp/$PACKAGENAME --jobs=4 install
}
