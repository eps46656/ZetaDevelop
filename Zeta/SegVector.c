#include "SegVector.h"

#include "BinTree.h"
#include "CircularVector.h"
#include "RBTree.h"
#include "utils.h"

void Zeta_SegVector_Init(void* sv_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    ZETA_DebugAssert(0 < sv->width);
    ZETA_DebugAssert(0 < sv->seg_capacity);

    ZETA_DebugAssert(sv->node_allocator != NULL);
    ZETA_DebugAssert(sv->node_allocator->Allocate != NULL);
    ZETA_DebugAssert(sv->node_allocator->Deallocate != NULL);

    ZETA_DebugAssert(sv->seg_allocator != NULL);
    ZETA_DebugAssert(sv->seg_allocator->Allocate != NULL);
    ZETA_DebugAssert(sv->seg_allocator->Deallocate != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_OrdCntRBTreeNode_Init(NULL, &sv->lb);
    Zeta_OrdCntRBTreeNode_Init(NULL, &sv->rb);

    Zeta_OrdCntRBTreeNode_SetAccSize(NULL, &sv->lb, 1);
    Zeta_OrdCntRBTreeNode_SetAccSize(NULL, &sv->rb, 1);

    void* root = NULL;

    root = Zeta_RBTree_InsertR(&btn_opr, root, &sv->lb);
    root = Zeta_RBTree_InsertR(&btn_opr, root, &sv->rb);

    sv->root = root;
}

size_t Zeta_SegVector_GetWidth(void* sv_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    return sv->width;
}

size_t Zeta_SegVector_GetSize(void* sv_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    return Zeta_OrdCntRBTreeNode_GetAccSize(NULL, sv->root) - 2;
}

void Zeta_SegVector_GetLB(void* sv_, void* dst_cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    if (dst_cursor == NULL) { return; }

    dst_cursor->idx = -1;
    dst_cursor->n = &sv->lb;
    dst_cursor->seg_idx = 0;
    dst_cursor->ele = NULL;
}

void Zeta_SegVector_GetRB(void* sv_, void* dst_cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    if (dst_cursor == NULL) { return; }

    dst_cursor->idx = Zeta_SegVector_GetSize(sv);
    dst_cursor->n = &sv->rb;
    dst_cursor->seg_idx = 0;
    dst_cursor->ele = NULL;
}

void* Zeta_SegVector_PeekL(void* sv_, void* dst_cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = sv->width;
    size_t seg_capacity = sv->seg_capacity;

    void* n = Zeta_BinTree_StepR(&btn_opr, &sv->lb);

    if (&sv->rb == n) {
        if (dst_cursor != NULL) {
            dst_cursor->idx = 0;
            dst_cursor->n = n;
            dst_cursor->seg_idx = 0;
            dst_cursor->ele = NULL;
        }

        return NULL;
    }

    Zeta_SegVector_Node* node =
        ZETA_GetStructFromMember(Zeta_SegVector_Node, n, n);

    Zeta_CircularVector cv;
    cv.width = width;
    cv.stride = width;
    cv.offset = node->offset;
    cv.size = Zeta_BinTree_GetSize(&btn_opr, n);
    cv.capacity = seg_capacity;
    cv.data = node->seg;

    void* ele = Zeta_CircularVector_PeekL(&cv, NULL);

    if (dst_cursor != NULL) {
        dst_cursor->idx = 0;
        dst_cursor->n = n;
        dst_cursor->seg_idx = 0;
        dst_cursor->ele = ele;
    }

    return ele;
}

void* Zeta_SegVector_PeekR(void* sv_, void* dst_cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = sv->width;
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_SegVector_GetSize(sv);

    void* n = Zeta_BinTree_StepL(&btn_opr, &sv->rb);

    if (&sv->lb == n) {
        if (dst_cursor != NULL) {
            dst_cursor->idx = -1;
            dst_cursor->n = n;
            dst_cursor->seg_idx = 0;
            dst_cursor->ele = NULL;
        }

        return NULL;
    }

    Zeta_SegVector_Node* node =
        ZETA_GetStructFromMember(Zeta_SegVector_Node, n, n);

    Zeta_CircularVector cv;
    cv.width = width;
    cv.stride = width;
    cv.offset = node->offset;
    cv.size = Zeta_BinTree_GetSize(&btn_opr, n);
    cv.capacity = seg_capacity;

    void* ele = Zeta_CircularVector_PeekR(&cv, NULL);

    if (dst_cursor != NULL) {
        dst_cursor->idx = size - 1;
        dst_cursor->n = n;
        dst_cursor->seg_idx = cv.size - 1;
        dst_cursor->ele = ele;
    }

    return ele;
}

void* Zeta_SegVector_Access(void* sv_, void* dst_cursor_, size_t idx) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = sv->width;
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_SegVector_GetSize(sv);

    ZETA_DebugAssert(idx + 1 < size + 2);

    void* m_n;
    Zeta_SegVector_Node* m_node;
    size_t seg_idx;

    Zeta_BinTree_AccessL(&m_n, &seg_idx, &btn_opr, sv->root, idx + 1);

    void* ele;

    if (idx < size) {
        m_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, m_n);

        Zeta_CircularVector m_cv;
        m_cv.width = width;
        m_cv.stride = width;
        m_cv.offset = m_node->offset;
        m_cv.size = Zeta_BinTree_GetSize(&btn_opr, m_n);
        m_cv.capacity = seg_capacity;
        m_cv.data = m_node->seg;

        ele = Zeta_CircularVector_Access(&m_cv, NULL, seg_idx);
    } else {
        ele = NULL;
    }

    if (dst_cursor != NULL) {
        dst_cursor->idx = idx;
        dst_cursor->n = m_n;
        dst_cursor->seg_idx = seg_idx;
        dst_cursor->ele = ele;
    }

    return ele;
}

