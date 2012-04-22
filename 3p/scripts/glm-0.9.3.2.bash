#!/bin/bash -e

PACKAGENAME="glm-0.9.3.2"

function build_glm() {
    echo ""
    echo "======================================================================"
    echo "    BUILDING $PACKAGENAME @ "`date`
    echo "======================================================================"

    echo ""
    echo "==> Unpacking $PACKAGENAME."
    tar xvf $BASEPATH/src/$PACKAGENAME.tar.xz --directory=$BASEPATH/tmp

    echo ""
    echo "==> Patching $PACKAGENAME."
    patch --directory=$BASEPATH/tmp/$PACKAGENAME -p1 < $BASEPATH/patches/glm-vec-msgpack.patch

    echo ""
    echo "==> Copying $PACKAGENAME."
    cp --recursive $BASEPATH/tmp/$PACKAGENAME/glm $BASEPATH/build/include
}
