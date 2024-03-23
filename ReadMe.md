# NewKernel+NewBoot (codename: HCore)

![Kernel](Meta/HCore.svg)

## MicroKernel and Executives source code.

You need:

- The GCC toolchain (MinGW) for the kernel and bootloader.
- HintKit for Compilation check.
- Netwide Assembler to output COFF object code.

Start by cloning the repo:

```
git clone git@github.com:Mahrouss-Logic/h-core.git
```

And execute:

```
make h-core-<cpu>-<hardware>
```

##### Copyright, Mahrouss Logic, all rights reserved.
