#pragma once

#include "DebugHashMap.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_Allocator);

struct Zeta_Allocator {
    void* context;

    size_t (*GetAlign)(void* context);

    size_t (*Query)(void* context, size_t size);

    void* (*Allocate)(void* context, size_t size);

    void (*Deallocate)(void* context, void* ptr);

    Zeta_DebugHashMap* (*GetRecords)(void* context);
};

void Zeta_Allocator_Init(void* allocator);

void Zeta_Allocator_MatchRecords(Zeta_DebugHashMap* src_records,
                                 Zeta_DebugHashMap* dst_records);

ZETA_ExternC_End;
