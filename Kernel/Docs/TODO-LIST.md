# TODO list

- We need preemptive multi-threading. [ X ]
- We then need sync primitives. [ X ]
- We also need a system library for the OS. [ X ]
- We need a bootloader for AMD64 [ X ]
  - Implement Boot Services [ X ]
  - Design Handover [ X ]
  - Load kernel into memory [ X ]
  - Fix bug in kernel loader, which causes a 06 #UD. [ X ]
  - Load Kernel [ X ]
  - Add IDT [ X ]
  - AHCI driver [ WiP ]
- Context switch x87/SSE/AVX registers [ X ]
- Framebuffer [ X ]
- AHCI support [ ]
- Make installer [ ]

Status:

NewBoot: Need to boot from EPM partition.
<br>
NewKernel: New Filesystem in progress.
