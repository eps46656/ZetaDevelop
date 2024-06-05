#pragma once

#include "Allocator.h"
#include "DebugHashMap.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_MultiLevelTable);
ZETA_DeclareStruct(Zeta_MultiLevelTable_Node);

/**
 * The maximum number of level.
 */
#define ZETA_MultiLevelTable_max_level 12

#define ZETA_MultiLevelTable_max_branch_num ZETA_GetWidth(unsigned long long)

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct Zeta_MultiLevelTable {
    int level;
    size_t branch_nums[ZETA_MultiLevelTable_max_level];

    size_t size;

    Zeta_MultiLevelTable_Node* root;

    Zeta_Allocator* node_allocator;
};

struct Zeta_MultiLevelTable_Node {
    unsigned long long hot;
    void* ptrs[];
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Initialize the mlt.
 *
 * @param mlt The target mlt.
 */
void Zeta_MultiLevelTable_Init(void* mlt);

/**
 * @brief Deinitialize the mlt.
 *
 * @param mlt The target mlt.
 */
void Zeta_MultiLevelTable_Deinit(void* mlt);

/**
 * @brief Get the number of entries in mlt.
 *
 * @param mlt The target mlt.
 */
size_t Zeta_MultiLevelTable_GetSize(void* mlt);

/**
 * @brief Get the total capacity of mlt. Assume the value does not overflow
 * max range of size_t.
 *
 * @param mlt The target mlt.
 */
size_t Zeta_MultiLevelTable_GetCapacity(void* mlt);

/**
 * @brief Get the reference of target entry by indexes.
 *
 * @param mlt The target mlt.
 * @param idx The index of target entry in each level.
 *
 * @return The reference of target entry. If the it is not inserted, return
 * NULL.
 */
void** Zeta_MultiLevelTable_Access(void* mlt, size_t const* idxes);

void** Zeta_MultiLevelTable_FindFirst(void* mlt, size_t* idxes);

void** Zeta_MultiLevelTable_FindLast(void* mlt, size_t* idxes);

void** Zeta_MultiLevelTable_FindPrev(void* mlt, size_t* idxes, bool_t included);

/**
 * @brief Find the next inserted element after idxes pointing.
 *
 * @param mlt The target mlt.
 * @param idxes The indexes of beginning of searching range in each level. If
 * the element is found, they will be set as the indexes of the element,
 * otherwise they will be set to 0.
 * @param included Indicates wheather idxes is included in the searching range.
 *
 * @return The reference of target entry.
 */
void** Zeta_MultiLevelTable_FindNext(void* mlt, size_t* idxes, bool_t included);

/**
 * @brief Insert a new entry at idxes then return its reference. If
 * it has already existed, remaining its origin value.
 *
 * @param mlt The target mlt.
 * @param idxes The indexes of target entry in each level.
 *
 * @return The reference of target entry.
 */
void** Zeta_MultiLevelTable_Insert(void* mlt, size_t* idxes);

/**
 * @brief Erase the target entry by indexes. If it has not existen.
 *
 * @param mlt The target mlt.
 * @param idx The indexes of target entry in each level.
 *
 * @return The reference of target entry.
 */
void Zeta_MultiLevelTable_Erase(void* mlt, size_t* idxes);

/**
 * @brief Erase all existed entries.
 *
 * @param mlt The target mlt.
 */
void Zeta_MultiLevelTable_EraseAll(void* mlt);

void Zeta_MultiLevelTable_Check(void* mlt, Zeta_DebugHashMap* dst_node);

ZETA_ExternC_End;
