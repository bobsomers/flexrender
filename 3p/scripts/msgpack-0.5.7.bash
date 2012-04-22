#!/bin/bash -e

PACKAGENAME="msgpack-0.5.7"

function build_msgpack() {
    echo ""
    echo "======================================================================"
    echo "    BUILDING $PACKAGENAME @ "`date`
    echo "======================================================================"

    echo ""
    echo "==> Unpacking $PACKAGENAME."
    tar xvf $BASEPATH/src/$PACKAGENAME.tar.xz --directory=$BASEPATH/tmp

    echo ""
    echo "==> Patching $PACKAGENAME."
    patch --directory=$BASEPATH/tmp/$PACKAGENAME -p1 < $BASEPATH/patches/msgpack-auto_ptr-fix.patch

    echo ""
    echo "==> Configuring $PACKAGENAME."
    pushd $BASEPATH/tmp/$PACKAGENAME
    ./configure --prefix=$BASEPATH/build --enable-static=yes --enable-shared=no
    popd

    echo ""
    echo "==> Making $PACKAGENAME."
    make --directory=$BASEPATH/tmp/$PACKAGENAME --jobs=4 install
}
