#include "DynamicHashTable.h"

#include "Debugger.h"
#include "utils.h"

#if ZETA_IsDebug

#define CheckDHT_(dht) Zeta_DynamicHashTable_Check(dht)

#define CheckDHTCursor_(dht, cursor) \
    Zeta_DynamicHashTable_Cursor_Check(dht, cursor)

#else

#define CheckDHT_(dht)

#define CheckDHTCursor_(dht, cursor)

#endif

static int GHTNodeCompare_(void const* dht_, void const* htn_a,
                           void const* htn_b) {
    Zeta_DynamicHashTable const* dht = dht_;

    void const* elem_a = (unsigned char*)ZETA_MemberToStruct(
                             Zeta_DynamicHashTable_Node, htn, htn_a) -
                         dht->stride;

    void const* elem_b = (unsigned char*)ZETA_MemberToStruct(
                             Zeta_DynamicHashTable_Node, htn, htn_b) -
                         dht->stride;

    return dht->ElemCompare(dht->elem_cmp_context, elem_a, elem_b);
}

static int GHTNodeKeyCompare_(void const* dht_, void const* htn,
                              void const* key) {
    Zeta_DynamicHashTable const* dht = dht_;

    void const* elem = (unsigned char*)ZETA_MemberToStruct(
                           Zeta_DynamicHashTable_Node, htn, htn) -
                       dht->stride;

    return dht->ElemKeyCompare(dht->elem_key_cmp_context, elem, key);
}

void Zeta_DynamicHashTable_Init(void* dht_) {
    Zeta_DynamicHashTable* dht = dht_;
    ZETA_DebugAssert(dht != NULL);

    size_t width = dht->width;
    size_t stride = dht->stride;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    dht->stride = stride = (stride + alignof(Zeta_DynamicHashTable_Node) - 1) /
                           alignof(Zeta_DynamicHashTable_Node) *
                           alignof(Zeta_DynamicHashTable_Node);

    ZETA_DebugAssert(dht->KeyHash != NULL);
    ZETA_DebugAssert(dht->ElemHash != NULL);

    ZETA_DebugAssert(dht->ElemCompare != NULL);
    ZETA_DebugAssert(dht->ElemKeyCompare != NULL);

    ZETA_DebugAssert(dht->node_allocator != NULL);
    ZETA_DebugAssert(dht->node_allocator->GetAlign != NULL);
    ZETA_DebugAssert(dht->node_allocator->Allocate != NULL);
    ZETA_DebugAssert(dht->node_allocator->Deallocate != NULL);
    ZETA_DebugAssert(ZETA_Allocator_GetAlign(dht->node_allocator) %
                         alignof(Zeta_DynamicHashTable_Node) ==
                     0);

    dht->lln = ZETA_Allocator_SafeAllocate(dht->node_allocator,
                                           alignof(Zeta_OrdLinkedListNode),
                                           sizeof(Zeta_OrdLinkedListNode));

    Zeta_OrdLinkedListNode_Init(dht->lln);

    dht->ght.node_cmp_context = dht;
    dht->ght.NodeCompare = GHTNodeCompare_;

    dht->ght.node_key_cmp_context = dht;
    dht->ght.NodeKeyCompare = GHTNodeKeyCompare_;

    Zeta_GenericHashTable_Init(&dht->ght);
}

void Zeta_DynamicHashTable_Deinit(void* dht_) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckDHT_(dht);

    Zeta_GenericHashTable_Deinit(&dht->ght);

    for (;;) {
        Zeta_OrdLinkedListNode* nxt_lln = Zeta_OrdLinkedListNode_GetR(dht->lln);
        if (nxt_lln == dht->lln) { break; }

        Zeta_DynamicHashTable_Node* nxt_node =
            ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, lln, nxt_lln);

        Zeta_OrdLinkedListNode_Extract(nxt_lln);

        ZETA_Allocator_Deallocate(dht->node_allocator,
                                  (unsigned char*)nxt_node - dht->stride);
    }

    ZETA_Allocator_Deallocate(dht->node_allocator, dht->lln);
}

size_t Zeta_DynamicHashTable_GetWidth(void* dht_) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckDHT_(dht);

    return dht->width;
}

size_t Zeta_DynamicHashTable_GetStride(void* dht_) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckDHT_(dht);

    return dht->stride;
}

