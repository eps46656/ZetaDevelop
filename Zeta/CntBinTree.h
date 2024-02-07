#pragma once

#include "define.h"

ZETA_extern_c_beg;

    typedef struct Zeta_CntBinTreeNodeOpr Zeta_CntBinTreeNodeOpr;

struct Zeta_CntBinTreeNodeOpr {
    void* context;

    void* (*GetP)(void* context, void* n);
    void* (*GetL)(void* context, void* n);
    void* (*GetR)(void* context, void* n);

    size_t (*GetAccSize)(void* context, void* n);
    size_t (*GetSize)(void* context, void* n);
    void (*SetSize)(void* context, void* n, size_t size);
};

void Zeta_CntBinTree_Access(void** dst_n, size_t* dst_idx,
                            Zeta_CntBinTreeNodeOpr* cbtn_opr, void* n,
                            size_t idx);

void Zeta_CntBinTree_GetAccSize(size_t* dst_l_acc_size, size_t* dst_r_acc_size,
                                Zeta_CntBinTreeNodeOpr* cbtn_opr, void* n);

ZETA_extern_c_end;
