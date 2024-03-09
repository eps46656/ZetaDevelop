#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef unsigned long long key_t;
typedef unsigned long long val_t;

void* DebugHashTable_Create();

void DebugHashTable_Destroy(void* ht);

size_t DebugHashTable_GetSize(void* ht);

val_t* DebugHashTable_Access(void* ht, key_t key);

val_t* DebugHashTable_Insert(void* ht, key_t key);

bool_t DebugHashTable_Erase(void* ht, key_t key);

void DebugHashTable_EraseAll(void* ht);

void DebugHashTable_Print(void* ht);

ZETA_extern_c_end;
