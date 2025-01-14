#!/bin/sh

cd dev/Boot
make -f amd64-desktop.make efi
make -f amd64-desktop.make epm-img