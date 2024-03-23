#include "SegVector.h"

#include "BinTree.h"
#include "CircularVector.h"
#include "CntBinTree.h"
#include "RBTree.h"
#include "RawVector.h"
#include "utils.h"

static void ToCircularVector_(size_t width, size_t seg_capacity,
                              Zeta_SegVector_Node* node, Zeta_RawVector* rv,
                              Zeta_Vector* v, Zeta_CircularVector* cv) {
    rv->data = node->seg;
    rv->width = width;
    rv->stride = width;
    rv->size = seg_capacity;

    Zeta_RawVector_ToVector(rv, v);

    cv->vec = v;
    cv->offset = node->offset;
    cv->size = Zeta_RelCntRBTreeNode_GetSize(NULL, node);
}

void Zeta_SegVector_Init(void* sv_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    ZETA_DebugAssert(0 < sv->width);
    ZETA_DebugAssert(0 < sv->seg_capacity);

    sv->root = NULL;

    ZETA_DebugAssert(sv->node_allocator != NULL);
    ZETA_DebugAssert(sv->node_allocator->Allocate != NULL);
    ZETA_DebugAssert(sv->node_allocator->Deallocate != NULL);

    ZETA_DebugAssert(sv->seg_allocator != NULL);
    ZETA_DebugAssert(sv->seg_allocator->Allocate != NULL);
    ZETA_DebugAssert(sv->seg_allocator->Deallocate != NULL);
}

size_t Zeta_SegVector_GetWidth(void* sv_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    return sv->width;
}

size_t Zeta_SegVector_GetSize(void* sv_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    return Zeta_RelCntRBTreeNode_GetAccSize(NULL, sv->root);
}

void* Zeta_SegVector_PeekL(void* sv_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    size_t size = Zeta_SegVector_GetSize(sv);
    ZETA_DebugAssert(0 < size);

    return Zeta_SegVector_Access(sv, 0);
}

void* Zeta_SegVector_PeekR(void* sv_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    size_t size = Zeta_SegVector_GetSize(sv);
    ZETA_DebugAssert(0 < size);

    return Zeta_SegVector_Access(sv, size - 1);
}

void* Zeta_SegVector_Access(void* sv_, size_t idx) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    size_t width = sv->width;
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_SegVector_GetSize(sv);

    ZETA_DebugAssert(idx < size);

    Zeta_CntBinTreeNodeOpr cbtn_opr = (Zeta_CntBinTreeNodeOpr){
        .GetP = Zeta_RelCntRBTreeNode_GetP,
        .GetL = Zeta_RelCntRBTreeNode_GetL,
        .GetR = Zeta_RelCntRBTreeNode_GetR,

        .GetAccSize = Zeta_RelCntRBTreeNode_GetAccSize,
        .GetSize = Zeta_RelCntRBTreeNode_GetSize,
        .SetSize = Zeta_RelCntRBTreeNode_SetSize,
    };

    Zeta_SegVector_Node* m_node;
    void* m_n;
    size_t seg_idx;

    Zeta_CntBinTree_Access(&m_n, &seg_idx, &cbtn_opr, sv->root, idx);

    m_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, m_n);

    Zeta_RawVector m_rv;
    Zeta_Vector m_v;
    Zeta_CircularVector m_cv;

    ToCircularVector_(width, seg_capacity, m_node, &m_rv, &m_v, &m_cv);

    return Zeta_CircularVector_Access(&m_cv, seg_idx);
}

static Zeta_SegVector_Node* InsertSeg_(Zeta_SegVector* sv, void* pos) {
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

    Zeta_RBTreeNodeOpr rbtn_opr = (Zeta_RBTreeNodeOpr){
        .context = NULL,

        .GetP = Zeta_RelCntRBTreeNode_GetP,
        .GetL = Zeta_RelCntRBTreeNode_GetL,
        .GetR = Zeta_RelCntRBTreeNode_GetR,

        .GetColor = Zeta_RelCntRBTreeNode_GetColor,
        .ReverseColor = Zeta_RelCntRBTreeNode_ReverseColor,

        .AttachL = Zeta_RelCntRBTreeNode_AttachL,
        .AttachR = Zeta_RelCntRBTreeNode_AttachR,

        .Detach = Zeta_RelCntRBTreeNode_Detach,

        .Swap = Zeta_RelCntRBTreeNode_Swap,

        .RotateL = Zeta_RelCntRBTreeNode_RotateL,
        .RotateR = Zeta_RelCntRBTreeNode_RotateR,
    };

    Zeta_SegVector_Node* node =
        AllocateNode(sv->node_allocator->context, sizeof(Zeta_SegVector_Node));

    ZETA_DebugAssert(node != NULL);

    Zeta_RelCntRBTreeNode_Init(NULL, &node->n);

    node->seg = AllocateSeg(sv->seg_allocator->context, width * seg_capacity);
    ZETA_DebugAssert(node->seg != NULL);

    node->offset = 0;

    sv->root = Zeta_RBTree_InsertL(&rbtn_opr, pos, &node->n);

    return node;
}

