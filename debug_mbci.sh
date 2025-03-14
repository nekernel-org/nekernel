#!/bin/sh

export MBCI_SUPPORT=1
export UFS_SUPPORT=
export AHCI_SUPPORT=
export ATA_DMA_SUPPORT=
export ATA_PIO_SUPPORT=
export DEBUG_SUPPORT=1

cd kernel
make -f arm64-desktop.make all
cd ../boot
make -f arm64-desktop.make all
make -f arm64-desktop.make run