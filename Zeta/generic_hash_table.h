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

    void* cur_table_root;
    void* nxt_table_root;

    size_t cur_table_size;
    size_t nxt_table_size;

    size_t cur_capacity;
    size_t nxt_capacity;

    size_t size;

    void* node_hash_context;
    Zeta_Hash NodeHash;

    void* node_cmp_context;
    Zeta_Compare NodeCompare;

    Zeta_Allocator* table_node_allocator;
};

struct Zeta_GenericHashTable_Node {
    Zeta_OrdRBTreeNode n;
};

void Zeta_GenericHashTable_Init(void* ght);

void Zeta_GenericHashTable_Deinit(void* ght);

size_t Zeta_GenericHashTable_GetSize(void const* ght);

bool_t Zeta_GenericHashTable_Contain(void const* ght, void const* node);

void* Zeta_GenericHashTable_Find(void const* ght, void const* key,
                                 void const* key_hash_context,
                                 Zeta_Hash KeyHash,
                                 void const* key_node_cmp_context,
                                 Zeta_Compare KeyNodeCompare);

void Zeta_GenericHashTable_Insert(void* ght, void* node);

void Zeta_GenericHashTable_Extract(void* ght, void* node);

void Zeta_GenericHashTable_ExtractAll(void* ght);

unsigned long long Zeta_GenericHashTable_GetEffFactor(void const* ght);

void Zeta_GenericHashTable_Check(void const* ght);

void Zeta_GenericHashTable_Sanitize(void const* ght,
                                    Zeta_MemRecorder* dst_table,
                                    Zeta_MemRecorder* dst_node);

void Zeta_GenericHashTable_Node_Init(void* ghtn);

ZETA_ExternC_End;
