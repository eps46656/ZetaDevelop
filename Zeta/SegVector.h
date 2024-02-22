#pragma once

#include "Allocator.h"
#include "DebugTreeMap.h"
#include "RelCntRBTreeNode.h"
#include "Vector.h"

ZETA_extern_c_beg;

typedef struct Zeta_SegVector_Node Zeta_SegVector_Node;

struct Zeta_SegVector_Node {
    Zeta_RelCntRBTreeNode n;
    void* seg;
    size_t offset;
};

typedef struct Zeta_SegVector Zeta_SegVector;

struct Zeta_SegVector {
    /** The width(bytes) of element occupying. */
    size_t width;

    /** The max number of elements in one seg. */
    size_t seg_capacity;

    Zeta_RelCntRBTreeNode* root;

    Zeta_Allocator* node_allocator;
    Zeta_Allocator* seg_allocator;
};

void Zeta_SegVector_Init(void* sv);

size_t Zeta_SegVector_GetWidth(void* sv);

size_t Zeta_SegVector_GetSize(void* sv);

void* Zeta_SegVector_PeekL(void* sv);

void* Zeta_SegVector_PeekR(void* sv);

void* Zeta_SegVector_Access(void* sv, size_t idx);

void* Zeta_SegVector_PushL(void* sv);

void* Zeta_SegVector_PushR(void* sv);

void* Zeta_SegVector_Insert(void* sv, size_t idx);

void Zeta_SegVector_PopL(void* sv);

void Zeta_SegVector_PopR(void* sv);

void Zeta_SegVector_Erase(void* sv, size_t idx);

void Zeta_SegVector_Check(void* sv_, Zeta_DebugTreeMap* dst_node_tm,
                          Zeta_DebugTreeMap* dst_seg_tm);

void Zeta_SegVector_ToVector(void* sv, Zeta_Vector* dst);

ZETA_extern_c_end;
