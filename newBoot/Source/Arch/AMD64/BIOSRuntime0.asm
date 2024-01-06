;; Copyright Mahrouss Logic, all rights reserved

extern __STACK
global NBRuntimeZero

bits 16

NBRuntimeZero:
	cli

	lgdt [NewBootGdtRegister]

	mov ebx, cr0
	or  ebx, 1
	mov cr0, ebx

	jmp 0x8:NBProtectedMode

%include "BIOSApiGdt.inc"

[bits 32]

NBMasterPartitionTable:
    resb 32
    resw 1
    resw 1
    resw 1
    resb 211
NBMasterPartitionTableEnd:

NBProtectedMode:
	mov   ax, 0x10 ; 0x10 is a stand-in for your data segment
	mov   ds, ax
	mov   es, ax
	mov   fs, ax
	mov   gs, ax
	mov   ss, ax

	mov   esp, __STACK
	jmp   0x8000000

NBLoopOne:
	cli
	hlt
	jmp $

NBMasterBootRecord:
	times 510 - ($-$$) db 0
	dw 0xAA55
NBEndMasterBootRecord: