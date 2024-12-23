#include "debug_hash_table.h"

#include <iostream>
#include <unordered_set>

#include "debugger.h"

struct Pack {
    bool_t is_elem;
    void const* ptr;
};

struct PackHash {
    Zeta_DebugHashTable* debug_hash_table;

    unsigned long long operator()(const Pack& pack) const {
        return pack.is_elem
                   ? debug_hash_table->ElemHash(
                         debug_hash_table->elem_hash_context, pack.ptr)
                   : debug_hash_table->KeyHash(
                         debug_hash_table->elem_hash_context, pack.ptr);
    }
};

struct PackCompare {
    Zeta_DebugHashTable* debug_hash_table;

    bool_t operator()(const Pack& pack_a, const Pack& pack_b) const {
        ZETA_DebugAssert(pack_a.is_elem || pack_b.is_elem);

        if (pack_a.is_elem && pack_b.is_elem) {
            return this->debug_hash_table->ElemCompare(
                       this->debug_hash_table->elem_cmp_context, pack_a.ptr,
                       pack_b.ptr) == 0;
        }

        return (pack_a.is_elem
                    ? this->debug_hash_table->ElemKeyCompare(
                          this->debug_hash_table->elem_key_cmp_context,
                          pack_a.ptr, pack_b.ptr)
                    : this->debug_hash_table->ElemKeyCompare(
                          this->debug_hash_table->elem_key_cmp_context,
                          pack_b.ptr, pack_a.ptr)) == 0;
    }
};

typedef std::unordered_multiset<Pack, PackHash, PackCompare> hash_table_t;

void Zeta_DebugHashTable_Init(void* debug_ht_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table =
        new hash_table_t{ 0, PackHash{ debug_ht }, PackCompare{ debug_ht } };

    debug_ht->hash_table = hash_table;

    ZETA_DebugAssert(debug_ht->ElemHash != NULL);
    ZETA_DebugAssert(debug_ht->KeyHash != NULL);

    ZETA_DebugAssert(debug_ht->ElemCompare != NULL);
    ZETA_DebugAssert(debug_ht->ElemKeyCompare != NULL);
}

void Zeta_DebugHashTable_Deinit(void* debug_ht_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    delete hash_table;

    debug_ht->hash_table = NULL;
}

size_t Zeta_DebugHashTable_GetWidth(void* debug_ht_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    return debug_ht->width;
}

size_t Zeta_DebugHashTable_GetSize(void* debug_ht_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    return hash_table->size();
}

size_t Zeta_DebugHashTable_GetCapacity(void* debug_ht_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    return ZETA_SIZE_MAX;
}

void Zeta_DebugHashTable_GetRBCursor(void* debug_ht_, void* dst_cursor_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* dst_cursor = (hash_table_t::iterator*)dst_cursor_;
    ZETA_DebugAssert(dst_cursor != NULL);

    new (dst_cursor) hash_table_t::iterator{ hash_table->end() };
}

void* Zeta_DebugHashTable_PeekL(void* debug_ht_, void* dst_cursor_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* dst_cursor = (hash_table_t::iterator*)dst_cursor_;
    ZETA_DebugAssert(dst_cursor != NULL);

    auto iter{ hash_table->begin() };

    if (dst_cursor != NULL) { new (dst_cursor) hash_table_t::iterator{ iter }; }

    return const_cast<void*>(iter->ptr);
}

void* Zeta_DebugHashTable_Refer(void* debug_ht_, void const* pos_cursor_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* pos_cursor = (hash_table_t::iterator*)pos_cursor_;
    ZETA_DebugAssert(pos_cursor != NULL);

    return const_cast<void*>((*pos_cursor)->ptr);
}

