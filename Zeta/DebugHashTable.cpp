#include "DebugHashTable.h"

#include <iostream>
#include <unordered_map>

typedef std::unordered_map<key_t, val_t> map_t;

void* DebugHashTable_Create() { return new map_t; }

void DebugHashTable_Destroy(void* ht_) {
    map_t* ht{ static_cast<map_t*>(ht_) };
    ZETA_DebugAssert(ht != NULL);

    delete ht;
}

size_t DebugHashTable_GetSize(void* ht_) {
    map_t* ht{ static_cast<map_t*>(ht_) };
    ZETA_DebugAssert(ht != NULL);

    return ht->size();
}

val_t* DebugHashTable_Access(void* ht_, key_t key) {
    map_t* ht{ static_cast<map_t*>(ht_) };
    ZETA_DebugAssert(ht != NULL);

    auto iter{ ht->find(key) };

    return iter == ht->end() ? NULL : &iter->second;
}

val_t* DebugHashTable_Insert(void* ht_, key_t key) {
    map_t* ht{ static_cast<map_t*>(ht_) };
    ZETA_DebugAssert(ht != NULL);

    return &ht->insert({ key, 0 }).first->second;
}

bool_t DebugHashTable_Erase(void* ht_, key_t key) {
    map_t* ht{ static_cast<map_t*>(ht_) };
    ZETA_DebugAssert(ht != NULL);

    return ht->erase(key) != 0;
}

void DebugHashTable_EraseAll(void* ht_) {
    map_t* ht{ static_cast<map_t*>(ht_) };
    ZETA_DebugAssert(ht != NULL);

    ht->clear();
}

void DebugHashTable_Print(void* ht_) {
    map_t* ht{ static_cast<map_t*>(ht_) };
    ZETA_DebugAssert(ht != NULL);

    std::cout << "{ ";

    for (auto iter{ ht->begin() }, end{ ht->end() }; iter != end; ++iter) {
        std::cout << "(" << iter->first << " " << iter->second << "), ";
    }

    std::cout << "}\n";
}
