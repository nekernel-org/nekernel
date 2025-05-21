#!/bin/sh

# LOG HISTORY:
# 03/25/25: Add 'disk' build step.
# 04/05/25: Improve and fix script.

cd dev/user
cd src
make sci_asm_io_x64
cd ..
btb user.json
cd ../ddk
btb ddk.json
cd ../boot
make -f amd64-desktop.make efi
make -f amd64-desktop.make epm-img
