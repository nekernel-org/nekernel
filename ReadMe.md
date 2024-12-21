<!-- README of ZkaOS -->

# Zeta Kernel Architecture OS.

ZkaOS is designed for real time environements (such as servers or workstations.)

## Installation:

- MinGW/GCC for the Kernel, Bootloader and the SCI.
- Netwide Assembler, for the x64 assemblies if needed.
- The ToolchainKit, to embed a C++ suite in the OS.

### Installation

Clone repository:

```
git clone git@github.com:tq-corp/zka.git
cd zka
```

</br>

And then select the makefile (arm64 and amd64 are stable and EFI based) to execute:

```
make -f amd64-efi.make all
```

###### Copyright (C) 2024, TQ B.V, all rights reserved. All rights reserved.
