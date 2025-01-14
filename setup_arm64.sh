#!/bin/sh

cd dev/Boot
make -f arm64-desktop.make efi
make -f arm64-desktop.make epm-img