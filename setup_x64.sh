#!/bin/sh

# LOG HISTORY:
# 03/25/25: Add 'disk' build step.
# 04/05/25: Improve and fix script.

cd dev/user
btb user.json
cd ../boot
make -f amd64-desktop.make efi
make -f amd64-desktop.make disk
make -f amd64-desktop.make epm-img
cd ../../dev/kernel/HALKit/AMD64
./make_ap_blob.sh
