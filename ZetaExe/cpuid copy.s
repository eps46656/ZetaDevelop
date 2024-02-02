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

    andq 0xFFFFFFFF, %rax
    movq %rax, 0(%rdi)
    addq $8, %rdi

    movl %ebx, 0(%rdi)
    addq $4, %rdi

    movl %ecx, 0(%rdi)
    addq $4, %rdi

    movl %edx, 0(%rdi)
    addq $4, %rdi

    movl $0, 0(%rdi)
    addq $4, %rdi

    ###

    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbp
    popq %rbx

    ret
