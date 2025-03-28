#!/bin/sh

export AHCI_SUPPORT=1
export ATA_PIO_SUPPORT=
export ATA_DMA_SUPPORT=
export DEBUG_SUPPORT=1

cd dev/kernel
make -f amd64-desktop.make all
cd ../boot
make -f amd64-desktop.make all
make -f amd64-desktop.make run-efi-amd64-ahci