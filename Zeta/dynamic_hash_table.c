#include "dynamic_hash_table.h"

#include "debugger.h"
#include "utils.h"

#if ZETA_EnableDebug

#define CheckCntr_(dht) Zeta_DynamicHashTable_Check((dht))

#define CheckCursor_(dht, cursor) \
    Zeta_DynamicHashTable_Cursor_Check((dht), (cursor))

#else

#define CheckCntr_(dht) ZETA_Unused((dht))

#define CheckCursor_(dht, cursor) \
    {                             \
        ZETA_Unused((dht));       \
        ZETA_Unused((cursor));    \
    }                             \
    ZETA_StaticAssert(TRUE)

#endif

static unsigned long long GHTNodeHash_(void const* dht_, void const* node,
                                       unsigned long long salt) {
    Zeta_DynamicHashTable const* dht = dht_;

    void const* elem =
        ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, htn, node)->data;

    return dht->ElemHash(dht->elem_hash_context, elem, salt);
}

static int GHTNodeCompare_(void const* dht_, void const* node_a,
                           void const* node_b) {
    Zeta_DynamicHashTable const* dht = dht_;

    void const* elem_a =
        ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, htn, node_a)->data;

    void const* elem_b =
        ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, htn, node_b)->data;

    return dht->ElemCompare(dht->elem_cmp_context, elem_a, elem_b);
}

ZETA_DeclareStruct(GHTKeyNodeCompareContext);

struct GHTKeyNodeCompareContext {
    Zeta_Compare KeyElemCompare;
    void const* key_elem_cmp_context;
};

static int GHTKeyNodeCompare_(void const* context_, void const* key,
                              void const* node) {
    GHTKeyNodeCompareContext const* context = context_;

    void const* elem =
        ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, htn, node)->data;

    return context->KeyElemCompare(context->key_elem_cmp_context, key, elem);
}

void Zeta_DynamicHashTable_Init(void* dht_) {
    Zeta_DynamicHashTable* dht = dht_;
    ZETA_DebugAssert(dht != NULL);

    ZETA_DebugAssert(0 < dht->width);

    dht->width = (dht->width + alignof(Zeta_DynamicHashTable_Node) - 1) /
                 alignof(Zeta_DynamicHashTable_Node) *
                 alignof(Zeta_DynamicHashTable_Node);

    ZETA_DebugAssert(dht->ElemHash != NULL);
    ZETA_DebugAssert(dht->ElemCompare != NULL);

    ZETA_Allocator_Check(dht->node_allocator,
                         alignof(Zeta_DynamicHashTable_Node));

    dht->lln = ZETA_Allocator_SafeAllocate(dht->node_allocator,
                                           alignof(Zeta_OrdLListNode),
                                           sizeof(Zeta_OrdLListNode));

    Zeta_OrdLListNode_Init(dht->lln);

    dht->ght.NodeHash = GHTNodeHash_;
    dht->ght.node_hash_context = dht;

    dht->ght.NodeCompare = GHTNodeCompare_;
    dht->ght.node_cmp_context = dht;

    Zeta_GenericHashTable_Init(&dht->ght);
}

void Zeta_DynamicHashTable_Deinit(void* dht_) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckCntr_(dht);

    Zeta_DynamicHashTable_EraseAll(dht);

    Zeta_GenericHashTable_Deinit(&dht->ght);

    ZETA_Allocator_Deallocate(dht->node_allocator, dht->lln);
}

size_t Zeta_DynamicHashTable_GetWidth(void const* dht_) {
    Zeta_DynamicHashTable const* dht = dht_;
    CheckCntr_(dht);

    return dht->width;
}

size_t Zeta_DynamicHashTable_GetSize(void const* dht_) {
    Zeta_DynamicHashTable const* dht = dht_;
    CheckCntr_(dht);

    return Zeta_GenericHashTable_GetSize(&dht->ght);
}

size_t Zeta_DynamicHashTable_GetCapacity(void const* dht_) {
    Zeta_DynamicHashTable const* dht = dht_;
    CheckCntr_(dht);

    return ZETA_max_capacity;
}

