#pragma once

#include "Allocator.h"
#include "DebugTreeMap.h"

ZETA_extern_c_beg;

/**
 * The maximum number of level.
 */
#define ZETA_MultiLevelEntryTable_max_level 12

/**
 * The maximum branch number of each level.
 */
#define ZETA_MultiLevelEntryTable_max_branch_num 4096

typedef struct Zeta_MultiLevelEntryTable Zeta_MultiLevelEntryTable;

struct Zeta_MultiLevelEntryTable {
    int level;
    size_t branch_nums[ZETA_MultiLevelEntryTable_max_level];

    void* root;

    Zeta_Allocator* allocator;
};

/**
 * @brief Initialize the mlet.
 *
 * @param mlet The target mlet.
 */
void Zeta_MultiLevelEntryTable_Init(void* mlet);

/**
 * @brief Get the total capacity of mlet. Assume the value does not overflow max
 * range of size_t.
 *
 * @param mlet The target mlet.
 */
size_t Zeta_MultiLevelEntryTable_GetCapacity(void* mlet);

/**
 * @brief Get the reference of target entry by indexes.
 *
 * @param mlet The target mlet.
 * @param idxes The indexes of target entry in each level.
 *
 * @return The reference of target entry. If the it is not inserted, return
 * NULL.
 */
void** Zeta_MultiLevelEntryTable_Access(void* mlet, size_t* idxes);

void** Zeta_MultiLevelEntryTable_FindPrevNotNull(void* mlet, size_t* idxes,
                                                 bool_t included);

void** Zeta_MultiLevelEntryTable_FindNextNotNull(void* mlet, size_t* idxes,
                                                 bool_t included);

/**
 * @brief Insert a new entry at idxes then return its reference. If
 * it has already existed, remaining its origin value.
 *
 * @param mlet The target mlet.
 * @param idxes The indexes of target entry in each level.
 *
 * @return The reference of target entry.
 */
void** Zeta_MultiLevelEntryTable_Insert(void* mlet, size_t* idxes);

/**
 * @brief Erase the target entry by indexes. If it has not existen.
 *
 * @param mlet The target mlet.
 * @param idx The indexes of target entry in each level.
 *
 * @return The reference of target entry.
 */
void Zeta_MultiLevelEntryTable_Erase(void* mlet, size_t* idxes);

/**
 * @brief Erase all existed entries.
 *
 * @param mlet The target mlet.
 */
void Zeta_MultiLevelEntryTable_EraseAll(void* mlet);

bool_t Zeta_MultiLevelEntryTable_IsClean(void* mlet);

/**
 * @brief Erase all existed entries which contain NULL.
 *
 * @param mlet The target mlet.
 */
void Zeta_MultiLevelEntryTable_Clear(void* mlet);

void Zeta_MultiLevelEntryTable_GetAllPages(void* mlet, Zeta_DebugTreeMap* dst);

ZETA_extern_c_end;
