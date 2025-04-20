#!/bin/sh

export AHCI_SUPPORT=
export ATA_PIO_SUPPORT=1
export ATA_DMA_SUPPORT=
export DEBUG_SUPPORT=1

cd dev/kernel
make -f amd64-desktop.make all
cd ../boot
make -f amd64-desktop.make all
cd ../../
./tooling/copy_to_fat32.py ./dev/boot/src/nekernel-esp.img ./dev/boot/src/root
cd dev/boot
make -f amd64-desktop.make run-efi-amd64-ata-pio
