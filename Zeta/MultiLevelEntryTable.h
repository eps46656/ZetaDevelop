#pragma once

#include "Allocator.h"

#define Zeta_MultiLevelEntryTable_max_branch_num 512
#define Zeta_MultiLevelEntryTable_max_level 32

ZETA_DECL_STRUCT(Zeta_MultiLevelEntryTable) {
    diff_t level;
    diff_t branch_nums[Zeta_MultiLevelEntryTable_max_level];

    void *root;

    Zeta_Allocator *allocator;
};

void Zeta_MultiLevelEntryTable_Init(void *mlet);

diff_t Zeta_MultiLevelEntryTable_GetCapacity(void *mlet);

void **Zeta_MultiLevelEntryTable_Access(void *mlet, diff_t *idxes);

void **Zeta_MultiLevelEntryTable_FindPrevNotNull(void *mlet, diff_t *idxes,
                                                 _Bool included);

void **Zeta_MultiLevelEntryTable_FindNextNotNull(void *mlet, diff_t *idxes,
                                                 _Bool included);

void **Zeta_MultiLevelEntryTable_Insert(void *mlet, diff_t *idxes);

void Zeta_MultiLevelEntryTable_Attach(void *mlet, diff_t *idxes,
                                      void *target_mlet);

void Zeta_MultiLevelEntryTable_Erase(void *mlet, diff_t *idxes);

void Zeta_MultiLevelEntryTable_EraseAll(void *mlet,
                                        void (*Callback)(void *ptr));

void Zeta_MultiLevelEntryTable_Clear(void *mlet);
