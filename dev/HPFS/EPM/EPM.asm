;; FILE: EBS.asm
;; PURPOSE: EXPLICIT PARTITION MAP BOOT STRUCTURE (HPFS RELATED).
;; AUTHOR: AMLAL EL MAHROUSS

[bits 64]

HPFS_EPM_HEADER:
	db "EPMAM", 0
	db "HPFS                           ", 0
HPFS_EPM_UUID:
	dd 0
	dw 0
	db 0
	db 0
	db 0
	db 0
	db 0
	db 0
	db 0
	db 0
	dd 0xAD ;; EPM revision
	dd 0 ;; Number of blocks, set to 0 by default.
	dd 512 ;; Sector size, HPFS was made with drives in mind, so 512.
	dd 0
	dd 0
	dw 0x1f ;; NewOS (ZKA) present on partition.
	dd 1
	db "FileSystem (A:)", 0
	times 401 db 0
HPFS_EPM_HEADER_END:


%include "EBS.i"

times 16 db 0

;; Here the partition starts
