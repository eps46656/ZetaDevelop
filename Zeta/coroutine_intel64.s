.intel_syntax

.equ ZETA_CoroutineBlock_rbx, 0
.equ ZETA_CoroutineBlock_rsp, 8
.equ ZETA_CoroutineBlock_rbp, 16
.equ ZETA_CoroutineBlock_r12, 24
.equ ZETA_CoroutineBlock_r13, 32
.equ ZETA_CoroutineBlock_r14, 40
.equ ZETA_CoroutineBlock_r15, 48

.text

.globl Zeta_Coroutine_SetFunc_
Zeta_Coroutine_SetFunc_:
    # rdi cor_blk
    # rsi cor_func_context
    # rdx cor_func
    # rcx stack_top

    mov rax, 0
    mov [rdi + ZETA_CoroutineBlock_rbx], rax
    mov [rdi + ZETA_CoroutineBlock_r13], rax
    mov [rdi + ZETA_CoroutineBlock_r14], rax
    mov [rdi + ZETA_CoroutineBlock_r15], rax

    mov rax, rcx
    sub rax, 16
    mov [rax], rdx

    mov [rdi + ZETA_CoroutineBlock_rsp], rax
    mov [rdi + ZETA_CoroutineBlock_rbp], rcx
    mov [rdi + ZETA_CoroutineBlock_r12], rsi

    ret

.globl Zeta_Coroutine_Jump_
Zeta_Coroutine_Jump_:
    # rdi dst_cor_blk
    # rsi code

    mov rbx, [rdi + ZETA_CoroutineBlock_rbx]
    mov rsp, [rdi + ZETA_CoroutineBlock_rsp]
    mov rbp, [rdi + ZETA_CoroutineBlock_rbp]
    mov r12, [rdi + ZETA_CoroutineBlock_r12]
    mov r13, [rdi + ZETA_CoroutineBlock_r13]
    mov r14, [rdi + ZETA_CoroutineBlock_r14]
    mov r15, [rdi + ZETA_CoroutineBlock_r15]

    mov rdi, r12
    mov rax, rsi

    ret

.globl Zeta_Coroutine_Switch_
Zeta_Coroutine_Switch_:
    # rdi src_cor_blk
    # rsi dst_cor_blk
    # rdx code

    mov [rdi + ZETA_CoroutineBlock_rbx], rbx
    mov [rdi + ZETA_CoroutineBlock_rsp], rsp
    mov [rdi + ZETA_CoroutineBlock_rbp], rbp
    mov [rdi + ZETA_CoroutineBlock_r12], r12
    mov [rdi + ZETA_CoroutineBlock_r13], r13
    mov [rdi + ZETA_CoroutineBlock_r14], r14
    mov [rdi + ZETA_CoroutineBlock_r15], r15

    mov rbx, [rsi + ZETA_CoroutineBlock_rbx]
    mov rsp, [rsi + ZETA_CoroutineBlock_rsp]
    mov rbp, [rsi + ZETA_CoroutineBlock_rbp]
    mov r12, [rsi + ZETA_CoroutineBlock_r12]
    mov r13, [rsi + ZETA_CoroutineBlock_r13]
    mov r14, [rsi + ZETA_CoroutineBlock_r14]
    mov r15, [rsi + ZETA_CoroutineBlock_r15]

    mov rdi, r12
    mov rax, rdx

    ret
