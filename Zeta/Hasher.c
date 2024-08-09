

#include "Hasher.h"

void Zeta_Hasher_Init(void* hasher_) {
    Zeta_Hasher* hasher = hasher_;
    ZETA_DebugAssert(hasher != NULL);

    hasher->context = NULL;

    hasher->Reset = NULL;

    hasher->GetSize = NULL;

    hasher->GetResultWidth = NULL;

    hasher->GetResult = NULL;

    hasher->Rotate = NULL;
}