static void EraseSeg_(Zeta_SegVector* sv, void* n) {
    ZETA_DebugAssert(sv->node_allocator != NULL);
    ZETA_DebugAssert(sv->seg_allocator != NULL);

    void (*DeallocateNode)(void* context, void* ptr) =
        sv->node_allocator->Deallocate;
    ZETA_DebugAssert(DeallocateNode != NULL);

    void (*DeallocateSeg)(void* context, void* ptr) =
        sv->seg_allocator->Deallocate;
    ZETA_DebugAssert(DeallocateSeg != NULL);

    Zeta_RBTreeNodeOpr rbtn_opr = (Zeta_RBTreeNodeOpr){
        .context = NULL,

        .GetP = Zeta_RelCntRBTreeNode_GetP,
        .GetL = Zeta_RelCntRBTreeNode_GetL,
        .GetR = Zeta_RelCntRBTreeNode_GetR,

        .GetColor = Zeta_RelCntRBTreeNode_GetColor,
        .ReverseColor = Zeta_RelCntRBTreeNode_ReverseColor,

        .AttachL = Zeta_RelCntRBTreeNode_AttachL,
        .AttachR = Zeta_RelCntRBTreeNode_AttachR,

        .Detach = Zeta_RelCntRBTreeNode_Detach,

        .Swap = Zeta_RelCntRBTreeNode_Swap,

        .RotateL = Zeta_RelCntRBTreeNode_RotateL,
        .RotateR = Zeta_RelCntRBTreeNode_RotateR,
    };

    Zeta_SegVector_Node* node =
        ZETA_GetStructFromMember(Zeta_SegVector_Node, n, n);

    sv->root = Zeta_RBTree_Extract(&rbtn_opr, n);

    DeallocateSeg(sv->seg_allocator->context, node->seg);
    DeallocateNode(sv->node_allocator->context, node);
}

void* Zeta_SegVector_PushL(void* sv_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    return Zeta_SegVector_Insert(sv, 0);
}

void* Zeta_SegVector_PushR(void* sv_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    size_t size = Zeta_SegVector_GetSize(sv);

    return Zeta_SegVector_Insert(sv, size);
}

