#pragma once

#include "Allocator.h"
#include "DebugTreeMap.h"

ZETA_ExternC_Beg;

/**
 * The maximum number of level.
 */
#define ZETA_MultiLevelVector_level_max 12

/**
 * The maximum branch number of each level.
 */
#define ZETA_MultiLevelVector_branch_num_max 4096

typedef struct Zeta_MultiLevelVector Zeta_MultiLevelVector;

struct Zeta_MultiLevelVector {
    int level;
    size_t branch_nums[ZETA_MultiLevelVector_level_max];

    void* root;

    Zeta_Allocator allocator;
};

/**
 * @brief Initialize the mlv.
 *
 * @param mlv The target mlv.
 */
void Zeta_MultiLevelVector_Init(void* mlv);

/**
 * @brief Get the total capacity of mlv. Assume the value does not overflow max
 * range of size_t.
 *
 * @param mlv The target mlv.
 */
size_t Zeta_MultiLevelVector_GetCapacity(void* mlv);

/**
 * @brief Get the reference of target entry by indexes.
 *
 * @param mlv The target mlv.
 * @param idxes The indexes of target entry in each level.
 *
 * @return The reference of target entry. If the it is not inserted, return
 * NULL.
 */
void** Zeta_MultiLevelVector_Access(void* mlv, size_t* idxes);

void** Zeta_MultiLevelVector_FindFirstNotNull(void* mlv, size_t* idxes);

void** Zeta_MultiLevelVector_FindLastNotNull(void* mlv, size_t* idxes);

void** Zeta_MultiLevelVector_FindPrevNotNull(void* mlv, size_t* idxes,
                                             bool_t included);

void** Zeta_MultiLevelVector_FindNextNotNull(void* mlv, size_t* idxes,
                                             bool_t included);

/**
 * @brief Insert a new entry at idxes then return its reference. If
 * it has already existed, remaining its origin value.
 *
 * @param mlv The target mlv.
 * @param idxes The indexes of target entry in each level.
 *
 * @return The reference of target entry.
 */
void** Zeta_MultiLevelVector_Insert(void* mlv, size_t* idxes);

/**
 * @brief Erase the target entry by indexes. If it has not existen.
 *
 * @param mlv The target mlv.
 * @param idx The indexes of target entry in each level.
 *
 * @return The reference of target entry.
 */
void Zeta_MultiLevelVector_Erase(void* mlv, size_t* idxes);

/**
 * @brief Erase all existed entries.
 *
 * @param mlv The target mlv.
 */
void Zeta_MultiLevelVector_EraseAll(void* mlv);

bool_t Zeta_MultiLevelVector_IsClean(void* mlv);

/**
 * @brief Erase all existed entries which contain NULL.
 *
 * @param mlv The target mlv.
 */
void Zeta_MultiLevelVector_Clear(void* mlv);

void Zeta_MultiLevelVector_GetAllPages(void* mlv, Zeta_DebugTreeMap* dst);

ZETA_ExternC_End;
