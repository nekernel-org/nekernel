.global rt_jump_to_address

.text

/**
 @brief this function setups a stack and then jumps to
 a function */
rt_jump_to_address:
    mov sp, x1
    br x0
    ret

