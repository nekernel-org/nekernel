===================================

# 0: General Information

===================================

- Programming Language: C/C++
- Build System: Make/BTB
- Purpose: Driver Tool Kit, which you link against libddk.sys

===================================

# 1: How It works

===================================

- Driver shall directly call the kernel at specific ports. (Or a kernel call)
- Kernel must respond according to kernel call, otherwise a panic will occur.