<!-- Read Me of NeKernel -->

<div align="center">
  <img src="meta/png/nekernel.png" alt="Logo" width="128"/>
  <h1>NeKernel</h1>
</div>

![CI](https://github.com/amlel-el-mahrouss/nekernel/actions/workflows/boot-pio.yml/badge.svg)
![CI](https://github.com/amlel-el-mahrouss/nekernel/actions/workflows/kernel-ahci.yml/badge.svg)
[![License: GPL-3.0](https://img.shields.io/badge/license-GPL--3.0-blue.svg)](LICENSE)
[![QEMU Tested](https://img.shields.io/badge/QEMU-Tested-success)](#)

## Brief:

A kernel for engineers who want a custom solution for their stack, written mostly in C++ it aims to provide a base for other projects/products.

## User Friendly Diagram:

<div align="center">
  <img src="docs/svg/OS_DESIGN.png" alt="Diagram"/>
</div>

## Notice for Doxygen:

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

- You must mount the contents to the nekernel-esp.img prior running!
    - It depends mostly on how your platform manages them, so have a look at it.

- Run `format.sh` before commiting, it formats the code according to the .clang-format.

## Getting Started:

```sh
git clone git@github.com:amlel-el-mahrouss/nekernel.git
cd nekernel
./setup_amd64.sh
./debug_ata_x64.sh # Generic ATA PIO target
```

###### Copyright (C) 2024-2025 Amlal El Mahrouss, All rights reserved.
