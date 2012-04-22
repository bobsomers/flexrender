#!/bin/bash -e

# Where are we in the filesystem?
BASEPATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "======================================================================"
echo "    CLEAN STARTING @ "`date`
echo "======================================================================"
echo ""

rm -rf $BASEPATH/build/*
rm -rf $BASEPATH/tmp/*

echo "======================================================================"
echo "    CLEAN COMPLETE @ "`date`
echo "======================================================================"
echo ""
