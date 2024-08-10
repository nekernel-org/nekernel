<!-- README of ZKA 1 -->

![ZKA](Meta/NewOS.svg)

# ZKA (ZKA Kernel Architecture)

ZKA is an operating system kernel of New OS.

## Installation:

- MinGW/GCC for the kernel and bootloader.
- Netwide Assembler/NDK to output COFF/PEF object code.

### Start by cloning:

```
git clone git@bitbucket.org:mahrouss/boot-krnl-dll.git
cd boot-krnl-dll
```

### And then select the makefile to execute:

```
make -f amd64-efi.make all
```

##### Copyright, ZKA Technologies, all rights reserved.
