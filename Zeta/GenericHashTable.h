#pragma once

#include "Allocator.h"
#include "DebugHashMap.h"
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
};

struct Zeta_GenericHashTable_Node {
    Zeta_OrdRBTreeNode n;
    unsigned long long hash_code;
};

/*

maintain 2 hash table, cur_ht, nxt_ht

insert elements into cur_ht and initialize nxt_ht.

each ht is a ptr array, points to element tree

salt:
    the process of mapping hash code to bucket_idx is salted.
    corresping to cur_ht and nxt_ht, cur_salt and nxt_salt are needed.

    salt will be generated randomly to prevent attack.

cur state:
    only cur_ht is using.
    nxt_ht is NULL.

    insert:
        insert into cur_ht.

initilizing state:
    cur_ht reaches a quata. initializing nxt_ht.

migrating state:
    nxt_ht has been initialized.
    insert element into nxt_ht.
    find element in cur_ht and nxt_ht.

    migrate some elements from cur_ht to nxt_ht


find(key: hash code):
    cur state:
        find key in cur_ht, return the first node which hash code is key.

    initializeing state:
        initialize constant amount of nxt_ht. if nxt_ht is fully initialized,
        transfer state to migrating state.

        find key in cur_ht, return the first node which hash code is key.

    migrating state:
        find key in cur_ht, migrate these nodes whoses hash code are
        key to nxt_ht.

        find key in nxt_ht, return the first node which hash code is key.

insert(node):
    cur state:
        insert into cur_ht

    initializeing state:
        initialize constant amount of nxt_ht. if nxt_ht is fully initialized,
        transfer state to migrating state.

        if state is still initializeing state:
            insert into cur_ht.

    migrating state:
        migrate constant amount element from cur_ht to nxt_ht, is cur_ht is
        fully migrated, transfer state to cur state.

        if state is stll  migrating state:
            insert into nxt_ht.

extract(node):
    directly extract the node from ht.

*/

void Zeta_GenericHashTable_Init(void* ght);

void* Zeta_GenericHashTable_Find(void* ght, unsigned long long hash_code);

void Zeta_GenericHashTable_Insert(void* ght, void* node);

void Zeta_GenericHashTable_Erase(void* ght, void* node);

void Zeta_GenericHashTable_Check(void* ght);

void Zeta_GenericHashTable_Sanitize(void* ght, Zeta_DebugHashMap* dst_table,
                                    Zeta_DebugHashMap* dst_node);

ZETA_ExternC_End;
