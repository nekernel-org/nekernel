.global rt_jump_to_address
.global rt_reset_hardware

.text

.intel_syntax noprefix

/**
 @brief this function setups a stack and then jumps to
 a function */
rt_jump_to_address:
    mov rbx, rcx
    mov rcx, rdx
    jmp rbx

    ret

rt_reset_hardware:
    /* dont raise any interrupts. (except ofc NMIs.) */
    cli
wait_gate1:
    /* wait for gate 1 to be ready? */
    in al,0x64
    and al,2
    jnz wait_gate1
    mov al,0x0D1
    out 0x64,al
wait_gate2:
    /* wait for gate 2 to be ready? */
    in al,0x64
    and al,2
    jnz wait_gate2
    mov al,0x0FE
    out 0x60,al

    /* trigger triple fault, by writing to cr4 */

    xor rax,rax
    mov cr4,rax
reset_wait:
    jmp reset_wait

.global write_cr3

write_cr3:
    mov cr3, rcx
    ret
