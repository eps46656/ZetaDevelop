#include "cache_manager.h"

void Zeta_CacheManager_Init(Zeta_CacheManager* cm) {
    cm->context = NULL;

    cm->Open = NULL;

    cm->Close = NULL;

    cm->GetBlockSize = NULL;

    cm->SetMaxCacheCnt = NULL;

    cm->ReadBlock = NULL;

    cm->WriteBlock = NULL;

    cm->FlushBlock = NULL;

    cm->Flush = NULL;

    cm->FlushAll = NULL;
}
