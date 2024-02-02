.text

.global TargetFunc
TargetFunc:
    mov rax, 0
    ret

.global GetRIPA
GetRIPA:
    # lea rax, [rip]
    # lea rax, [TargetFunc]
    # lea rax, [rip+TargetFunc] # abs addr of label
    # lea rax, [TargetFunc]
    lea rax, [TargetFunc]
    ret

.global GetRIPB
GetRIPB:
    # lea rax, [offset TargetFunc]
    # lea rax, TargetFunc
    # lea rax, [rip+TargetFunc]
    lea rax, [TargetFunc]
    # lea rax, [rip]
    ret

.global GetCpuInfo
GetCpuInfo:
    # rdi cpu_id

    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    ###

    mov eax, 0
    cpuid

    # eax max_func_num
    # ebx ecx edx:  vender_id

    mov [rdi], eax
    add rdi, 4

    mov [rdi], ebx
    add rdi, 4

    mov [rdi], ecx
    add rdi, 4

    mov [rdi], edx
    add rdi, 4

    mov qword ptr [rdi], 0
    add rdi, 4

    ###

    mov eax, 1
    cpuid

    # type
    mov r8, rax
    shr r8, 12
    and r8, 0b11
    mov [rdi], r8d
    add rdi, 4

    # famaily
    mov r8, rax
    shr r8, 8
    and r8, 0b1111
    mov [rdi], r8d
    add rdi, 4

    # ext_famaily
    mov r8, rax
    shr r8, 20
    and r8, 0b11111111
    mov [rdi], r8d
    add rdi, 4

    # model
    mov r8, rax
    shr r8, 4
    and r8, 0b1111
    mov [rdi], r8d
    add rdi, 4

    # ext_model
    mov r8, rax
    shr r8, 16
    and r8, 0b1111
    mov [rdi], r8d
    add rdi, 4

    # stepping
    mov r8, rax
    and r8, 0b1111
    mov [rdi], r8d
    add rdi, 4

    ###

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx

    ret
