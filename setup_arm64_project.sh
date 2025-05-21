#!/bin/sh

cd dev/boot
make -f arm64-desktop.make efi
make -f arm64-desktop.make epm-img