#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_CoroutineBlock);

#if defined(__x86_64)

struct Zeta_CoroutineBlock {
    unsigned char regs[56];
} __attribute__((aligned(8)));

#endif

typedef __attribute__((sysv_abi,
                       noreturn)) void (*Zeta_CoroutineFunc)(void* context);

void Zeta_Coroutine_SetFunc(Zeta_CoroutineBlock* cor_blk, void* cor_context,
                            Zeta_CoroutineFunc cor_func, void* stack_top);

__attribute__((noreturn)) void Zeta_Coroutine_Jump(
    Zeta_CoroutineBlock* dst_cor_blk, int code);

int Zeta_Coroutine_Switch(Zeta_CoroutineBlock* src_cor_blk,
                          Zeta_CoroutineBlock* dst_cor_blk, int code);

ZETA_ExternC_End;
