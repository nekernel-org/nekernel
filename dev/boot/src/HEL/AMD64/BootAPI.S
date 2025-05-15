.global rt_jump_to_address
.global rt_reset_hardware

.text

.intel_syntax noprefix

.global hal_load_idt

hal_load_idt:
    ret

.global sched_jump_to_task

sched_jump_to_task:
    ret

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
    mov rax, cr3
    ret

boot_write_cr3:
    mov cr3, rcx
    ret

.section .text

.extern rt_wait_400ns

.global rt_out8
.global rt_out16
.global rt_out32

.global rt_in8
.global rt_in16
.global rt_in32

rt_out8:
    mov al, dl
    mov dx, cx
    out dx, al
    ret

rt_out16:
    mov ax, dx
    mov dx, cx
    out dx, ax
    ret

rt_out32:
    mov eax, edx
    mov edx, ecx
    out dx, eax
    ret

rt_in8:
    mov dx, cx
    in al, dx
    ret

rt_in16:
    mov edx, ecx
    in ax, dx
    ret

rt_in32:
    mov rdx, rcx
    in eax, dx
    ret