size_t Zeta_DynamicHashTable_GetSize(void* dht_) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckDHT_(dht);

    return Zeta_GenericHashTable_GetSize(&dht->ght);
}

size_t Zeta_DynamicHashTable_GetCapacity(void* dht_) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckDHT_(dht);

    return ZETA_RangeMaxOf(size_t);
}

void Zeta_DynamicHashTable_GetLBCursor(void* dht_, void* dst_cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckDHT_(dht);

    Zeta_DynamicHashTable_Cursor* dst_cursor = dst_cursor_;
    ZETA_DebugAssert(dst_cursor != NULL);

    dst_cursor->dht = dht;
    dst_cursor->lln = dht->lln;
}

void Zeta_DynamicHashTable_GetRBCursor(void* dht_, void* dst_cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckDHT_(dht);

    Zeta_DynamicHashTable_Cursor* dst_cursor = dst_cursor_;
    ZETA_DebugAssert(dst_cursor != NULL);

    dst_cursor->dht = dht;
    dst_cursor->lln = dht->lln;
}

void* Zeta_DynamicHashTable_PeekL(void* dht_, void* dst_cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckDHT_(dht);

    Zeta_DynamicHashTable_Cursor* dst_cursor = dst_cursor_;

    Zeta_DynamicHashTable_Cursor cursor;
    cursor.dht = dht;
    cursor.lln = Zeta_OrdLinkedListNode_GetR(dht->lln);

    if (dst_cursor != NULL) {
        dst_cursor->dht = cursor.dht;
        dst_cursor->lln = cursor.lln;
    }

    if (dht->lln == cursor.lln) { return NULL; }

    return (unsigned char*)ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, lln,
                                               cursor.lln) -
           dht->stride;
}

void* Zeta_DynamicHashTable_PeekR(void* dht_, void* dst_cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckDHT_(dht);

    Zeta_DynamicHashTable_Cursor* dst_cursor = dst_cursor_;

    Zeta_DynamicHashTable_Cursor cursor;
    cursor.dht = dht;
    cursor.lln = Zeta_OrdLinkedListNode_GetL(dht->lln);

    if (dst_cursor != NULL) {
        dst_cursor->dht = cursor.dht;
        dst_cursor->lln = cursor.lln;
    }

    return dht->lln == cursor.lln
               ? NULL
               : (unsigned char*)ZETA_MemberToStruct(Zeta_DynamicHashTable_Node,
                                                     lln, cursor.lln) -
                     dht->stride;
}

void* Zeta_DynamicHashTable_Refer(void* dht_, void const* pos_cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    Zeta_DynamicHashTable_Cursor const* pos_cursor = pos_cursor_;

    CheckDHTCursor_(dht, pos_cursor);

    return dht->lln == pos_cursor->lln
               ? NULL
               : (unsigned char*)ZETA_MemberToStruct(Zeta_DynamicHashTable_Node,
                                                     lln, pos_cursor->lln) -
                     dht->stride;
}

void* Zeta_DynamicHashTable_Find(void* dht_, void* dst_cursor_,
                                 void const* key) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckDHT_(dht);

    Zeta_DynamicHashTable_Cursor* dst_cursor = dst_cursor_;

    ZETA_DebugAssert(key != NULL);

    unsigned long long hash_code = dht->KeyHash(dht->key_hash_context, key);

    void* htn = Zeta_GenericHashTable_Find(&dht->ght, key, hash_code);

    if (htn == NULL) {
        if (dst_cursor == NULL) { return NULL; }

        dst_cursor->dht = dht;
        dst_cursor->lln = dht->lln;

        return NULL;
    }

    Zeta_DynamicHashTable_Node* node =
        ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, htn, htn);

    if (dst_cursor != NULL) {
        dst_cursor->dht = dht;
        dst_cursor->lln = &node->lln;
    }

    return (unsigned char*)node - dht->stride;
}

