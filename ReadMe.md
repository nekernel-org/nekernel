<!-- README of ZKA 1 -->

![ZKA](res/zka.svg)

# ZKA Kernel Architecture (Mark 1)

ZKA is an general purpose entreprise OS.
</br>
It is designed for heavy loads and low tolerance environements (such as servers)
</br>
A Win32 compat layer is available, as well as XPCOM and ZT frameworks support.

## Installation:

- MinGW/GCC for the Kernel, Bootloader and their Components.
- Netwide Assembler, for the x64 assemblies if needed.
- The NDK, to embed a C++ suite into the distribution (if XPCOM apps are needed)

### Installation

Clone repository:

```
git clone git@bitbucket.org:mahrouss/boot-krnl-dll.git
cd boot-krnl-dll
```

</br>

And then select the makefile (arm64 and amd64 are stable and EFI based) to execute:

```
make -f amd64-efi.make all
```

###### Copyright EL Mahrouss Logic. All rights reserved.
