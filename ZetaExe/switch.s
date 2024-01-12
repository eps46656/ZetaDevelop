.text

.globl my_switch_to_
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

.globl Thread_Create_
Thread_Create_:
    # rdi stack
    # rsi func

    # ret new_stack

    movq %rsp, %r8
    movq %rdi, %rsp

    pushq %rsi # func arg

    leaq Thread_Frame_(%rip), %rax
    pushq %rax # ret addr

    pushq $0 # rbx
    pushq %rdi # rbp
    pushq $0 # r12
    pushq $0 # r13
    pushq $0 # r14
    pushq $0 # r15

    movq %rsp, %rax
    movq %r8, %rsp

    ret

.globl Thread_Switch
Thread_Switch:
    # rdi src
    # rsi dst

    pushq %rbx
    pushq %rbp
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    movq %rsp, 0(%rdi)
    movq $0, %rdi

    movq 0(%rsi), %rsp

    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbp
    popq %rbx

    ret

.global Thread_Frame_
Thread_Frame_:
    popq %rdi # func
    callq *%rdi

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


