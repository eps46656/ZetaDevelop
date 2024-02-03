typedef unsigned long long key_t;
typedef unsigned long long val_t;

#include "define.h"

EXTERN_C void* DebugHashTable_Create();

EXTERN_C void DebugHashTable_Destroy(void* ht);

EXTERN_C size_t DebugHashTable_GetSize(void* ht);

EXTERN_C val_t* DebugHashTable_Access(void* ht, key_t key);

EXTERN_C val_t* DebugHashTable_Insert(void* ht, key_t key);

EXTERN_C bool_t DebugHashTable_Erase(void* ht, key_t key);

EXTERN_C void DebugHashTable_EraseAll(void* ht);

EXTERN_C void DebugHashTable_Print(void* ht);