void Zeta_DynamicHashTable_GetLBCursor(void const* dht_, void* dst_cursor_) {
    Zeta_DynamicHashTable const* dht = dht_;
    CheckCntr_(dht);

    Zeta_DynamicHashTable_Cursor* dst_cursor = dst_cursor_;

    if (dst_cursor == NULL) { return; }

    dst_cursor->dht = dht;
    dst_cursor->lln = dht->lln;
}

void Zeta_DynamicHashTable_GetRBCursor(void const* dht_, void* dst_cursor_) {
    Zeta_DynamicHashTable const* dht = dht_;
    CheckCntr_(dht);

    Zeta_DynamicHashTable_Cursor* dst_cursor = dst_cursor_;

    if (dst_cursor == NULL) { return; }

    dst_cursor->dht = dht;
    dst_cursor->lln = dht->lln;
}

void* Zeta_DynamicHashTable_PeekL(void* dht_, void* dst_cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckCntr_(dht);

    Zeta_DynamicHashTable_Cursor* dst_cursor = dst_cursor_;

    Zeta_DynamicHashTable_Cursor cursor;
    cursor.dht = dht;
    cursor.lln = Zeta_OrdLListNode_GetR(dht->lln);

    if (dst_cursor != NULL) {
        dst_cursor->dht = cursor.dht;
        dst_cursor->lln = cursor.lln;
    }

    return dht->lln == cursor.lln
               ? NULL
               : ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, lln,
                                     cursor.lln)
                     ->data;
}

void* Zeta_DynamicHashTable_PeekR(void* dht_, void* dst_cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckCntr_(dht);

    Zeta_DynamicHashTable_Cursor* dst_cursor = dst_cursor_;

    Zeta_DynamicHashTable_Cursor cursor;
    cursor.dht = dht;
    cursor.lln = Zeta_OrdLListNode_GetL(dht->lln);

    if (dst_cursor != NULL) {
        dst_cursor->dht = cursor.dht;
        dst_cursor->lln = cursor.lln;
    }

    return dht->lln == cursor.lln
               ? NULL
               : ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, lln,
                                     cursor.lln)
                     ->data;
}

void* Zeta_DynamicHashTable_Refer(void* dht_, void const* pos_cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    Zeta_DynamicHashTable_Cursor const* pos_cursor = pos_cursor_;

    CheckCursor_(dht, pos_cursor);

    return dht->lln == pos_cursor->lln
               ? NULL
               : ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, lln,
                                     pos_cursor->lln)
                     ->data;
}

void* Zeta_DynamicHashTable_Find(void* dht_, void const* key, Zeta_Hash KeyHash,
                                 void const* key_hash_context,
                                 Zeta_Compare KeyElemCompare,
                                 void const* key_elem_cmp_context,
                                 void* dst_cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckCntr_(dht);

    Zeta_DynamicHashTable_Cursor* dst_cursor = dst_cursor_;

    ZETA_DebugAssert(key != NULL);

    GHTKeyNodeCompareContext ght_key_node_cmp_context = {
        .KeyElemCompare = KeyElemCompare,
        .key_elem_cmp_context = key_elem_cmp_context,
    };

    void* htn = Zeta_GenericHashTable_Find(&dht->ght, key, KeyHash,
                                           key_hash_context, GHTKeyNodeCompare_,
                                           &ght_key_node_cmp_context);

    if (htn == NULL) {
        if (dst_cursor != NULL) {
            dst_cursor->dht = dht;
            dst_cursor->lln = dht->lln;
        }

        return NULL;
    }

    Zeta_DynamicHashTable_Node* node =
        ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, htn, htn);

    if (dst_cursor != NULL) {
        dst_cursor->dht = dht;
        dst_cursor->lln = &node->lln;
    }

    return node->data;
}

