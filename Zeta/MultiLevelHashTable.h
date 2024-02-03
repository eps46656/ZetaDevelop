#pragma once

#include "MultiLevelEntryTable.h"

typedef struct Zeta_MultiLevelHashTableNode Zeta_MultiLevelHashTableNode;

struct Zeta_MultiLevelHashTableNode {
    size_t hash_code;
    Zeta_MultiLevelHashTableNode* prev;
    Zeta_MultiLevelHashTableNode* next;
};

typedef struct Zeta_MultiLevelHashTable Zeta_MultiLevelHashTable;

struct Zeta_MultiLevelHashTable {
    Zeta_MultiLevelEntryTable* mlet;
};

/* initialize the mlhtn */
EXTERN_C void Zeta_MultiLevelHashTableNode_Init(void* mlhtn);

/* initialize the mlht */
EXTERN_C void Zeta_MultiLevelHashTable_Init(void* mlht);

/* get the first mlhtn. */
EXTERN_C void* Zeta_MultiLevelHashTable_GetFirst(void* mlht);

/* insert the mlhtn to mlht. */
EXTERN_C void Zeta_MultiLevelHashTable_Insert(void* mlht, void* mlhtn);

/* extract the mlhtn from mlht. */
EXTERN_C void Zeta_MultiLevelHashTable_Extract(void* mlht, void* mlhtn);

/* return the first mlhtn with same hash code. */
EXTERN_C void* Zeta_MultiLevelHashTable_Find(void* mlht, size_t hash_code);

/* return the next first mlhtn with same hash code to mlhtn's one. */
EXTERN_C void* Zeta_MultiLevelHashTable_FindNext(void* mlhtn);
