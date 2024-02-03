typedef unsigned long long key_t;
typedef unsigned long long val_t;

#include "define.h"

typedef struct KeyValPair KeyValPair;

struct KeyValPair {
    bool_t b;
    const key_t* key;
    val_t* val;
};

EXTERN_C void* DebugTreeMap_Create();

EXTERN_C void DebugTreeMap_Destroy(void* tm);

EXTERN_C size_t DebugTreeMap_GetSize(void* tm);

EXTERN_C KeyValPair DebugTreeMap_Find(void* tm, key_t key);

EXTERN_C KeyValPair DebugTreeMap_LowerBound(void* tm, key_t target);

EXTERN_C KeyValPair DebugTreeMap_UpperBound(void* tm, key_t target);

EXTERN_C KeyValPair DebugTreeMap_Insert(void* tm, key_t key);

EXTERN_C bool_t DebugTreeMap_Erase(void* tm, key_t key);

EXTERN_C void DebugTreeMap_EraseAll(void* tm);

EXTERN_C void DebugTreeMap_Print(void* tm);
