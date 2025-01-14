#include "cache_manager.h"

void Zeta_CacheManager_Init(Zeta_CacheManager* cm) {
    ZETA_DebugAssert(cm != NULL);

    cm->context = NULL;

    cm->Open = NULL;

    cm->Close = NULL;

    cm->GetOrigin = NULL;

    cm->SetMaxCacheCnt = NULL;

    cm->Read = NULL;

    cm->Write = NULL;
}
