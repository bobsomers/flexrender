#!/bin/bash -e

function build_glm {
    PACKAGENAME="glm-0.9.4.1"

    echo ""
    echo "======================================================================"
    echo "    BUILDING $PACKAGENAME @ "`date`
    echo "======================================================================"

    echo ""
    echo "==> Unpacking $PACKAGENAME."
    tar xvf $BASEPATH/src/$PACKAGENAME.tar.gz --directory=$BASEPATH/tmp

    echo ""
    echo "==> Patching $PACKAGENAME."
    patch --directory=$BASEPATH/tmp/$PACKAGENAME -p1 < $BASEPATH/patches/glm-msgpack.patch

    echo ""
    echo "==> Copying $PACKAGENAME."
    cp --recursive $BASEPATH/tmp/$PACKAGENAME/glm $BASEPATH/build/include
}
