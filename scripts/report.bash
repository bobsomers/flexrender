#!/bin/bash -e

# Where are we in the filesystem?
BASEPATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

sudo opcontrol --stop
sudo opreport -l $BASEPATH/bin/flexworker | tee $BASEPATH/profile.txt
