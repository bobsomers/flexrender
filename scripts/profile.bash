#!/bin/bash -e

sudo opcontrol --deinit
echo 0 | sudo tee /proc/sys/kernel/nmi_watchdog
sudo opcontrol --vmlinux=/usr/src/linux-3.3.5-1-ARCH/vmlinux
sudo opcontrol --separate=kernel
sudo opcontrol --init
sudo opcontrol --reset
sudo opcontrol --start
