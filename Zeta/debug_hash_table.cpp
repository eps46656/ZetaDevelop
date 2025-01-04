#include "debug_hash_table.h"

#include <iostream>
#include <unordered_set>

#include "debugger.h"

struct Wrapper {
    bool_t is_key;
    void const* ptr;
};

struct HashCore {
    void const* elem_hash_context;
    Zeta_Hash ElemHash;

    void const* key_hash_context;
    Zeta_Hash KeyHash;

    unsigned long long operator()(const Wrapper& wrapper) const {
        if (wrapper.is_key) {
            return this->KeyHash(this->key_hash_context, wrapper.ptr, 0);
        }

        return this->ElemHash(this->elem_hash_context, wrapper.ptr, 0);
    }
};

struct Hash {
    HashCore* core;

    template <typename Value>
    unsigned long long operator()(const Value& value) const {
        return (*this->core)(value);
    }
};

struct KeyEqCore {
    void const* elem_cmp_context;
    Zeta_Compare ElemCompare;

    void const* key_elem_cmp_context;
    Zeta_Compare KeyElemCompare;

    bool_t operator()(const Wrapper& a, const Wrapper& b) const {
        if (a.is_key) {
            return this->KeyElemCompare(this->key_elem_cmp_context, a.ptr,
                                        b.ptr) == 0;
        }

        if (b.is_key) {
            return this->KeyElemCompare(this->key_elem_cmp_context, a.ptr,
                                        b.ptr) == 0;
        }

        return this->ElemCompare(this->elem_cmp_context, a.ptr, b.ptr) == 0;
    }
};

struct KeyEq {
    KeyEqCore* core;

    template <typename ValueA, typename ValueB>
    bool_t operator()(const ValueA& a, const ValueB& b) const {
        return (*this->core)(a, b);
    }
};

typedef std::unordered_multiset<Wrapper, Hash, KeyEq> hash_table_t;

extern "C" void Zeta_DebugHashTable_Init(void* debug_ht_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = new hash_table_t{
        0,
        Hash{ .core =
                  new HashCore{
                      .elem_hash_context = debug_ht->elem_hash_context,
                      .ElemHash = debug_ht->ElemHash,
                      .key_hash_context = nullptr,
                      .KeyHash = nullptr,
                  } },
        KeyEq{ .core =
                   new KeyEqCore{
                       .elem_cmp_context = debug_ht->elem_cmp_context,
                       .ElemCompare = debug_ht->ElemCompare,
                       .key_elem_cmp_context = nullptr,
                       .KeyElemCompare = nullptr,
                   } }
    };

    debug_ht->hash_table = hash_table;

    ZETA_DebugAssert(debug_ht->ElemHash != NULL);
    ZETA_DebugAssert(debug_ht->ElemCompare != NULL);
}

extern "C" void Zeta_DebugHashTable_Deinit(void* debug_ht_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    delete hash_table;

    debug_ht->hash_table = NULL;
}

