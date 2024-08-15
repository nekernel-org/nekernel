.global rt_jump_to_address
.text

.text
.intel_syntax noprefix

/**
 @brief this function setups a stack and then jumps to
 a function */
rt_jump_to_address:
    mov rsp, r8

    push rax
    push rdx
    mov rbx, rcx
    mov rcx, rdx
    jmp rbx
    pop rdx
    pop rax

    ret
