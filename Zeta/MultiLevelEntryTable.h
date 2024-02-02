#pragma once

#include "Allocator.h"

/**
 * The maximum branch number of each level.
 */
#define Zeta_MultiLevelEntryTable_max_branch_num 4096

/**
 * The maximum number of level.
 */
#define Zeta_MultiLevelEntryTable_max_level 32

typedef struct Zeta_MultiLevelEntryTable Zeta_MultiLevelEntryTable;

struct Zeta_MultiLevelEntryTable {
    int level;
    size_t branch_nums[Zeta_MultiLevelEntryTable_max_level];

    void *root;

    Zeta_Allocator *allocator;
};

/**
 * @brief Initialize mlet.
 *
 * @param mlet The target mlet.
 */
void Zeta_MultiLevelEntryTable_Init(void *mlet);

/**
 * @brief Get the total capacity of mlet. Assume the value does not overflow max
 * range of size_t.
 *
 * @param mlet The target mlet.
 */
size_t Zeta_MultiLevelEntryTable_GetCapacity(void *mlet);

/**
 * @brief Get the reference of target entry by indexes.
 *
 * @param mlet The target mlet.
 * @param idxes The indexes of target entry in each level.
 *
 * @return The reference of target entry. If the it is not inserted, return
 * NULL.
 */
void **Zeta_MultiLevelEntryTable_Access(void *mlet, size_t *idxes);

void **Zeta_MultiLevelEntryTable_FindPrevNotNull(void *mlet, size_t *idxes,
                                                 bool_t included);

void **Zeta_MultiLevelEntryTable_FindNextNotNull(void *mlet, size_t *idxes,
                                                 bool_t included);

/**
 * @brief Insert a new entry at indexes then return its reference. If
 * it has already existed, remaining its origin value.
 *
 * @param mlet The target mlet.
 * @param idxes The indexes of target entry in each level.
 *
 * @return The reference of target entry.
 */
void **Zeta_MultiLevelEntryTable_Insert(void *mlet, size_t *idxes);

/**
 * @brief Erase the target entry by idx. If it has not existen.
 *
 * @param mlet The target mlet.
 * @param idx The indexes of target entry in each level.
 *
 * @return The reference of target entry.
 */
void Zeta_MultiLevelEntryTable_Erase(void *mlet, size_t *idxes);

/**
 * @brief Erase all existed entries. Call Callback for each entry  before
 * erased. If Callback is NULL, it will be ignored.
 *
 * @param mlet The target mlet.
 * @param Callback The Callback function will be called before erasing each
 * entry.
 */
void Zeta_MultiLevelEntryTable_EraseAll(void *mlet,
                                        void (*Callback)(void *ptr));

/**
 * @brief Erase all existed entries which contain NULL.
 *
 * @param mlet The target mlet.
 */
void Zeta_MultiLevelEntryTable_Clear(void *mlet);