void* Zeta_DynamicHashTable_Insert(void* dht_, void* dst_cursor_,
                                   void const* elem) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckDHT_(dht);

    Zeta_DynamicHashTable_Cursor* dst_cursor = dst_cursor_;

    ZETA_DebugAssert(elem != NULL);

    Zeta_DynamicHashTable_Node* node =
        (void*)((unsigned char*)ZETA_Allocator_SafeAllocate(
                    dht->node_allocator, alignof(Zeta_DynamicHashTable_Node*),
                    dht->stride + sizeof(Zeta_DynamicHashTable_Node)) +
                dht->stride);

    Zeta_OrdLinkedListNode_Init(&node->lln);

    Zeta_OrdRBTreeNode_Init(NULL, &node->htn.n);

    node->htn.hash_code = dht->ElemHash(dht->elem_hash_context, elem);

    Zeta_MemCopy((unsigned char*)node - dht->stride, elem, dht->width);

    Zeta_GenericHashTable_Insert(&dht->ght, &node->htn);

    Zeta_OrdLinkedListNode_InsertL(dht->lln, &node->lln);

    if (dst_cursor != NULL) {
        dst_cursor->dht = dht;
        dst_cursor->lln = &node->lln;
    }

    return (unsigned char*)node - dht->stride;
}

void Zeta_DynamicHashTable_Erase(void* dht_, void* pos_cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    Zeta_DynamicHashTable_Cursor* pos_cursor = pos_cursor_;

    CheckDHTCursor_(dht, pos_cursor);

    ZETA_DebugAssert(dht->lln != pos_cursor->lln);

    Zeta_OrdLinkedListNode* lln = pos_cursor->lln;

    pos_cursor->lln = Zeta_OrdLinkedListNode_GetR(pos_cursor->lln);

    Zeta_DynamicHashTable_Node* node =
        ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, lln, lln);

    Zeta_OrdLinkedListNode_Extract(&node->lln);

    Zeta_GenericHashTable_Erase(&dht->ght, &node->htn);

    ZETA_Allocator_Deallocate(dht->node_allocator,
                              (unsigned char*)node - dht->stride);
}

void Zeta_DynamicHashTable_EraseAll(void* dht_) {
    Zeta_DynamicHashTable* dht = dht_;
    ZETA_DebugAssert(dht);

    for (;;) {
        Zeta_OrdLinkedListNode* nxt_lln = Zeta_OrdLinkedListNode_GetR(dht->lln);
        if (nxt_lln == dht->lln) { break; }

        Zeta_DynamicHashTable_Node* nxt_node =
            ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, lln, nxt_lln);

        Zeta_GenericHashTable_Erase(&dht->ght, &nxt_node->htn);
        Zeta_OrdLinkedListNode_Extract(nxt_lln);

        ZETA_Allocator_Deallocate(dht->node_allocator,
                                  (unsigned char*)nxt_node - dht->stride);
    }
}

unsigned long long Zeta_DynamicHashTable_GetEffFactor(void* dht_) {
    Zeta_DynamicHashTable* dht = dht_;
    ZETA_DebugAssert(dht);

    return Zeta_GenericHashTable_GetEffFactor(&dht->ght);
}

void Zeta_DynamicHashTable_Check(void* dht_) {
    Zeta_DynamicHashTable* dht = dht_;
    ZETA_DebugAssert(dht != NULL);

    size_t width = dht->width;
    size_t stride = dht->stride;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    ZETA_DebugAssert(stride % alignof(Zeta_DynamicHashTable_Node) == 0);

    ZETA_DebugAssert(dht->KeyHash != NULL);
    ZETA_DebugAssert(dht->ElemHash != NULL);

    ZETA_DebugAssert(dht->ElemCompare != NULL);
    ZETA_DebugAssert(dht->ElemKeyCompare != NULL);

    ZETA_DebugAssert(dht->node_allocator != NULL);
    ZETA_DebugAssert(dht->node_allocator->GetAlign != NULL);
    ZETA_DebugAssert(dht->node_allocator->Allocate != NULL);
    ZETA_DebugAssert(dht->node_allocator->Deallocate != NULL);
    ZETA_DebugAssert(ZETA_Allocator_GetAlign(dht->node_allocator) %
                         alignof(Zeta_DynamicHashTable_Node) ==
                     0);

    Zeta_GenericHashTable_Check(&dht->ght);
}

