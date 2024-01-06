; =====================
;
; newBoot (c) Mahrouss Logic.
;
; =====================

; This file is meant to be for ARC

extern __AppMain

__probe_hardware_threads:
	lea 0x8100000 ; same as before, this alters the rr.
	ret

__init_hardware_threads:
	lea 0x8000000 ; this alters return register, do not touch it.	
	ret

__start:
	jmp __probe_hardware_threads ; Probe and register these threads.
	jmp __init_hardware_threads ; Setup, and hang any threads.
	jmp __AppMain
L0:
	spi ; stop process interrupt
	hlt
	jmp L0

;; put this at the end, XPM header.
%include "XPM.inc"