void* Zeta_DebugHashTable_Find(void* debug_ht_, void const* key,
                               void* dst_cursor_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* dst_cursor = (hash_table_t::iterator*)dst_cursor_;

    Pack key_pack{ TRUE, key };

    auto iter{ hash_table->find(key_pack) };

    if (dst_cursor != NULL) { new (dst_cursor) hash_table_t::iterator{ iter }; }

    return iter == hash_table->end() ? NULL : const_cast<void*>(iter->ptr);
}

void* Zeta_DebugHashTable_Insert(void* debug_ht_, void const* elem,
                                 void* dst_cursor_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* dst_cursor = (hash_table_t::iterator*)dst_cursor_;

    Pack pack{ FALSE, std::malloc(debug_ht->width) };
    std::memcpy(const_cast<void*>(pack.ptr), elem, debug_ht->width);

    auto iter{ hash_table->insert(pack) };

    if (dst_cursor != NULL) { new (dst_cursor) hash_table_t::iterator{ iter }; }

    return const_cast<void*>(iter->ptr);
}

void Zeta_DebugHashTable_Erase(void* debug_ht_, void* pos_cursor_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* pos_cursor = (hash_table_t::iterator*)pos_cursor_;
    ZETA_DebugAssert(pos_cursor != NULL);

    ZETA_DebugAssert(*pos_cursor != hash_table->end());

    std::free(const_cast<void*>((*pos_cursor)->ptr));

    hash_table->erase(*pos_cursor);
}

void Zeta_DebugHashTable_EraseAll(void* debug_ht_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table->clear();
}

bool_t Zeta_DebugHashTable_Cursor_AreEqual(void* debug_ht_,
                                           void const* cursor_a_,
                                           void const* cursor_b_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* cursor_a = (hash_table_t::iterator*)cursor_a_;
    hash_table_t::iterator* cursor_b = (hash_table_t::iterator*)cursor_b_;

    ZETA_DebugAssert(cursor_a != NULL);
    ZETA_DebugAssert(cursor_b != NULL);

    return *cursor_a == *cursor_b;
}

void Zeta_DebugHashTable_Cursor_StepL(void* debug_ht_, void* cursor_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* cursor = (hash_table_t::iterator*)cursor_;
    ZETA_DebugAssert(cursor != NULL);

    --(*cursor);
}

void Zeta_DebugHashTable_Cursor_StepR(void* debug_ht_, void* cursor_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* cursor = (hash_table_t::iterator*)cursor_;
    ZETA_DebugAssert(cursor != NULL);

    ++(*cursor);
}

void Zeta_DebugHashTable_DeployAssocCntr(void* debug_ht_,
                                         Zeta_AssocCntr* assoc_cntr) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    Zeta_AssocCntr_Init(assoc_cntr);

    assoc_cntr->context = debug_ht;

    assoc_cntr->cursor_size = sizeof(hash_table_t::iterator);

    assoc_cntr->GetWidth = Zeta_DebugHashTable_GetWidth;

    assoc_cntr->GetSize = Zeta_DebugHashTable_GetSize;

    assoc_cntr->GetCapacity = Zeta_DebugHashTable_GetCapacity;

    assoc_cntr->GetRBCursor = Zeta_DebugHashTable_GetRBCursor;

    assoc_cntr->PeekL = Zeta_DebugHashTable_PeekL;

    assoc_cntr->Refer = Zeta_DebugHashTable_Refer;

    assoc_cntr->Find = Zeta_DebugHashTable_Find;

    assoc_cntr->Insert = Zeta_DebugHashTable_Insert;

    assoc_cntr->Erase = Zeta_DebugHashTable_Erase;

    assoc_cntr->EraseAll = Zeta_DebugHashTable_EraseAll;

    assoc_cntr->Cursor_AreEqual = Zeta_DebugHashTable_Cursor_AreEqual;

    assoc_cntr->Cursor_StepL = Zeta_DebugHashTable_Cursor_StepL;

    assoc_cntr->Cursor_StepR = Zeta_DebugHashTable_Cursor_StepR;
}
