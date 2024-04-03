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
    size_t const* key;
    size_t* val;
};

void Zeta_DebugTreeMap_Create(void* tm);

void Zeta_DebugTreeMap_Destroy(void* tm);

size_t Zeta_DebugTreeMap_GetSize(void* tm);

void Zeta_DebugTreeMap_Move(void* tm, void* src_tm);

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_Find(void* tm, size_t key);

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_LowerBound(void* tm,
                                                          size_t target);

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_UpperBound(void* tm,
                                                          size_t target);

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_Insert(void* tm, size_t key);

bool_t Zeta_DebugTreeMap_Erase(void* tm, size_t key);

void Zeta_DebugTreeMap_EraseAll(void* tm);

void Zeta_DebugTreeMap_Print(void* tm);

ZETA_ExternC_Beg;
