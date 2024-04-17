#include "SegList.h"

void Zeta_SegList_Init(void* sl_) {
    Zeta_SegList* sl = (Zeta_SegList*)sl_;
    ZETA_DebugAssert(sl != NULL);

    ZETA_DebugAssert(0 < sl->width);
    ZETA_DebugAssert(0 < sl->seg_capacity);

    ZETA_DebugAssert(sl->node_allocator != NULL);
    ZETA_DebugAssert(sl->node_allocator->Allocate != NULL);
    ZETA_DebugAssert(sl->node_allocator->Deallocate != NULL);

    ZETA_DebugAssert(sl->seg_allocator != NULL);
    ZETA_DebugAssert(sl->seg_allocator->Allocate != NULL);
    ZETA_DebugAssert(sl->seg_allocator->Deallocate != NULL);

    sl->offset = 0;
    sl->size = 0;

    Zeta_OrdLinkedListNode_Init(&sl->head);
}

size_t Zeta_SegList_GetWidth(void* sl_) {
    Zeta_SegList* sl = (Zeta_SegList*)sl_;
    ZETA_DebugAssert(sl != NULL);

    return sl->width;
}

size_t Zeta_SegList_GetSize(void* sl_) {
    Zeta_SegList* sl = (Zeta_SegList*)sl_;
    ZETA_DebugAssert(sl != NULL);

    return sl->size;
}

void* Zeta_SegList_PeekL(void* sl_) {
    Zeta_SegList* sl = (Zeta_SegList*)sl_;
    ZETA_DebugAssert(sl != NULL);

    size_t size = Zeta_SegList_GetSize(sl);
    ZETA_DebugAssert(0 < size);

    Zeta_SegList_Node* node = ZETA_GetStructFromMember(
        Zeta_SegList_Node, n, Zeta_OrdLinkedListNode_GetR(&sl->head));

    return (unsigned char*)(node->seg) + sl->width * sl->offset;
}

void* Zeta_SegList_PeekR(void* sl_) {
    Zeta_SegList* sl = (Zeta_SegList*)sl_;
    ZETA_DebugAssert(sl != NULL);

    size_t size = Zeta_SegList_GetSize(sl);
    ZETA_DebugAssert(0 < size);

    Zeta_SegList_Node* node = ZETA_GetStructFromMember(
        Zeta_SegList_Node, n, Zeta_OrdLinkedListNode_GetL(&sl->head));

    size_t i = (sl->offset + sl->size - 1) % sl->seg_capacity;

    return (unsigned char*)(node->seg) + sl->width * i;
}

Zeta_SegList_Node* AllocateNode_(Zeta_SegList* sl) {
    size_t width = sl->width;
    size_t seg_capacity = sl->seg_capacity;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(0 < seg_capacity);

    Zeta_Allocator* node_allocator = sl->node_allocator;

    ZETA_DebugAssert(node_allocator != NULL);
    ZETA_DebugAssert(node_allocator->Allocate != NULL);

    Zeta_Allocator* seg_allocator = sl->seg_allocator;

    ZETA_DebugAssert(seg_allocator != NULL);
    ZETA_DebugAssert(seg_allocator->Allocate != NULL);

    Zeta_SegList_Node* node = node_allocator->Allocate(
        node_allocator->context, sizeof(Zeta_SegList_Node));

    ZETA_DebugAssert(node != NULL);

    Zeta_OrdLinkedListNode_Init(&node->n);

    void* seg =
        seg_allocator->Allocate(seg_allocator->context, width * seg_capacity);

    ZETA_DebugAssert(seg != NULL);

    node->seg = seg;

    return node;
}

void DeallocateNode_(Zeta_SegList* sl, Zeta_SegList_Node* node) {
    Zeta_Allocator* node_allocator = sl->node_allocator;

    ZETA_DebugAssert(node_allocator != NULL);
    ZETA_DebugAssert(node_allocator->Deallocate != NULL);

    Zeta_Allocator* seg_allocator = sl->seg_allocator;

    ZETA_DebugAssert(seg_allocator != NULL);
    ZETA_DebugAssert(seg_allocator->Deallocate != NULL);

    seg_allocator->Deallocate(seg_allocator->context, node->seg);
    node_allocator->Deallocate(node_allocator->context, node);
}

void* Zeta_SegList_PushL(void* sl_) {
    Zeta_SegList* sl = (Zeta_SegList*)sl_;
    ZETA_DebugAssert(sl != NULL);

    Zeta_SegList_Node* node;

    if (sl->offset == 0) {
        node = AllocateNode_(sl);
        Zeta_OrdLinkedListNode_InsertR(&sl->head, &node->n);
        sl->offset = sl->seg_capacity - 1;
    } else {
        node = ZETA_GetStructFromMember(Zeta_SegList_Node, n,
                                        Zeta_OrdLinkedListNode_GetR(&sl->head));
        --sl->offset;
    }

    ++sl->size;

    return (unsigned char*)(node->seg) + sl->width * sl->offset;
}

