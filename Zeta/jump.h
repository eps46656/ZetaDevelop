#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_JumpBlock);

#if defined(__x86_64)

struct Zeta_JumpBlock {
    unsigned long long regs[8];
};

#endif

typedef __attribute__((sysv_abi)) void (*Zeta_JumpEntry)();

__attribute__((sysv_abi)) unsigned long long Zeta_SetJump(
    Zeta_JumpBlock* jmp_blk);

__attribute__((sysv_abi, noreturn)) unsigned long long Zeta_Jump(
    Zeta_JumpBlock* jmp_blk, unsigned code);

__attribute__((sysv_abi)) void Zeta_SetJumpEntry(Zeta_JumpBlock* jmp_blk,
                                                 Zeta_JumpEntry jmp_entry,
                                                 void* stack_top);

ZETA_ExternC_End;