void* Zeta_SegVector_FindFirst(void* sv_, void* context, void* dst_cursor_,
                               bool_t (*Compare)(void* context, void* ele)) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    ZETA_DebugAssert(Compare != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = sv->width;
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_SegVector_GetSize(sv);

    size_t base = -1;

    size_t ret_idx = size;
    void* ret_n = &sv->rb;
    size_t ret_seg_idx = 0;
    void* ret_ele = NULL;

    Zeta_CircularVector cv;
    cv.width = width;
    cv.stride = width;
    cv.capacity = seg_capacity;

    void* n = sv->root;

    while (n != NULL) {
        void* nl = Zeta_OrdCntRBTreeNode_GetL(NULL, n);
        void* nr = Zeta_OrdCntRBTreeNode_GetR(NULL, n);

        if (&sv->lb == n) {
            base += 1;
            n = nr;
            continue;
        }

        if (&sv->rb == n) {
            n = nl;
            continue;
        }

        Zeta_SegVector_Node* node =
            ZETA_GetStructFromMember(Zeta_SegVector_Node, n, n);

        cv.offset = node->offset;
        cv.size = Zeta_BinTree_GetSize(&btn_opr, n);

        void* first_ele = Zeta_CircularVector_PeekL(&cv, NULL);

        if (Compare(context, first_ele)) {
            ret_idx = base + Zeta_OrdCntRBTreeNode_GetAccSize(NULL, nl);
            ret_n = n;
            ret_seg_idx = 0;
            ret_ele = first_ele;

            n = nl;
            continue;
        }

        void* last_ele = Zeta_CircularVector_PeekR(&cv, NULL);

        if (!Compare(context, last_ele)) {
            base += Zeta_OrdCntRBTreeNode_GetAccSize(NULL, n) -
                    Zeta_OrdCntRBTreeNode_GetAccSize(
                        NULL, nr);  // nl_acc_size + n_size
            n = nr;
            continue;
        }

        size_t lb = 1;
        size_t rb = cv.size - 1;

        while (lb < rb) {
            size_t mb = (lb + rb) / 2;

            void* ele = Zeta_CircularVector_Access(&cv, NULL, mb);

            if (Compare(context, ele)) {
                rb = mb;
            } else {
                lb = mb + 1;
            }
        }

        ret_idx = base + Zeta_OrdCntRBTreeNode_GetAccSize(NULL, nl) + lb;
        ret_n = n;
        ret_seg_idx = lb;
        ret_ele = Zeta_CircularVector_Access(&cv, NULL, lb);

        break;
    }

    if (dst_cursor != NULL) {
        dst_cursor->idx = ret_idx;
        dst_cursor->n = ret_n;
        dst_cursor->seg_idx = ret_seg_idx;
        dst_cursor->ele = ret_ele;
    }

    return ret_ele;
}

