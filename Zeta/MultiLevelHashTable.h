#pragma once

#include "MultiLevelVector.h"
#include "OrdLinkedListNode.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_MultiLevelHashTableNode);
ZETA_DeclareStruct(Zeta_MultiLevelHashTable);

struct Zeta_MultiLevelHashTableNode {
    size_t hash_code;
    Zeta_MultiLevelHashTableNode* nxt;
};

struct Zeta_MultiLevelHashTable {
    Zeta_MultiLevelVector mlv;
};

/* initialize the mlhtn */
void Zeta_MultiLevelHashTableNode_Init(void* mlhtn);

/* initialize the mlht */
void Zeta_MultiLevelHashTable_Init(void* mlht);

/* get the first mlhtn. */
void* Zeta_MultiLevelHashTable_GetFirst(void* mlht);

/* insert the mlhtn to mlht. */
void Zeta_MultiLevelHashTable_Insert(void* mlht, void* mlhtn);

/* extract the mlhtn from mlht. */
void Zeta_MultiLevelHashTable_Extract(void* mlht, void* mlhtn);

/* return the first mlhtn with same hash code. */
void* Zeta_MultiLevelHashTable_Find(void* mlht, size_t hash_code);

/* return the next first mlhtn with same hash code to mlhtn's one. */
void* Zeta_MultiLevelHashTable_FindNext(void* mlhtn);

ZETA_ExternC_End;
