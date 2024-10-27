#pragma once

#include "block_vector.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_DummyBlockVector);

struct Zeta_DummyBlockVector {
    size_t blk_size;
    size_t blk_cnt;

    byte_t* data;

    size_t read_cnt;
    size_t write_cnt;
};

void Zeta_DummyBlockVector_Init(void* dbv);

size_t Zeta_DummyBlockVector_GetBlockSize(void* dbv);

void Zeta_DummyBlockVector_ReadBlock(void* dbv, size_t blk_num, void* dst);

void Zeta_DummyBlockVector_WriteBlock(void* dbv, size_t blk_num,
                                      void const* src);

void Zeta_DummyBlockVector_DeployBlockVector(void* dbv,
                                             Zeta_BlockVector* blk_vec);

ZETA_ExternC_End;
