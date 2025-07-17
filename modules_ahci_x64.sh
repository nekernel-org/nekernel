#!/bin/sh

# LOG HISTORY:
# 03/25/25: Add 'disk' build step.
# 04/05/25: Improve and fix script.

cd dev/boot/modules/SysChk
nebuild amd64-ahci-epm.json
cd ../
cd BootNet
nebuild amd64.json