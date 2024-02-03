
#include "OkMap.h"
#include <iostream>
#include <unordered_map>

typedef std::unordered_map<key_t, val_t> map_t;

EXTERNC void* OkMap_Create() { return new map_t; }

EXTERNC val_t* OkMap_Access(void* ok_map_, key_t key) {
    map_t* ok_map{static_cast<map_t*>(ok_map_)};
    auto iter{ok_map->find(key)};
    return iter == ok_map->end() ? NULL : &iter->second;
}

EXTERNC val_t* OkMap_Insert(void* ok_map_, key_t key) {
    map_t* ok_map{static_cast<map_t*>(ok_map_)};
    return &ok_map->insert({key, 0}).first->second;
}

EXTERNC int OkMap_Erase(void* ok_map_, key_t key) {
    map_t* ok_map{static_cast<map_t*>(ok_map_)};
    return ok_map->erase(key) != 0;
}

EXTERNC void OkMap_Print(void* ok_map_) {
    map_t* ok_map{static_cast<map_t*>(ok_map_)};

    for (auto iter{ok_map->begin()}, end{ok_map->end()}; iter != end; ++iter) {
        std::cout << iter->first << " " << iter->second << "\n";
    }

    std::cout << '\n';
}
