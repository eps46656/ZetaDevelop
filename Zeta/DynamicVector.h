#pragma once

#include "Allocator.h"
#include "CircularVector.h"
#include "MultiLevelVector.h"
#include "SeqContainer.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_DynamicVector);

/**
 * @brief A double-ended vector.
 */
struct Zeta_DynamicVector {
    /** The width(byte) of an element occupying. */
    size_t width;

    /** The number of elements in a cluster. */
    size_t cluster_capacity;

    /** The offset(element) of begin. */
    size_t offset;

    /** The number of elements in vector. */
    size_t size;

    /** The multi level entry table to provide large entry space. */
    Zeta_MultiLevelVector mlv;

    /** The allocator to allocate cluster. */
    Zeta_Allocator* allocator;
};

/**
 * @brief Initialize the dv.
 *
 * @param dv The target DynamicVector
 */
void Zeta_DynamicVector_Init(void* dv);

void Zeta_DynamicVector_StdConfig(void* dv, size_t width, size_t cluster_size,
                                  Zeta_Allocator* allocator);

/**
 * @brief Get the width(byte) of an element occupying.
 */
size_t Zeta_DynamicVector_GetWidth(void* dv);

/**
 * @brief Get the number of elements.
 *
 * @param dv The target DynamicVector
 */
size_t Zeta_DynamicVector_GetSize(void* dv);

/**
 * @brief Access the target element by idx.
 *
 * @param dv The target DynamicVector.
 * @param idx The index of target element.
 *
 * @return The target element.
 */
void* Zeta_DynamicVector_Access(void* dv, size_t idx);

/**
 * @brief Insert an new element at idx. Auto shift left or right elements to
 * insert new element at idx.
 *
 * @param dv The target DynamicVector.
 * @param idx The index of target element.
 *
 * @return The target element.
 */
void* Zeta_DynamicVector_Insert(void* dv, size_t idx);

/**
 * @brief Push an element at left end of dv. A syntatic sugar of
 * Zeta_DynamicVector_Insert(dv, 0).
 *
 * @param dv The target DynamicVector.
 *
 * @return The target element.
 */
void* Zeta_DynamicVector_PushL(void* dv);

/**
 * @brief Push an element at right end of dv. A syntatic sugar of
 * Zeta_DynamicVector_Insert(dv, dv->size).
 *
 * @param dv The target DynamicVector.
 *
 * @return The target element.
 */
void* Zeta_DynamicVector_PushR(void* dv);

/**
 * @brief Erase the target element by index. Auto shift left or right elements.
 *
 * @param dv The target DynamicVector.
 * @param idx The index of target element.
 */
void Zeta_DynamicVector_Erase(void* dv, size_t idx);

/**
 * @brief Pop an element at left end of dv. A syntatic sugar of
 * Zeta_DynamicVector_Insert(dv, 0).
 *
 * @param dv The target DynamicVector.
 */
void Zeta_DynamicVector_PopL(void* dv);

/**
 * @brief Pop an element at right end of dv. A syntatic sugar of
 * Zeta_DynamicVector_Insert(dv, dv->size - 1).
 *
 * @param dv The target DynamicVector.
 */
void Zeta_DynamicVector_PopR(void* dv);

/**
 * @brief Erase all existed entries.
 *
 * @param dv The target dv.
 */
void Zeta_DynamicVector_EraseAll(void* dv);

/**
 * @brief Construct a vector interface from dv. Place result at dst.
 *
 * @param dv The target DynamicVector.
 * @param dst The destination of Vector.
 */
void Zeta_DynamicVector_ToVector(void* dv, Zeta_Vector* dst);

ZETA_ExternC_End;
