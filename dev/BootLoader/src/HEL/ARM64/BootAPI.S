.global rt_jump_to_address

.text

/**
 @brief this function setups a stack and then jumps to
 a function */
rt_jump_to_address:
    mov x19, x0
    mov sp, x2
    blr x19

