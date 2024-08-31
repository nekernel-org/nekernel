<!-- README of ZKA 1 -->

![ZKA](res/zka.svg)

# ZKA Kernel Architecture (Mark 1)

ZKA is an operating system Kernel of New OS.

## Installation:

- MinGW/GCC for the Kernel and bootloader.
- Netwide Assembler/NDK to output COFF/PEF object code.

### Start by cloning the OS:

```
git clone git@bitbucket.org:mahrouss/boot-krnl-dll.git
cd boot-krnl-dll
```

### And then select the makefile to execute:

```
make -f amd64-efi.make all
```

##### Copyright ZKA Technologies. All rights reserved.