void* Zeta_DynamicHashTable_Insert(void* dht_, void const* elem,
                                   void* dst_cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckCntr_(dht);

    Zeta_DynamicHashTable_Cursor* dst_cursor = dst_cursor_;

    ZETA_DebugAssert(elem != NULL);

    Zeta_DynamicHashTable_Node* node = (void*)(ZETA_Allocator_SafeAllocate(
        dht->node_allocator, alignof(Zeta_DynamicHashTable_Node*),
        offsetof(Zeta_DynamicHashTable_Node, data[dht->width])));

    Zeta_OrdLListNode_Init(&node->lln);

    Zeta_GenericHashTable_Node_Init(&node->htn);

    Zeta_MemCopy(node->data, elem, dht->width);

    Zeta_GenericHashTable_Insert(&dht->ght, &node->htn);

    Zeta_LList_OrdLListNode_InsertL(dht->lln, &node->lln);

    if (dst_cursor != NULL) {
        dst_cursor->dht = dht;
        dst_cursor->lln = &node->lln;
    }

    return node->data;
}

void Zeta_DynamicHashTable_Erase(void* dht_, void* pos_cursor_) {
    Zeta_DynamicHashTable* dht = dht_;
    Zeta_DynamicHashTable_Cursor* pos_cursor = pos_cursor_;

    CheckCursor_(dht, pos_cursor);

    ZETA_DebugAssert(dht->lln != pos_cursor->lln);

    Zeta_OrdLListNode* lln = pos_cursor->lln;

    pos_cursor->lln = Zeta_OrdLListNode_GetR(pos_cursor->lln);

    Zeta_DynamicHashTable_Node* node =
        ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, lln, lln);

    Zeta_LList_OrdLListNode_Extract(&node->lln);

    Zeta_GenericHashTable_Extract(&dht->ght, &node->htn);

    ZETA_Allocator_Deallocate(dht->node_allocator, node);
}

void Zeta_DynamicHashTable_EraseAll(void* dht_) {
    Zeta_DynamicHashTable* dht = dht_;
    ZETA_DebugAssert(dht);

    for (;;) {
        Zeta_OrdLListNode* nxt_lln = Zeta_OrdLListNode_GetR(dht->lln);
        if (nxt_lln == dht->lln) { break; }

        Zeta_DynamicHashTable_Node* nxt_node =
            ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, lln, nxt_lln);

        Zeta_LList_OrdLListNode_Extract(nxt_lln);

        Zeta_GenericHashTable_Extract(&dht->ght, &nxt_node->htn);

        ZETA_Allocator_Deallocate(dht->node_allocator, nxt_node);
    }
}

unsigned long long Zeta_DynamicHashTable_GetEffFactor(void* dht_) {
    Zeta_DynamicHashTable* dht = dht_;
    ZETA_DebugAssert(dht);

    return Zeta_GenericHashTable_GetEffFactor(&dht->ght);
}

void Zeta_DynamicHashTable_Check(void const* dht_) {
    Zeta_DynamicHashTable const* dht = dht_;
    ZETA_DebugAssert(dht != NULL);

    size_t width = dht->width;

    ZETA_DebugAssert(0 < width);

    ZETA_DebugAssert(width % alignof(Zeta_DynamicHashTable_Node) == 0);

    ZETA_DebugAssert(dht->ElemHash != NULL);
    ZETA_DebugAssert(dht->ElemCompare != NULL);

    ZETA_Allocator_Check(dht->node_allocator,
                         alignof(Zeta_DynamicHashTable_Node));

    Zeta_GenericHashTable_Check(&dht->ght);
}

