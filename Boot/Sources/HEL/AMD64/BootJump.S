.global rt_jump_to_address
.text

.text
.intel_syntax noprefix

/**
 @brief this function setups a stack and then jumps to
 a function */
rt_jump_to_address:
    mov r8, rsp

    push rax
    push rdx
    jmp rcx
    pop rdx
    pop rax

    ret
