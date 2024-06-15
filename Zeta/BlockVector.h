#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_BlockVector);

struct Zeta_BlockVector {
    void* context;

    size_t (*GetBlockSize)(void* context);

    size_t (*GetBlockCnt)(void* context);

    void (*ReadBlock)(void* context, size_t blk_idx, void* dst);

    void (*WriteBlock)(void* context, size_t blk_idx, void const* src);
};

void Zeta_BlockVector_Init(void* bv);

ZETA_ExternC_End;
