<!-- README of ZKA 1 -->

# ZKA (Zeta Kernel Architecture)

ZKA is an operating system kernel, also codenamed 'Ne'.

## Installation:

- MinGW/GCC for the kernel and bootloader.
- Netwide Assembler/CodeTools to output COFF/PEF object code.

### Start by cloning:

```
git clone git@bitbucket.org:mahrouss/newoskrnl.git
cd newoskrnl
```

### And then select the makefile to execute:

```
make -f amd64-efi.make all
```

##### Copyright, Zeta Electronics Corporation, all rights reserved.
