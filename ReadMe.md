<!-- Read Me of NeKernel -->

# NeKernel Architecture

## Requirements:

- MinGW/GCC, Clang.
- LibCompiler Runtime Headers and Toolchain.
- Netwide Assembler.
- Git.
- BTB.

## Installation:

Clone repository:

```sh
git clone git@github.com:devrije/kernel.git
cd kernel
./debug_ahci.sh # Generic AHCI target
```

## Good first PRs

- Contribute to the AHCI generic driver (PRDs are not being read)

## Warning:

You have to mount the FAT32 image in order to put the src/Root/ contents in it! (To make the emulator boot NeOS)

###### Copyright (C) 2024-2025 Amlal EL Mahrouss, All rights reserved.
