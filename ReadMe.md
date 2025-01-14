<!-- README of ZkaOS -->

# Zeta Kernel Architecture.

ZkaOS is designed for real time environements (such as servers or workstations.)

## Requirements

- MinGW/GCC for the Kernel, Bootloader and the LibSCI.
- Netwide Assembler, for the x64 assemblies if needed.

### Installation

Clone repository:

```
git clone git@github.com:tq-corp/kernel-services.git
cd kernel-services
```

</br>

And then select the makefile (arm64 and amd64 are stable and EFI based) to execute:

```
make -f amd64-efi.make all
```

###### Copyright (C) 2024, t& Labs, all rights reserved. All rights reserved.
