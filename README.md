<!-- Read Me of NeKernel -->

<div align="center">
  <img src="meta/png/nekernel.png" alt="Logo" width="256"/>
</div>

<br/>

![CI](https://github.com/amlel-el-mahrouss/nekernel/actions/workflows/boot-pio.yml/badge.svg)
![CI](https://github.com/amlel-el-mahrouss/nekernel/actions/workflows/kernel-ahci.yml/badge.svg)
[![License: GPL-3.0](https://img.shields.io/badge/license-GPL--3.0-blue.svg)](LICENSE)
[![QEMU Tested](https://img.shields.io/badge/QEMU-Tested-success)](#)

## Notice for Doxygen:

Use the doxygen command to build documentation.
<br/>
A documentation for NeKernel is available here: https://nekernel-org.github.io/docs/

## Requirements:

- [MINGW-W64](https://www.mingw-w64.org/)
- [CLANG](https://clang.llvm.org/)
- [NASM](https://nasm.us/)
- [GIT](https://git-scm.com/)
- [BTB](https://github.com/nekernel-org/btb)

## Notice for Contributors:

- Run `format.sh` before commiting, it formats the code according to the .clang-format.

## Getting Started:

```sh
git clone git@github.com:nekernel-org/nekernel.git
cd nekernel
./setup_x64.sh
./debug_ata_x64.sh # Generic ATA PIO target
```

###### Copyright (C) 2024-2025 Amlal El Mahrouss, All rights reserved.