void* Zeta_SegVector_Insert(void* sv_, size_t idx) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    size_t width = sv->width;
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_SegVector_GetSize(sv);

    ZETA_DebugAssert(idx <= size);

    Zeta_BinTreeNodeOpr btn_accessor = (Zeta_BinTreeNodeOpr){
        .context = NULL,

        .GetP = Zeta_RelCntRBTreeNode_GetP,
        .GetL = Zeta_RelCntRBTreeNode_GetL,
        .GetR = Zeta_RelCntRBTreeNode_GetR,
    };

    Zeta_CntBinTreeNodeOpr cbtn_opr = (Zeta_CntBinTreeNodeOpr){
        .GetP = Zeta_RelCntRBTreeNode_GetP,
        .GetL = Zeta_RelCntRBTreeNode_GetL,
        .GetR = Zeta_RelCntRBTreeNode_GetR,

        .GetAccSize = Zeta_RelCntRBTreeNode_GetAccSize,
        .GetSize = Zeta_RelCntRBTreeNode_GetSize,
        .SetSize = Zeta_RelCntRBTreeNode_SetSize,
    };

    Zeta_SegVector_Node* m_node;
    void* m_n;
    size_t seg_idx;

    if (size == 0) {
        m_node = InsertSeg_(sv, NULL);
        m_n = &m_node->n;
        seg_idx = 0;
    } else {
        Zeta_CntBinTree_Access(&m_n, &seg_idx, &cbtn_opr, sv->root, idx);

        if (m_n == NULL) {
            m_n = Zeta_GetMostLink(NULL, Zeta_RelCntRBTreeNode_GetR, sv->root);
            seg_idx = Zeta_RelCntRBTreeNode_GetSize(NULL, m_n);
        }

        m_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, m_n);
    }

    Zeta_RawVector m_rv;
    Zeta_Vector m_v;
    Zeta_CircularVector m_cv;

    ToCircularVector_(width, seg_capacity, m_node, &m_rv, &m_v, &m_cv);

    size_t m_vacant = seg_capacity - Zeta_CircularVector_GetSize(&m_cv);

    if (0 < m_vacant) {
        void* ret = Zeta_CircularVector_Insert(&m_cv, seg_idx);
        --m_vacant;

        Zeta_RelCntRBTreeNode_SetSize(NULL, m_n, seg_capacity - m_vacant);
        m_node->offset = m_cv.offset;

        return ret;
    }

    void* l_n = Zeta_BinTree_StepL(&btn_accessor, m_n);
    void* r_n = Zeta_BinTree_StepR(&btn_accessor, m_n);

    Zeta_SegVector_Node* l_node;
    Zeta_SegVector_Node* r_node;

    size_t l_vacant;
    size_t r_vacant;

    if (l_n == NULL) {
        l_node = NULL;
        l_vacant = 0;
    } else {
        l_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, l_n);
        l_vacant = seg_capacity - Zeta_RelCntRBTreeNode_GetSize(NULL, l_n);
    }

    if (r_n == NULL) {
        r_node = NULL;
        r_vacant = 0;
    } else {
        r_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, r_n);
        r_vacant = seg_capacity - Zeta_RelCntRBTreeNode_GetSize(NULL, r_n);
    }

    if (l_vacant == 0 && r_vacant == 0) {
        l_node = InsertSeg_(sv, m_n);
        l_n = &l_node->n;
        l_vacant = seg_capacity;
    }

    if (seg_idx == 0 && 0 < l_vacant) {
        Zeta_RawVector l_rv;
        Zeta_Vector l_v;
        Zeta_CircularVector l_cv;

        ToCircularVector_(width, seg_capacity, l_node, &l_rv, &l_v, &l_cv);

        void* ret = Zeta_CircularVector_PushR(&l_cv);
        --l_vacant;

        Zeta_RelCntRBTreeNode_SetSize(NULL, l_n, seg_capacity - l_vacant);
        l_node->offset = l_cv.offset;

        return ret;
    }

    int ran =
        Zeta_SimpleHash(ZETA_PTR_TO_UINT(l_n) ^ ZETA_PTR_TO_UINT(r_n)) % 2;

    if (l_vacant < r_vacant || (l_vacant == r_vacant && ran == 0)) {
        Zeta_RawVector r_rv;
        Zeta_Vector r_v;
        Zeta_CircularVector r_cv;

        ToCircularVector_(width, seg_capacity, r_node, &r_rv, &r_v, &r_cv);

        for (; seg_idx < seg_capacity - m_vacant && 0 < r_vacant &&
               m_vacant <= r_vacant;) {
            Zeta_MemCopy(Zeta_CircularVector_PushL(&r_cv),
                         Zeta_CircularVector_PeekR(&m_cv), width);
            --r_vacant;

            Zeta_CircularVector_PopR(&m_cv);
            ++m_vacant;
        }

        Zeta_RelCntRBTreeNode_SetSize(NULL, r_n, seg_capacity - r_vacant);
        r_node->offset = r_cv.offset;
    } else {
        Zeta_RawVector l_rv;
        Zeta_Vector l_v;
        Zeta_CircularVector l_cv;

        ToCircularVector_(width, seg_capacity, l_node, &l_rv, &l_v, &l_cv);

        for (; 0 < seg_idx && 0 < l_vacant && m_vacant <= l_vacant; --seg_idx) {
            Zeta_MemCopy(Zeta_CircularVector_PushR(&l_cv),
                         Zeta_CircularVector_PeekL(&m_cv), width);
            --l_vacant;

            Zeta_CircularVector_PopL(&m_cv);
            ++m_vacant;
        }

        Zeta_RelCntRBTreeNode_SetSize(NULL, l_n, seg_capacity - l_vacant);
        l_node->offset = l_cv.offset;
    }

    void* ret = Zeta_CircularVector_Insert(&m_cv, seg_idx);
    --m_vacant;

    Zeta_RelCntRBTreeNode_SetSize(NULL, m_n, seg_capacity - m_vacant);
    m_node->offset = m_cv.offset;

    return ret;
}

