<!-- Read Me of NeKernel -->

# New Kernel Architecture (NeKernel)

## Brief:

A Microkernel written in C++ 20, with a System Call Interface Library, and custom bootloader.

## Screenshots:

![doc/apic.png](doc/apic.png)
![doc/filesystem.png](doc/filesystem.png)

## Requirements:

- MinGW/GCC
- LibCompiler
- Netwide Assembler
- Git
- BTB/OpenBTB

## Installation:

Clone repository:

```
git clone git@github.com:mediaswirl/nekernel.git
cd nekernel
```

</br>

And then select the makefile (arm64 and amd64 are stable and EFI based) to execute:

```
make -f amd64-efi.make all
```

###### Copyright (C) 2024-2025 Amlal EL Mahrouss, All rights reserved.
