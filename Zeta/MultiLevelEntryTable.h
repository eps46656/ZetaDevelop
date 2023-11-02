#pragma once

#include "define.h"

#define Zeta_MultiLevelEntryTable_max_branch_num 512
#define Zeta_MultiLevelEntryTable_max_level 32

ZETA_DECL_STRUCT(Zeta_MultiLevelEntryTable) {
    diff_t level;
    diff_t branch_nums[Zeta_MultiLevelEntryTable_max_level];

    void *(*Allocate)(diff_t size);
    void (*Deallocate)(void *ptr);

    void *root;
};

void Zeta_MultiLevelEntryTable_Init(void *mlet);

void **Zeta_MultiLevelEntryTable_Access(void *mlet, diff_t *idxes);

void **Zeta_MultiLevelEntryTable_GetNextNotNullIdx(void *mlet, diff_t *idxes,
                                                   diff_t step);

void **Zeta_MultiLevelEntryTable_Insert(void *mlet, diff_t *idxes);

void Zeta_MultiLevelEntryTable_Erase(void *mlet, diff_t *idxes);

void Zeta_MultiLevelEntryTable_Clear(void *mlet);
