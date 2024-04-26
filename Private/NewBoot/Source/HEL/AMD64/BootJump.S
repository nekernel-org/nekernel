.global rt_jump_to_address
.text

.code64
.intel_syntax noprefix

rt_jump_to_address:
    jmp rcx
    ret
