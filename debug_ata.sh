#!/bin/sh

export ATA_PIO_SUPPORT=
export ATA_DMA_SUPPORT=1
export DEBUG_SUPPORT=1

cd kernel
make -f amd64-desktop.make all
cd ../boot
make -f amd64-desktop.make all
make -f amd64-desktop.make run-efi-amd64-ata-dma
