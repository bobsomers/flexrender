#!/bin/bash -e

# Where are we in the filesystem?
BASEPATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

function fetch {
    if [ ! -f $BASEPATH/$1 ]; then
        echo ""
        echo "==> Downloading $1.xz"
        wget -O $BASEPATH/$1.xz http://flexrender.org/files/assets/$1.xz

        echo ""
        echo "==> Unpacking $1.xz"
        xz --decompress $BASEPATH/$1.xz
    fi
}

echo "======================================================================"
echo "    DOWNLOADING ASSETS @ "`date`
echo "======================================================================"

fetch "buddha-hi.obj"
fetch "buddha-med.obj"
fetch "buddha-lo.obj"
fetch "bunny-hi.obj"
fetch "bunny-med.obj"
fetch "bunny-lo.obj"
fetch "dragon-hi.obj"
fetch "dragon-med.obj"
fetch "dragon-lo.obj"
fetch "tile1.tga"
fetch "wood1.tga"
fetch "marble1.tga"
fetch "brick1.tga"

echo ""
echo "======================================================================"
echo "    DOWNLOAD COMPLETE @ "`date`
echo "======================================================================"
