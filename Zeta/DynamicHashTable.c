#include "DynamicHashTable.h"

#include "Debugger.h"

#if ZETA_IsDebug

#define CheckDHT_(dht) Zeta_DynamicHashTable_Check

#define CheckDHTCursor_(dht) Zeta_DynamicHashTable_Cursor_Check

#else

#define CheckDHT_(dht)

#define CheckDHTCursor_(dht)

#endif

static void* AllocateNode_(Zeta_DynamicHashTable_Node* dht) {
    //
}

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

    ZETA_DebugAssert(dht->HashKey != NULL);
    ZETA_DebugAssert(dht->HashElem != NULL);

    ZETA_DebugAssert(dht->node_allocator != NULL);
    ZETA_DebugAssert(dht->node_allocator->GetAlign != NULL);
    ZETA_DebugAssert(dht->node_allocator->Allocate != NULL);
    ZETA_DebugAssert(dht->node_allocator->Deallocate != NULL);
    ZETA_DebugAssert(
        dht->node_allocator->GetAlign(dht->node_allocator->context) %
            alignof(Zeta_DynamicHashTable_Node) ==
        0);

    dht->lln = ZETA_Allocator_SafeAllocate(dht->node_allocator,
                                           alignof(Zeta_OrdLinkedListNode),
                                           sizeof(Zeta_OrdLinkedListNode));

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
            ZETA_MemberToStruct(Zeta_DynamicHashTable, lln, nxt_lln);

        Zeta_OrdLinkedListNode_Extract(nxt_lln);

        dht->node_allocator->Deallocate(dht->node_allocator->context,
                                        (unsigned char*)nxt_node - dht->stride);
    }

    dht->node_allocator->Deallocate(dht->node_allocator->context, dht->lln);
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

void* Zeta_DynamicHashTable_Refer(void* dht_, void* pos_cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    Zeta_DynamicHashTable_Cursor* pos_cursor = pos_cursor_;

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
        dst_cursor->dht;
        dst_cursor->lln = &node->lln;
    }

    return (unsigned char*)node - dht->stride;
}

void* Zeta_DynamicHashTable_Insert(void* dht_, void* dst_cursor_,
                                   void const* key) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckDHT_(dht);

    Zeta_DynamicHashTable_Cursor* dst_cursor = dst_cursor_;

    ZETA_DebugAssert(key != NULL);

    unsigned long long hash_code = dht->KeyHash(dht->key_hash_context, key);

    Zeta_DynamicHashTable_Node* node =
        (unsigned char*)ZETA_Allocator_SafeAllocoate(
            dht->node_allocator, alignof(Zeta_DynamicHashTable_Node*),
            dht->stride + sizeof(Zeta_DynamicHashTable_Node)) +
        dht->stride;

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

    Zeta_DynamicHashTable_Node* lln = pos_cursor->lln;

    pos_cursor->lln = Zeta_OrdLinkedListNode_GetR(pos_cursor->lln);

    Zeta_DynamicHashTable_Node* node =
        ZETA_MemberToStruct(Zeta_DynamicHashTable, lln, lln);

    Zeta_GenericHashTable_Erase(&dht->ght, &node->htn);

    dht->node_allocator->Deallocate(dht->node_allocator->context,
                                    (unsigned char*)node - dht->stride);
}

void Zeta_DynamicHashTable_EraseAll(void* dht_) {
    Zeta_DynamicHashTable* dht = dht_;
    ZETA_DebugAssert(dht);

    for (;;) {
        Zeta_OrdLinkedListNode* nxt_lln = Zeta_OrdLinkedListNode_GetR(dht->lln);
        if (nxt_lln == dht->lln) { break; }

        Zeta_DynamicHashTable_Node* nxt_node =
            ZETA_MemberToStruct(Zeta_DynamicHashTable, lln, nxt_lln);

        Zeta_GenericHashTable_Erase(&dht->ght, &nxt_node->htn);
        Zeta_OrdLinkedListNode_Extract(nxt_lln);

        dht->node_allocator->Deallocate(dht->node_allocator->context,
                                        (unsigned char*)nxt_node - dht->stride);
    }
}

void Zeta_DynamicHashTable_Check(void* dht_) {
    Zeta_DynamicHashTable* dht = dht_;
    ZETA_DebugAssert(dht != NULL);

    size_t width = dht->width;
    size_t stride = dht->stride;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    ZETA_DebugAssert(stride % alignof(Zeta_DynamicHashTable_Node) == 0);

    ZETA_DebugAssert(dht->HashKey != NULL);
    ZETA_DebugAssert(dht->HashElem != NULL);

    ZETA_DebugAssert(dht->node_allocator != NULL);
    ZETA_DebugAssert(dht->node_allocator->GetAlign != NULL);
    ZETA_DebugAssert(dht->node_allocator->Allocate != NULL);
    ZETA_DebugAssert(dht->node_allocator->Deallocate != NULL);
    ZETA_DebugAssert(
        dht->node_allocator->GetAlign(dht->node_allocator->context) %
            alignof(Zeta_DynamicHashTable_Node) ==
        0);

    Zeta_GenericHashTable_Check(&dht->ght);
}

void Zeta_DynamicHashTable_Sanitize(void* dht_, Zeta_MemRecorder* dst_table,
                                    Zeta_MemRecorder* dst_node) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckDHT_(dht);

    //
}

void Zeta_DynamicHashTable_Cursor_IsEqual(void* dht_, void const* cursor_a_,
                                          void const* cursor_b_) {
    Zeta_DynamicHashTable* dht = dht_;

    Zeta_DynamicHashTable_Cursor* cursor_a = cursor_a_;
    Zeta_DynamicHashTable_Cursor* cursor_b = cursor_b_;

    CheckDHTCursor_(dht, cursor_a);
    CheckDHTCursor_(dht, cursor_b);
}

void Zeta_DynamicHashTable_Cursor_StepL(void* dht_, void const* cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    Zeta_DynamicHashTable_Cursor* cursor = cursor_;

    CheckDHTCursor_(dht, cursor);

    cursor->lln = Zeta_OrdLinkedList_GetL(cursor->lln);
}

void Zeta_DynamicHashTable_Cursor_StepR(void* dht_, void const* cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    Zeta_DynamicHashTable_Cursor* cursor = cursor_;

    CheckDHTCursor_(dht, cursor);

    cursor->lln = Zeta_OrdLinkedList_GetR(cursor->lln);
}

void Zeta_DynamicHashTable_Cursor_Check(void* dht_, void const* cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckDHT_(dht);

    Zeta_DynamicHashTable_Cursor* cursor = cursor_;

    ZETA_DebugAssert(cursor->dht == dht);
}

void Zeta_DynamicHashTable_Cursor_Check(void* dht_, void const* cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckDHT_(dht);

    Zeta_DynamicHashTable_Cursor const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    ZETA_DebugAssert(cursor->dht == dht);

    if (dht->lln != cursor->lln) {
        Zeta_GenericHashTable_CheckNode(
            &dht->ght,
            &ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, lln, cursor->lln)
                 ->htn);
    }
}

ZETA_ExternC_End;