static Zeta_SegVector_Node* InsertSeg_(Zeta_SegVector* sv,
                                       Zeta_OrdCntRBTreeNode* pos) {
    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = sv->width;
    size_t seg_capacity = sv->seg_capacity;

    ZETA_DebugAssert(sv->node_allocator != NULL);
    ZETA_DebugAssert(sv->seg_allocator != NULL);

    void* (*AllocateNode)(void* context, size_t size) =
        sv->node_allocator->Allocate;
    ZETA_DebugAssert(AllocateNode != NULL);

    void* (*AllocateSeg)(void* context, size_t size) =
        sv->seg_allocator->Allocate;
    ZETA_DebugAssert(AllocateSeg != NULL);

    Zeta_SegVector_Node* node =
        AllocateNode(sv->node_allocator->context, sizeof(Zeta_SegVector_Node));

    ZETA_DebugAssert(node != NULL);

    Zeta_OrdCntRBTreeNode_Init(NULL, &node->n);
    Zeta_OrdCntRBTreeNode_SetAccSize(NULL, &node->n, 0);

    node->offset = 0;

    node->seg = AllocateSeg(sv->seg_allocator->context, width * seg_capacity);
    ZETA_DebugAssert(node->seg != NULL);

    sv->root = Zeta_RBTree_InsertL(&btn_opr, pos, &node->n);

    return node;
}

static void* EraseSeg_(Zeta_SegVector* sv, Zeta_BinTreeNodeOperator* btn_opr,
                       Zeta_SegVector_Node* node) {
    ZETA_DebugAssert(sv->node_allocator != NULL);
    ZETA_DebugAssert(sv->seg_allocator != NULL);

    void (*DeallocateNode)(void* context, void* ptr) =
        sv->node_allocator->Deallocate;
    ZETA_DebugAssert(DeallocateNode != NULL);

    void (*DeallocateSeg)(void* context, void* ptr) =
        sv->seg_allocator->Deallocate;
    ZETA_DebugAssert(DeallocateSeg != NULL);

    void* n = &node->n;

    void* ret = Zeta_BinTree_StepR(btn_opr, n);

    sv->root = Zeta_RBTree_Extract(btn_opr, n);

    DeallocateSeg(sv->seg_allocator->context, node->seg);
    DeallocateNode(sv->node_allocator->context, node);

    return ret;
}

void* Zeta_SegVector_PushL(void* sv_, void* dst_cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_SegVector_Cursor pos_cursor;
    Zeta_SegVector_PeekL(sv, &pos_cursor);

    void* ele = Zeta_SegVector_Insert(sv, &pos_cursor);

    if (dst_cursor != NULL) {
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->n = pos_cursor.n;
        dst_cursor->seg_idx = pos_cursor.seg_idx;
        dst_cursor->ele = ele;
    }

    return ele;
}

void* Zeta_SegVector_PushR(void* sv_, void* dst_cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_SegVector_Cursor pos_cursor;
    Zeta_SegVector_GetRB(sv, &pos_cursor);

    void* ele = Zeta_SegVector_Insert(sv, &pos_cursor);

    if (dst_cursor != NULL) {
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->n = pos_cursor.n;
        dst_cursor->seg_idx = pos_cursor.seg_idx;
        dst_cursor->ele = ele;
    }

    return ele;
}

