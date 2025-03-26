<!-- Read Me of NeKernel -->

# NeKernel

Use the doxygen command to build documentation.
<br/>
A documentation regarding building, and developing for NeKernel is available here: https://amlel-el-mahrouss.github.io/nekernel-docs/

## Requirements:

- [MINGW-W64](https://www.mingw-w64.org/)
- [CLANG](https://clang.llvm.org/)
- [NASM](https://nasm.us/)
- [GIT](https://git-scm.com/)
- [BTB](https://github.com/amlel-el-mahrouss/btb)

## Notice for Contributors:

- You must mount the contents to the neos_esp.img prior running! Otherwise the emulator won't find a valid ESP.
- Always use `run_format.sh` before commiting and pushing your code!

## Quick Install:

Clone the repository:

```sh
git clone git@github.com:amlel-el-mahrouss/nekernel.git
cd nekernel
./setup_amd64.sh
./debug_ata.sh # Generic ATA PIO target
```

###### Copyright (C) 2024-2025 Amlal EL Mahrouss, All rights reserved.
