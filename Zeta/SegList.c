#include "SegList.h"

#include "BinTree.h"
#include "CircularVector.h"
#include "CntBinTree.h"
#include "RBTree.h"
#include "RawVector.h"
#include "utils.h"

static void DeployCircularVector_(size_t width, size_t cluster_capacity,
                                  Zeta_SegList_Node* node, Zeta_RawVector* rv,
                                  Zeta_Vector* v, Zeta_CircularVector* cv) {
    rv->data = node->cluster;
    rv->width = width;
    rv->stride = width;
    rv->size = cluster_capacity;

    Zeta_RawVector_ToVector(rv, v);

    cv->vec = v;
    cv->offset = node->offset;
    cv->size = Zeta_RelCntRBTreeNode_GetSize(NULL, node);
}

void Zeta_SegList_Init(void* sl_) {
    Zeta_SegList* sl = sl_;
    ZETA_DebugAssert(sl != NULL);

    ZETA_DebugAssert(0 < sl->width);
    ZETA_DebugAssert(0 < sl->cluster_capacity);

    sl->root = NULL;

    ZETA_DebugAssert(sl->node_allocator != NULL);
    ZETA_DebugAssert(sl->node_allocator->Allocate != NULL);
    ZETA_DebugAssert(sl->node_allocator->Deallocate != NULL);

    ZETA_DebugAssert(sl->cluster_allocator != NULL);
    ZETA_DebugAssert(sl->cluster_allocator->Allocate != NULL);
    ZETA_DebugAssert(sl->cluster_allocator->Deallocate != NULL);
}

size_t Zeta_SegList_GetWidth(void* sl_) {
    Zeta_SegList* sl = sl_;
    ZETA_DebugAssert(sl != NULL);

    return sl->width;
}

size_t Zeta_SegList_GetSize(void* sl_) {
    Zeta_SegList* sl = sl_;
    ZETA_DebugAssert(sl != NULL);

    return Zeta_RelCntRBTreeNode_GetAccSize(NULL, sl->root);
}

void* Zeta_SegList_Access(void* sl_, size_t idx) {
    Zeta_SegList* sl = sl_;
    ZETA_DebugAssert(sl != NULL);

    size_t width = sl->width;
    size_t cluster_capacity = sl->cluster_capacity;
    size_t size = Zeta_SegList_GetSize(sl);

    ZETA_DebugAssert(idx < size);

    Zeta_CntBinTreeNodeOpr cbtn_opr = (Zeta_CntBinTreeNodeOpr){
        .GetP = Zeta_RelCntRBTreeNode_GetP,
        .GetL = Zeta_RelCntRBTreeNode_GetL,
        .GetR = Zeta_RelCntRBTreeNode_GetR,

        .GetAccSize = Zeta_RelCntRBTreeNode_GetAccSize,
        .GetSize = Zeta_RelCntRBTreeNode_GetSize,
        .SetSize = Zeta_RelCntRBTreeNode_SetSize,
    };

    Zeta_SegList_Node* m_node;
    void* m_n;
    size_t cluster_idx;

    Zeta_CntBinTree_Access(&m_n, &cluster_idx, &cbtn_opr, sl->root, idx);

    m_node = ZETA_GetStructFromMember(Zeta_SegList_Node, n, m_n);

    Zeta_RawVector m_rv;
    Zeta_Vector m_v;
    Zeta_CircularVector m_cv;

    DeployCircularVector_(width, cluster_capacity, m_node, &m_rv, &m_v, &m_cv);

    return Zeta_CircularVector_Access(&m_cv, cluster_idx);
}

void* Zeta_SegList_PeekL(void* sl_) {
    Zeta_SegList* sl = sl_;
    ZETA_DebugAssert(sl != NULL);

    size_t size = Zeta_SegList_GetSize(sl);
    ZETA_DebugAssert(0 < size);

    return Zeta_SegList_Access(sl, 0);
}

void* Zeta_SegList_PeekR(void* sl_) {
    Zeta_SegList* sl = sl_;
    ZETA_DebugAssert(sl != NULL);

    size_t size = Zeta_SegList_GetSize(sl);
    ZETA_DebugAssert(0 < size);

    return Zeta_SegList_Access(sl, size - 1);
}

