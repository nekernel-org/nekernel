[BITS 32]

NewBootGdtRegister:
	dw NewBootGdtEnd - NewBootGdt - 1
	dq NewBootGdt

[BITS 32]

NewBootGdt:
	;; Entry 0x0: Null descriptor
	dd 0x0
	dd 0x0
	;; Entry 0x8: Code segment
NewBootGdtCode:
	dw 0xffff          ; Limit
	dw 0x0000          ; Base 15:00
	db 0x00            ; Base 23:16
	dw 0xcf9a          ; Flags / Limit / Type [F,L,F,Type]
	db 0x00            ; Base 32:24
	;; Entry 0x10: Data segment
NewBootGdtData:
	dw 0xffff          ; Limit
	dw 0x0000          ; Base 15:00
	db 0x00            ; Base 23:16
	dw 0xcf92          ; Flags / Limit / Type [F,L,F,Type]
	db 0x00            ;Base 32:24
	;; Entry 0x18: GS Data segment
	dw 0x0100          ; Limit
	dw 0x1000          ; Base 15:00
	db 0x00            ; Base 23:16
	dw 0x4092          ; Flags / Limit / Type [F,L,F,Type]
	db 0x00            ; Base 32:24
NewBootGdtEnd:

NB_CODE: db 0x8
NB_DATA: db 0x10