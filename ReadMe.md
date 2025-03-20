<!-- Read Me of NeKernel -->

# NeKernel



## Required:

- MinGW/GCC, Clang.
- Netwide Assembler.
- Git.
- BTB.

## Install:

Clone repository:

```sh
git clone git@github.com:ne-gh/kernel.git
cd kernel
./debug_ata.sh # Generic ATA PIO target
```

## Good first PRs:

- Contribute to the AHCI generic driver, it's the current main effort to bring modern storage standards to the kernel.

## Warning:

You have to mount the FAT32 image in order to put the src/Root/ contents in it! (To make the emulator boot NeOS)

###### Copyright (C) 2024-2025 Amlal EL Mahrouss, All rights reserved.
