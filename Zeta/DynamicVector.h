#pragma once

#include "Allocator.h"
#include "CircularVector.h"
#include "MultiLevelEntryTable.h"
#include "Vector.h"

ZETA_DECL_STRUCT(Zeta_DynamicVector) {
    diff_t width;
    diff_t cluster_size;

    diff_t offset;
    diff_t size;

    Zeta_MultiLevelEntryTable mlet;

    Zeta_Allocator* allocator;
};

void Zeta_DynamicVector_Init(void* dv);

void Zeta_DynamicVector_StdConfig(void* dv, diff_t width, diff_t cluster_size,
                                  Zeta_Allocator* allocator);

diff_t Zeta_DynamicVector_GetSize(void* dv);

void* Zeta_DynamicVector_Access(void* dv, diff_t idx);

void** Zeta_DynamicVector_PushL(void* dv);

void** Zeta_DynamicVector_PushR(void* dv);

void Zeta_DynamicVector_PopL(void* dv, void* destry_context,
                             void (*Destroy)(void* context, void* ptr));

void Zeta_DynamicVector_PopR(void* dv, void* destry_context,
                             void (*Destroy)(void* context, void* ptr));

void Zeta_DynamicVector_ToVector(void* dv_, Zeta_Vector* dst);
