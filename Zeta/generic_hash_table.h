#pragma once

#include "allocator.h"
#include "mem_check_utils.h"
#include "ord_rb_tree_node.h"

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

    void* node_hash_context;
    Zeta_Hash NodeHash;

    void* key_hash_context;
    Zeta_Hash KeyHash;

    void* node_cmp_context;
    Zeta_Compare NodeCompare;

    void* node_key_cmp_context;
    Zeta_Compare NodeKeyCompare;
};

struct Zeta_GenericHashTable_Node {
    Zeta_OrdRBTreeNode n;
};

void Zeta_GenericHashTable_Init(void* ght);

void Zeta_GenericHashTable_Deinit(void* ght);

size_t Zeta_GenericHashTable_GetSize(void* ght);

void* Zeta_GenericHashTable_Find(void* ght, void const* key);

void Zeta_GenericHashTable_Insert(void* ght, void* node);

void Zeta_GenericHashTable_Erase(void* ght, void* node);

void Zeta_GenericHashTable_EraseAll(void* ght);

unsigned long long Zeta_GenericHashTable_GetEffFactor(void* ght);

void Zeta_GenericHashTable_Check(void* ght);

void Zeta_GenericHashTable_Sanitize(void* ght, Zeta_MemRecorder* dst_table,
                                    Zeta_MemRecorder* dst_node);

void Zeta_GenericHashTable_CheckNode(void* ght, void* node);

ZETA_ExternC_End;
