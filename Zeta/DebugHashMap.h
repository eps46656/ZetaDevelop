#pragma once

#include "define.h"

ZETA_ExternC_Beg;

typedef struct Zeta_DebugHashMap Zeta_DebugHashMap;

struct Zeta_DebugHashMap {
    void* hash_map;
};

typedef struct Zeta_DebugHashMap_KeyValPair Zeta_DebugHashMap_KeyValPair;

struct Zeta_DebugHashMap_KeyValPair {
    bool_t b;
    unsigned long long const* key;
    unsigned long long* val;
};

void Zeta_DebugHashMap_Create(void* tm);

void Zeta_DebugHashMap_Destroy(void* tm);

size_t Zeta_DebugHashMap_GetSize(void* tm);

void Zeta_DebugHashMap_Move(void* tm, void* src_tm);

Zeta_DebugHashMap_KeyValPair Zeta_DebugHashMap_Find(void* tm,
                                                    unsigned long long key);

Zeta_DebugHashMap_KeyValPair Zeta_DebugHashMap_Insert(void* tm,
                                                      unsigned long long key);

bool_t Zeta_DebugHashMap_Erase(void* tm, unsigned long long key);

void Zeta_DebugHashMap_EraseAll(void* tm);

void Zeta_DebugHashMap_Print(void* tm);

ZETA_ExternC_End;
