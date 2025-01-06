#include "coroutine.h"

#include "debugger.h"

__attribute__((sysv_abi)) void Zeta_Coroutine_SetFunc_(
    Zeta_CoroutineBlock* cor_blk, void* cor_context,
    Zeta_CoroutineFunc cor_func, void* stack_top);

void Zeta_Coroutine_SetFunc(Zeta_CoroutineBlock* cor_blk, void* cor_context,
                            Zeta_CoroutineFunc cor_func, void* stack_top) {
    ZETA_DebugAssert(cor_blk != NULL);
    ZETA_DebugAssert(cor_func != NULL);
    ZETA_DebugAssert(stack_top != NULL);

    Zeta_Coroutine_SetFunc_(cor_blk, cor_context, cor_func,
                            __builtin_align_down(stack_top, 16));
}

__attribute__((sysv_abi, noreturn)) void Zeta_Coroutine_Jump_(
    Zeta_CoroutineBlock* dst_cor_blk, int code);

void Zeta_Coroutine_Jump(Zeta_CoroutineBlock* dst_cor_blk, int code) {
    ZETA_DebugAssert(dst_cor_blk != NULL);

    Zeta_Coroutine_Jump_(dst_cor_blk, code);
}

__attribute__((sysv_abi)) int Zeta_Coroutine_Switch_(
    Zeta_CoroutineBlock* src_cor_blk, Zeta_CoroutineBlock* dst_cor_blk,
    int code);

int Zeta_Coroutine_Switch(Zeta_CoroutineBlock* src_cor_blk,
                          Zeta_CoroutineBlock* dst_cor_blk, int code) {
    ZETA_DebugAssert(src_cor_blk != NULL);
    ZETA_DebugAssert(dst_cor_blk != NULL);

    return Zeta_Coroutine_Switch_(src_cor_blk, dst_cor_blk, code);
}
