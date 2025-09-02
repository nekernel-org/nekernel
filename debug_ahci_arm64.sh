#!/bin/sh

export AHCI_SUPPORT=1
export ATA_PIO_SUPPORT=
export ATA_DMA_SUPPORT=
export DEBUG_SUPPORT=1

cd dev/kernel
make -f arm64-desktop.make all
cd ../boot
make -f arm64-desktop.make all
make -f arm64-desktop.make disk
cd ../../
./tools/mk_img.py ./dev/boot/src/nekernel-esp.img ./dev/boot/src/root
cd dev/boot
make -f arm64-desktop.make run-efi-arm64
