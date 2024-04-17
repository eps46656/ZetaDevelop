#pragma once

#include "Allocator.h"
#include "DebugTreeMap.h"

ZETA_ExternC_Beg;

/**
 * The maximum number of level.
 */
#define ZETA_MultiLevelVector_max_level 12

/**
 * The maximum branch number of each level.
 */
#define ZETA_MultiLevelVector_max_branch_num 4096

typedef unsigned long long Zeta_MultiLevelVector_mask_t;

typedef struct Zeta_MultiLevelVector_Node Zeta_MultiLevelVector_Node;

struct Zeta_MultiLevelVector_Node {
    void** table;
    Zeta_MultiLevelVector_mask_t mask[1];
};

typedef struct Zeta_MultiLevelVector Zeta_MultiLevelVector;

struct Zeta_MultiLevelVector {
    int level;
    size_t branch_nums[ZETA_MultiLevelVector_max_level];

    size_t size;

    Zeta_MultiLevelVector_Node* root;

    Zeta_Allocator* node_allocator;
    Zeta_Allocator* table_allocator;
};

/**
 * @brief Initialize the mlv.
 *
 * @param mlv The target mlv.
 */
void Zeta_MultiLevelVector_Init(void* mlv);

/**
 * @brief Get the number of entries in mlv.
 *
 * @param mlv The target mlv.
 */
size_t Zeta_MultiLevelVector_GetSize(void* mlv);

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
 * @param idx The index of target entry in each level.
 *
 * @return The reference of target entry. If the it is not inserted, return
 * NULL.
 */
void** Zeta_MultiLevelVector_Access(void* mlv, size_t const* idxes);

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

void Zeta_MultiLevelVector_GetAllPages(void* mlv, Zeta_DebugTreeMap* dst_node,
                                       Zeta_DebugTreeMap* dst_table);

ZETA_ExternC_End;
