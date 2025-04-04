#!/bin/sh

# LOG HISTORY:
# 25/03/25: Add 'disk' build step.

cd dev/boot/modules/SysChk
btb amd64.json
cd ../../
make -f amd64-desktop.make efi
make -f amd64-desktop.make disk
make -f amd64-desktop.make epm-img
cd ../../dev/kernel/HALKit/AMD64
./make_ap_blob.sh