void* Zeta_SegVector_Insert(void* sv_, void* pos_cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* pos_cursor = pos_cursor_;
    Zeta_SegVector_Cursor_Check(sv, pos_cursor);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = sv->width;
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_SegVector_GetSize(sv);

    Zeta_OrdCntRBTreeNode* m_n = pos_cursor->n;
    size_t seg_idx = pos_cursor->seg_idx;

    ZETA_DebugAssert(&sv->lb != m_n);

    if (size == 0) {
        m_n = &InsertSeg_(sv, &sv->rb)->n;
    } else if (m_n == &sv->rb) {
        m_n = Zeta_BinTree_StepL(&btn_opr, m_n);
        seg_idx = Zeta_BinTree_GetSize(&btn_opr, m_n);
    }

    Zeta_SegVector_Node* m_node =
        ZETA_GetStructFromMember(Zeta_SegVector_Node, n, m_n);

    Zeta_CircularVector m_cv;
    m_cv.width = width;
    m_cv.stride = width;
    m_cv.offset = m_node->offset;
    m_cv.size = Zeta_BinTree_GetSize(&btn_opr, m_n);
    m_cv.capacity = seg_capacity;
    m_cv.data = m_node->seg;

    size_t m_vacant = seg_capacity - m_cv.size;

    if (0 < m_vacant) {
        Zeta_CircularVector_Cursor cv_cursor;
        cv_cursor.idx = seg_idx;

        void* ele = Zeta_CircularVector_Insert(&m_cv, &cv_cursor);

        Zeta_BinTree_SetSize(&btn_opr, m_n, m_cv.size);
        m_node->offset = m_cv.offset;

        pos_cursor->n = m_n;
        pos_cursor->seg_idx = seg_idx;
        pos_cursor->ele = ele;

        return ele;
    }

    Zeta_OrdCntRBTreeNode* l_n = Zeta_BinTree_StepL(&btn_opr, m_n);
    Zeta_OrdCntRBTreeNode* r_n = Zeta_BinTree_StepR(&btn_opr, m_n);

    Zeta_SegVector_Node* l_node;
    Zeta_SegVector_Node* r_node;

    size_t l_vacant;
    size_t r_vacant;

    if (l_n == &sv->lb) {
        l_node = NULL;
        l_vacant = 0;
    } else {
        l_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, l_n);
        l_vacant = seg_capacity - Zeta_BinTree_GetSize(&btn_opr, l_n);
    }

    if (r_n == &sv->rb) {
        r_node = NULL;
        r_vacant = 0;
    } else {
        r_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, r_n);
        r_vacant = seg_capacity - Zeta_BinTree_GetSize(&btn_opr, r_n);
    }

    if (l_vacant == 0 && r_vacant == 0) {
        l_node = InsertSeg_(sv, m_n);
        l_n = &l_node->n;
        l_vacant = seg_capacity;
    }

    if (seg_idx == 0 && 0 < l_vacant) {
        Zeta_CircularVector l_cv;
        l_cv.width = width;
        l_cv.stride = width;
        l_cv.offset = l_node->offset;
        l_cv.size = seg_capacity - l_vacant;
        l_cv.capacity = seg_capacity;
        l_cv.data = l_node->seg;

        void* ele = Zeta_CircularVector_PushR(&l_cv, NULL);

        Zeta_BinTree_SetSize(&btn_opr, l_n, l_cv.size);
        l_node->offset = l_cv.offset;

        pos_cursor->n = l_n;
        pos_cursor->seg_idx = l_cv.size - 1;
        pos_cursor->ele = ele;

        return ele;
    }

    if (l_vacant < r_vacant ||
        (l_vacant == r_vacant &&
         Zeta_SimpleHash(ZETA_PTR_TO_UINT(l_n) ^ ZETA_PTR_TO_UINT(r_n)) % 2 ==
             0)) {
        Zeta_CircularVector r_cv;
        r_cv.width = width;
        r_cv.stride = width;
        r_cv.offset = r_node->offset;
        r_cv.size = seg_capacity - r_vacant;
        r_cv.capacity = seg_capacity;
        r_cv.data = r_node->seg;

        size_t k;

        {
            size_t a = seg_capacity - seg_idx;
            size_t b = r_vacant / 2 + 1;

            k = a < b ? a : b;
        }

        for (size_t i = 0; i < k; ++i) {
            Zeta_MemCopy(Zeta_CircularVector_PushL(&r_cv, NULL),
                         Zeta_CircularVector_PeekR(&m_cv, NULL), width);
            Zeta_CircularVector_PopR(&m_cv);
        }

        Zeta_BinTree_SetSize(&btn_opr, r_n, r_cv.size);
        r_node->offset = r_cv.offset;
    } else {
        Zeta_CircularVector l_cv;
        l_cv.width = width;
        l_cv.stride = width;
        l_cv.offset = l_node->offset;
        l_cv.size = seg_capacity - l_vacant;
        l_cv.capacity = seg_capacity;
        l_cv.data = l_node->seg;

        size_t k;

        {
            size_t a = seg_idx;
            size_t b = l_vacant / 2 + 1;

            k = a < b ? a : b;
        }

        for (size_t i = 0; i < k; ++i) {
            Zeta_MemCopy(Zeta_CircularVector_PushR(&l_cv, NULL),
                         Zeta_CircularVector_PeekL(&m_cv, NULL), width);
            Zeta_CircularVector_PopL(&m_cv);
        }

        seg_idx -= k;

        Zeta_BinTree_SetSize(&btn_opr, l_n, l_cv.size);
        l_node->offset = l_cv.offset;
    }

    Zeta_CircularVector_Cursor cv_cursor;
    cv_cursor.idx = seg_idx;

    void* ele = Zeta_CircularVector_Insert(&m_cv, &cv_cursor);

    Zeta_BinTree_SetSize(&btn_opr, m_n, m_cv.size);
    m_node->offset = m_cv.offset;

    pos_cursor->n = m_n;
    pos_cursor->seg_idx = seg_idx;
    pos_cursor->ele = ele;

    return ele;
}

void Zeta_SegVector_PopL(void* sv) {
    Zeta_SegVector_Cursor pos_cursor;
    Zeta_SegVector_PeekL(sv, &pos_cursor);

    Zeta_SegVector_Erase(sv, &pos_cursor);
}

