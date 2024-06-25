#include "DebugHashMap.h"

#include <iostream>
#include <unordered_map>

#include "Debugger.h"

typedef std::unordered_map<unsigned long long, unsigned long long> hash_map_t;

void Zeta_DebugHashMap_Create(void* hm_) {
    Zeta_DebugHashMap* hm = (Zeta_DebugHashMap*)hm_;
    ZETA_DebugAssert(hm != NULL);

    hm->hash_map = new hash_map_t{};
}

void Zeta_DebugHashMap_Destroy(void* hm_) {
    Zeta_DebugHashMap* hm = (Zeta_DebugHashMap*)hm_;
    ZETA_DebugAssert(hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    delete hash_map;

    hm->hash_map = NULL;
}

size_t Zeta_DebugHashMap_GetSize(void* hm_) {
    Zeta_DebugHashMap* hm = (Zeta_DebugHashMap*)hm_;
    ZETA_DebugAssert(hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    return hash_map->size();
}

void Zeta_DebugHashMap_Move(void* hm_, void* src_hm_) {
    Zeta_DebugHashMap* hm = (Zeta_DebugHashMap*)hm_;
    ZETA_DebugAssert(hm != NULL);

    Zeta_DebugHashMap* src_hm = (Zeta_DebugHashMap*)src_hm_;
    ZETA_DebugAssert(src_hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    hash_map_t* src_tree_map = (hash_map_t*)src_hm->hash_map;
    ZETA_DebugAssert(src_tree_map != NULL);

    *hash_map = std::move(*src_tree_map);
}

Zeta_DebugHashMap_KeyValPair Zeta_DebugHashMap_Find(void* hm_,
                                                    unsigned long long key) {
    Zeta_DebugHashMap* hm = (Zeta_DebugHashMap*)hm_;
    ZETA_DebugAssert(hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    auto iter{ hash_map->find(key) };

    return iter == hash_map->end()
               ? Zeta_DebugHashMap_KeyValPair{ FALSE, NULL, NULL }
               : Zeta_DebugHashMap_KeyValPair{ TRUE, &iter->first,
                                               &iter->second };
}

Zeta_DebugHashMap_KeyValPair Zeta_DebugHashMap_Insert(void* hm_,
                                                      unsigned long long key) {
    Zeta_DebugHashMap* hm = (Zeta_DebugHashMap*)hm_;
    ZETA_DebugAssert(hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    auto iter_b{ hash_map->insert({ key, 0 }) };

    return { iter_b.second, &iter_b.first->first, &iter_b.first->second };
}

bool_t Zeta_DebugHashMap_Erase(void* hm_, unsigned long long key) {
    Zeta_DebugHashMap* hm = (Zeta_DebugHashMap*)hm_;
    ZETA_DebugAssert(hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    return hash_map->erase(key) != 0;
}

void Zeta_DebugHashMap_EraseAll(void* hm_) {
    Zeta_DebugHashMap* hm = (Zeta_DebugHashMap*)hm_;
    ZETA_DebugAssert(hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    hash_map->clear();
}

void Zeta_DebugHashMap_Print(void* hm_) {
    Zeta_DebugHashMap* hm = (Zeta_DebugHashMap*)hm_;
    ZETA_DebugAssert(hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    std::cout << "{ ";

    for (auto iter{ hash_map->begin() }, end{ hash_map->end() }; iter != end;
         ++iter) {
        std::cout << "(" << iter->first << " " << iter->second << "), ";
    }

    std::cout << "}\n";
}
