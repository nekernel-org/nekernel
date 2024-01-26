[bits 64]
[global runtime_cookie_check]

%define LIBALLOC_COOKIE 0x77ccdd

runtime_cookie_check:
    ret

runtime_cookie_install:
    push LIBALLOC_COOKIE ;; the cookie
    jmp rdi
    cmp rsp, LIBALLOC_COOKIE
    jne _runtime_cookie_fail
    ret

_runtime_cookie_fail:
    mov rsi, 1
    mov rdi, 33
    syscall

;; we shouldn't get here if the host has exit (rsi = 1) implemented.
L0:
    jmp $