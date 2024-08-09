#include "Allocator.h"

void Zeta_Allocator_Init(void* allocator_) {
    Zeta_Allocator* allocator = allocator_;
    ZETA_DebugAssert(allocator != NULL);

    allocator->context = NULL;

    allocator->GetAlign = NULL;

    allocator->Query = NULL;

    allocator->Allocate = NULL;

    allocator->Deallocate = NULL;

    allocator->GetRecords = NULL;
}

void Zeta_Allocator_MatchRecords(Zeta_DebugHashMap* src_records,
                                 Zeta_DebugHashMap* dst_records) {
    //
}
