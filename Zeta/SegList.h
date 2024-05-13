#pragma once

#include "Allocator.h"
#include "DebugHashMap.h"
#include "OrdLinkedListNode.h"
#include "SeqContainer.h"

ZETA_ExternC_Beg;

typedef struct Zeta_SegList_Node Zeta_SegList_Node;

struct Zeta_SegList_Node {
    char padding[8];
    Zeta_OrdLinkedListNode n;
    size_t offset;
    size_t size;
    void* seg;
};

typedef struct Zeta_SegList_Cursor Zeta_SegList_Cursor;

struct Zeta_SegList_Cursor {
    size_t idx;
    Zeta_OrdLinkedListNode* n;
    size_t seg_idx;
    void* ele;
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

void* Zeta_SegList_Access(void* sl, void* dst_cursor, size_t idx);

void* Zeta_SegList_PushL(void* sl);

void* Zeta_SegList_PushR(void* sl);

void Zeta_SegList_PopL(void* sl);

void Zeta_SegList_PopR(void* sl);

void* Zeta_SegList_Erase(void* sl, void* dst_cursor, size_t idx);

void Zeta_SegList_Check(void* sl, Zeta_DebugHashMap* dst_node_records,
                        Zeta_DebugHashMap* dst_seg_records);

void Zeta_SegList_DeploySeqContainer(void* sl, Zeta_SeqContainer* dst);

ZETA_ExternC_End;
