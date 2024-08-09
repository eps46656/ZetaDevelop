#include "DebugHashMap.h"

#include <iostream>
#include <unordered_map>

#include "Debugger.h"

typedef std::unordered_multimap<unsigned long long, Zeta_DebugHashMap_Pair>
    hash_map_t;

struct Zeta_DebugHashMap_Cursor {
    hash_map_t iter;
    Zeta_DebugHashMap_Pair* pair;
};

void Zeta_DebugHashMap_Init(void* debug_hm_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    debug_hm->hash_map = new hash_map_t{};
}

void Zeta_DebugHashMap_Deinit(void* debug_hm_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)debug_hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    delete hash_map;

    debug_hm->hash_map = NULL;
}

size_t Zeta_DebugHashMap_GetWidth(void* debug_hm_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    return sizeof(Zeta_DebugHashMap_Pair);
}

size_t Zeta_DebugHashMap_GetStride(void* debug_hm_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    return sizeof(Zeta_DebugHashMap_Pair);
}

size_t Zeta_DebugHashMap_GetSize(void* debug_hm_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)debug_hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    return hash_map->size();
}

size_t Zeta_DebugHashMap_GetCapacity(void* debug_hm_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)debug_hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    return ZETA_RangeMaxOf(size_t);
}

void Zeta_DebugHashMap_GetRBCursor(void* debug_hm_, void* dst_cursor_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)debug_hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    hash_map_t::iterator* dst_cursor = (hash_map_t::iterator*)dst_cursor_;
    ZETA_DebugAssert(dst_cursor != NULL);

    new (dst_cursor) hash_map_t::iterator{ hash_map->end() };
}

void* Zeta_DebugHashMap_PeekL(void* debug_hm_, void* dst_cursor_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)debug_hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    hash_map_t::iterator* dst_cursor = (hash_map_t::iterator*)dst_cursor_;
    ZETA_DebugAssert(dst_cursor != NULL);

    auto iter{ hash_map->begin() };

    if (dst_cursor != NULL) { new (dst_cursor) hash_map_t::iterator{ iter }; }

    return &iter->second;
}

/*
void Zeta_DebugHashMap_Move(void* debug_hm_, void* src_debug_hm_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    Zeta_DebugHashMap* src_debug_hm = (Zeta_DebugHashMap*)src_debug_hm_;
    ZETA_DebugAssert(src_debug_hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)debug_hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    hash_map_t* src_tree_map = (hash_map_t*)src_debug_hm->hash_map;
    ZETA_DebugAssert(src_tree_map != NULL);

    *hash_map = std::move(*src_tree_map);
}
*/

void* Zeta_DebugHashMap_Refer(void* debug_hm_, void const* pos_cursor_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)debug_hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    hash_map_t::iterator* pos_cursor = (hash_map_t::iterator*)pos_cursor_;
    ZETA_DebugAssert(pos_cursor != NULL);

    return &(*pos_cursor)->second;
}

void* Zeta_DebugHashMap_Find(void* debug_hm_, void* dst_cursor_,
                             void const* key_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)debug_hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    hash_map_t::iterator* dst_cursor = (hash_map_t::iterator*)dst_cursor_;

    unsigned long long key = *(unsigned long long*)key_;

    auto iter{ hash_map->find(key) };

    if (dst_cursor != NULL) { new (dst_cursor) hash_map_t::iterator{ iter }; }

    return iter == hash_map->end() ? NULL : &iter->second;
}

void* Zeta_DebugHashMap_Insert(void* debug_hm_, void* dst_cursor_,
                               void const* key_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)debug_hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    hash_map_t::iterator* dst_cursor = (hash_map_t::iterator*)dst_cursor_;

    unsigned long long key = *(unsigned long long*)key_;

    auto iter{ hash_map->insert({ key, { key } }) };

    if (dst_cursor != NULL) { new (dst_cursor) hash_map_t::iterator{ iter }; }

    return &iter->second;
}

void Zeta_DebugHashMap_Erase(void* debug_hm_, void* pos_cursor_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)debug_hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    hash_map_t::iterator* pos_cursor = (hash_map_t::iterator*)pos_cursor_;
    ZETA_DebugAssert(pos_cursor != NULL);

    hash_map->erase(*pos_cursor);
}

void Zeta_DebugHashMap_EraseAll(void* debug_hm_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)debug_hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    hash_map->clear();
}

bool_t Zeta_DebugHashMap_Cursor_IsEqual(void* debug_hm_, void const* cursor_a_,
                                        void const* cursor_b_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)debug_hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    hash_map_t::iterator* cursor_a = (hash_map_t::iterator*)cursor_a_;
    hash_map_t::iterator* cursor_b = (hash_map_t::iterator*)cursor_b_;

    ZETA_DebugAssert(cursor_a != NULL);
    ZETA_DebugAssert(cursor_b != NULL);

    return *cursor_a == *cursor_b;
}

void Zeta_DebugHashMap_Cursor_StepL(void* debug_hm_, void const* cursor_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)debug_hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    hash_map_t::iterator* cursor = (hash_map_t::iterator*)cursor_;
    ZETA_DebugAssert(cursor != NULL);

    --(*cursor);
}

void Zeta_DebugHashMap_Cursor_StepR(void* debug_hm_, void const* cursor_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)debug_hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    hash_map_t::iterator* cursor = (hash_map_t::iterator*)cursor_;
    ZETA_DebugAssert(cursor != NULL);

    ++(*cursor);
}

void Zeta_DebugHashMap_Print(void* debug_hm_) {
    Zeta_DebugHashMap* debug_hm = (Zeta_DebugHashMap*)debug_hm_;
    ZETA_DebugAssert(debug_hm != NULL);

    hash_map_t* hash_map = (hash_map_t*)debug_hm->hash_map;
    ZETA_DebugAssert(hash_map != NULL);

    std::cout << "{ ";

    for (auto iter{ hash_map->begin() }, end{ hash_map->end() }; iter != end;
         ++iter) {
        std::cout << "(" << iter->second.key << " " << iter->second.val
                  << "), ";
    }

    std::cout << "}\n";
}
