#include "../Zeta/define.h"

typedef struct PCB PCB;

struct PCB {
    unsigned long long rax;
    unsigned long long rbx;
    unsigned long long rcx;
    unsigned long long rdx;

    unsigned long long rsi;
    unsigned long long rdi;

    unsigned long long rbp;
    unsigned long long rsp;

    unsigned long long r8;
    unsigned long long r9;
    unsigned long long r10;
    unsigned long long r11;

    unsigned long long r12;
    unsigned long long r13;
    unsigned long long r14;
    unsigned long long r15;

    unsigned long long eflags;

    unsigned long long rip;
};

unsigned long long RIP_SAVE;

void __stdcall SwitchTo(PCB* pcb);

typedef unsigned long long RX;

typedef struct Thread Thread;

struct Thread {
    RX stack;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

Thread main_thread;
Thread func1_thread;
Thread func2_thread;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

RX Thread_Create_(RX stack, void (*func)()) __attribute__((sysv_abi));

void Thread_Frame_() __attribute__((sysv_abi));

void Thread_Create(Thread* thread, void (*func)()) {
    const size_t stack_size = 16 * 1024 * 1024;

    thread->stack =
        Thread_Create_((RX)(uintptr_t)malloc(stack_size) + stack_size, func);
}

void Thread_Switch(Thread* dst, Thread* src) __attribute__((sysv_abi));

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

int func1_ret;
int func2_ret;

void Func1() {
    printf("func1\n");

    for (int i = 99; 90 <= i; --i) {
        func1_ret = i;
        Thread_Switch(&func1_thread, &main_thread);
    }

    for (;;) {
        func1_ret = INT_MIN;
        Thread_Switch(&func1_thread, &main_thread);
    }
}

void Func2() {
    printf("func2\n");

    for (int i = 0; i < 10; ++i) {
        func2_ret = i;
        Thread_Switch(&func2_thread, &main_thread);
    }

    for (;;) {
        func2_ret = INT_MIN;
        Thread_Switch(&func2_thread, &main_thread);
    }
}

void Func3() { printf("func3\n"); }

void PrintPCB(const PCB* pcb) {
    printf("rax\t%lld\t%llu\n", pcb->rax, pcb->rax);
    printf("rbx\t%lld\t%llu\n", pcb->rbx, pcb->rbx);
    printf("rcx\t%lld\t%llu\n", pcb->rcx, pcb->rcx);
    printf("rdx\t%lld\t%llu\n", pcb->rdx, pcb->rdx);

    printf("rsi\t%lld\t%llu\n", pcb->rsi, pcb->rsi);
    printf("rdi\t%lld\t%llu\n", pcb->rdi, pcb->rdi);

    printf("rbp\t%lld\t%llu\n", pcb->rbp, pcb->rbp);
    printf("rsp\t%lld\t%llu\n", pcb->rsp, pcb->rsp);

    printf("r8\t%lld\t%llu\n", pcb->r8, pcb->r8);
    printf("r9\t%lld\t%llu\n", pcb->r9, pcb->r9);
    printf("r10\t%lld\t%llu\n", pcb->r10, pcb->r10);
    printf("r11\t%lld\t%llu\n", pcb->r11, pcb->r11);

    printf("r12\t%lld\t%llu\n", pcb->r12, pcb->r12);
    printf("r13\t%lld\t%llu\n", pcb->r13, pcb->r13);
    printf("r14\t%lld\t%llu\n", pcb->r14, pcb->r14);
    printf("r15\t%lld\t%llu\n", pcb->r15, pcb->r15);

    printf("eflags\t%lld\t%llu\n", pcb->eflags, pcb->eflags);

    printf("rip\t%lld\t%llu\n", pcb->rip, pcb->rip);
}

int main() {
    Thread_Create(&func1_thread, Func1);
    Thread_Create(&func2_thread, Func2);

    func1_ret = INT_MAX;
    func2_ret = INT_MAX;

    ZETA_PRINT_POS;

    do {
        Thread_Switch(&main_thread, &func1_thread);
        ZETA_PRINT_VAR("%d\n", func1_ret);

        Thread_Switch(&main_thread, &func2_thread);
        ZETA_PRINT_VAR("%d\n", func2_ret);
    } while (func2_ret != INT_MIN);

    return 0;
}
