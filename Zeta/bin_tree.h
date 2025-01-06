#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_BinTreeNodeOperator);

struct Zeta_BinTreeNodeOperator {
    void* context;

    void* (*GetP)(void const* context, void* n);
    void* (*GetL)(void const* context, void* n);
    void* (*GetR)(void const* context, void* n);

    void (*SetP)(void const* context, void* n, void* m);
    void (*SetL)(void const* context, void* n, void* m);
    void (*SetR)(void const* context, void* n, void* m);

    int (*GetColor)(void const* context, void* n);
    void (*SetColor)(void const* context, void* n, int p_color);

    size_t (*GetAccSize)(void const* context, void* n);
    void (*SetAccSize)(void const* context, void* n, size_t acc_size);
};

// -----------------------------------------------------------------------------

#define ZETA_BinTreeNodeOperator_Call_(btn_opr, member_func, ...)              \
    ZETA_CallMemberFunc(                                                       \
        (Zeta_BinTreeNodeOperator const*)ZETA_ToVoidPtr(btn_opr), member_func, \
        __VA_ARGS__)

#define ZETA_BinTreeNodeOperator_GetP(btn_opr, n) \
    ZETA_BinTreeNodeOperator_Call_((btn_opr), GetP, (n))

#define ZETA_BinTreeNodeOperator_GetL(btn_opr, n) \
    ZETA_BinTreeNodeOperator_Call_((btn_opr), GetL, (n))

#define ZETA_BinTreeNodeOperator_GetR(btn_opr, n) \
    ZETA_BinTreeNodeOperator_Call_((btn_opr), GetR, (n))

#define ZETA_BinTreeNodeOperator_SetP(btn_opr, n, m) \
    ZETA_BinTreeNodeOperator_Call_((btn_opr), SetP, (n), (m))

#define ZETA_BinTreeNodeOperator_SetL(btn_opr, n, m) \
    ZETA_BinTreeNodeOperator_Call_((btn_opr), SetL, (n), (m))

#define ZETA_BinTreeNodeOperator_SetR(btn_opr, n, m) \
    ZETA_BinTreeNodeOperator_Call_((btn_opr), SetR, (n), (m))

#define ZETA_BinTreeNodeOperator_GetColor(btn_opr, n) \
    ZETA_BinTreeNodeOperator_Call_((btn_opr), GetColor, (n))

#define ZETA_BinTreeNodeOperator_SetColor(btn_opr, n, color) \
    ZETA_BinTreeNodeOperator_Call_((btn_opr), SetColor, (n), (color))

#define ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, n) \
    ZETA_BinTreeNodeOperator_Call_((btn_opr), GetAccSize, (n))

#define ZETA_BinTreeNodeOperator_SetAccSize(btn_opr, n, acc_size) \
    ZETA_BinTreeNodeOperator_Call_((btn_opr), SetAccSize, (n), (acc_size))

// -----------------------------------------------------------------------------

void Zeta_BinTree_InitOpr(Zeta_BinTreeNodeOperator* btn_opr);

size_t Zeta_BinTree_Count(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

size_t Zeta_BinTree_GetSize(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

void Zeta_BinTree_SetSize(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                          size_t size);

void Zeta_BinTree_SetDiffSize(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                              size_t diff_size);

void Zeta_BinTree_AttatchL(Zeta_BinTreeNodeOperator const* btn_opr, void* pos,
                           void* n);

void Zeta_BinTree_AttatchR(Zeta_BinTreeNodeOperator const* btn_opr, void* pos,
                           void* n);

void Zeta_BinTree_Detach(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

void Zeta_BinTree_EraseAll(Zeta_BinTreeNodeOperator const* btn_opr, void* root,
                           void* callback_context,
                           void (*Callback)(void* callback_context, void* n));

void Zeta_BinTree_Swap(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                       void* m);

void Zeta_BinTree_RotateL(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

void Zeta_BinTree_RotateR(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

void* Zeta_BinTree_StepL(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

void* Zeta_BinTree_StepR(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

void Zeta_BinTree_AccessL(void** dst_n, size_t* dst_tail_idx,
                          Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                          size_t idx);

void Zeta_BinTree_AccessR(void** dst_n, size_t* dst_tail_idx,
                          Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                          size_t idx);

void Zeta_BinTree_AdvanceL(void** dst_n, size_t* dst_tail_idx,
                           Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                           size_t step);

void Zeta_BinTree_AdvanceR(void** dst_n, size_t* dst_tail_idx,
                           Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                           size_t step);

void Zeta_BinTree_GetAccSize(size_t* dst_l_acc_size, size_t* dst_r_acc_size,
                             Zeta_BinTreeNodeOperator const* btn_opr, void* n);

void Zeta_BinTree_Sanitize(Zeta_BinTreeNodeOperator const* btn_opr, void* root);

ZETA_ExternC_End;
