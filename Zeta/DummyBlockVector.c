#include "DummyBlockVector.h"

#include "utils.h"

void Zeta_DummyBlockVector_Init(void* dbv_) {
    Zeta_DummyBlockVector* dbv = dbv_;
    ZETA_DebugAssert(dbv != NULL);

    ZETA_DebugAssert(0 < dbv->blk_size);

    dbv->read_cnt = 0;
    dbv->write_cnt = 0;
}

size_t Zeta_DummyBlockVector_GetBlockSize(void* dbv_) {
    Zeta_DummyBlockVector* dbv = dbv_;
    ZETA_DebugAssert(dbv != NULL);

    return dbv->blk_size;
}

size_t Zeta_DummyBlockVector_GetBlockNum(void* dbv_) {
    Zeta_DummyBlockVector* dbv = dbv_;
    ZETA_DebugAssert(dbv != NULL);

    return dbv->blk_num;
}

void Zeta_DummyBlockVector_ReadBlock(void* dbv_, size_t blk_idx, void* dst) {
    Zeta_DummyBlockVector* dbv = dbv_;
    ZETA_DebugAssert(dbv != NULL);

    ZETA_DebugAssert(blk_idx < dbv->blk_num);

    ++dbv->read_cnt;

    Zeta_MemCopy(dst, dbv->data + dbv->blk_size * blk_idx, dbv->blk_size);
}

void Zeta_DummyBlockVector_WriteBlock(void* dbv_, size_t blk_idx,
                                      void const* src) {
    Zeta_DummyBlockVector* dbv = dbv_;
    ZETA_DebugAssert(dbv != NULL);

    ZETA_DebugAssert(blk_idx < dbv->blk_num);

    ++dbv->write_cnt;

    Zeta_MemCopy(dbv->data + dbv->blk_size * blk_idx, src, dbv->blk_size);
}

void Zeta_DummyBlockVector_DeployBlockVector(void* dbv_,
                                             Zeta_BlockVector* blk_vec) {
    Zeta_DummyBlockVector* dbv = dbv_;
    ZETA_DebugAssert(dbv != NULL);

    ZETA_DebugAssert(blk_vec != NULL);

    blk_vec->context = dbv;

    blk_vec->GetBlockSize = Zeta_DummyBlockVector_GetBlockSize;
    blk_vec->GetBlockNum = Zeta_DummyBlockVector_GetBlockNum;

    blk_vec->ReadBlock = Zeta_DummyBlockVector_ReadBlock;
    blk_vec->WriteBlock = Zeta_DummyBlockVector_WriteBlock;
}
