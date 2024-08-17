#pragma once

#include "Allocator.h"
#include "DebugHashTable.h"
#include "OrdRBTreeNode.h"

ZETA_ExternC_Beg;

ZETA_StaticAssert(alignof(void*) % 4 == 0);

ZETA_DeclareStruct(Zeta_GenericHashTable);
ZETA_DeclareStruct(Zeta_GenericHashTable_Node);

struct Zeta_GenericHashTable {
    unsigned long long cur_salt;
    unsigned long long nxt_salt;

    void* cur_table;
    void* nxt_table;

    size_t cur_table_cnt;
    size_t nxt_table_cnt;

    size_t cur_table_capacity;
    size_t nxt_table_capacity;

    Zeta_Allocator* table_allocator;

    void const* node_cmp_context;

    int (*NodeCompare)(void const* node_cmper_context, void const* node_a,
                       void const* node_b);

    void const* node_key_cmp_context;

    int (*NodeKeyCompare)(void const* node_key_cmp_context, void const* node,
                          void const* key);
};

struct Zeta_GenericHashTable_Node {
    Zeta_OrdRBTreeNode n;
    unsigned long long hash_code;
};

void Zeta_GenericHashTable_Init(void* ght);

void Zeta_GenericHashTable_Deinit(void* ght);

size_t Zeta_GenericHashTable_GetSize(void* ght);

void* Zeta_GenericHashTable_Find(void* ght, void const* key,
                                 unsigned long long hash_code);

void Zeta_GenericHashTable_Insert(void* ght, void* node);

void Zeta_GenericHashTable_Erase(void* ght, void* node);

void Zeta_GenericHashTable_EraseAll(void* ght);

void Zeta_GenericHashTable_Check(void* ght);

void Zeta_GenericHashTable_Sanitize(void* ght, Zeta_AssocContainer* dst_table,
                                    Zeta_AssocContainer* dst_node);

void Zeta_GenericHashTable_CheckNode(void* ght, void* node);

ZETA_ExternC_End;
