.text
    .globl Thread_Create_
    .globl Thread_SwitchTo_
    .globl my_switch_

.global RIP_SAVE

my_switch_to_:
    # rdi pcb
    #

    movq    %rdi,   %rax

    # recover rax later
    movq      8(%rax),      %rbx
    movq     16(%rax),      %rcx
    movq     24(%rax),      %rdx

    movq     32(%rax),      %rsi
    movq     40(%rax),      %rdi

    movq     48(%rax),      %rbp
    movq     56(%rax),      %rsp

    movq     64(%rax),      %r8
    movq     72(%rax),      %r9
    movq     80(%rax),      %r10
    movq     88(%rax),      %r11

    movq     96(%rax),      %r12
    movq    104(%rax),      %r13
    movq    112(%rax),      %r14
    movq    120(%rax),      %r15

Thread_Create_:
    # rdi stack
    # rsi func

    # ret new_stack

    mov %rsp, %r8
    mov %rdi, %rsp

    pushq $0 # rbx
    pushq $0 # rsp
    pushq $0 # rbp
    pushq $0 # r12
    pushq $0 # r13
    pushq $0 # r14
    pushq $0 # r15
    pushq %rsi

    mov %rsp, %rax
    mov %r8, %rsp

    ret

Thread_SwitchTo_:
    # rdi dst
    # rsi src

    pushq %rbx
    pushq %rsp
    pushq %rbp
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    mov %rsp, 0(%rsi)
    mov $0, %rsi

    mov 0(%rdi), %rsp

    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbp
    popq %rsp
    popq %rbx

    ret

my_switch_2_:
    # rdi   pcb

    movq     %rdi,   %rax

    movq     $0,     0(%rax)
    movq     $1,     8(%rax)
    movq     $2,     16(%rax)
    movq     $3,     24(%rax)

    movq     $4,     32(%rax)
    movq     $5,     40(%rax)

    movq     $6,     48(%rax)
    movq     $7,     56(%rax)

    movq     $8,     64(%rax)
    movq     $9,     72(%rax)
    movq     $10,    80(%rax)
    movq     $11,    88(%rax)

    movq     $12,    96(%rax)
    movq     $13,   104(%rax)
    movq     $14,   112(%rax)
    movq     $15,   120(%rax)

    movq     $16,   128(%rax)

    movq     $17,   136(%rax)

    movq    $123456,      RIP_SAVE

    ret


