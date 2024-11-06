<!-- README of ZKA -->

![ZKA](res/zka.svg)

# ZKA Kernel Architecture.

ZKA is an general purpose entreprise OS.
</br>
It is designed for heavy loads and low tolerance environements (such as servers)
</br>
A Win32 compat layer is available, as well as XPCOM and ZT frameworks support.

## Installation:

- MinGW/GCC for the Kernel, Bootloader and the SCI.
- Netwide Assembler, for the x64 assemblies if needed.
- The ToolchainKit, to embed a C++ suite in the OS.

### Installation

Clone repository:

```
git clone git@github.com:ElMahroussLogic/zka-dev.git
cd zka-dev
```

</br>

And then select the makefile (arm64 and amd64 are stable and EFI based) to execute:

```
make -f amd64-efi.make all
```

###### Copyright (C) 2024, EL Mahrouss Logic, all rights reserved. All rights reserved.
