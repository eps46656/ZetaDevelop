#include "DebugTreeMap.h"

#include <iostream>
#include <map>

typedef std::map<Zeta_DebugTreeMap_key_t, Zeta_DebugTreeMap_val_t> map_t;

void* Zeta_DebugTreeMap_Create() { return new map_t; }

void Zeta_DebugTreeMap_Destroy(void* tm_) {
    map_t* tm{ static_cast<map_t*>(tm_) };
    ZETA_DebugAssert(tm != NULL);

    delete tm;
}

size_t Zeta_DebugTreeMap_GetSize(void* tm_) {
    map_t* tm{ static_cast<map_t*>(tm_) };
    ZETA_DebugAssert(tm != NULL);

    return tm->size();
}

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_Find(
    void* tm_, Zeta_DebugTreeMap_key_t key) {
    map_t* tm{ static_cast<map_t*>(tm_) };
    ZETA_DebugAssert(tm != NULL);

    auto iter{ tm->find(key) };

    return iter == tm->end() ? Zeta_DebugTreeMap_KeyValPair{ false, NULL, NULL }
                             : Zeta_DebugTreeMap_KeyValPair{ true, &iter->first,
                                                             &iter->second };
}

Zeta_DebugTreeMap_KeyValPair Zeta_DebugTreeMap_Insert(
    void* tm_, Zeta_DebugTreeMap_key_t key) {
    map_t* tm{ static_cast<map_t*>(tm_) };
    ZETA_DebugAssert(tm != NULL);

    auto iter_b{ tm->insert({ key, 0 }) };

    return { iter_b.second, &iter_b.first->first, &iter_b.first->second };
}

bool_t Zeta_DebugTreeMap_Erase(void* tm_, Zeta_DebugTreeMap_key_t key) {
    map_t* tm{ static_cast<map_t*>(tm_) };
    ZETA_DebugAssert(tm != NULL);

    return tm->erase(key) != 0;
}

void Zeta_DebugTreeMap_EraseAll(void* tm_) {
    map_t* tm{ static_cast<map_t*>(tm_) };
    ZETA_DebugAssert(tm != NULL);

    tm->clear();
}

void Zeta_DebugTreeMap_Print(void* tm_) {
    map_t* tm{ static_cast<map_t*>(tm_) };
    ZETA_DebugAssert(tm != NULL);

    std::cout << "{ ";

    for (auto iter{ tm->begin() }, end{ tm->end() }; iter != end; ++iter) {
        std::cout << "(" << iter->first << " " << iter->second << "), ";
    }

    std::cout << "}\n";
}
