[bits 64]


section .text
global ATAWaitForIO

ATAWaitForIO:
    push ax
    push dx

    xor ax, ax
    mov dx, 01F7h

ATAWaitForIO_Loop:
    in  al, dx
    and al, 0C0h
    cmp al, 040h
    jne ATAWaitForIO_Loop

ATAWaitForIO_End:
    pop dx
    pop ax

    ret
