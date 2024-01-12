.text
    .globl syscall__write_

my__ADD_:
    addq    %rdx,   %rcx
    movq    %rcx,   %rax
    ret

my__GetClk_:
    rdtsc
    shl     $32,    %rdx
    or      %rdx,   %rax
    ret

my__GetProcessorType_:
    movl    $1,     %eax
    cpuid
    shr     $8,    %eax
    and     $0b1111,  %eax
    ret

my__GetProcessorSerialNumber_:
    movl    $3,     %eax
    cpuid
    ret

my__GetNumOfCores_:
    mov $0xb, %eax
    mov %rbx, %r8
    cpuid
    mov %r8, %rbx
    mov %edx, %eax
    ret

my__GetCoreID_:
    mov $0xb, %eax
    mov %rbx, %r8
    cpuid
    mov %r8, %rbx
    mov %edx, %eax
    ret


my__GetManID_:
    push    %rcx
    movl    $0,     %eax
    cpuid
    pop     %rax
    movl    %ebx,   0(%rax)
    movl    %edx,   4(%rax)
    movl    %ecx,   8(%rax)
    movl    $0,     12(%rax)
    ret

syscall__write_:
    # rdi   fd
    # rsi   data
    # rdx   data_len

    mov     $1,     %rax

    # write
    # rax   1
    # rdi   fd
    # rsi   data
    # rdx   data_len

    syscall
    ret


