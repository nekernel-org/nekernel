===================================
# 0: General Information
===================================

- **Programming Language**: C / C++
- **Build System**: Make / BTB (Build the Build)
- **Purpose**: System Call Interface (SCI) for NeKernel

===================================
# 1: How It Works
===================================

- This header provides the raw API surface for accessing NeKernel's system calls.

- It is **not** directly used by applications.
    - Instead, it is abstracted by the **SystemSDK**, which presents a stable, high-level interface.

- At runtime:
    - System calls are routed from user-space code to the **kernel syscall manager**.
    - The syscall manager executes the requested operation and returns control to `libSystem`.
    - Finally, the result is delivered back to the originating process.

- This separation ensures that:
    - The SCI remains low-level and close to the ABI.
    - `SystemSDK` provides portability and shielding from changes in syscall internals.