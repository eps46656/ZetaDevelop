#include "flow.h"

#include "debugger.h"

__attribute__((sysv_abi)) void Zeta_Flow_SetFunc_(Zeta_FlowBlock* flow_blk,
                                                  void* flow_context,
                                                  Zeta_FlowFunc flow_func,
                                                  void* stack_top);

void Zeta_Flow_SetFunc(Zeta_FlowBlock* flow_blk, void* flow_context,
                       Zeta_FlowFunc flow_func, void* stack_top) {
    ZETA_DebugAssert(flow_blk != NULL);
    ZETA_DebugAssert(flow_func != NULL);
    ZETA_DebugAssert(stack_top != NULL);

    Zeta_Flow_SetFunc_(flow_blk, flow_context, flow_func,
                       __builtin_align_down(stack_top, 64));
}

__attribute__((sysv_abi, noreturn)) void Zeta_Flow_Jump_(
    Zeta_FlowBlock* dst_flow_blk, void* code);

void Zeta_Flow_Jump(Zeta_FlowBlock* dst_flow_blk, void* code) {
    ZETA_DebugAssert(dst_flow_blk != NULL);

    Zeta_Flow_Jump_(dst_flow_blk, code);
}
