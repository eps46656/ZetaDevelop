#pragma once

#include "Allocator.h"
#include "DebugHashMap.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_ExtMultiLevelTable);
ZETA_DeclareStruct(Zeta_ExtMultiLevelTable_Node);

/**
 * The maximum number of level.
 */
#define ZETA_ExtMultiLevelTable_max_level 12

#define ZETA_ExtMultiLevelTable_max_branch_num \
    (ZETA_GetWidth(unsigned long long) * 4)

struct Zeta_ExtMultiLevelTable_Node {
    unsigned long long hot[4];
    void* ptrs[];
};

struct Zeta_ExtMultiLevelTable {
    int level;
    size_t branch_nums[ZETA_ExtMultiLevelTable_max_level];

    size_t size;

    Zeta_ExtMultiLevelTable_Node* root;

    Zeta_Allocator* node_allocator;
};

/**
 * @brief Initialize the mlt.
 *
 * @param mlt The target mlt.
 */
void Zeta_ExtMultiLevelTable_Init(void* mlt);

/**
 * @brief Deinitialize the mlt.
 *
 * @param mlt The target mlt.
 */
void Zeta_ExtMultiLevelTable_Deinit(void* mlt);

/**
 * @brief Get the number of entries in mlt.
 *
 * @param mlt The target mlt.
 */
size_t Zeta_ExtMultiLevelTable_GetSize(void* mlt);

/**
 * @brief Get the total capacity of mlt. Assume the value does not overflow
 * max range of size_t.
 *
 * @param mlt The target mlt.
 */
size_t Zeta_ExtMultiLevelTable_GetCapacity(void* mlt);

/**
 * @brief Get the reference of target entry by indexes.
 *
 * @param mlt The target mlt.
 * @param idx The index of target entry in each level.
 *
 * @return The reference of target entry. If the it is not inserted, return
 * NULL.
 */
void** Zeta_ExtMultiLevelTable_Access(void* mlt, size_t const* idxes);

void** Zeta_ExtMultiLevelTable_FindFirst(void* mlt, size_t* idxes);

void** Zeta_ExtMultiLevelTable_FindLast(void* mlt, size_t* idxes);

void** Zeta_ExtMultiLevelTable_FindPrev(void* mlt, size_t* idxes,
                                        bool_t included);

void** Zeta_ExtMultiLevelTable_FindNext(void* mlt, size_t* idxes,
                                        bool_t included);

/**
 * @brief Insert a new entry at idxes then return its reference. If
 * it has already existed, remaining its origin value.
 *
 * @param mlt The target mlt.
 * @param idxes The indexes of target entry in each level.
 *
 * @return The reference of target entry.
 */
void** Zeta_ExtMultiLevelTable_Insert(void* mlt, size_t* idxes);

/**
 * @brief Erase the target entry by indexes. If it has not existen.
 *
 * @param mlt The target mlt.
 * @param idx The indexes of target entry in each level.
 *
 * @return The reference of target entry.
 */
void Zeta_ExtMultiLevelTable_Erase(void* mlt, size_t* idxes);

/**
 * @brief Erase all existed entries.
 *
 * @param mlt The target mlt.
 */
void Zeta_ExtMultiLevelTable_EraseAll(void* mlt);

void Zeta_ExtMultiLevelTable_Check(void* mlt, Zeta_DebugHashMap* dst_node);

ZETA_ExternC_End;
