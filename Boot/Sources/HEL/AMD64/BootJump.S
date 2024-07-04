.global rt_jump_to_address
.text

.text
.intel_syntax noprefix

/**
 @brief this function setups a stack and then jumps to
 a function */
rt_jump_to_address:
    mov rdx, rsp
    mov rdi, rcx
    mov rdx, rbp
    push rax
    mov r8, rcx
    call rdi
    pop rax
    ret
