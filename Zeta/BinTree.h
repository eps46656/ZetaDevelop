#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef struct Zeta_BinTreeNodeOperator Zeta_BinTreeNodeOperator;

struct Zeta_BinTreeNodeOperator {
    void* context;

    void* (*GetP)(void* context, void* n);
    void* (*GetL)(void* context, void* n);
    void* (*GetR)(void* context, void* n);

    void (*SetP)(void* context, void* n, void* m);
    void (*SetL)(void* context, void* n, void* m);
    void (*SetR)(void* context, void* n, void* m);

    int (*GetPColor)(void* context, void* n);
    int (*GetLColor)(void* context, void* n);
    int (*GetRColor)(void* context, void* n);

    void (*SetPColor)(void* context, void* n, int p_color);
    void (*SetLColor)(void* context, void* n, int l_color);
    void (*SetRColor)(void* context, void* n, int r_color);

    size_t (*GetAccSize)(void* context, void* n);
    void (*SetAccSize)(void* context, void* n, size_t acc_size);
};

size_t Zeta_BinTree_GetSize(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

void Zeta_BinTree_SetSize(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                          size_t size);

void Zeta_BinTree_AttatchL(Zeta_BinTreeNodeOperator const* btn_opr, void* pos,
                           void* n);

void Zeta_BinTree_AttatchR(Zeta_BinTreeNodeOperator const* btn_opr, void* pos,
                           void* n);

void Zeta_BinTree_Detach(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

void Zeta_BinTree_Swap(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                       void* m);

void Zeta_BinTree_RotateL(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

void Zeta_BinTree_RotateR(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

void* Zeta_BinTree_StepL(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

void* Zeta_BinTree_StepR(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

void Zeta_BinTree_Access(void** dst_n, size_t* dst_tail_idx,
                         Zeta_BinTreeNodeOperator const* cbtn_opr, void* n,
                         size_t idx);

void Zeta_BinTree_GetAccSize(size_t* dst_l_acc_size, size_t* dst_r_acc_size,
                             Zeta_BinTreeNodeOperator const* cbtn_opr, void* n);

ZETA_extern_c_end;
