#pragma once

#include "allocator.h"
#include "assoc_cntr.h"
#include "bin_tree_node.h"
#include "generic_hash_table.h"
#include "llist_node.h"
#include "mem_check_utils.h"

ZETA_ExternC_Beg;

ZETA_StaticAssert(alignof(void*) % 4 == 0);

ZETA_DeclareStruct(Zeta_DynamicHashTable);
ZETA_DeclareStruct(Zeta_DynamicHashTable_Node);
ZETA_DeclareStruct(Zeta_DynamicHashTable_Cursor);

struct Zeta_DynamicHashTable {
    size_t width;

    Zeta_OrdLListNode* lln;

    Zeta_Hash ElemHash;
    void* elem_hash_context;

    Zeta_Compare ElemCompare;
    void* elem_cmp_context;

    Zeta_Allocator node_allocator;

    Zeta_GenericHashTable ght;
};

struct Zeta_DynamicHashTable_Node {
    ZETA_DebugStructPadding;

    Zeta_OrdLListNode lln;

    ZETA_DebugStructPadding;

    Zeta_GenericHashTable_Node htn;

    ZETA_DebugStructPadding;

    unsigned char data[] __attribute__((aligned(alignof(max_align_t))));
};

struct Zeta_DynamicHashTable_Cursor {
    Zeta_DynamicHashTable const* dht;
    Zeta_OrdLListNode* lln;
};

void Zeta_DynamicHashTable_Init(void* dht);

void Zeta_DynamicHashTable_Deinit(void* dht);

size_t Zeta_DynamicHashTable_GetWidth(void const* dht);

size_t Zeta_DynamicHashTable_GetSize(void const* dht);

size_t Zeta_DynamicHashTable_GetCapacity(void const* dht);

void Zeta_DynamicHashTable_GetLBCursor(void const* dht, void* dst_cursor);

void Zeta_DynamicHashTable_GetRBCursor(void const* dht, void* dst_cursor);

void* Zeta_DynamicHashTable_PeekL(void* dht, void* dst_cursor);

void* Zeta_DynamicHashTable_PeekR(void* dht, void* dst_cursor);

void* Zeta_DynamicHashTable_Refer(void* dht, void const* pos_cursor);

void* Zeta_DynamicHashTable_Find(void* dht, void const* key, Zeta_Hash KeyHash,
                                 void const* key_hash_context,
                                 Zeta_Compare KeyElemCompare,
                                 void const* key_elem_cmp_context,
                                 void* dst_cursor);

void* Zeta_DynamicHashTable_Insert(void* dht, void const* elem,
                                   void* dst_cursor);

void Zeta_DynamicHashTable_Erase(void* dht, void* pos_cursor);

void Zeta_DynamicHashTable_EraseAll(void* dht);

unsigned long long Zeta_DynamicHashTable_GetEffFactor(void* dht);

void Zeta_DynamicHashTable_Check(void const* dht);

void Zeta_DynamicHashTable_Sanitize(void* dht, Zeta_MemRecorder* dst_table,
                                    Zeta_MemRecorder* dst_node);

bool_t Zeta_DynamicHashTable_Cursor_AreEqual(void const* dht,
                                             void const* cursor_a,
                                             void const* cursor_b);

void Zeta_DynamicHashTable_Cursor_StepL(void const* dht, void* cursor);

void Zeta_DynamicHashTable_Cursor_StepR(void const* dht, void* cursor);

void Zeta_DynamicHashTable_Cursor_Check(void const* dht, void const* cursor);

extern Zeta_AssocCntr_VTable const zeta_dynamic_hash_table_assoc_cntr_vtable;

ZETA_ExternC_End;
