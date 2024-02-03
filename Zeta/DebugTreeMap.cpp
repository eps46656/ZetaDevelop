#include "DebugTreeMap.h"
#include <iostream>
#include <map>

typedef std::map<key_t, val_t> map_t;

void* DebugTreeMap_Create() { return new map_t; }

void DebugTreeMap_Destroy(void* tm_) {
    map_t* tm{static_cast<map_t*>(tm_)};
    ZETA_DEBUG_ASSERT(tm != NULL);

    delete tm;
}

size_t DebugTreeMap_GetSize(void* tm_) {
    map_t* tm{static_cast<map_t*>(tm_)};
    ZETA_DEBUG_ASSERT(tm != NULL);

    return tm->size();
}

KeyValPair DebugTreeMap_Find(void* tm_, key_t key) {
    map_t* tm{static_cast<map_t*>(tm_)};
    ZETA_DEBUG_ASSERT(tm != NULL);

    auto iter{tm->find(key)};

    return iter == tm->end() ? KeyValPair{false, NULL, NULL}
                             : KeyValPair{true, &iter->first, &iter->second};
}

KeyValPair DebugTreeMap_Insert(void* tm_, key_t key) {
    map_t* tm{static_cast<map_t*>(tm_)};
    ZETA_DEBUG_ASSERT(tm != NULL);

    auto iter_b{tm->insert({key, 0})};

    return KeyValPair{iter_b.second, &iter_b.first->first,
                      &iter_b.first->second};
}

bool_t DebugTreeMap_Erase(void* tm_, key_t key) {
    map_t* tm{static_cast<map_t*>(tm_)};
    ZETA_DEBUG_ASSERT(tm != NULL);

    return tm->erase(key) != 0;
}

void DebugTreeMap_EraseAll(void* tm_) {
    map_t* tm{static_cast<map_t*>(tm_)};
    ZETA_DEBUG_ASSERT(tm != NULL);

    tm->clear();
}

void DebugTreeMap_Print(void* tm_) {
    map_t* tm{static_cast<map_t*>(tm_)};
    ZETA_DEBUG_ASSERT(tm != NULL);

    std::cout << "{ ";

    for (auto iter{tm->begin()}, end{tm->end()}; iter != end; ++iter) {
        std::cout << "(" << iter->first << " " << iter->second << "), ";
    }

    std::cout << "}\n";
}
