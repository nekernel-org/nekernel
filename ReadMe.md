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

```
git clone git@github.com:mediaswirl/nekernel.git
cd nekernel
```

</br>

And then select the makefile (ARM64 and AMD64 are stable and EFI based) to execute:

```
make -f amd64-efi.make all
```

###### Copyright (C) 2024-2025 Amlal EL Mahrouss, All rights reserved.
