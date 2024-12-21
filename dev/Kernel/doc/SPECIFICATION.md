===================================

# 0: General Information

===================================

- ABI and Format: PEF/PE32+.
- Kernel architecture: Portable hybrid Kernel.
- Language: C++/(Assembly (AMD64, X64000, X86S, ARM64, POWER, RISCV))

===================================

# 1: The Kernel

===================================

- Drive/Device Abstraction.
- SMP, Preemptive Multi Threading.
- Separation of Files/Devices.
- Networking.
- Hardware Abstraction Layer.
- Native Filesystem support (NeFS, FAT32 and ffs2).
- Program Loaders interfaces.
- TLS (Thread Local Storage) support.
- Semaphore, Locks, Timers.
- Canary mechanisms.
- Dynamic Sys.
- Cross Platform.
- Permission Selectors.

===================================

# 2: The Filesystem

===================================

- Catalog object with associated forks.
- Large storage support.
- Long file names.
- UNIX path style.

==================================

# 3: Common naming conventions:

==================================

- Kernel -> ke_init_x
- RunTime -> rt_copy_mem
- Hal -> hal_foo_bar
- Class methods -> Class::FooBar

===================================

# 4: The zbaosldr

===================================

- Capable of booting from a network drive.
- Loads a PE file which is the Kernel.
- Sanity checks, based on the number of sections.
- Handover compliant.
- Does check for a valid partition (useful in the case of recovering)
