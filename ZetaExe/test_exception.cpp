#include <cascade_allocator.h>
#include <debugger.h>
#include <flow.h>
#include <memory.h>

#include <iostream>

#define STACK_SIZE (256 * 1024)

void* AllocateStack() {
    return (unsigned char*)std::malloc(STACK_SIZE) + STACK_SIZE;
}

void DeallocateStack(void* stack) {
    return std::free((unsigned char*)stack - STACK_SIZE);
}

Zeta_CascadeAllocator cascade_allocator_instance;
Zeta_Allocator cascade_allocator;

void InitCascadeAllocator() {
    size_t mem_size{ 256 * 1024 * 1024 };

    cascade_allocator_instance.align = 16;
    cascade_allocator_instance.mem = std::malloc(mem_size);
    cascade_allocator_instance.size = mem_size;

    Zeta_CascadeAllocator_Init(&cascade_allocator_instance);

    Zeta_CascadeAllocator_DeployAllocator(&cascade_allocator_instance,
                                          &cascade_allocator);

    if (zeta_cascade_allocator == NULL) {
        zeta_cascade_allocator = &cascade_allocator;
    }
}

void* botton_exception_handler_stack;
Zeta_FlowBlock bottom_exception_handler_blk;

Zeta_FlowBlock* prv_exception_handler_blk = &bottom_exception_handler_blk;

ZETA_DeclareStruct(ExceptionBlock);

struct ExceptionBlock {
    int code;
    char const* message;
};

__attribute__((sysv_abi, noreturn)) void ButtomExceptionHandler(
    void* context, void* exception_blk_) {
    ZETA_Unused(context);

    ExceptionBlock* exception_blk = (ExceptionBlock*)exception_blk_;

    if (exception_blk == NULL) {
        ZETA_PrintVar("NULL");
        exit(0);
    }

    ZETA_PrintVar(exception_blk->code);
    ZETA_PrintVar(exception_blk->message);

    ZETA_Allocator_Deallocate(zeta_cascade_allocator, exception_blk);

    exit(1);
}

void G() {
    int exception_code;

    std::cout << "input exceotion code:";
    std::cin >> exception_code;

    ExceptionBlock* exception_blk;

    switch (exception_code) {
        case 0: ZETA_PrintVar("no exception"); return;

        case 1:
            ZETA_PrintVar("throw exception 1");

            exception_blk = (ExceptionBlock*)ZETA_Allocator_SafeAllocate(
                zeta_cascade_allocator, alignof(ExceptionBlock),
                sizeof(ExceptionBlock));

            exception_blk->code = 1;
            exception_blk->message = "exception 1";

            ZETA_Flow_Jump(prv_exception_handler_blk, exception_blk);

            return;

        case 2:
            ZETA_PrintVar("throw exception 2");

            exception_blk = (ExceptionBlock*)ZETA_Allocator_SafeAllocate(
                zeta_cascade_allocator, alignof(ExceptionBlock),
                sizeof(ExceptionBlock));

            exception_blk->code = 2;
            exception_blk->message = "exception 2";

            ZETA_Flow_Jump(prv_exception_handler_blk, exception_blk);

            return;

        case 3:
            ZETA_PrintVar("throw exception 3");

            exception_blk = (ExceptionBlock*)ZETA_Allocator_SafeAllocate(
                zeta_cascade_allocator, alignof(ExceptionBlock),
                sizeof(ExceptionBlock));

            exception_blk->code = 3;
            exception_blk->message = "exception 3";

            ZETA_Flow_Jump(prv_exception_handler_blk, exception_blk);

            return;
    }
};

void F() {
    Zeta_FlowBlock cur_exception_handler_blk;

    Zeta_FlowBlock* origin_prv_exception_handler_blk =
        prv_exception_handler_blk;

    prv_exception_handler_blk = &cur_exception_handler_blk;

    void* code = ZETA_Flow_Switch(&cur_exception_handler_blk,
                                  &cur_exception_handler_blk, NULL);

    ZETA_PrintCurPos;

    if (code == NULL) {
        ZETA_PrintVar("normal a");

        G();

        ZETA_PrintVar("normal b");

        return;
    }

    ExceptionBlock* exception_blk = (ExceptionBlock*)ZETA_AddrToPtr(code);

    switch (exception_blk->code) {
        case 1:
            ZETA_PrintVar("catched exception 1");

            ZETA_PrintVar(exception_blk->message);

            ZETA_Allocator_Deallocate(zeta_cascade_allocator, exception_blk);

            break;

        case 2:
            ZETA_PrintVar("catched exception 2");

            ZETA_PrintVar(exception_blk->message);

            ZETA_Allocator_Deallocate(zeta_cascade_allocator, exception_blk);

            break;

        default:
            ZETA_PrintVar("unrecognized exception");
            ZETA_PrintVar("cascaded throw exception");

            ZETA_Flow_Jump(origin_prv_exception_handler_blk, code);
    }
};

void main1() {
    InitCascadeAllocator();

    botton_exception_handler_stack = AllocateStack();

    ZETA_Flow_SetFunc(&bottom_exception_handler_blk, NULL,
                      ButtomExceptionHandler, botton_exception_handler_stack);

    ZETA_PrintCurPos;

    F();

    ZETA_PrintCurPos;
}

int main() {
    ZETA_PrintCurPos;

    main1();

    ZETA_PrintCurPos;

    return 0;
}