void* Zeta_SegList_PushR(void* sl_) {
    Zeta_SegList* sl = (Zeta_SegList*)sl_;
    ZETA_DebugAssert(sl != NULL);

    size_t i = (sl->offset + sl->size) % sl->seg_capacity;

    Zeta_SegList_Node* node;

    if (i == 0) {
        node = AllocateNode_(sl);
        Zeta_OrdLinkedListNode_InsertL(&sl->head, &node->n);
    } else {
        node = ZETA_GetStructFromMember(Zeta_SegList_Node, n,
                                        Zeta_OrdLinkedListNode_GetL(&sl->head));
    }

    ++sl->size;

    return (unsigned char*)(node->seg) + sl->width * i;
}

void Zeta_SegList_PopL(void* sl_) {
    Zeta_SegList* sl = (Zeta_SegList*)sl_;
    ZETA_DebugAssert(sl != NULL);

    ZETA_DebugAssert(0 < sl->size);

    if (sl->offset < sl->seg_capacity - 1 && 1 < sl->size) {
        ++sl->offset;
        --sl->size;
        return;
    }

    Zeta_SegList_Node* node = ZETA_GetStructFromMember(
        Zeta_SegList_Node, n, Zeta_OrdLinkedListNode_GetR(&sl->head));

    Zeta_OrdLinkedListNode_Extract(&node->n);

    DeallocateNode_(sl, node);

    sl->offset = 0;
    --sl->size;
}

void Zeta_SegList_PopR(void* sl_) {
    Zeta_SegList* sl = (Zeta_SegList*)sl_;
    ZETA_DebugAssert(sl != NULL);

    ZETA_DebugAssert(0 < sl->size);

    size_t i = (sl->offset + sl->size - 1) % sl->seg_capacity;

    if (0 < i && 1 < sl->size) {
        --sl->size;
        return;
    }

    Zeta_SegList_Node* node = ZETA_GetStructFromMember(
        Zeta_SegList_Node, n, Zeta_OrdLinkedListNode_GetL(&sl->head));

    Zeta_OrdLinkedListNode_Extract(&node->n);

    DeallocateNode_(sl, node);

    --sl->size;

    if (sl->size == 0) { sl->offset = 0; }
}

void Zeta_SegList_Check(void* sl_, Zeta_DebugTreeMap* dst_node_tm,
                        Zeta_DebugTreeMap* dst_seg_tm) {
    Zeta_SegList* sl = (Zeta_SegList*)sl_;
    ZETA_DebugAssert(sl != NULL);

    size_t idx = sl->offset;

    Zeta_OrdLinkedListNode* n = &sl->head;

    for (;;) {
        n = Zeta_OrdLinkedListNode_GetR(n);
        if (n == &sl->head) { break; }

        Zeta_SegList_Node* node =
            ZETA_GetStructFromMember(Zeta_SegList_Node, n, n);

        {
            Zeta_DebugTreeMap_KeyValPair kvp = Zeta_DebugTreeMap_Insert(
                dst_node_tm, ZETA_GetAddrFromPtr(node));

            ZETA_DebugAssert(kvp.b);

            kvp.val = sizeof(Zeta_SegList_Node);
        }

        {
            Zeta_DebugTreeMap_KeyValPair kvp = Zeta_DebugTreeMap_Insert(
                dst_seg_tm, ZETA_GetAddrFromPtr(node->seg));

            ZETA_DebugAssert(kvp.b);

            *kvp.val = sizeof(sl->width) * sl->seg_capacity;
        }
    }
}

void Zeta_SegList_DeploySeqContainer(void* sl_, Zeta_SeqContainer* dst) {
    Zeta_SegList* sl = (Zeta_SegList*)sl_;
    ZETA_DebugAssert(sl != NULL);

    ZETA_DebugAssert(dst != NULL);

    dst->context = sl;

    dst->GetWidth = Zeta_SegList_GetWidth;
    dst->GetSize = Zeta_SegList_GetSize;

    dst->PeekL = Zeta_SegList_PeekL;
    dst->PeekR = Zeta_SegList_PeekR;

    dst->PushL = Zeta_SegList_PushL;
    dst->PushR = Zeta_SegList_PushR;

    dst->PopL = Zeta_SegList_PopL;
    dst->PopR = Zeta_SegList_PopR;
}
