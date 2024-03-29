#pragma once

#include "Allocator.h"
#include "DebugTreeMap.h"
#include "OrdLinkedListNode.h"
#include "Vector.h"

ZETA_extern_c_beg;

typedef struct Zeta_SegList_Node Zeta_SegList_Node;

struct Zeta_SegList_Node {
    char padding[8];
    Zeta_OrdLinkedListNode n;
    void* seg;
};

typedef struct Zeta_SegList Zeta_SegList;

struct Zeta_SegList {
    /** The width(bytes) of element occupying. */
    size_t width;

    /** The number of slots have not been used in first segment. */
    size_t offset;

    /** The max number of elements in one segment. */
    size_t seg_capacity;

    /** The number of elements this seg list contains. */
    size_t size;

    Zeta_Allocator* node_allocator;
    Zeta_Allocator* seg_allocator;

    Zeta_OrdLinkedListNode head;
};

void Zeta_SegList_Init(void* sl);

size_t Zeta_SegList_GetWidth(void* sl);

size_t Zeta_SegList_GetSize(void* sl);

void* Zeta_SegList_PeekL(void* sl);

void* Zeta_SegList_PeekR(void* sl);

void* Zeta_SegList_PushL(void* sl);

void* Zeta_SegList_PushR(void* sl);

void Zeta_SegList_PopL(void* sl);

void Zeta_SegList_PopR(void* sl);

void Zeta_SegList_Check(void* sl, Zeta_DebugTreeMap* dst_node_tm,
                        Zeta_DebugTreeMap* dst_seg_tm);

void Zeta_SegList_ToVector(void* sl, Zeta_Vector* dst);

ZETA_extern_c_end;
