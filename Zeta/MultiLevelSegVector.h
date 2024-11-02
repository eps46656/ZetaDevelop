#pragma once

#include "MultiLevelTable.h"
#include "allocator.h"
#include "circular_array.h"
#include "seq_cntr.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_MultiLevelSegVector);
ZETA_DeclareStruct(Zeta_MultiLevelSegVector_Node);

#define Zeta_MultiLevelSegVector_max_seg_capacity \
    ZETA_RangeMaxOf(unsigned short)

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief A double-ended vector.
 */
struct Zeta_MultiLevelSegVector {
    /** The width(byte) of an element occupying. */
    size_t width;

    size_t stride;

    /** The number of elements in a cluster. */
    size_t seg_capacity;

    /** The offset(element) of begin. */
    size_t offset;

    /** The number of elements in vector. */
    size_t size;

    /** The multi level table to provide large entry space. */
    Zeta_MultiLevelTable mlt;

    /** The allocator to allocate node. */
    Zeta_Allocator* node_allocator;

    /** The allocator to allocate data */
    Zeta_Allocator* data_allocator;
};

struct Zeta_MultiLevelSegVector_Node {
    void* data;
    unsigned short offset;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Initialize the dv.
 *
 * @param dv The target MultiLevelSegVector
 */
void Zeta_MultiLevelSegVector_Init(void* dv);

/**
 * @brief Get the width(byte) of an element occupying.
 */
size_t Zeta_MultiLevelSegVector_GetWidth(void* dv);

/**
 * @brief Get the width(byte) of an element occupying.
 */
size_t Zeta_MultiLevelSegVector_GetWidth(void* dv);

/**
 * @brief Get the number of elements.
 *
 * @param dv The target MultiLevelSegVector
 */
size_t Zeta_MultiLevelSegVector_GetSize(void* dv);

/**
 * @brief Access the target element by idx.
 *
 * @param dv The target MultiLevelSegVector.
 * @param idx The index of target element.
 *
 * @return The target element.
 */
void* Zeta_MultiLevelSegVector_Access(void* dv, size_t idx);

/**
 * @brief Insert an new element at idx. Auto shift left or right elements to
 * insert new element at idx.
 *
 * @param dv The target MultiLevelSegVector.
 * @param idx The index of target element.
 *
 * @return The target element.
 */
void* Zeta_MultiLevelSegVector_Insert(void* dv, size_t idx);

/**
 * @brief Push an element at left end of dv. A syntatic sugar of
 * Zeta_MultiLevelSegVector_Insert(dv, 0).
 *
 * @param dv The target MultiLevelSegVector.
 *
 * @return The target element.
 */
void* Zeta_MultiLevelSegVector_PushL(void* dv);

/**
 * @brief Push an element at right end of dv. A syntatic sugar of
 * Zeta_MultiLevelSegVector_Insert(dv, dv->size).
 *
 * @param dv The target MultiLevelSegVector.
 *
 * @return The target element.
 */
void* Zeta_MultiLevelSegVector_PushR(void* dv);

/**
 * @brief Erase the target element by index. Auto shift left or right elements.
 *
 * @param dv The target MultiLevelSegVector.
 * @param idx The index of target element.
 */
void Zeta_MultiLevelSegVector_Erase(void* dv, size_t idx);

/**
 * @brief Pop an element at left end of dv. A syntatic sugar of
 * Zeta_MultiLevelSegVector_Insert(dv, 0).
 *
 * @param dv The target MultiLevelSegVector.
 */
void Zeta_MultiLevelSegVector_PopL(void* dv);

/**
 * @brief Pop an element at right end of dv. A syntatic sugar of
 * Zeta_MultiLevelSegVector_Insert(dv, dv->size - 1).
 *
 * @param dv The target MultiLevelSegVector.
 */
void Zeta_MultiLevelSegVector_PopR(void* dv);

/**
 * @brief Erase all existed entries.
 *
 * @param dv The target dv.
 */
void Zeta_MultiLevelSegVector_EraseAll(void* dv);

/**
 * @brief Construct a vector interface from dv. Place result at dst.
 *
 * @param dv The target MultiLevelSegVector.
 * @param dst The destination of Vector.
 */
void Zeta_MultiLevelSegVector_ToVector(void* dv, Zeta_Vector* dst);

ZETA_ExternC_End;
