#include "DebugTreeMap.h"

#include <iostream>
#include <map>

typedef std::map<unsigned long long, unsigned long long> tree_map_t;

void Zeta_DebugTreeMap_Create(void* tm_) {
    Zeta_DebugTreeMap* tm = (Zeta_DebugTreeMap*)tm_;
    ZETA_DebugAssert(tm != NULL);

    tm->tree_map = new tree_map_t;
}

void Zeta_DebugTreeMap_Destroy(void* tm_) {
    Zeta_DebugTreeMap* tm = (Zeta_DebugTreeMap*)tm_;
    ZETA_DebugAssert(tm != NULL);

    tree_map_t* tree_map = (tree_map_t*)tm->tree_map;
    ZETA_DebugAssert(tree_map != NULL);

    delete tree_map;

    tm->tree_map = NULL;
}

size_t Zeta_DebugTreeMap_GetSize(void* tm_) {
    Zeta_DebugTreeMap* tm = (Zeta_DebugTreeMap*)tm_;
    ZETA_DebugAssert(tm != NULL);

    tree_map_t* tree_map = (tree_map_t*)tm->tree_map;
    ZETA_DebugAssert(tree_map != NULL);

    return tree_map->size();
}

void Zeta_DebugTreeMap_Move(void* tm_, void* src_tm_) {
    Zeta_DebugTreeMap* tm = (Zeta_DebugTreeMap*)tm_;
    ZETA_DebugAssert(tm != NULL);

    Zeta_DebugTreeMap* src_tm = (Zeta_DebugTreeMap*)src_tm_;
    ZETA_DebugAssert(src_tm != NULL);

    tree_map_t* tree_map = (tree_map_t*)tm->tree_map;
    ZETA_DebugAssert(tree_map != NULL);

    tree_map_t* src_tree_map = (tree_map_t*)src_tm->tree_map;
    ZETA_DebugAssert(src_tree_map != NULL);

    *tree_map = std::move(*src_tree_map);
}

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_Find(void* tm_,
                                                    unsigned long long key) {
    Zeta_DebugTreeMap* tm = (Zeta_DebugTreeMap*)tm_;
    ZETA_DebugAssert(tm != NULL);

    tree_map_t* tree_map = (tree_map_t*)tm->tree_map;
    ZETA_DebugAssert(tree_map != NULL);

    auto iter{ tree_map->find(key) };

    return iter == tree_map->end()
               ? Zeta_DebugTreeMap_KeyValPair{ FALSE, NULL, NULL }
               : Zeta_DebugTreeMap_KeyValPair{ TRUE, &iter->first,
                                               &iter->second };
}

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_Insert(void* tm_,
                                                      unsigned long long key) {
    Zeta_DebugTreeMap* tm = (Zeta_DebugTreeMap*)tm_;
    ZETA_DebugAssert(tm != NULL);

    tree_map_t* tree_map = (tree_map_t*)tm->tree_map;
    ZETA_DebugAssert(tree_map != NULL);

    auto iter_b{ tree_map->insert({ key, 0 }) };

    return { iter_b.second, &iter_b.first->first, &iter_b.first->second };
}

bool_t Zeta_DebugTreeMap_Erase(void* tm_, unsigned long long key) {
    Zeta_DebugTreeMap* tm = (Zeta_DebugTreeMap*)tm_;
    ZETA_DebugAssert(tm != NULL);

    tree_map_t* tree_map = (tree_map_t*)tm->tree_map;
    ZETA_DebugAssert(tree_map != NULL);

    return tree_map->erase(key) != 0;
}

void Zeta_DebugTreeMap_EraseAll(void* tm_) {
    Zeta_DebugTreeMap* tm = (Zeta_DebugTreeMap*)tm_;
    ZETA_DebugAssert(tm != NULL);

    tree_map_t* tree_map = (tree_map_t*)tm->tree_map;
    ZETA_DebugAssert(tree_map != NULL);

    tree_map->clear();
}

void Zeta_DebugTreeMap_Print(void* tm_) {
    Zeta_DebugTreeMap* tm = (Zeta_DebugTreeMap*)tm_;
    ZETA_DebugAssert(tm != NULL);

    tree_map_t* tree_map = (tree_map_t*)tm->tree_map;
    ZETA_DebugAssert(tree_map != NULL);

    std::cout << "{ ";

    for (auto iter{ tree_map->begin() }, end{ tree_map->end() }; iter != end;
         ++iter) {
        std::cout << "(" << iter->first << " " << iter->second << "), ";
    }

    std::cout << "}\n";
}
