;*
; *	========================================================
; *
; *	newBoot
; * Copyright Mahrouss Logic, all rights reserved.
; *
; * ========================================================
; *

;; Root directory

kBootBlockIdent: db "XPMNC", 0
kBootBlockMagic: db "bootloader", 0
kBootBlockGUID: db "C218439A-5939-4C9D-82AC-ADED46E9243C", 0
kBootBlockVersion: dw 1
kBootBlockNumBlocks: dw 1
kBootBlockSectorSz: dw 4096
kBootBlockSectorStart: dw 512

;; Boot directory

kPartBlockName: db "bootloader_exec", 0
kPartBlockMagic: dw 0xEDAD
kPartBlockSectorEnd: dw 0
kPartBlockSectorStart: dw 1024
kPartBlockSize: dw 0
kPartBlockPartType: dw 1
kPartBlockVer: dw 1
kPartBlockFs: db "Files32", 0

kPartBlockReserved1: dq 0xFFFF
kPartBlockReserved2: dq 0xFFFF
kPartBlockReserved3: dq 0xFFFF

;; End of directory
