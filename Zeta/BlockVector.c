#include "BlockVector.h"

void Zeta_BlockVector_Init(void* bv_) {
    Zeta_BlockVector* bv = bv_;

    bv->context = NULL;

    bv->GetBlockSize = NULL;
    bv->GetBlockNum = NULL;

    bv->ReadBlock = NULL;
    bv->WriteBlock = NULL;
}