void Zeta_DynamicHashTable_Sanitize(void* dht_, Zeta_MemRecorder* dst_table,
                                    Zeta_MemRecorder* dst_node) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckDHT_(dht);

    Zeta_MemRecorder* htn_records = Zeta_MemRecorder_Create();

    Zeta_GenericHashTable_Sanitize(&dht->ght, dst_table, htn_records);

    Zeta_MemRecorder_Record(dst_node, dht->lln, sizeof(Zeta_OrdLinkedListNode));

    for (Zeta_OrdLinkedListNode* lln = dht->lln;;) {
        lln = Zeta_OrdLinkedListNode_GetR(lln);
        if (lln == dht->lln) { break; }

        Zeta_DynamicHashTable_Node* node =
            ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, lln, lln);

        Zeta_MemRecorder_Record(
            dst_node, (unsigned char*)node - dht->stride,
            dht->stride + sizeof(Zeta_DynamicHashTable_Node));

        ZETA_DebugAssert(Zeta_MemRecorder_Unrecord(htn_records, &node->htn));
    }

    ZETA_DebugAssert(Zeta_MemRecorder_GetSize(htn_records) == 0);

    Zeta_MemRecorder_Destroy(htn_records);
}

bool_t Zeta_DynamicHashTable_Cursor_IsEqual(void* dht_, void const* cursor_a_,
                                            void const* cursor_b_) {
    Zeta_DynamicHashTable* dht = dht_;

    Zeta_DynamicHashTable_Cursor const* cursor_a = cursor_a_;
    Zeta_DynamicHashTable_Cursor const* cursor_b = cursor_b_;

    CheckDHTCursor_(dht, cursor_a);
    CheckDHTCursor_(dht, cursor_b);

    return cursor_a->lln == cursor_b->lln;
}

void Zeta_DynamicHashTable_Cursor_StepL(void* dht_, void* cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    Zeta_DynamicHashTable_Cursor* cursor = cursor_;

    CheckDHTCursor_(dht, cursor);

    cursor->lln = Zeta_OrdLinkedListNode_GetL(cursor->lln);
}

void Zeta_DynamicHashTable_Cursor_StepR(void* dht_, void* cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    Zeta_DynamicHashTable_Cursor* cursor = cursor_;

    CheckDHTCursor_(dht, cursor);

    cursor->lln = Zeta_OrdLinkedListNode_GetR(cursor->lln);
}

void Zeta_DynamicHashTable_Cursor_Check(void* dht_, void const* cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckDHT_(dht);

    Zeta_DynamicHashTable_Cursor const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    ZETA_DebugAssert(cursor->dht == dht);

    if (dht->lln == cursor->lln) { return; }

    Zeta_GenericHashTable_CheckNode(
        &dht->ght,
        &ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, lln, cursor->lln)
             ->htn);
}

void Zeta_DynamicHashTable_DeployAssocContainer(
    void* dht_, Zeta_AssocContainer* assoc_cntr) {
    Zeta_DynamicHashTable* dht = dht_;
    ZETA_DebugAssert(dht != NULL);

    Zeta_AssocContainer_Init(assoc_cntr);

    assoc_cntr->context = dht;

    assoc_cntr->cursor_size = sizeof(Zeta_DynamicHashTable_Cursor);

    assoc_cntr->GetWidth = Zeta_DynamicHashTable_GetWidth;

    assoc_cntr->GetStride = Zeta_DynamicHashTable_GetStride;

    assoc_cntr->GetSize = Zeta_DynamicHashTable_GetSize;

    assoc_cntr->GetCapacity = Zeta_DynamicHashTable_GetCapacity;

    assoc_cntr->GetLBCursor = Zeta_DynamicHashTable_GetLBCursor;

    assoc_cntr->GetRBCursor = Zeta_DynamicHashTable_GetRBCursor;

    assoc_cntr->PeekL = Zeta_DynamicHashTable_PeekL;

    assoc_cntr->PeekR = Zeta_DynamicHashTable_PeekR;

    assoc_cntr->Refer = Zeta_DynamicHashTable_Refer;

    assoc_cntr->Find = Zeta_DynamicHashTable_Find;

    assoc_cntr->Insert = Zeta_DynamicHashTable_Insert;

    assoc_cntr->Erase = Zeta_DynamicHashTable_Erase;

    assoc_cntr->EraseAll = Zeta_DynamicHashTable_EraseAll;

    assoc_cntr->Cursor_IsEqual = Zeta_DynamicHashTable_Cursor_IsEqual;

    assoc_cntr->Cursor_StepL = Zeta_DynamicHashTable_Cursor_StepL;

    assoc_cntr->Cursor_StepR = Zeta_DynamicHashTable_Cursor_StepR;
}

ZETA_ExternC_End;