void Zeta_SegVector_PopL(void* sv_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    size_t size = Zeta_SegVector_GetSize(sv);
    ZETA_DebugAssert(0 < size);

    Zeta_SegVector_Erase(sv, 0);
}

void Zeta_SegVector_PopR(void* sv_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    size_t size = Zeta_SegVector_GetSize(sv);
    ZETA_DebugAssert(0 < size);

    Zeta_SegVector_Erase(sv, size - 1);
}

void Zeta_SegVector_Erase(void* sv_, size_t idx) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    size_t width = sv->width;
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_SegVector_GetSize(sv);

    ZETA_DebugAssert(idx < size);

    Zeta_BinTreeNodeOpr btn_accessor = (Zeta_BinTreeNodeOpr){
        .context = NULL,

        .GetP = Zeta_RelCntRBTreeNode_GetP,
        .GetL = Zeta_RelCntRBTreeNode_GetL,
        .GetR = Zeta_RelCntRBTreeNode_GetR,
    };

    Zeta_CntBinTreeNodeOpr cbtn_opr = (Zeta_CntBinTreeNodeOpr){
        .GetP = Zeta_RelCntRBTreeNode_GetP,
        .GetL = Zeta_RelCntRBTreeNode_GetL,
        .GetR = Zeta_RelCntRBTreeNode_GetR,

        .GetAccSize = Zeta_RelCntRBTreeNode_GetAccSize,
        .GetSize = Zeta_RelCntRBTreeNode_GetSize,
        .SetSize = Zeta_RelCntRBTreeNode_SetSize,
    };

    Zeta_SegVector_Node* m_node;
    void* m_n;
    size_t seg_idx;

    Zeta_CntBinTree_Access(&m_n, &seg_idx, &cbtn_opr, sv->root, idx);

    m_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, m_n);

    Zeta_RawVector m_rv;
    Zeta_Vector m_v;
    Zeta_CircularVector m_cv;

    ToCircularVector_(width, seg_capacity, m_node, &m_rv, &m_v, &m_cv);

    Zeta_CircularVector_Erase(&m_cv, seg_idx);

    size_t m_size = Zeta_CircularVector_GetSize(&m_cv);

    if (m_size == 0) {
        EraseSeg_(sv, m_n);
        return;
    }

    Zeta_RelCntRBTreeNode_SetSize(NULL, m_n, m_size);
    m_node->offset = m_cv.offset;

    if (seg_capacity <= m_size * 2) { return; }

    size_t m_vacant = seg_capacity - m_size;

    void* l_n = Zeta_BinTree_StepL(&btn_accessor, m_n);
    void* r_n = Zeta_BinTree_StepR(&btn_accessor, m_n);

    Zeta_SegVector_Node* l_node;
    Zeta_SegVector_Node* r_node;

    size_t l_vacant;
    size_t r_vacant;

    if (l_n == NULL) {
        l_node = NULL;
        l_vacant = 0;
    } else {
        l_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, l_n);
        l_vacant = seg_capacity - Zeta_RelCntRBTreeNode_GetSize(NULL, l_n);
    }

    if (r_n == NULL) {
        r_node = NULL;
        r_vacant = 0;
    } else {
        r_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, r_n);
        r_vacant = seg_capacity - Zeta_RelCntRBTreeNode_GetSize(NULL, r_n);
    }

    size_t lm_vacant = l_vacant + m_vacant;
    size_t mr_vacant = m_vacant + r_vacant;

    int ran =
        Zeta_SimpleHash(ZETA_PTR_TO_UINT(l_n) ^ ZETA_PTR_TO_UINT(r_n)) % 2;

    if (lm_vacant < mr_vacant || (lm_vacant == mr_vacant && ran == 0)) {
        l_n = m_n;
        l_node = m_node;
        l_vacant = m_vacant;
    } else {
        r_n = m_n;
        r_node = m_node;
        r_vacant = m_vacant;
    }

    size_t lr_vacant = l_vacant + r_vacant;

    // if (lr_vacant < seg_capacity * 1.25) { return; }
    if (lr_vacant * 4 < seg_capacity * 5) { return; }

    Zeta_RawVector l_rv;
    Zeta_Vector l_v;
    Zeta_CircularVector l_cv;

    ToCircularVector_(width, seg_capacity, l_node, &l_rv, &l_v, &l_cv);

    Zeta_RawVector r_rv;
    Zeta_Vector r_v;
    Zeta_CircularVector r_cv;

    ToCircularVector_(width, seg_capacity, r_node, &r_rv, &r_v, &r_cv);

    if (l_vacant <= r_vacant) {
        while (r_vacant < seg_capacity) {
            Zeta_MemCopy(Zeta_CircularVector_PushR(&l_cv),
                         Zeta_CircularVector_PeekL(&r_cv), width);
            --l_vacant;

            Zeta_CircularVector_PopL(&r_cv);
            ++r_vacant;
        }

        EraseSeg_(sv, r_n);

        Zeta_RelCntRBTreeNode_SetSize(NULL, l_n, seg_capacity - l_vacant);
        l_node->offset = l_cv.offset;
    } else {
        while (l_vacant < seg_capacity) {
            Zeta_MemCopy(Zeta_CircularVector_PushL(&r_cv),
                         Zeta_CircularVector_PeekR(&l_cv), width);
            --r_vacant;

            Zeta_CircularVector_PopR(&l_cv);
            ++l_vacant;
        }

        EraseSeg_(sv, l_n);

        Zeta_RelCntRBTreeNode_SetSize(NULL, r_n, seg_capacity - r_vacant);
        r_node->offset = r_cv.offset;
    }
}

