#!/bin/bash -e

echo "======================================================================"
echo "    BUILD STARTING @ "`date`
echo "======================================================================"

source $BASEPATH/scripts/download.bash
download_packages

source $BASEPATH/scripts/luajit-2.0.0.bash
build_luajit

source $BASEPATH/scripts/lpack.bash
build_lpack

source $BASEPATH/scripts/libuv-0.9.8.bash
build_libuv

source $BASEPATH/scripts/glm-0.9.4.1.bash
build_glm

source $BASEPATH/scripts/msgpack-0.5.7.bash
build_msgpack

source $BASEPATH/scripts/ilmbase-1.0.3.bash
build_ilmbase

source $BASEPATH/scripts/openexr-1.7.1.bash
build_openexr

echo ""
echo "======================================================================"
echo "    BUILD COMPLETE @ "`date`
echo "======================================================================"