void Zeta_SegVector_PopR(void* sv) {
    Zeta_SegVector_Cursor pos_cursor;
    Zeta_SegVector_PeekR(sv, &pos_cursor);

    Zeta_SegVector_Erase(sv, &pos_cursor);
}

void* Zeta_SegVector_Erase(void* sv_, void* pos_cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* pos_cursor = pos_cursor_;
    Zeta_SegVector_Cursor_Check(sv, pos_cursor);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = sv->width;
    size_t seg_capacity = sv->seg_capacity;

    void* m_n = pos_cursor->n;

    ZETA_DebugAssert(&sv->lb != m_n);
    ZETA_DebugAssert(&sv->rb != m_n);

    Zeta_SegVector_Node* m_node =
        ZETA_GetStructFromMember(Zeta_SegVector_Node, n, m_n);
    size_t seg_idx = pos_cursor->seg_idx;

    void* ret_n;
    size_t ret_seg_idx;

    Zeta_CircularVector m_cv;
    m_cv.width = width;
    m_cv.stride = width;
    m_cv.offset = m_node->offset;
    m_cv.size = Zeta_BinTree_GetSize(&btn_opr, m_n);
    m_cv.capacity = seg_capacity;
    m_cv.data = m_node->seg;

    ZETA_DebugAssert(seg_idx < m_cv.size);

    Zeta_CircularVector_Cursor cv_cursor;
    cv_cursor.idx = seg_idx;

    Zeta_CircularVector_Erase(&m_cv, &cv_cursor);

    if (m_cv.size == 0) {
        ret_n = EraseSeg_(sv, &btn_opr, m_node);
        ret_seg_idx = 0;
        goto RET;
    }

    Zeta_BinTree_SetSize(&btn_opr, m_n, m_cv.size);
    m_node->offset = m_cv.offset;

    if (seg_capacity <= m_cv.size * 2) {
        if (seg_idx == m_cv.size) {
            ret_n = Zeta_BinTree_StepR(&btn_opr, m_n);
            ret_seg_idx = 0;
        } else {
            ret_n = m_n;
            ret_seg_idx = seg_idx;
        }

        goto RET;
    }

    size_t m_vacant = seg_capacity - m_cv.size;

    void* l_n = Zeta_BinTree_StepL(&btn_opr, m_n);
    void* r_n = Zeta_BinTree_StepR(&btn_opr, m_n);

    Zeta_SegVector_Node* l_node;
    Zeta_SegVector_Node* r_node;

    Zeta_CircularVector l_cv;
    Zeta_CircularVector r_cv;

    l_cv.width = width;
    r_cv.width = width;

    l_cv.stride = width;
    r_cv.stride = width;

    size_t l_vacant;
    size_t r_vacant;

    if (&sv->lb == l_n) {
        l_node = NULL;

        l_cv.offset = 0;
        l_cv.size = 0;
        l_cv.capacity = 0;
        l_cv.data = NULL;

        l_vacant = 0;
    } else {
        l_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, l_n);

        l_cv.offset = l_node->offset;
        l_cv.size = Zeta_BinTree_GetSize(&btn_opr, l_n);
        l_cv.capacity = seg_capacity;
        l_cv.data = l_node->seg;

        l_vacant = seg_capacity - l_cv.size;
    }

    if (&sv->rb == r_n) {
        r_node = NULL;

        r_cv.offset = 0;
        r_cv.size = 0;
        r_cv.capacity = 0;
        r_cv.data = NULL;

        r_vacant = 0;
    } else {
        r_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, r_n);

        r_cv.offset = r_node->offset;
        r_cv.size = Zeta_BinTree_GetSize(&btn_opr, r_n);
        r_cv.capacity = seg_capacity;
        r_cv.data = r_node->seg;

        r_vacant = seg_capacity - r_cv.size;
    }

    size_t lm_vacant = l_vacant + m_vacant;
    size_t mr_vacant = m_vacant + r_vacant;

    void* a_n;
    void* b_n;

    Zeta_SegVector_Node* a_node;
    Zeta_SegVector_Node* b_node;

    Zeta_CircularVector* a_cv;
    Zeta_CircularVector* b_cv;

    size_t a_vacant;
    size_t b_vacant;

    if (lm_vacant < mr_vacant ||
        (lm_vacant == mr_vacant &&
         Zeta_SimpleHash(ZETA_PTR_TO_UINT(l_n) ^ ZETA_PTR_TO_UINT(r_n)) % 2 ==
             0)) {
        a_n = m_n;
        a_node = m_node;
        a_cv = &m_cv;
        a_vacant = m_vacant;

        b_n = r_n;
        b_node = r_node;
        b_cv = &r_cv;
        b_vacant = r_vacant;
    } else {
        a_n = l_n;
        a_node = l_node;
        a_cv = &l_cv;
        a_vacant = l_vacant;

        b_n = m_n;
        b_node = m_node;
        b_cv = &m_cv;
        b_vacant = m_vacant;
    }

    size_t ab_vacant = a_vacant + b_vacant;

    // if (lr_vacant < seg_capacity * 1.25) { return; }
    if (ab_vacant * 4 < seg_capacity * 5) {
        if (seg_idx == m_cv.size) {
            ret_n = r_n;
            ret_seg_idx = 0;
        } else {
            ret_n = m_n;
            ret_seg_idx = seg_idx;
        }

        goto RET;
    }

    /*
    case l to m
        if seg_idx == m_cv.size
            ret_n = r_n
            ret_seg_idx = 0
        else
            ret_n = m_n
            ret_seg_idx = l_cv.size + seg_idx

    case m to r
        ret_n = r_n
        ret_seg_idx = seg_idx

    case m to l
        if seg_idx == m_cv.size
            ret_n = r_n
            ret_seg_idx = 0
        else
            ret_n = l_n
            ret_seg_idx = l_cv.size + seg_idx

    case r to m
        ret_n = m_n
        ret_seg_idx = seg_idx
    */

    if (a_vacant < b_vacant ||
        (a_vacant == b_vacant &&
         Zeta_SimpleHash(ZETA_PTR_TO_UINT(a_n) ^ ZETA_PTR_TO_UINT(b_n)) % 2 ==
             0)) {  // b to a

        if (a_n == m_n) {  // r(b) to m(a)

            ret_n = m_n;
            ret_seg_idx = seg_idx;
        } else if (seg_idx == m_cv.size) {  // m(b) to l(a)

            ret_n = r_n;
            ret_seg_idx = 0;
        } else {
            ret_n = l_n;
            ret_seg_idx = l_cv.size + seg_idx;
        }

        while (0 < b_cv->size) {
            Zeta_MemCopy(Zeta_CircularVector_PushR(a_cv, NULL),
                         Zeta_CircularVector_PeekL(b_cv, NULL), width);

            Zeta_CircularVector_PopL(b_cv);
        }

        EraseSeg_(sv, &btn_opr, b_node);

        Zeta_BinTree_SetSize(&btn_opr, a_n, a_cv->size);
        a_node->offset = a_cv->offset;
    } else {  // a to b

        if (a_n == m_n) {  // m(a) to r(b)

            ret_n = r_n;
            ret_seg_idx = seg_idx;
        } else if (seg_idx == m_cv.size) {  // l(a) to m(b)

            ret_n = r_n;
            ret_seg_idx = 0;
        } else {
            ret_n = m_n;
            ret_seg_idx = l_cv.size + seg_idx;
        }

        while (0 < a_cv->size) {
            Zeta_MemCopy(Zeta_CircularVector_PushL(b_cv, NULL),
                         Zeta_CircularVector_PeekR(a_cv, NULL), width);

            Zeta_CircularVector_PopR(a_cv);
        }

        EraseSeg_(sv, &btn_opr, a_node);

        Zeta_BinTree_SetSize(&btn_opr, b_n, b_cv->size);
        b_node->offset = b_cv->offset;
    }

