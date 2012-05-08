#!/bin/bash -e

function download {
    if [ ! -f $BASEPATH/src/$1.tar.xz ]; then
        echo ""
        echo "==> Downloading $1"
        wget -O $BASEPATH/src/$1.tar.xz http://flexrender.org/files/$1.tar.xz
    fi
}

function download_packages {
    echo ""
    echo "======================================================================"
    echo "    DOWNLOADING PACKAGES @ "`date`
    echo "======================================================================"

    download "LuaJIT-2.0.0-beta9"
    download "libuv-0.6"
    download "glm-0.9.3.2"
    download "msgpack-0.5.7"
    download "ilmbase-1.0.2"
    download "openexr-1.7.0"
}
