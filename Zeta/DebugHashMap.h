#pragma once

#include "AssocContainer.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_DebugHashMap);
ZETA_DeclareStruct(Zeta_DebugHashMap_Pair);

struct Zeta_DebugHashMap {
    void* hash_map;
};

struct Zeta_DebugHashMap_Pair {
    unsigned long long const key;
    unsigned long long val;
};

void Zeta_DebugHashMap_Init(void* debug_hm);

void Zeta_DebugHashMap_Deinit(void* debug_hm);

size_t Zeta_DebugHashMap_GetWidth(void* debug_hm);

size_t Zeta_DebugHashMap_GetStride(void* debug_hm);

size_t Zeta_DebugHashMap_GetSize(void* debug_hm);

size_t Zeta_DebugHashMap_GetCapacity(void* debug_hm);

// void Zeta_DebugHashMap_GetLBCursor(void* debug_hm, void* dst_cursor);

void Zeta_DebugHashMap_GetRBCursor(void* debug_hm, void* dst_cursor);

void* Zeta_DebugHashMap_PeekL(void* debug_hm, void* dst_cursor);

// void Zeta_DebugHashMap_PeekR(void* debug_hm, void* dst_cursor);

void* Zeta_DebugHashMap_Refer(void* debug_hm, void const* pos_cursor);

void* Zeta_DebugHashMap_Find(void* debug_hm, void* dst_cursor, void const* key);

void* Zeta_DebugHashMap_Insert(void* debug_hm, void* dst_cursor,
                               void const* key);

void Zeta_DebugHashMap_Erase(void* debug_hm, void* pos_cursor);

void Zeta_DebugHashMap_EraseAll(void* debug_hm);

bool_t Zeta_DebugHashMap_Cursor_IsEqual(void* debug_hm, void const* cursor_a,
                                        void const* cursor_b);

/*
int Zeta_DebugHashMap_Cursor_Compare(void* debug_hm, void const* cursor_a,
                                     void const* cursor_b);
*/

/*
size_t Zeta_DebugHashMap_Cursor_GetDist(void* debug_hm, void const* cursor_a,
                                        void const* cursor_b);
*/

// size_t Zeta_DebugHashMap_Cursor_GetIdx(void* debug_hm, void const* cursor);

void Zeta_DebugHashMap_Cursor_StepL(void* debug_hm, void* cursor);

void Zeta_DebugHashMap_Cursor_StepR(void* debug_hm, void* cursor);

/* void Zeta_DebugHashMap_Cursor_AdvanceL(void* debug_hm, void* cursor,
                                       size_t step); */

/* void Zeta_DebugHashMap_Cursor_AdvanceR(void* debug_hm, void* cursor,
                                       size_t step); */

void Zeta_DebugHashMap_Move(void* debug_hm, void* src_debug_hm);

void Zeta_DebugHashMap_Print(void* debug_hm);

ZETA_ExternC_End;