RET:;

    void* ele;

    if (&sv->rb == ret_n) {
        ele = NULL;
    } else {
        Zeta_SegVector_Node* ret_node =
            ZETA_GetStructFromMember(Zeta_SegVector_Node, n, ret_n);

        m_cv.offset = ret_node->offset;
        m_cv.size = ret_seg_idx + 1;
        m_cv.data = ret_node->seg;

        ele = Zeta_CircularVector_PeekR(&m_cv, NULL);
    }

    pos_cursor->n = ret_n;
    pos_cursor->seg_idx = ret_seg_idx;
    pos_cursor->ele = ele;

    return ele;
}

void Check_(Zeta_SegVector* sv, Zeta_SegVector_Node* node,
            Zeta_DebugTreeMap* dst_node_tm, Zeta_DebugTreeMap* dst_seg_tm) {
    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    {
        Zeta_DebugTreeMap_KeyValPair p =
            Zeta_DebugTreeMap_Insert(dst_node_tm, ZETA_PTR_TO_UINT(node));
        ZETA_DebugAssert(p.b);
        *p.val = sizeof(Zeta_SegVector_Node);
    }

    Zeta_OrdCntRBTreeNode* n = &node->n;

    size_t size = Zeta_BinTree_GetSize(&btn_opr, n);

    ZETA_DebugAssert(0 < size);
    ZETA_DebugAssert(size <= sv->seg_capacity);
    ZETA_DebugAssert(node->offset < sv->seg_capacity);

    {
        Zeta_DebugTreeMap_KeyValPair p =
            Zeta_DebugTreeMap_Insert(dst_seg_tm, ZETA_PTR_TO_UINT(node->seg));
        ZETA_DebugAssert(p.b);
        *p.val = sv->width * sv->seg_capacity;
    }

    Zeta_OrdCntRBTreeNode* l_n = Zeta_OrdCntRBTreeNode_GetL(NULL, n);

    if (l_n != NULL) {
        Zeta_SegVector_Node* l_node =
            ZETA_GetStructFromMember(Zeta_SegVector_Node, n, l_n);
        Check_(sv, l_node, dst_node_tm, dst_seg_tm);
    }

    Zeta_OrdCntRBTreeNode* r_n = Zeta_OrdCntRBTreeNode_GetR(NULL, n);

    if (r_n != NULL) {
        Zeta_SegVector_Node* r_node =
            ZETA_GetStructFromMember(Zeta_SegVector_Node, n, r_n);
        Check_(sv, r_node, dst_node_tm, dst_seg_tm);
    }
}

