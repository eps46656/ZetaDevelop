#include <flow.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <list>
#include <unordered_map>

#include "../Zeta/define.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

constexpr size_t stack_size{ 16 * 1024 * 1024 };

void* AllocateStack() {
    void* stack{ std::malloc(stack_size) };
    return (unsigned char*)stack + stack_size;
}

void DeallocateStack(void* stack) {
    std::free((unsigned char*)stack - stack_size);
}

void* f_stack{ AllocateStack() };
void* g_stack{ AllocateStack() };
void* h_stack{ AllocateStack() };

Zeta_FlowBlock main_flow_blk;
Zeta_FlowBlock f_flow_blk;
Zeta_FlowBlock g_flow_blk;
Zeta_FlowBlock h_flow_blk;

bool f_is_finished{ false };
bool g_is_finished{ false };
bool h_is_finished{ false };

unsigned char continue_ptr[1];
unsigned char terminate_ptr[1];

__attribute__((sysv_abi, noreturn)) void FFunc(void* context, void* code) {
    ZETA_Unused(context);
    ZETA_Unused(code);

    for (int i{ 0 }; i < 100; ++i) {
        ZETA_PrintVar(i);

        void* code = ZETA_Flow_Switch(&f_flow_blk, &main_flow_blk, 0);

        ZETA_PrintVar(code);

        if (code == terminate_ptr) {
            ZETA_PrintVar("terminated");
            break;
        }
    }

    f_is_finished = true;

    ZETA_Flow_Jump(&main_flow_blk, 0);
}

__attribute__((sysv_abi, noreturn)) void GFunc(void* context, void* code) {
    ZETA_Unused(context);
    ZETA_Unused(code);

    for (int i{ 0 }; i < 70; ++i) {
        ZETA_PrintVar(i);

        if (ZETA_Flow_Switch(&g_flow_blk, &main_flow_blk, NULL) ==
            terminate_ptr) {
            ZETA_PrintVar("terminated");
            break;
        }
    }

    g_is_finished = true;

    ZETA_Flow_Jump(&main_flow_blk, 0);
}

__attribute__((sysv_abi, noreturn)) void HFunc(void* context, void* code) {
    ZETA_Unused(context);
    ZETA_Unused(code);

    for (int i{ 0 }; i < 50; ++i) {
        ZETA_PrintVar(i);

        if (ZETA_Flow_Switch(&h_flow_blk, &main_flow_blk, NULL) ==
            terminate_ptr) {
            ZETA_PrintVar("terminated");
            break;
        }
    }

    h_is_finished = true;

    ZETA_Flow_Jump(&main_flow_blk, 0);
}

void main1() {
    ZETA_Flow_SetFunc(&f_flow_blk, nullptr, FFunc, f_stack);
    ZETA_Flow_SetFunc(&g_flow_blk, nullptr, GFunc, g_stack);
    ZETA_Flow_SetFunc(&h_flow_blk, nullptr, HFunc, h_stack);

    ZETA_PrintCurPos;

    ZETA_PrintVar((void*)continue_ptr);
    ZETA_PrintVar((void*)terminate_ptr);

    bool b = TRUE;

    while (b) {
        b = FALSE;

        ZETA_PrintCurPos;

        bool termiate;

        if (!f_is_finished) {
            b = TRUE;

            std::cout << "determine to terminate F:";
            std::cin >> termiate;

            ZETA_Flow_Switch(&main_flow_blk, &f_flow_blk,
                             termiate ? terminate_ptr : continue_ptr);
        }

        ZETA_PrintCurPos;

        if (!g_is_finished) {
            b = TRUE;

            std::cout << "determine to terminate G:";
            std::cin >> termiate;

            ZETA_Flow_Switch(&main_flow_blk, &g_flow_blk,
                             termiate ? terminate_ptr : continue_ptr);
        }

        ZETA_PrintCurPos;

        if (!h_is_finished) {
            b = TRUE;

            std::cout << "determine to terminate H:";
            std::cin >> termiate;

            ZETA_Flow_Switch(&main_flow_blk, &h_flow_blk,
                             termiate ? terminate_ptr : continue_ptr);
        }
    }

    ZETA_PrintCurPos;

    return;
}

int main() {
    main1();
    printf("ok\n");
    return 0;
};
