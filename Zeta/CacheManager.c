#include "CacheManager.h"

void Zeta_CacheManager_Init(Zeta_CacheManager* cm) {
    cm->context = NULL;

    cm->Open = NULL;
    cm->Close = NULL;

    cm->SetMaxCachesNum = NULL;

    cm->ReadBlock = NULL;
    cm->WriteBlock = NULL;

    cm->FlushBlock = NULL;
    cm->Flush = NULL;
    cm->FlushAll = NULL;
}
