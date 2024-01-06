[bits 64]

[global rt_do_context_switch]
[extern rt_debug_fence]
[extern rt_debug_fence_end]

rt_do_context_switch:
    mov rsi, [rt_do_context_switch]
    call rt_debug_fence

    mov rsi, rdi
    mov rax, rsi

    call rt_debug_fence_end

    iret