static Zeta_SegList_Node* InsertSeg_(Zeta_SegList* sl, void* pos) {
    size_t width = sl->width;
    size_t cluster_capacity = sl->cluster_capacity;

    ZETA_DebugAssert(sl->node_allocator != NULL);
    ZETA_DebugAssert(sl->cluster_allocator != NULL);

    void* (*AllocateNode)(void* context, size_t size) =
        sl->node_allocator->Allocate;
    ZETA_DebugAssert(AllocateNode != NULL);

    void* (*AllocateCluster)(void* context, size_t size) =
        sl->cluster_allocator->Allocate;
    ZETA_DebugAssert(AllocateCluster != NULL);

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

    Zeta_SegList_Node* node =
        AllocateNode(sl->node_allocator->context, sizeof(Zeta_SegList_Node));

    ZETA_DebugAssert(node != NULL);

    Zeta_RelCntRBTreeNode_Init(NULL, &node->n);

    node->cluster = AllocateCluster(sl->cluster_allocator->context,
                                    width * cluster_capacity);
    ZETA_DebugAssert(node->cluster != NULL);

    node->offset = 0;

    sl->root = Zeta_RBTree_InsertL(&rbtn_opr, pos, &node->n);

    return node;
}

static void EraseSeg_(Zeta_SegList* sl, void* n) {
    ZETA_DebugAssert(sl->node_allocator != NULL);
    ZETA_DebugAssert(sl->cluster_allocator != NULL);

    void (*DeallocateNode)(void* context, void* ptr) =
        sl->node_allocator->Deallocate;
    ZETA_DebugAssert(DeallocateNode != NULL);

    void (*DeallocateCluster)(void* context, void* ptr) =
        sl->cluster_allocator->Deallocate;
    ZETA_DebugAssert(DeallocateCluster != NULL);

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

    Zeta_SegList_Node* node = ZETA_GetStructFromMember(Zeta_SegList_Node, n, n);

    sl->root = Zeta_RBTree_Extract(&rbtn_opr, n);

    DeallocateCluster(sl->cluster_allocator->context, node->cluster);
    DeallocateNode(sl->node_allocator->context, node);
}

