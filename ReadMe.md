<!-- Read Me of NeKernel -->

# New Kernel Architecture OS (NeOS)

## Brief:

An OS written in C++ 20, with a System Call Interface Library, and a custom bootloader.

## Requirements:

- MinGW/GCC.
- LibCompiler Runtime Headers and Toolchain.
- Netwide Assembler.
- Git.
- BTB.

## Installation:

Clone repository:

```sh
git clone git@github.com:devrije/kernel.git
cd kernel
./debug_ata.sh # Generic ATA PIO target
```

## Warning:

You have to mount the FAT32 image in order to put the src/Root/ contents in it! (To make the emulator boot NeOS)

###### Copyright (C) 2024-2025 Amlal EL Mahrouss, All rights reserved.
