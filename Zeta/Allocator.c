#include "llocator.h"

void Zeta_Allocator_Init(void* allocator_) {
    Zeta_Allocator* allocator = allocator_;

    allocator->context = NULL;

    allocator->GetAlign = NULL;

    allocator->Query = NULL;

    allocator->Allocate = NULL;
    allocator->Deallocate = NULL;
}