extern "C" size_t Zeta_DebugHashTable_GetWidth(void const* debug_ht_) {
    Zeta_DebugHashTable const* debug_ht = (Zeta_DebugHashTable const*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    return debug_ht->width;
}

extern "C" size_t Zeta_DebugHashTable_GetSize(void const* debug_ht_) {
    Zeta_DebugHashTable const* debug_ht = (Zeta_DebugHashTable const*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    return hash_table->size();
}

extern "C" size_t Zeta_DebugHashTable_GetCapacity(void const* debug_ht_) {
    Zeta_DebugHashTable const* debug_ht = (Zeta_DebugHashTable const*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    return ZETA_max_capacity;
}

extern "C" void Zeta_DebugHashTable_GetRBCursor(void const* debug_ht_,
                                                void* dst_cursor_) {
    Zeta_DebugHashTable const* debug_ht = (Zeta_DebugHashTable const*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* dst_cursor = (hash_table_t::iterator*)dst_cursor_;

    if (dst_cursor == NULL) { return; }

    new (dst_cursor) hash_table_t::iterator{ hash_table->end() };
}

extern "C" void* Zeta_DebugHashTable_PeekL(void* debug_ht_, void* dst_cursor_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* dst_cursor = (hash_table_t::iterator*)dst_cursor_;

    auto iter{ hash_table->begin() };

    if (dst_cursor != NULL) { new (dst_cursor) hash_table_t::iterator{ iter }; }

    return const_cast<void*>(iter->ptr);
}

extern "C" void* Zeta_DebugHashTable_Refer(void* debug_ht_,
                                           void const* pos_cursor_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* iter = (hash_table_t::iterator*)pos_cursor_;
    ZETA_DebugAssert(iter != NULL);

    return *iter == hash_table->end() ? NULL : const_cast<void*>((*iter)->ptr);
}

extern "C" void* Zeta_DebugHashTable_Find(void* debug_ht_, void const* key,
                                          void const* key_hash_context,
                                          Zeta_Hash KeyHash,
                                          void const* key_elem_cmp_context,
                                          Zeta_Compare KeyElemCompare,
                                          void* dst_cursor_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* dst_cursor = (hash_table_t::iterator*)dst_cursor_;

    HashCore* hash_core{ hash_table->hash_function().core };
    KeyEqCore* key_eq_core{ hash_table->key_eq().core };

    hash_core->key_hash_context = key_hash_context;
    hash_core->KeyHash = KeyHash;

    key_eq_core->key_elem_cmp_context = key_elem_cmp_context;
    key_eq_core->KeyElemCompare = KeyElemCompare;

    auto iter{ hash_table->find(Wrapper{ .is_key = TRUE, .ptr = key }) };

    if (dst_cursor != NULL) { new (dst_cursor) hash_table_t::iterator{ iter }; }

    return iter == hash_table->end() ? NULL : const_cast<void*>(iter->ptr);
}

extern "C" void* Zeta_DebugHashTable_Insert(void* debug_ht_, void const* elem,
                                            void* dst_cursor_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* dst_cursor = (hash_table_t::iterator*)dst_cursor_;

    Wrapper wrapper{ .is_key = FALSE, .ptr = std::malloc(debug_ht->width) };
    std::memcpy(const_cast<void*>(wrapper.ptr), elem, debug_ht->width);

    auto iter{ hash_table->insert(wrapper) };

    if (dst_cursor != NULL) { new (dst_cursor) hash_table_t::iterator{ iter }; }

    return const_cast<void*>(iter->ptr);
}

extern "C" void Zeta_DebugHashTable_Erase(void* debug_ht_, void* pos_cursor_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* pos_cursor = (hash_table_t::iterator*)pos_cursor_;
    ZETA_DebugAssert(pos_cursor != NULL);

    ZETA_DebugAssert(*pos_cursor != hash_table->end());

    void const* elem = (*pos_cursor)->ptr;

    hash_table->erase(*pos_cursor);

    std::free(const_cast<void*>(elem));
}

extern "C" void Zeta_DebugHashTable_EraseAll(void* debug_ht_) {
    Zeta_DebugHashTable* debug_ht = (Zeta_DebugHashTable*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table->clear();
}

extern "C" bool_t Zeta_DebugHashTable_Cursor_AreEqual(void const* debug_ht_,
                                                      void const* cursor_a_,
                                                      void const* cursor_b_) {
    Zeta_DebugHashTable const* debug_ht = (Zeta_DebugHashTable const*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* cursor_a = (hash_table_t::iterator*)cursor_a_;
    hash_table_t::iterator* cursor_b = (hash_table_t::iterator*)cursor_b_;

    ZETA_DebugAssert(cursor_a != NULL);
    ZETA_DebugAssert(cursor_b != NULL);

    return *cursor_a == *cursor_b;
}

extern "C" void Zeta_DebugHashTable_Cursor_StepL(void const* debug_ht_,
                                                 void* cursor_) {
    Zeta_DebugHashTable const* debug_ht = (Zeta_DebugHashTable const*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* cursor = (hash_table_t::iterator*)cursor_;
    ZETA_DebugAssert(cursor != NULL);

    --(*cursor);
}

extern "C" void Zeta_DebugHashTable_Cursor_StepR(void const* debug_ht_,
                                                 void* cursor_) {
    Zeta_DebugHashTable const* debug_ht = (Zeta_DebugHashTable const*)debug_ht_;
    ZETA_DebugAssert(debug_ht != NULL);

    hash_table_t* hash_table = (hash_table_t*)debug_ht->hash_table;
    ZETA_DebugAssert(hash_table != NULL);

    hash_table_t::iterator* cursor = (hash_table_t::iterator*)cursor_;
    ZETA_DebugAssert(cursor != NULL);

    ++(*cursor);
}

extern "C" void Zeta_DebugHashTable_DeployAssocCntr(
    void* debug_ht_, Zeta_AssocCntr* assoc_cntr) {
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
