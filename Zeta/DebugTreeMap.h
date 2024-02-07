#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef unsigned long long Zeta_DebugTreeMap_key_t;
typedef unsigned long long Zeta_DebugTreeMap_val_t;

typedef struct Zeta_DebugTreeMap Zeta_DebugTreeMap;

typedef struct Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_KeyValPair;

struct Zeta_DebugTreeMap_KeyValPair {
    bool_t b;
    const Zeta_DebugTreeMap_key_t* key;
    Zeta_DebugTreeMap_val_t* val;
};

void* Zeta_DebugTreeMap_Create();

void Zeta_DebugTreeMap_Destroy(void* tm);

size_t Zeta_DebugTreeMap_GetSize(void* tm);

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_Find(
    void* tm, Zeta_DebugTreeMap_key_t key);

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_LowerBound(
    void* tm, Zeta_DebugTreeMap_key_t target);

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_UpperBound(
    void* tm, Zeta_DebugTreeMap_key_t target);

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_Insert(
    void* tm, Zeta_DebugTreeMap_key_t key);

bool_t Zeta_DebugTreeMap_Erase(void* tm, Zeta_DebugTreeMap_key_t key);

void Zeta_DebugTreeMap_EraseAll(void* tm);

void Zeta_DebugTreeMap_Print(void* tm);

ZETA_extern_c_end;
