#!/bin/sh

export AHCI_SUPPORT=1

cd dev/Kernel
make -f amd64-desktop.make all
cd ../Boot
make -f amd64-desktop.make all
make -f amd64-desktop.make run-efi-amd64-ahci