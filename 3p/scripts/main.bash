#!/bin/bash -e

echo "======================================================================"
echo "    BUILD STARTING @ "`date`
echo "======================================================================"

source $BASEPATH/scripts/luajit-2.0.0-beta9.bash
build_luajit

source $BASEPATH/scripts/libuv-0.6.bash
build_libuv

source $BASEPATH/scripts/glm-0.9.3.2.bash
build_glm

source $BASEPATH/scripts/msgpack-0.5.7.bash
build_msgpack

source $BASEPATH/scripts/ilmbase-1.0.2.bash
build_ilmbase

source $BASEPATH/scripts/openexr-1.7.0.bash
build_openexr

echo ""
echo "======================================================================"
echo "    BUILD COMPLETE @ "`date`
echo "======================================================================"