void Zeta_SegVector_Check(void* sv_, Zeta_DebugTreeMap* dst_node_tm,
                          Zeta_DebugTreeMap* dst_seg_tm) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    if (sv->root == NULL) { return; }

    Check_(sv, ZETA_GetStructFromMember(Zeta_SegVector_Node, n, sv->root),
           dst_node_tm, dst_seg_tm);
}

void Zeta_SegVector_DeploySeqContainer(void* sv_, Zeta_SeqContainer* seq_cntr) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    ZETA_DebugAssert(seq_cntr != NULL);

    seq_cntr->context = sv;

    seq_cntr->GetWidth = Zeta_SegVector_GetWidth;
    seq_cntr->GetSize = Zeta_SegVector_GetSize;

    seq_cntr->PeekL = Zeta_SegVector_PeekL;
    seq_cntr->PeekR = Zeta_SegVector_PeekR;

    seq_cntr->Access = Zeta_SegVector_Access;

    seq_cntr->PushL = Zeta_SegVector_PushL;
    seq_cntr->PushR = Zeta_SegVector_PushR;

    seq_cntr->Insert = Zeta_SegVector_Insert;

    seq_cntr->PopL = Zeta_SegVector_PopL;
    seq_cntr->PopR = Zeta_SegVector_PopR;

    seq_cntr->Erase = Zeta_SegVector_Erase;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Zeta_SegVector_Cursor_Check(void* sv_, void const* cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    Zeta_SegVector_Cursor re_cursor;
    Zeta_SegVector_Access(sv, &re_cursor, cursor->idx);

    ZETA_DebugAssert(re_cursor.idx == cursor->idx);
    ZETA_DebugAssert(re_cursor.n == cursor->n);
    ZETA_DebugAssert(re_cursor.seg_idx == cursor->seg_idx);
    ZETA_DebugAssert(re_cursor.ele == cursor->ele);
}

bool_t Zeta_SegVector_Cursor_IsEqual(void* sv_, void const* cursor_a_,
                                     void const* cursor_b_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_SegVector_Cursor const* cursor_a = cursor_a_;
    Zeta_SegVector_Cursor const* cursor_b = cursor_b_;

    Zeta_SegVector_Cursor_Check(sv, cursor_a);
    Zeta_SegVector_Cursor_Check(sv, cursor_b);

    return cursor_a->idx == cursor_b->idx;
}

int Zeta_SegVector_Cursor_Compare(void* sv_, void const* cursor_a_,
                                  void const* cursor_b_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_SegVector_Cursor const* cursor_a = cursor_a_;
    Zeta_SegVector_Cursor const* cursor_b = cursor_b_;

    Zeta_SegVector_Cursor_Check(sv, cursor_a);
    Zeta_SegVector_Cursor_Check(sv, cursor_b);

    if (cursor_a->idx < cursor_b->idx) { return -1; }
    if (cursor_b->idx < cursor_a->idx) { return 1; }
    return 0;
}