void Zeta_DynamicHashTable_Sanitize(void* dht_, Zeta_MemRecorder* dst_table,
                                    Zeta_MemRecorder* dst_node) {
    Zeta_DynamicHashTable* dht = dht_;
    CheckCntr_(dht);

    Zeta_MemRecorder* htn_records = Zeta_MemRecorder_Create();

    Zeta_GenericHashTable_Sanitize(&dht->ght, dst_table, htn_records);

    Zeta_MemRecorder_Record(dst_node, dht->lln, sizeof(Zeta_OrdLListNode));

    size_t node_size = offsetof(Zeta_DynamicHashTable_Node, data[dht->width]);

    for (Zeta_OrdLListNode* lln = dht->lln;;) {
        lln = Zeta_OrdLListNode_GetR(lln);
        if (lln == dht->lln) { break; }

        Zeta_DynamicHashTable_Node* node =
            ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, lln, lln);

        Zeta_MemRecorder_Record(dst_node, node, node_size);

        ZETA_DebugAssert(Zeta_MemRecorder_Unrecord(htn_records, &node->htn));
    }

    ZETA_DebugAssert(Zeta_MemRecorder_GetSize(htn_records) == 0);

    Zeta_MemRecorder_Destroy(htn_records);
}

bool_t Zeta_DynamicHashTable_Cursor_AreEqual(void const* dht_,
                                             void const* cursor_a_,
                                             void const* cursor_b_) {
    Zeta_DynamicHashTable const* dht = dht_;

    Zeta_DynamicHashTable_Cursor const* cursor_a = cursor_a_;
    Zeta_DynamicHashTable_Cursor const* cursor_b = cursor_b_;

    CheckCursor_(dht, cursor_a);
    CheckCursor_(dht, cursor_b);

    return cursor_a->lln == cursor_b->lln;
}

void Zeta_DynamicHashTable_Cursor_StepL(void const* dht_, void* cursor_) {
    Zeta_DynamicHashTable const* dht = dht_;
    Zeta_DynamicHashTable_Cursor* cursor = cursor_;

    CheckCursor_(dht, cursor);

    cursor->lln = Zeta_OrdLListNode_GetL(cursor->lln);
}

void Zeta_DynamicHashTable_Cursor_StepR(void const* dht_, void* cursor_) {
    Zeta_DynamicHashTable const* dht = dht_;
    Zeta_DynamicHashTable_Cursor* cursor = cursor_;

    CheckCursor_(dht, cursor);

    cursor->lln = Zeta_OrdLListNode_GetR(cursor->lln);
}

void Zeta_DynamicHashTable_Cursor_Check(void const* dht_, void const* cursor_) {
    Zeta_DynamicHashTable const* dht = dht_;
    CheckCntr_(dht);

    Zeta_DynamicHashTable_Cursor const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    ZETA_DebugAssert(cursor->dht == dht);

    if (dht->lln == cursor->lln) { return; }

    ZETA_DebugAssert(Zeta_GenericHashTable_Contain(
        &dht->ght,
        &ZETA_MemberToStruct(Zeta_DynamicHashTable_Node, lln, cursor->lln)
             ->htn));
}

Zeta_AssocCntr_VTable const zeta_dynamic_hash_table_assoc_cntr_vtable = {
    .cursor_size = sizeof(Zeta_DynamicHashTable_Cursor),

    .Deinit = Zeta_DynamicHashTable_Deinit,

    .GetWidth = Zeta_DynamicHashTable_GetWidth,

    .GetSize = Zeta_DynamicHashTable_GetSize,

    .GetCapacity = Zeta_DynamicHashTable_GetCapacity,

    .GetLBCursor = Zeta_DynamicHashTable_GetLBCursor,

    .GetRBCursor = Zeta_DynamicHashTable_GetRBCursor,

    .PeekL = Zeta_DynamicHashTable_PeekL,

    .PeekR = Zeta_DynamicHashTable_PeekR,

    .Refer = Zeta_DynamicHashTable_Refer,

    .Find = Zeta_DynamicHashTable_Find,

    .Insert = Zeta_DynamicHashTable_Insert,

    .Erase = Zeta_DynamicHashTable_Erase,

    .EraseAll = Zeta_DynamicHashTable_EraseAll,

    .Cursor_AreEqual = Zeta_DynamicHashTable_Cursor_AreEqual,

    .Cursor_StepL = Zeta_DynamicHashTable_Cursor_StepL,

    .Cursor_StepR = Zeta_DynamicHashTable_Cursor_StepR,
};
