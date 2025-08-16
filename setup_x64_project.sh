#!/bin/sh

# LOG HISTORY:
# 03/25/25: Add 'disk' build step.
# 04/05/25: Improve and fix script.

cd dev/libSystem
cd src
make libsys_asm_io_x64
cd ..
nebuild libSystem.json
cd ../ddk
nebuild ddk.json
cd ../boot
make -f amd64-desktop.make efi
make -f amd64-desktop.make epm-img
