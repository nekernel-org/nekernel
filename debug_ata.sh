#!/bin/sh

export ATA_PIO_SUPPORT=1
export DEBUG_SUPPORT=1

cd dev/Kernel
make -f amd64-desktop.make all
cd ../Boot
make -f amd64-desktop.make all
make -f amd64-desktop.make run-efi-amd64-ata