size_t Zeta_SegVector_Cursor_Differ(void* sv_, void const* cursor_a_,
                                    void const* cursor_b_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_SegVector_Cursor const* cursor_a = cursor_a_;
    Zeta_SegVector_Cursor const* cursor_b = cursor_b_;

    Zeta_SegVector_Cursor_Check(sv, cursor_a);
    Zeta_SegVector_Cursor_Check(sv, cursor_b);

    return cursor_a->idx - cursor_b->idx;
}

void* Zeta_SegVector_Cursor_Refer(void* sv_, void const* cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor const* cursor = cursor_;
    Zeta_SegVector_Cursor_Check(sv, cursor);

    return cursor->ele;
}

void Zeta_SegVector_Cursor_StepL(void* sv, void* cursor) {
    Zeta_SegVector_Cursor_AdvanceL(sv, cursor, 1);
}

void Zeta_SegVector_Cursor_StepR(void* sv, void* cursor) {
    Zeta_SegVector_Cursor_AdvanceR(sv, cursor, 1);
}

void Zeta_SegVector_Cursor_AdvanceL(void* sv_, void* cursor_, size_t step) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* cursor = cursor_;
    Zeta_SegVector_Cursor_Check(sv, cursor);

    if (step == 0) { return; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    void* dst_n;
    size_t dst_seg_idx;

    size_t n_size = Zeta_BinTree_GetSize(&btn_opr, cursor->n);

    Zeta_BinTree_AdvanceL(&dst_n, &dst_seg_idx, &btn_opr, cursor->n,
                          n_size - 1 - cursor->seg_idx + step);

    ZETA_DebugAssert(dst_n != NULL);

    size_t dst_n_size = Zeta_BinTree_GetSize(&btn_opr, dst_n);

    dst_seg_idx = dst_n_size - 1 - dst_seg_idx;

    cursor->idx -= step;
    cursor->n = dst_n;
    cursor->seg_idx = dst_seg_idx;

    if (&sv->lb == dst_n) {
        cursor->ele = NULL;
    } else {
        Zeta_SegVector_Node* dst_node =
            ZETA_GetStructFromMember(Zeta_SegVector_Node, n, dst_n);

        Zeta_CircularVector cv;
        cv.width = sv->width;
        cv.stride = sv->width;
        cv.offset = dst_node->offset;
        cv.size = dst_n_size;
        cv.capacity = sv->seg_capacity;
        cv.data = dst_node->seg;

        cursor->ele = Zeta_CircularVector_Access(&cv, NULL, dst_seg_idx);
    }
}

void Zeta_SegVector_Cursor_AdvanceR(void* sv_, void* cursor_, size_t step) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* cursor = cursor_;
    Zeta_SegVector_Cursor_Check(sv, cursor);

    if (step == 0) { return; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    void* dst_n;
    size_t dst_seg_idx;

    Zeta_BinTree_AdvanceR(&dst_n, &dst_seg_idx, &btn_opr, cursor->n,
                          cursor->seg_idx + step);

    ZETA_DebugAssert(dst_n != NULL);

    size_t dst_n_size = Zeta_BinTree_GetSize(&btn_opr, dst_n);

    cursor->idx += step;
    cursor->n = dst_n;
    cursor->seg_idx = dst_seg_idx;

    if (&sv->rb == dst_n) {
        cursor->ele = NULL;
    } else {
        Zeta_SegVector_Node* dst_node =
            ZETA_GetStructFromMember(Zeta_SegVector_Node, n, dst_n);

        Zeta_CircularVector cv;
        cv.width = sv->width;
        cv.stride = sv->width;
        cv.offset = dst_node->offset;
        cv.size = dst_n_size;
        cv.capacity = sv->seg_capacity;
        cv.data = dst_node->seg;

        cursor->ele = Zeta_CircularVector_Access(&cv, NULL, dst_seg_idx);
    }
}

void Zeta_SegVector_Cursor_DeployCursorOperator(
    void* sv_, Zeta_CursorOperator* cursor_opr) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    ZETA_DebugAssert(cursor_opr != NULL);

    cursor_opr->context = sv;

    cursor_opr->IsEqual = Zeta_SegVector_Cursor_IsEqual;
    cursor_opr->Compare = Zeta_SegVector_Cursor_Compare;
    cursor_opr->Differ = Zeta_SegVector_Cursor_Differ;

    cursor_opr->Refer = Zeta_SegVector_Cursor_Refer;

    cursor_opr->StepL = Zeta_SegVector_Cursor_StepL;
    cursor_opr->StepR = Zeta_SegVector_Cursor_StepR;

    cursor_opr->AdvanceL = Zeta_SegVector_Cursor_AdvanceL;
    cursor_opr->AdvanceR = Zeta_SegVector_Cursor_AdvanceR;
}
