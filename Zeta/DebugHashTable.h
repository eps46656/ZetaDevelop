#pragma once

#include "AssocContainer.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_DebugHashTable);
ZETA_DeclareStruct(Zeta_DebugHashTable_Pair);

struct Zeta_DebugHashTable {
    size_t width;

    void* hash_table;

    void* elem_hash_context;

    unsigned long long (*ElemHash)(void* elem_hash_context, void const* elem);

    void* key_hash_context;

    unsigned long long (*KeyHash)(void* key_hash_context, void const* key);

    void* elem_cmp_context;

    int (*ElemCompare)(void* elem_cmp_context, void const* elem_a,
                       void const* elem_b);

    void* elem_key_cmp_context;

    int (*ElemKeyCompare)(void* elem_cmp_context, void const* elem,
                          void const* key);
};

void Zeta_DebugHashTable_Init(void* debug_ht);

void Zeta_DebugHashTable_Deinit(void* debug_ht);

size_t Zeta_DebugHashTable_GetWidth(void* debug_ht);

size_t Zeta_DebugHashTable_GetStride(void* debug_ht);

size_t Zeta_DebugHashTable_GetSize(void* debug_ht);

size_t Zeta_DebugHashTable_GetCapacity(void* debug_ht);

// void Zeta_DebugHashTable_GetLBCursor(void* debug_ht, void* dst_cursor);

void Zeta_DebugHashTable_GetRBCursor(void* debug_ht, void* dst_cursor);

void* Zeta_DebugHashTable_PeekL(void* debug_ht, void* dst_cursor);

// void Zeta_DebugHashTable_PeekR(void* debug_ht, void* dst_cursor);

void* Zeta_DebugHashTable_Refer(void* debug_ht, void const* pos_cursor);

void* Zeta_DebugHashTable_Find(void* debug_ht, void const* key,
                               void* dst_cursor);

void* Zeta_DebugHashTable_Insert(void* debug_ht, void const* key,
                                 void* dst_cursor);

void Zeta_DebugHashTable_Erase(void* debug_ht, void* pos_cursor);

void Zeta_DebugHashTable_EraseAll(void* debug_ht);

bool_t Zeta_DebugHashTable_Cursor_AreEqual(void* debug_ht, void const* cursor_a,
                                           void const* cursor_b);

/*
int Zeta_DebugHashTable_Cursor_Compare(void* debug_ht, void const* cursor_a,
                                     void const* cursor_b);
*/

/*
size_t Zeta_DebugHashTable_Cursor_GetDist(void* debug_ht, void const* cursor_a,
                                        void const* cursor_b);
*/

// size_t Zeta_DebugHashTable_Cursor_GetIdx(void* debug_ht, void const* cursor);

void Zeta_DebugHashTable_Cursor_StepL(void* debug_ht, void* cursor);

void Zeta_DebugHashTable_Cursor_StepR(void* debug_ht, void* cursor);

/* void Zeta_DebugHashTable_Cursor_AdvanceL(void* debug_ht, void* cursor,
                                       size_t step); */

/* void Zeta_DebugHashTable_Cursor_AdvanceR(void* debug_ht, void* cursor,
                                       size_t step); */

void Zeta_DebugHashTable_DeployAssocContainer(void* debug_ht,
                                              Zeta_AssocContainer* assoc_cntr);

ZETA_ExternC_End;
