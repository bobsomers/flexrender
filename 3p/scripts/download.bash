#!/bin/bash -e

function download {
    if [ ! -f $BASEPATH/src/$1.tar.gz ]; then
        echo ""
        echo "==> Downloading $1"
        wget -O $BASEPATH/src/$1.tar.gz http://flexrender.org/files/$1.tar.gz
    fi
}

function download_packages {
    echo ""
    echo "======================================================================"
    echo "    DOWNLOADING PACKAGES @ "`date`
    echo "======================================================================"

    download "LuaJIT-2.0.0"
    download "lpack"
    download "libuv-0.9.8"
    download "glm-0.9.4.1"
    download "msgpack-0.5.7"
    download "ilmbase-1.0.3"
    download "openexr-1.7.1"
}
