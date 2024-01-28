;; COPYRIGHT Mahrouss Logic, all rights reserved

bits 32

[global __BIOSLoadRegister]
[global __BIOSAlloc]

__BIOSLoadRegister:
    cmp [ebp - 4], 0
    jmp __BIOSLoadGDT
    ret
__BIOSLoadGDT:
    lgdt [ebp - 8]
    ret

;; This memory resides in the BIOS Memory Region BMR (0x9000-(0x9000 + 0x1000))
;; It contains bootloader data.

__BIOSAlloc:
    push ebp
    mov ebp, esp
    mov eax, __bios_lookup_table
__BIOSAllocLoop:
    cmp eax, __end_bios_lookup_table
    jne __BIOSAllocDone
    add eax, 4
    jmp $
__BIOSAllocDone:
    cmp eax, 0
    je __BIOSAllocLoop
    add esp, 12
    mov esp, ebp
    pop ebp
    ret

;; Allocation table.

__bios_lookup_table:
    resb 8096 * 4
__end_bios_lookup_table:
