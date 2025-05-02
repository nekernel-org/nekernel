#!/bin/sh

export ATA_PIO_SUPPORT=1

cd dev/kernel
make -f amd64-desktop.make all
cd ../boot
make -f amd64-desktop.make all
make -f amd64-desktop.make disk
cd ../../
./tooling/mk_img.py ./dev/boot/src/nekernel-esp.img ./dev/boot/src/root
cd dev/boot
make -f amd64-desktop.make run-efi-amd64-ata-pio