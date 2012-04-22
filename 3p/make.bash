#!/bin/bash -e

# Where are we in the filesystem?
BASEPATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Where are we saving the build log?
BUILDLOG=$BASEPATH/build.log

# Create/clear the build log.
echo "" > $BUILDLOG

# Off we go!
source $BASEPATH/scripts/main.bash | tee -a $BUILDLOG
