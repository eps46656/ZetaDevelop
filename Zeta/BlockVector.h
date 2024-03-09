#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef struct Zeta_BlockVector Zeta_BlockVector;

struct Zeta_BlockVector {
    void* context;

    size_t (*GetBlockSize)(void* context);

    size_t (*GetBlockNum)(void* context);

    void (*ReadBlock)(void* context, size_t blk_idx, byte_t* dst);

    void (*WriteBlock)(void* context, size_t blk_idx, byte_t const* src);
};

ZETA_extern_c_end;
