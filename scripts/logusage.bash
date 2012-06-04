#!/bin/bash -e

# Where are we in the filesystem?
BASEPATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

LOGFILE=$BASEPATH/../usage-`/sbin/ifconfig eth0 | grep 'inet addr:' | cut -d: -f2 | awk '{ print $1}'`.log
vmstat -n 1 | tee $LOGFILE
