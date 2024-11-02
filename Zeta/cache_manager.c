#include "cache_manager.h"

void Zeta_CacheManager_Init(Zeta_CacheManager* cm) {
    cm->context = NULL;

    cm->Open = NULL;

    cm->Close = NULL;

    cm->GetWidth = NULL;

    cm->SetMaxCacheCnt = NULL;

    cm->Read = NULL;

    cm->Write = NULL;

    cm->FlushBlock = NULL;

    cm->Flush = NULL;

    cm->FlushAll = NULL;
}
