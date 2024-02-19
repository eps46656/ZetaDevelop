#pragma once

#include "Allocator.h"
#include "RelCntRBTreeNode.h"
#include "Vector.h"

ZETA_extern_c_beg;

typedef struct Zeta_SegList_Node Zeta_SegList_Node;

struct Zeta_SegList_Node {
    Zeta_RelCntRBTreeNode n;
    void* cluster;
    size_t offset;
};

typedef struct Zeta_SegList Zeta_SegList;

struct Zeta_SegList {
    /** The width(bytes) of element occupying. */
    size_t width;

    /** The max number of elements in one cluster. */
    size_t cluster_capacity;

    Zeta_RelCntRBTreeNode* root;

    Zeta_Allocator* node_allocator;
    Zeta_Allocator* cluster_allocator;
};

void Zeta_SegList_Init(void* sl);

size_t Zeta_SegList_GetWidth(void* sl);

size_t Zeta_SegList_GetSize(void* sl);

void* Zeta_SegList_Access(void* sl, size_t idx);

void* Zeta_SegList_PeekL(void* sl);

void* Zeta_SegList_PeekR(void* sl);

void* Zeta_SegList_Insert(void* sl, size_t idx);

void* Zeta_SegList_PushL(void* sl);

void* Zeta_SegList_PushR(void* sl);

void Zeta_SegList_Erase(void* sl, size_t idx);

void Zeta_SegList_PopL(void* sl);

void Zeta_SegList_PopR(void* sl);

void Zeta_SegList_ToVector(void* sl, Zeta_Vector* dst);

ZETA_extern_c_end;