void* Zeta_SegList_Insert(void* sl_, size_t idx) {
    Zeta_SegList* sl = sl_;
    ZETA_DebugAssert(sl != NULL);

    size_t width = sl->width;
    size_t cluster_capacity = sl->cluster_capacity;
    size_t size = Zeta_SegList_GetSize(sl);

    ZETA_DebugAssert(idx <= size);

    Zeta_BinTreeNodeAccessor btn_accessor = (Zeta_BinTreeNodeAccessor){
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

    Zeta_SegList_Node* m_node;
    void* m_n;
    size_t cluster_idx;

    if (size == 0) {
        m_node = InsertSeg_(sl, NULL);
        m_n = &m_node->n;
        cluster_idx = 0;
    } else {
        Zeta_CntBinTree_Access(&m_n, &cluster_idx, &cbtn_opr, sl->root, idx);

        if (m_n == NULL) {
            m_n = Zeta_GetMostLink(NULL, Zeta_RelCntRBTreeNode_GetR, sl->root);
            cluster_idx = Zeta_RelCntRBTreeNode_GetSize(NULL, m_n);
        }

        m_node = ZETA_GetStructFromMember(Zeta_SegList_Node, n, m_n);
    }

    Zeta_RawVector m_rv;
    Zeta_Vector m_v;
    Zeta_CircularVector m_cv;

    DeployCircularVector_(width, cluster_capacity, m_node, &m_rv, &m_v, &m_cv);

    size_t m_vacant = cluster_capacity - Zeta_CircularVector_GetSize(&m_cv);

    if (0 < m_vacant) {
        void* ret = Zeta_CircularVector_Insert(&m_cv, cluster_idx);
        --m_vacant;

        Zeta_RelCntRBTreeNode_SetSize(NULL, m_n, cluster_capacity - m_vacant);
        m_node->offset = m_cv.offset;

        return ret;
    }

    void* l_n = Zeta_BinTree_StepL(&btn_accessor, m_n);
    void* r_n = Zeta_BinTree_StepR(&btn_accessor, m_n);

    Zeta_SegList_Node* l_node;
    Zeta_SegList_Node* r_node;

    size_t l_vacant;
    size_t r_vacant;

    if (l_n == NULL) {
        l_node = NULL;
        l_vacant = 0;
    } else {
        l_node = ZETA_GetStructFromMember(Zeta_SegList_Node, n, l_n);
        l_vacant = cluster_capacity - Zeta_RelCntRBTreeNode_GetSize(NULL, l_n);
    }

    if (r_n == NULL) {
        r_node = NULL;
        r_vacant = 0;
    } else {
        r_node = ZETA_GetStructFromMember(Zeta_SegList_Node, n, r_n);
        r_vacant = cluster_capacity - Zeta_RelCntRBTreeNode_GetSize(NULL, r_n);
    }

    if (l_vacant == 0 && r_vacant == 0) {
        l_node = InsertSeg_(sl, m_n);
        l_n = &l_node->n;
        l_vacant = cluster_capacity;
    }

    if (cluster_idx == 0 && 0 < l_vacant) {
        Zeta_RawVector l_rv;
        Zeta_Vector l_v;
        Zeta_CircularVector l_cv;

        DeployCircularVector_(width, cluster_capacity, l_node, &l_rv, &l_v,
                              &l_cv);

        void* ret = Zeta_CircularVector_PushR(&l_cv);
        --l_vacant;

        Zeta_RelCntRBTreeNode_SetSize(NULL, l_n, cluster_capacity - l_vacant);
        l_node->offset = l_cv.offset;

        return ret;
    }

    int ran =
        Zeta_SimpleHash(ZETA_PTR_TO_UINT(l_n) ^ ZETA_PTR_TO_UINT(r_n)) % 2;

    if (l_vacant < r_vacant || (l_vacant == r_vacant && ran == 0)) {
        Zeta_RawVector r_rv;
        Zeta_Vector r_v;
        Zeta_CircularVector r_cv;

        DeployCircularVector_(width, cluster_capacity, r_node, &r_rv, &r_v,
                              &r_cv);

        for (; cluster_idx < cluster_capacity - m_vacant && 0 < r_vacant &&
               m_vacant <= r_vacant;) {
            Zeta_MemCopy(width, Zeta_CircularVector_PushL(&r_cv),
                         Zeta_CircularVector_PeekR(&m_cv));
            --r_vacant;

            Zeta_CircularVector_PopR(&m_cv);
            ++m_vacant;
        }

        Zeta_RelCntRBTreeNode_SetSize(NULL, r_n, cluster_capacity - r_vacant);
        r_node->offset = r_cv.offset;
    } else {
        Zeta_RawVector l_rv;
        Zeta_Vector l_v;
        Zeta_CircularVector l_cv;

        DeployCircularVector_(width, cluster_capacity, l_node, &l_rv, &l_v,
                              &l_cv);

        for (; 0 < cluster_idx && 0 < l_vacant && m_vacant <= l_vacant;
             --cluster_idx) {
            Zeta_MemCopy(width, Zeta_CircularVector_PushR(&l_cv),
                         Zeta_CircularVector_PeekL(&m_cv));
            --l_vacant;

            Zeta_CircularVector_PopL(&m_cv);
            ++m_vacant;
        }

        Zeta_RelCntRBTreeNode_SetSize(NULL, l_n, cluster_capacity - l_vacant);
        l_node->offset = l_cv.offset;
    }

    void* ret = Zeta_CircularVector_Insert(&m_cv, cluster_idx);
    --m_vacant;

    Zeta_RelCntRBTreeNode_SetSize(NULL, m_n, cluster_capacity - m_vacant);
    m_node->offset = m_cv.offset;

    return ret;
}

void* Zeta_SegList_PushL(void* sl_) {
    Zeta_SegList* sl = sl_;
    ZETA_DebugAssert(sl != NULL);

    return Zeta_SegList_Insert(sl, 0);
}

void* Zeta_SegList_PushR(void* sl_) {
    Zeta_SegList* sl = sl_;
    ZETA_DebugAssert(sl != NULL);

    size_t size = Zeta_SegList_GetSize(sl);

    return Zeta_SegList_Insert(sl, size);
}

void Zeta_SegList_Erase(void* sl_, size_t idx) {
    Zeta_SegList* sl = sl_;
    ZETA_DebugAssert(sl != NULL);

    size_t width = sl->width;
    size_t cluster_capacity = sl->cluster_capacity;
    size_t size = Zeta_SegList_GetSize(sl);

    ZETA_DebugAssert(idx < size);

    Zeta_BinTreeNodeAccessor btn_accessor = (Zeta_BinTreeNodeAccessor){
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

    Zeta_SegList_Node* m_node;
    void* m_n;
    size_t cluster_idx;

    Zeta_CntBinTree_Access(&m_n, &cluster_idx, &cbtn_opr, sl->root, idx);

    m_node = ZETA_GetStructFromMember(Zeta_SegList_Node, n, m_n);

    Zeta_RawVector m_rv;
    Zeta_Vector m_v;
    Zeta_CircularVector m_cv;

    DeployCircularVector_(width, cluster_capacity, m_node, &m_rv, &m_v, &m_cv);

    Zeta_CircularVector_Erase(&m_rv, cluster_idx);

    size_t m_size = Zeta_CircularVector_GetSize(&m_rv);

    if (cluster_capacity <= m_size * 2) { return; }

    if (m_size == 0) {
        EraseSeg_(sl, m_n);
        return;
    }

    size_t m_vacant = cluster_capacity - m_size;

    void* l_n = Zeta_BinTree_StepL(&btn_accessor, m_n);
    void* r_n = Zeta_BinTree_StepR(&btn_accessor, m_n);

    Zeta_SegList_Node* l_node;
    Zeta_SegList_Node* r_node;

    size_t l_vacant;
    size_t r_vacant;

    if (l_n == NULL) {
        l_node = NULL;
        l_vacant = 0;
    } else {
        l_node = ZETA_GetStructFromMember(Zeta_SegList_Node, n, l_n);
        l_vacant = cluster_capacity - Zeta_RelCntRBTreeNode_GetSize(NULL, l_n);
    }

    if (r_n == NULL) {
        r_node = NULL;
        r_vacant = 0;
    } else {
        r_node = ZETA_GetStructFromMember(Zeta_SegList_Node, n, r_n);
        r_vacant = cluster_capacity - Zeta_RelCntRBTreeNode_GetSize(NULL, r_n);
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

    // if (lr_vacant < cluster_capacity * 1.25) { return; }
    if (lr_vacant * 4 < cluster_capacity * 5) { return; }

    Zeta_RawVector l_rv;
    Zeta_Vector l_v;
    Zeta_CircularVector l_cv;

    DeployCircularVector_(width, cluster_capacity, l_node, &l_rv, &l_v, &l_cv);

    Zeta_RawVector r_rv;
    Zeta_Vector r_v;
    Zeta_CircularVector r_cv;

    DeployCircularVector_(width, cluster_capacity, r_node, &r_rv, &r_v, &r_cv);

    if (l_vacant <= r_vacant) {
        while (r_vacant < cluster_capacity) {
            Zeta_MemCopy(width, Zeta_CircularVector_PushR(&l_cv),
                         Zeta_CircularVector_PeekL(&r_cv));
            --l_vacant;

            Zeta_CircularVector_PopL(&r_cv);
            ++r_vacant;
        }
    } else {
        while (l_vacant < cluster_capacity) {
            Zeta_MemCopy(width, Zeta_CircularVector_PushL(&r_cv),
                         Zeta_CircularVector_PeekR(&l_cv));
            --r_vacant;

            Zeta_CircularVector_PopR(&l_cv);
            ++l_vacant;
        }
    }

    Zeta_RelCntRBTreeNode_SetSize(NULL, l_n, cluster_capacity - l_vacant);
    l_node->offset = l_cv.offset;

    Zeta_RelCntRBTreeNode_SetSize(NULL, r_n, cluster_capacity - r_vacant);
    r_node->offset = r_cv.offset;
}

void Zeta_SegList_PopL(void* sl_) {
    Zeta_SegList* sl = sl_;
    ZETA_DebugAssert(sl != NULL);
}

void Zeta_SegList_PopR(void* sl_) {
    Zeta_SegList* sl = sl_;
    ZETA_DebugAssert(sl != NULL);
}

void Zeta_SegList_ToVector(void* sl_, Zeta_Vector* dst) {
    Zeta_SegList* sl = sl_;
    ZETA_DebugAssert(sl != NULL);

    ZETA_DebugAssert(dst != NULL);

    dst->context = sl;

    dst->GetWidth = Zeta_SegList_GetWidth;
    dst->GetSize = Zeta_SegList_GetSize;
    dst->Access = Zeta_SegList_Access;
    dst->Insert = Zeta_SegList_Insert;
    // dst->PushL = Zeta_SegList_PushL;
    // dst->PushR = Zeta_SegList_PushR;
    dst->Erase = Zeta_SegList_Erase;
    // dst->PopL = Zeta_SegList_PopL;
    // dst->PopR = Zeta_SegList_PopR;
}
