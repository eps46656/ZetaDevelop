#include "DebugTreeMap.h"

#include <iostream>
#include <map>

typedef std::map<size_t, size_t> tree_map_t;

void Zeta_DebugTreeMap_Create(void* tm_) {
    Zeta_DebugTreeMap* tm = (Zeta_DebugTreeMap*)tm_;
    ZETA_DebugAssert(tm != NULL);

    tm->tree_map = new tree_map_t;
}

void Zeta_DebugTreeMap_Destroy(void* tm_) {
    Zeta_DebugTreeMap* tm = (Zeta_DebugTreeMap*)tm_;
    ZETA_DebugAssert(tm != NULL);

    tree_map_t* tree_map = (tree_map_t*)tm->tree_map;

    delete tree_map;

    tm->tree_map = NULL;
}

size_t Zeta_DebugTreeMap_GetSize(void* tm_) {
    Zeta_DebugTreeMap* tm = (Zeta_DebugTreeMap*)tm_;
    ZETA_DebugAssert(tm != NULL);

    tree_map_t* tree_map = (tree_map_t*)tm->tree_map;

    return tree_map->size();
}

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_Find(void* tm_, size_t key) {
    Zeta_DebugTreeMap* tm = (Zeta_DebugTreeMap*)tm_;
    ZETA_DebugAssert(tm != NULL);

    tree_map_t* tree_map = (tree_map_t*)tm->tree_map;

    auto iter{ tree_map->find(key) };

    return iter == tree_map->end()
               ? Zeta_DebugTreeMap_KeyValPair{ false, NULL, NULL }
               : Zeta_DebugTreeMap_KeyValPair{ true, &iter->first,
                                               &iter->second };
}

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_Insert(void* tm_, size_t key) {
    Zeta_DebugTreeMap* tm = (Zeta_DebugTreeMap*)tm_;
    ZETA_DebugAssert(tm != NULL);

    tree_map_t* tree_map = (tree_map_t*)tm->tree_map;

    auto iter_b{ tree_map->insert({ key, 0 }) };

    return { iter_b.second, &iter_b.first->first, &iter_b.first->second };
}

bool_t Zeta_DebugTreeMap_Erase(void* tm_, size_t key) {
    Zeta_DebugTreeMap* tm = (Zeta_DebugTreeMap*)tm_;
    ZETA_DebugAssert(tm != NULL);

    tree_map_t* tree_map = (tree_map_t*)tm->tree_map;

    return tree_map->erase(key) != 0;
}

void Zeta_DebugTreeMap_EraseAll(void* tm_) {
    Zeta_DebugTreeMap* tm = (Zeta_DebugTreeMap*)tm_;
    ZETA_DebugAssert(tm != NULL);

    tree_map_t* tree_map = (tree_map_t*)tm->tree_map;

    tree_map->clear();
}

void Zeta_DebugTreeMap_Print(void* tm_) {
    Zeta_DebugTreeMap* tm = (Zeta_DebugTreeMap*)tm_;
    ZETA_DebugAssert(tm != NULL);

    tree_map_t* tree_map = (tree_map_t*)tm->tree_map;

    std::cout << "{ ";

    for (auto iter{ tree_map->begin() }, end{ tree_map->end() }; iter != end;
         ++iter) {
        std::cout << "(" << iter->first << " " << iter->second << "), ";
    }

    std::cout << "}\n";
}