void Check_(Zeta_SegVector* sv, Zeta_SegVector_Node* node,
            Zeta_DebugTreeMap* dst_node_tm, Zeta_DebugTreeMap* dst_seg_tm) {
    {
        Zeta_DebugTreeMap_KeyValPair p =
            Zeta_DebugTreeMap_Insert(dst_node_tm, ZETA_PTR_TO_UINT(node));
        ZETA_DebugAssert(p.b);
        *p.val = sizeof(Zeta_SegVector_Node);
    }

    Zeta_RelCntRBTreeNode* n = &node->n;

    size_t size = Zeta_RelCntRBTreeNode_GetSize(NULL, n);

    ZETA_DebugAssert(0 < size);
    ZETA_DebugAssert(size <= sv->seg_capacity);
    ZETA_DebugAssert(node->offset < sv->seg_capacity);

    {
        Zeta_DebugTreeMap_KeyValPair p =
            Zeta_DebugTreeMap_Insert(dst_seg_tm, ZETA_PTR_TO_UINT(node->seg));
        ZETA_DebugAssert(p.b);
        *p.val = sv->width * sv->seg_capacity;
    }

    Zeta_RelCntRBTreeNode* l_n = Zeta_RelCntRBTreeNode_GetL(NULL, n);

    if (l_n != NULL) {
        Zeta_SegVector_Node* l_node =
            ZETA_GetStructFromMember(Zeta_SegVector_Node, n, l_n);
        Check_(sv, l_node, dst_node_tm, dst_seg_tm);
    }

    Zeta_RelCntRBTreeNode* r_n = Zeta_RelCntRBTreeNode_GetR(NULL, n);

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

void Zeta_SegVector_ToVector(void* sv_, Zeta_Vector* dst) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    ZETA_DebugAssert(dst != NULL);

    dst->context = sv;
    dst->GetWidth = Zeta_SegVector_GetWidth;
    dst->GetSize = Zeta_SegVector_GetSize;
    dst->PeekL = Zeta_SegVector_PeekL;
    dst->PeekR = Zeta_SegVector_PeekR;
    dst->Access = Zeta_SegVector_Access;
    dst->PushL = Zeta_SegVector_PushL;
    dst->PushR = Zeta_SegVector_PushR;
    dst->Insert = Zeta_SegVector_Insert;
    dst->PopL = Zeta_SegVector_PopL;
    dst->PopR = Zeta_SegVector_PopR;
    dst->Erase = Zeta_SegVector_Erase;
}
