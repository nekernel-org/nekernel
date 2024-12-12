<!-- README of ZKA -->

![ZKA](resources/zka.svg)

# Zeta Kernel Architecture.

ZKA is a general purpose OS kernel.
</br>
It is designed for real time environements (such as servers)
</br>
A Win32 compatibility layer is available as well as ML Kits support.

## Installation:

- MinGW/GCC for the Kernel, Bootloader and the SCI.
- Netwide Assembler, for the x64 assemblies if needed.
- The ToolchainKit, to embed a C++ suite in the OS.

### Installation

Clone repository:

```
git clone git@github.com:ELMH-Group/zka.git
cd zka
```

</br>

And then select the makefile (arm64 and amd64 are stable and EFI based) to execute:

```
make -f amd64-efi.make all
```

## Boot Process of ZKA

![ZKA](resources/zka-window-server.png)

###### Copyright (C) 2024, Theater Quality Incorporated., all rights reserved. All rights reserved.
