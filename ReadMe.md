<!-- README -->

# ZKA (Zeta Kernel Architecture)

ZKA is an operating system kernel, also codenamed 'Ne'.
<br>
You need:

- MinGW/GCC for the kernel and bootloader.
- Netwide Assembler/CodeTools to output COFF/PEF object code.

## Start by cloning the repo:

```
git clone git@bitbucket.org:mahrouss/microkernel.git
```

## And execute:

```
make -f amd64-efi.make all
```

For the custom target 'AMD64 EPM'.

##### Copyright, Zeta Electronics Corporation, all rights reserved.
