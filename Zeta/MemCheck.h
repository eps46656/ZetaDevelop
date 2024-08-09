#pragma once

#include "AssocContainer.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_PtrSize);

struct Zeta_PtrSize {
    void const* ptr;
    size_t size;
};

unsigned long long Zeta_PtrSize_HashPtr(void const* ptr_size);

unsigned long long Zeta_PtrSize_HashPtrSize(void const* ptr_size);

void Zeta_MemCheck_AddPtrSize(Zeta_AssocContainer* dst_assoc, void const* ptr,
                              size_t size);

void Zeta_MemCheck_CheckOverlapping(Zeta_AssocContainer* dst_assoc);

ZETA_ExternC_End;
