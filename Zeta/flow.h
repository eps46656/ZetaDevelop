#pragma once

#include "debugger.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_FlowBlock);

#if defined(__x86_64)

struct Zeta_FlowBlock {
    unsigned char regs[64];
} __attribute__((aligned(8)));

#endif

typedef __attribute__((sysv_abi, noreturn)) void (*Zeta_FlowFunc)(void* context,
                                                                  void* code);

void Zeta_Flow_SetFunc(Zeta_FlowBlock* flow_blk, void* flow_context,
                       Zeta_FlowFunc flow_func, void* stack_top);

__attribute__((noreturn)) void Zeta_Flow_Jump(Zeta_FlowBlock* dst_flow_blk,
                                              void* code);

__attribute__((always_inline)) void* Zeta_Flow_Switch(
    Zeta_FlowBlock* src_flow_blk, Zeta_FlowBlock* dst_flow_blk, void* code);

ZETA_ExternC_End;
