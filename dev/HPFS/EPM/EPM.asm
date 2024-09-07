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
	dd 0xAD ;; REVISON
	dd 0 ;; NUM BLOCKS
	dd 512 ;; SECTOR SZ
	dd 0
	dd 0
	dw 0x1f ;; NewOS
	dd 1
	db "FileSystem (C:)", 0
	times 401 db 0
HPFS_EPM_HEADER_END:


%include "EBS.i"

times 512 db 0