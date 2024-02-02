.text




.global GetCpuInfo
GetCpuInfo:
    # rdi cpu_id

    pushq %rbx
    pushq %rbp
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    ###

    movl $0, %eax
    cpuid

    # eax max_func_num
    # ebx ecx edx:  vender_id

    movl %eax, 0(%rdi)
    addq $4, %rdi

    movl %ebx, 0(%rdi)
    addq $4, %rdi

    movl %ecx, 0(%rdi)
    addq $4, %rdi

    movl %edx, 0(%rdi)
    addq $4, %rdi

    movl $0, 0(%rdi)
    addq $4, %rdi

    ###

    movl $1, %eax
    cpuid

    movq %rax, %r8

    # famaily
    movq %r8, %rax
    shr $12, %eax
    and $0b11, %eax
    movl %eax, 0(%rdi)
    addq $4, %rdi

    # ext_famaily
    movq %r8, %rax
    shr $20, %eax
    and $0b11111111, %eax
    movl %eax, 0(%rdi)
    addq $4, %rdi

    # ext_famaily
    movq %r8, %rax
    shr $20, %eax
    and $0b11111111, %eax
    movl %eax, 0(%rdi)
    addq $4, %rdi

    ###

    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbp
    popq %rbx

    ret
