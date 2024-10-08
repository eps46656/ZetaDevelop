#include "Allocator.h"

#include "Debugger.h"

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
