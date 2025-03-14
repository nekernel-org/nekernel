#!/bin/sh

export UFS_SUPPORT=1
export AHCI_SUPPORT=
export ATA_DMA_SUPPORT=
export ATA_PIO_SUPPORT=
export DEBUG_SUPPORT=1

cd kernel
make -f arm64-mobile.make all
cd ../boot
make -f arm64-mobile.make all
make -f arm64-mobile.make run