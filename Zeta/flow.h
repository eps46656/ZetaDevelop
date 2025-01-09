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

__attribute__((sysv_abi)) void Zeta_Flow_SetFunc_(Zeta_FlowBlock* flow_blk,
                                                  void* flow_context,
                                                  Zeta_FlowFunc flow_func,
                                                  void* stack_top);

#define ZETA_Flow_SetFunc_(tmp_flow_blk, tmp_flow_func, tmp_stack_top,   \
                           flow_blk, flow_context, flow_func, stack_top) \
    {                                                                    \
        Zeta_FlowBlock* tmp_flow_blk = (flow_blk);                       \
        Zeta_FlowFunc tmp_flow_func = (flow_func);                       \
        void* tmp_stack_top = (stack_top);                               \
                                                                         \
        ZETA_DebugAssert(tmp_flow_blk != NULL);                          \
        ZETA_DebugAssert(tmp_flow_func != NULL);                         \
        ZETA_DebugAssert(tmp_stack_top != NULL);                         \
                                                                         \
        Zeta_Flow_SetFunc_(tmp_flow_blk, (flow_context), tmp_flow_func,  \
                           __builtin_align_down(tmp_stack_top, 64));     \
    }                                                                    \
    ZETA_StaticAssert(TRUE)

#define ZETA_Flow_SetFunc(flow_blk, flow_context, flow_func, stack_top)      \
    ZETA_Flow_SetFunc_(ZETA_TmpName, ZETA_TmpName, ZETA_TmpName, (flow_blk), \
                       (flow_context), (flow_func), (stack_top))

__attribute__((sysv_abi, noreturn)) void Zeta_Flow_Jump_(
    Zeta_FlowBlock* dst_flow_blk, void* code);

#define ZETA_Flow_Jump_(tmp_dst_flow_blk, dst_flow_blk, code) \
    ({                                                        \
        Zeta_FlowBlock* tmp_dst_flow_blk = (dst_flow_blk);    \
        ZETA_DebugAssert(tmp_dst_flow_blk != NULL);           \
                                                              \
        Zeta_Flow_Jump_(tmp_dst_flow_blk, (code));            \
    })

#define ZETA_Flow_Jump(dst_flow_blk, code) \
    ZETA_Flow_Jump_(ZETA_TmpName, (dst_flow_blk), (code))

__attribute__((sysv_abi)) void* Zeta_Flow_Switch_(Zeta_FlowBlock* src_flow_blk,
                                                  Zeta_FlowBlock* dst_flow_blk,
                                                  void* code);

#define ZETA_Flow_Switch_(tmp_src_flow_blk, tmp_dst_flow_blk, src_flow_blk, \
                          dst_flow_blk, code)                               \
    ({                                                                      \
        Zeta_FlowBlock* tmp_src_flow_blk = (src_flow_blk);                  \
        Zeta_FlowBlock* tmp_dst_flow_blk = (dst_flow_blk);                  \
                                                                            \
        ZETA_DebugAssert(tmp_src_flow_blk != NULL);                         \
        ZETA_DebugAssert(tmp_dst_flow_blk != NULL);                         \
                                                                            \
        Zeta_Flow_Switch_(tmp_src_flow_blk, tmp_dst_flow_blk, (code));      \
    })

#define ZETA_Flow_Switch(src_flow_blk, dst_flow_blk, code)        \
    ZETA_Flow_Switch_(ZETA_TmpName, ZETA_TmpName, (src_flow_blk), \
                      (dst_flow_blk), (code))

ZETA_ExternC_End;
