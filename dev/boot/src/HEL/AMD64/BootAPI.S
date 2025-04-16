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
    push rbx
    push rdx
    mov rsp, r8
    push rax
    jmp rbx

    pop rdx
    pop rbx
    pop rax

    ret

rt_reset_hardware:
    /* dont raise any interrupts. (except ofc NMIs.) */
    cli
    /* remap PIC */
wait_gate1:
    in al,0x64
    and al,2
    jnz wait_gate1
    mov al,0x0D1
    out 0x64,al
wait_gate2:
    in al,0x64
    and al,2
    jnz wait_gate2
    mov al,0x0FE
    out 0x60,al

    /* trigger triple fault, by writing to cr4 */

    mov rax, 0
    lidt [rax]

reset_wait:
    jmp reset_wait

.global boot_write_cr3
.global boot_read_cr3

boot_read_cr3:
    mov rax, rax
    ret

boot_write_cr3:
    mov cr3, rcx
    ret
