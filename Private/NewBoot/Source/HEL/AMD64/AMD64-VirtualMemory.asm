bits 64
global __cr3

section .text

__cr3:
    mov rax, cr3
    ret
