#!/bin/sh

# LOG HISTORY:
# 03/25/25: Add 'disk' build step.
# 04/05/25: Improve and fix script.

cd dev/boot/modules/SysChk
btb amd64-pio-epm.json
cd ../
cd BootNet
btb amd64.json