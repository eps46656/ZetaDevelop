#pragma once

#include "allocator.h"
#include "bin_tree_node.h"
#include "mem_check_utils.h"

ZETA_ExternC_Beg;

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

    Zeta_Hash NodeHash;
    void const* node_hash_context;

    Zeta_Compare NodeCompare;
    void const* node_cmp_context;

    Zeta_Allocator table_node_allocator;
};

struct Zeta_GenericHashTable_Node {
    ZETA_DebugStructPadding;

    Zeta_OrdRBTreeNode n;

    ZETA_DebugStructPadding;
};

void Zeta_GenericHashTable_Init(void* ght);

void Zeta_GenericHashTable_Deinit(void* ght);

size_t Zeta_GenericHashTable_GetSize(void const* ght);

bool_t Zeta_GenericHashTable_Contain(void const* ght, void const* node);

void* Zeta_GenericHashTable_Find(void const* ght, void const* key,
                                 Zeta_Hash KeyHash,
                                 void const* key_hash_context,
                                 Zeta_Compare KeyNodeCompare,
                                 void const* key_node_cmp_context);

void Zeta_GenericHashTable_Insert(void* ght, void* node);

void Zeta_GenericHashTable_Extract(void* ght, void* node);

void* Zeta_GenericHashTable_ExtractAny(void* ght);

void Zeta_GenericHashTable_ExtractAll(void* ght);

bool_t Zeta_GenericHashTable_RunPending(void* lrucm, size_t quata);

unsigned long long Zeta_GenericHashTable_GetEffFactor(void const* ght);

void Zeta_GenericHashTable_Check(void const* ght);

void Zeta_GenericHashTable_Sanitize(void const* ght,
                                    Zeta_MemRecorder* dst_table,
                                    Zeta_MemRecorder* dst_node);

void Zeta_GenericHashTable_Node_Init(void* ghtn);

ZETA_ExternC_End;
