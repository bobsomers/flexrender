#!/bin/bash -e

function build_ilmbase {
    PACKAGENAME="ilmbase-1.0.2"

    echo ""
    echo "======================================================================"
    echo "    BUILDING $PACKAGENAME @ "`date`
    echo "======================================================================"

    echo ""
    echo "==> Unpacking $PACKAGENAME."
    tar xvf $BASEPATH/src/$PACKAGENAME.tar.xz --directory=$BASEPATH/tmp

    echo ""
    echo "==> Patching $PACKAGENAME."
    patch --directory=$BASEPATH/tmp/$PACKAGENAME -p1 < $BASEPATH/patches/ilmbase-memset-fix.patch

    echo ""
    echo "==> Configuring $PACKAGENAME."
    pushd $BASEPATH/tmp/$PACKAGENAME
    ./configure --prefix=$BASEPATH/build --enable-static=yes --enable-shared=no
    popd

    echo ""
    echo "==> Making $PACKAGENAME."
    make --directory=$BASEPATH/tmp/$PACKAGENAME --jobs=4 install
}
