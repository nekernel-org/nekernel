<!-- Read Me of NeKernel -->

# NeKernel

## Required:

- [MINGW-W64](https://www.mingw-w64.org/)
- [CLANG](https://clang.llvm.org/)
- [NASM](https://nasm.us/)
- [GIT](https://git-scm.com/)
- [BTB](https://github.com/ne-gh/btb)

## Builders's Notice:

You must mount the contents to the fat32.img prior running! Otherwise the emulator won't find a valid ESP.

## Contributors's Notice:

Always use `run_format.sh` before commiting and pushing your code!

## Quick Install:

Clone the repository:

```sh
git clone git@github.com:ne-gh/kernel.git
cd kernel
./setup_amd64.sh
./debug_ata.sh # Generic ATA PIO target
```

###### Copyright (C) 2024-2025 Amlal EL Mahrouss, All rights reserved.
