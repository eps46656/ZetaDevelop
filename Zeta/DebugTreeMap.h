#pragma once

#include "define.h"

ZETA_ExternC_Beg;

typedef struct Zeta_DebugTreeMap Zeta_DebugTreeMap;

struct Zeta_DebugTreeMap {
    void* tree_map;
};

typedef struct Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_KeyValPair;

struct Zeta_DebugTreeMap_KeyValPair {
    bool_t b;
    unsigned long long const* key;
    unsigned long long* val;
};

void Zeta_DebugTreeMap_Create(void* tm);

void Zeta_DebugTreeMap_Destroy(void* tm);

size_t Zeta_DebugTreeMap_GetSize(void* tm);

void Zeta_DebugTreeMap_Move(void* tm, void* src_tm);

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_Find(void* tm,
                                                    unsigned long long key);

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_LowerBound(
    void* tm, unsigned long long target);

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_UpperBound(
    void* tm, unsigned long long target);

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_Insert(void* tm,
                                                      unsigned long long key);

bool_t Zeta_DebugTreeMap_Erase(void* tm, unsigned long long key);

void Zeta_DebugTreeMap_EraseAll(void* tm);

void Zeta_DebugTreeMap_Print(void* tm);

ZETA_ExternC_End;
