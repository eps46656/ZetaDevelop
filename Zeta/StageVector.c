#include "StageVector.h"

#include "CircularVector.h"
#include "Logger.h"
#include "RBTree.h"
#include "SegUtils.h"
#include "utils.h"

#define ref_color (ZETA_StageVector_ref_color)
#define dat_color (ZETA_StageVector_dat_color)

#define GetNodeColor(n)                               \
    ({                                                \
        ZETA_DebugAssert(sv->lb != n && sv->rb != n); \
        Zeta_OrdCnt3RBTreeNode_GetLColor(NULL, (n));  \
    })

#define SetNodeColor(n, c)                            \
    ZETA_DebugAssert(sv->lb != n && sv->rb != n);     \
    Zeta_OrdCnt3RBTreeNode_SetLColor(NULL, (n), (c)); \
    ZETA_StaticAssert(TRUE)

#define GetAvgCnt(x)                                                    \
    ({                                                                  \
        size_t x_ = (x);                                                \
        size_t seg_cnt = ZETA_CeilIntDiv(x_, seg_capacity);             \
        (x_ + Zeta_SimpleRandomRotate(&rand_seed) % seg_cnt) / seg_cnt; \
    })

static void CheckSV_(void* sv_) {
    Zeta_StageVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    ZETA_DebugAssert(sv->origin != NULL);
    ZETA_DebugAssert(sv->origin->GetWidth != NULL);
    ZETA_DebugAssert(sv->origin->GetStride != NULL);
    ZETA_DebugAssert(sv->origin->GetSize != NULL);
    ZETA_DebugAssert(sv->origin->Access != NULL);
    ZETA_DebugAssert(sv->origin->Read != NULL);

    size_t width = sv->origin->GetWidth(sv->origin->context);
    size_t stride = sv->origin->GetStride(sv->origin->context);

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);
    ZETA_DebugAssert(sv->seg_capacity <= ZETA_RangeMaxOf(unsigned short));

    ZETA_DebugAssert(sv->node_allocator != NULL);
    ZETA_DebugAssert(sv->node_allocator->Allocate != NULL);
    ZETA_DebugAssert(sv->node_allocator->Deallocate != NULL);

    ZETA_DebugAssert(sv->data_allocator != NULL);
    ZETA_DebugAssert(sv->data_allocator->Allocate != NULL);
    ZETA_DebugAssert(sv->data_allocator->Deallocate != NULL);
}

static Zeta_StageVector_Node* AllocateNode_(Zeta_StageVector* sv) {
    ZETA_DebugAssert(sv->node_allocator != NULL);
    ZETA_DebugAssert(sv->node_allocator->Allocate != NULL);

    void* node_ = sv->node_allocator->Allocate(sv->node_allocator->context,
                                               sizeof(Zeta_StageVector_Node));
    ZETA_DebugAssert(node_ != NULL);
    ZETA_DebugAssert(
        __builtin_is_aligned(node_, alignof(Zeta_StageVector_Node)));

    Zeta_StageVector_Node* node = node_;

    Zeta_OrdCnt3RBTreeNode_Init(NULL, &node->n);

    return node;
}

static void* AllocateData_(Zeta_StageVector* sv) {
    ZETA_DebugAssert(sv->data_allocator != NULL);
    ZETA_DebugAssert(sv->data_allocator->Allocate != NULL);

    void* data = sv->data_allocator->Allocate(
        sv->data_allocator->context,
        Zeta_StageVector_GetStride(sv) * sv->seg_capacity);
    ZETA_DebugAssert(data != NULL);

    return data;
}

static Zeta_StageVector_Node* AllocateRefNode_(Zeta_StageVector* sv) {
    Zeta_StageVector_Node* node = AllocateNode_(sv);

    Zeta_OrdCnt3RBTreeNode_Init(NULL, &node->n);
    Zeta_OrdCnt3RBTreeNode_SetAccSize(NULL, &node->n, 0);

    SetNodeColor(&node->n, ref_color);

    node->ref.beg = 0;
    node->ref.size = 0;

    return node;
}

static Zeta_StageVector_Node* AllocateDatNode_(Zeta_StageVector* sv) {
    Zeta_StageVector_Node* node = AllocateNode_(sv);

    Zeta_OrdCnt3RBTreeNode_Init(NULL, &node->n);
    Zeta_OrdCnt3RBTreeNode_SetAccSize(NULL, &node->n, 0);

    SetNodeColor(&node->n, dat_color);

    node->dat.data = AllocateData_(sv);

    node->dat.offset = 0;
    node->dat.size = 0;

    return node;
}

static void TransRefSegToDatSeg_(Zeta_StageVector* sv,
                                 Zeta_StageVector_Node* node) {
    unsigned char* data = AllocateData_(sv);

    size_t size = node->ref.size;

    Zeta_Cursor origin_cursor;
    sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                       node->ref.beg);
    sv->origin->Read(sv->origin->context, &origin_cursor, size, data, NULL);

    SetNodeColor(&node->n, dat_color);

    node->dat.data = data;
    node->dat.offset = 0;
    node->dat.size = size;
}

static void* SplitRefSeg_(Zeta_StageVector* sv,
                          Zeta_BinTreeNodeOperator* btn_opr, size_t width,
                          size_t stride, void* l_n, Zeta_StageVector_Node* node,
                          size_t seg_idx) {
    if (seg_idx == 0) { return l_n; }

    size_t seg_capacity = sv->seg_capacity;

    Zeta_StageVector_Node* l_node;
    size_t l_size;

    if (sv->lb == l_n) { goto L1; }

    l_node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, l_n);
    l_size =
        GetNodeColor(l_n) == ref_color ? l_node->ref.size : l_node->dat.size;

    if (seg_capacity < l_size + seg_idx) { goto L1; }

    goto L2;

L1:;

    Zeta_StageVector_Node* new_node = AllocateRefNode_(sv);
    sv->root = Zeta_RBTree_Insert(btn_opr, l_n, &node->n, &new_node->n);

    new_node->ref.beg = node->ref.beg;
    new_node->ref.size = seg_idx;

    node->ref.beg += seg_idx;
    node->ref.size -= seg_idx;

    return &new_node->n;

L2:;

    if (GetNodeColor(l_n) == ref_color) {
        unsigned char* data = AllocateData_(sv);

        Zeta_Cursor origin_cursor;
        sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                           l_node->ref.beg);
        sv->origin->Read(sv->origin->context, &origin_cursor, l_size, data,
                         NULL);
        sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                           node->ref.beg);
        sv->origin->Read(sv->origin->context, &origin_cursor, seg_idx,
                         data + stride * l_size, NULL);

        SetNodeColor(l_n, dat_color);

        l_node->dat.data = data;
        l_node->dat.offset = 0;
        l_node->dat.size = l_size + seg_idx;
    } else {
        Zeta_CircularVector cv;
        cv.data = l_node->dat.data;
        cv.width = width;
        cv.stride = stride;
        cv.offset = l_node->dat.offset;
        cv.size = l_size;
        cv.capacity = seg_capacity;

        Zeta_CircularVector_Cursor cv_cursor;
        Zeta_CircularVector_GetRBCursor(&cv, &cv_cursor);
        Zeta_CircularVector_Insert(&cv, &cv_cursor, seg_idx);

        Zeta_Cursor origin_cursor;

        sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                           node->ref.beg);

        for (size_t i = 0; i < seg_idx; ++i) {
            sv->origin->Read(
                sv->origin->context, &origin_cursor, 1,
                Zeta_CircularVector_Access(&cv, NULL, NULL, l_size + i),
                &origin_cursor);
        }

        l_node->dat.offset = cv.offset;
        l_node->dat.size = cv.size;
    }

    node->ref.beg += seg_idx;
    node->ref.size -= seg_idx;

    return l_n;
}

static void InitTree_(Zeta_StageVector* sv) {
    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_OrdCnt3RBTreeNode_Init(NULL, sv->lb);
    Zeta_OrdCnt3RBTreeNode_Init(NULL, sv->rb);

    Zeta_OrdCnt3RBTreeNode_SetAccSize(NULL, sv->lb, 1);
    Zeta_OrdCnt3RBTreeNode_SetAccSize(NULL, sv->rb, 1);

    void* root = NULL;

    root = Zeta_RBTree_InsertR(&btn_opr, root, sv->lb);
    root = Zeta_RBTree_InsertR(&btn_opr, root, sv->rb);

    sv->root = root;
}

void Zeta_StageVector_Init(void* sv_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    void* lb_ = sv->node_allocator->Allocate(sv->node_allocator->context,
                                             sizeof(Zeta_OrdCnt3RBTreeNode));
    void* rb_ = sv->node_allocator->Allocate(sv->node_allocator->context,
                                             sizeof(Zeta_OrdCnt3RBTreeNode));

    ZETA_DebugAssert(lb_ != NULL);
    ZETA_DebugAssert(rb_ != NULL);

    ZETA_DebugAssert(
        __builtin_is_aligned(lb_, alignof(Zeta_OrdCnt3RBTreeNode)));
    ZETA_DebugAssert(
        __builtin_is_aligned(rb_, alignof(Zeta_OrdCnt3RBTreeNode)));

    sv->lb = lb_;
    sv->rb = rb_;

    InitTree_(sv);

    size_t origin_size = sv->origin->GetSize(sv->origin->context);

    if (origin_size == 0) { return; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_StageVector_Node* node = AllocateRefNode_(sv);
    sv->root = Zeta_RBTree_Insert(&btn_opr, sv->lb, sv->rb, &node->n);

    node->ref.beg = 0;
    node->ref.size = origin_size;

    Zeta_BinTree_SetSize(&btn_opr, &node->n, origin_size);
}

size_t Zeta_StageVector_GetWidth(void* sv_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    return sv->origin->GetWidth(sv->origin->context);
}

size_t Zeta_StageVector_GetStride(void* sv_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    return sv->origin->GetStride(sv->origin->context);
}

size_t Zeta_StageVector_GetSize(void* sv_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    return Zeta_OrdCnt3RBTreeNode_GetAccSize(NULL, sv->root) - 2;
}

void Zeta_StageVector_GetLBCursor(void* sv_, void* dst_cursor_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* dst_cursor = dst_cursor_;

    if (dst_cursor == NULL) { return; }

    dst_cursor->sv = sv;
    dst_cursor->idx = -1;
    dst_cursor->n = sv->lb;
    dst_cursor->seg_idx = 0;
    dst_cursor->ele = NULL;
}

void Zeta_StageVector_GetRBCursor(void* sv_, void* dst_cursor_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* dst_cursor = dst_cursor_;

    if (dst_cursor == NULL) { return; }

    dst_cursor->sv = sv;
    dst_cursor->idx = Zeta_StageVector_GetSize(sv);
    dst_cursor->n = sv->rb;
    dst_cursor->seg_idx = 0;
    dst_cursor->ele = NULL;
}

void* Zeta_StageVector_PeekL(void* sv_, void* dst_cursor_, void* dst_ele) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = Zeta_StageVector_GetWidth(sv);
    size_t stride = Zeta_StageVector_GetStride(sv);
    size_t seg_capacity = sv->seg_capacity;

    void* n = Zeta_BinTree_StepR(&btn_opr, sv->lb);

    if (sv->rb == n) {
        if (dst_cursor != NULL) {
            dst_cursor->sv = sv;
            dst_cursor->idx = 0;
            dst_cursor->n = n;
            dst_cursor->seg_idx = 0;
            dst_cursor->ele = NULL;
        }

        return NULL;
    }

    Zeta_StageVector_Node* node =
        ZETA_MemberToStruct(Zeta_StageVector_Node, n, n);

    void* ele = NULL;

    if (GetNodeColor(n) == ref_color) {
        sv->origin->Access(sv->origin->context, NULL, dst_ele, node->ref.beg);
    } else {
        Zeta_CircularVector cv;
        cv.data = node->dat.data;
        cv.width = width;
        cv.stride = stride;
        cv.offset = node->dat.offset;
        cv.size = node->dat.size;
        cv.capacity = seg_capacity;

        ele = Zeta_CircularVector_PeekL(&cv, NULL, dst_ele);
    }

    if (dst_cursor != NULL) {
        dst_cursor->sv = sv;
        dst_cursor->idx = 0;
        dst_cursor->n = n;
        dst_cursor->seg_idx = 0;
        dst_cursor->ele = ele;
    }

    return ele;
}

void* Zeta_StageVector_PeekR(void* sv_, void* dst_cursor_, void* dst_ele) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = Zeta_StageVector_GetWidth(sv);
    size_t stride = Zeta_StageVector_GetStride(sv);
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_StageVector_GetSize(sv);

    void* n = Zeta_BinTree_StepL(&btn_opr, sv->rb);

    if (sv->lb == n) {
        if (dst_cursor != NULL) {
            dst_cursor->sv = sv;
            dst_cursor->idx = -1;
            dst_cursor->n = n;
            dst_cursor->seg_idx = 0;
            dst_cursor->ele = NULL;
        }

        return NULL;
    }

    Zeta_StageVector_Node* node =
        ZETA_MemberToStruct(Zeta_StageVector_Node, n, n);

    void* ele = NULL;

    size_t node_size;

    if (GetNodeColor(n) == ref_color) {
        node_size = node->ref.size;

        sv->origin->Access(sv->origin->context, NULL, dst_ele,
                           node->ref.beg + node_size - 1);
    } else {
        node_size = node->dat.size;

        Zeta_CircularVector cv;
        cv.data = node->dat.data;
        cv.width = width;
        cv.stride = stride;
        cv.offset = node->dat.offset;
        cv.size = node_size;
        cv.capacity = seg_capacity;

        ele = Zeta_CircularVector_PeekR(&cv, NULL, dst_ele);
    }

    if (dst_cursor != NULL) {
        dst_cursor->sv = sv;
        dst_cursor->idx = size - 1;
        dst_cursor->n = n;
        dst_cursor->seg_idx = node_size - 1;
        dst_cursor->ele = ele;
    }

    return ele;
}

void* Zeta_StageVector_Access(void* sv_, void* dst_cursor_, void* dst_ele,
                              size_t idx) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = Zeta_StageVector_GetWidth(sv);
    size_t stride = Zeta_StageVector_GetStride(sv);
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_StageVector_GetSize(sv);

    ZETA_DebugAssert(idx + 1 < size + 2);

    void* n;
    Zeta_StageVector_Node* node;
    size_t seg_idx;

    Zeta_BinTree_AccessL(&n, &seg_idx, &btn_opr, sv->root, idx + 1);

    void* ele = NULL;

    if (idx < size) {
        node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, n);

        if (GetNodeColor(n) == dat_color) {
            Zeta_CircularVector cv;
            cv.data = node->dat.data;
            cv.width = width;
            cv.stride = stride;
            cv.offset = node->dat.offset;
            cv.size = node->dat.size;
            cv.capacity = seg_capacity;

            ele = Zeta_CircularVector_Access(&cv, NULL, dst_ele, seg_idx);
        } else if (dst_ele != NULL) {
            sv->origin->Access(sv->origin->context, NULL, dst_ele,
                               node->ref.beg + seg_idx);
        }
    }

    if (dst_cursor != NULL) {
        dst_cursor->sv = sv;
        dst_cursor->idx = idx;
        dst_cursor->n = n;
        dst_cursor->seg_idx = seg_idx;
        dst_cursor->ele = ele;
    }

    return ele;
}

void* Zeta_StageVector_Refer(void* sv_, void* pos_cursor_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* pos_cursor = pos_cursor_;
    Zeta_StageVector_Cursor_Check(sv, pos_cursor);

    return pos_cursor->ele;
}

void Zeta_StageVector_Read(void* sv_, void* pos_cursor_, size_t cnt, void* dst_,
                           void* dst_cursor_) {
    Zeta_StageVector* sv = sv_;
    Zeta_StageVector_Cursor* pos_cursor = pos_cursor_;

    Zeta_StageVector_Cursor_Check(sv, pos_cursor);

    Zeta_StageVector_Cursor* dst_cursor = dst_cursor_;

    if (cnt == 0) {
        if (dst_cursor != NULL) {
            dst_cursor->sv = sv;
            dst_cursor->idx = pos_cursor->idx;
            dst_cursor->n = pos_cursor->n;
            dst_cursor->seg_idx = pos_cursor->seg_idx;
            dst_cursor->ele = pos_cursor->ele;
        }

        return;
    }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = Zeta_StageVector_GetWidth(sv);
    size_t stride = Zeta_StageVector_GetStride(sv);
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_StageVector_GetSize(sv);

    size_t end = pos_cursor->idx + cnt;
    ZETA_DebugAssert(end <= size);

    unsigned char* dst = dst_;
    ZETA_DebugAssert(dst);

    void* n = pos_cursor->n;
    Zeta_StageVector_Node* node;

    size_t seg_idx = pos_cursor->seg_idx;

    Zeta_Cursor origin_cursor;

    Zeta_CircularVector cv;
    cv.width = width;
    cv.stride = stride;
    cv.capacity = seg_capacity;

    while (0 < cnt) {
        node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, n);

        size_t cur_cnt;
        size_t node_size;

        if (GetNodeColor(n) == ref_color) {
            node_size = node->ref.size;
            cur_cnt = ZETA_GetMinOf(cnt, node_size - seg_idx);

            sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                               node->ref.beg + seg_idx);
            sv->origin->Read(sv->origin->context, &origin_cursor, cur_cnt, dst,
                             NULL);
        } else {
            node_size = node->dat.size;
            cur_cnt = ZETA_GetMinOf(cnt, node_size - seg_idx);

            cv.data = node->dat.data;
            cv.offset = node->dat.offset;
            cv.size = node_size;

            Zeta_CircularVector_Cursor cv_cursor;
            Zeta_CircularVector_Access(&cv, &cv_cursor, NULL, seg_idx);
            Zeta_CircularVector_Read(&cv, &cv_cursor, cur_cnt, dst, NULL);
        }

        dst += stride * cur_cnt;

        seg_idx += cur_cnt;
        cnt -= cur_cnt;

        if (seg_idx == node_size) {
            n = Zeta_BinTree_StepR(&btn_opr, n);
            seg_idx = 0;
        }
    }

    if (dst_cursor == NULL) { return; }

    dst_cursor->sv = sv;
    dst_cursor->idx = end;
    dst_cursor->n = n;
    dst_cursor->seg_idx = seg_idx;

    if (sv->rb == n || GetNodeColor(n) == ref_color) {
        dst_cursor->ele = NULL;
        return;
    }

    node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, n);

    cv.data = node->dat.data;
    cv.offset = node->dat.offset;
    cv.size = node->dat.size;

    dst_cursor->ele = Zeta_CircularVector_Access(&cv, NULL, NULL, seg_idx);
}

void Zeta_StageVector_Write(void* sv_, void* pos_cursor_, size_t cnt,
                            void const* src_, void* dst_cursor_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* pos_cursor = pos_cursor_;
    Zeta_StageVector_Cursor_Check(sv, pos_cursor);

    Zeta_StageVector_Cursor* dst_cursor = dst_cursor_;

    if (cnt == 0) {
        if (dst_cursor != NULL) {
            dst_cursor->sv = sv;
            dst_cursor->idx = pos_cursor->idx;
            dst_cursor->n = pos_cursor->n;
            dst_cursor->seg_idx = pos_cursor->seg_idx;
            dst_cursor->ele = pos_cursor->ele;
        }

        return;
    }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = Zeta_StageVector_GetWidth(sv);
    size_t stride = Zeta_StageVector_GetStride(sv);
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_StageVector_GetSize(sv);

    size_t end = pos_cursor->idx + cnt;
    ZETA_DebugAssert(end <= size);

    unsigned char const* src = src_;
    ZETA_DebugAssert(src != NULL);

    size_t seg_idx = pos_cursor->seg_idx;

    void* n = pos_cursor->n;

    Zeta_StageVector_Node* node =
        ZETA_MemberToStruct(Zeta_StageVector_Node, n, n);

    void* l_n = Zeta_BinTree_StepL(&btn_opr, n);

    if (GetNodeColor(n) == ref_color) {
        l_n = SplitRefSeg_(sv, &btn_opr, width, stride, l_n, node, seg_idx);

        if (sv->lb != l_n) {
            Zeta_StageVector_Node* l_node =
                ZETA_MemberToStruct(Zeta_StageVector_Node, n, l_n);

            Zeta_BinTree_SetSize(&btn_opr, l_n,
                                 GetNodeColor(l_n) == ref_color
                                     ? l_node->ref.size
                                     : l_node->dat.size);
        }

        seg_idx = 0;
    }

    Zeta_CircularVector cv;
    cv.width = width;
    cv.stride = stride;
    cv.capacity = seg_capacity;

    while (0 < cnt) {
        node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, n);

        if (GetNodeColor(n) == dat_color) {
            cv.data = node->dat.data;
            cv.offset = node->dat.offset;
            cv.size = node->dat.size;

            size_t cur_cnt = ZETA_GetMinOf(cnt, cv.size - seg_idx);

            Zeta_CircularVector cv_cursor;
            Zeta_CircularVector_Access(&cv, &cv_cursor, NULL, seg_idx);
            Zeta_CircularVector_Write(&cv, &cv_cursor, cur_cnt, src, NULL);

            src += stride * cur_cnt;

            seg_idx += cur_cnt;
            cnt -= cur_cnt;

            if (seg_idx == node->dat.size) {
                l_n = n;
                n = Zeta_BinTree_StepR(&btn_opr, n);
                seg_idx = 0;
            }

            continue;
        }

        void* r_n = NULL;

        size_t node_size = node->ref.size;

        if (seg_capacity < node_size) {
            size_t split_cnt = ZETA_CeilIntDiv(
                node_size, ZETA_CeilIntDiv(node_size, seg_capacity));

            Zeta_StageVector_Node* new_node = AllocateRefNode_(sv);

            sv->root = Zeta_RBTree_Insert(&btn_opr, l_n, n, &new_node->n);

            new_node->ref.beg = node->ref.beg;
            new_node->ref.size = split_cnt;

            node->ref.beg += split_cnt;
            node->ref.size -= split_cnt;

            r_n = n;

            node = new_node;
            n = &new_node->n;

            node_size = node->ref.size;
        }

        Zeta_BinTree_SetSize(&btn_opr, &node->n, node_size);

        size_t cur_cnt = ZETA_GetMinOf(cnt, node_size);

        unsigned char* data = AllocateData_(sv);

        Zeta_MemCopy(data, src, stride * cur_cnt);

        src += stride * cur_cnt;

        SetNodeColor(n, dat_color);

        if (cur_cnt == node_size) {
            node->dat.data = data;
            node->dat.offset = 0;
            node->dat.size = node_size;

            cnt -= cur_cnt;
            l_n = n;
            n = r_n == NULL ? Zeta_BinTree_StepR(&btn_opr, n) : r_n;
            continue;
        }

        Zeta_Cursor origin_cursor;
        sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                           node->ref.beg + cur_cnt);

        sv->origin->Read(sv->origin->context, &origin_cursor,
                         node_size - cur_cnt, data + stride * cur_cnt, NULL);

        node->dat.data = data;
        node->dat.offset = 0;
        node->dat.size = node_size;

        if (r_n != NULL) {
            Zeta_StageVector_Node* r_node =
                ZETA_MemberToStruct(Zeta_StageVector_Node, n, r_n);

            Zeta_BinTree_SetSize(&btn_opr, r_n,
                                 GetNodeColor(r_n) == ref_color
                                     ? r_node->ref.size
                                     : r_node->dat.size);
        }

        seg_idx = cur_cnt;

        break;
    }

    if (sv->rb != n) {
        Zeta_StageVector_Node* node =
            ZETA_MemberToStruct(Zeta_StageVector_Node, n, n);

        Zeta_BinTree_SetSize(
            &btn_opr, n,
            GetNodeColor(n) == ref_color ? node->ref.size : node->dat.size);
    }

    if (dst_cursor == NULL) { return; }

    dst_cursor->sv = sv;
    dst_cursor->idx = end;
    dst_cursor->n = n;
    dst_cursor->seg_idx = seg_idx;

    if (sv->rb == n || GetNodeColor(n) == ref_color) {
        dst_cursor->ele = NULL;
        return;
    }

    node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, n);

    cv.data = node->dat.data;
    cv.offset = node->dat.offset;
    cv.size = node->dat.size;

    dst_cursor->ele = Zeta_CircularVector_Access(&cv, NULL, NULL, seg_idx);
}

static void* EraseSeg_(Zeta_StageVector* sv, Zeta_BinTreeNodeOperator* btn_opr,
                       Zeta_StageVector_Node* node) {
    ZETA_DebugAssert(sv->node_allocator != NULL);
    ZETA_DebugAssert(sv->data_allocator != NULL);

    ZETA_DebugAssert(sv->node_allocator->Deallocate != NULL);
    ZETA_DebugAssert(sv->data_allocator->Deallocate != NULL);

    void* n = &node->n;

    void* ret = Zeta_BinTree_StepR(btn_opr, n);

    sv->root = Zeta_RBTree_Extract(btn_opr, n);

    if (GetNodeColor(n) == dat_color) {
        sv->data_allocator->Deallocate(sv->data_allocator->context,
                                       node->dat.data);
    }

    sv->node_allocator->Deallocate(sv->node_allocator->context, node);

    return ret;
}

void* Zeta_StageVector_PushL(void* sv_, void* dst_cursor_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_StageVector_Cursor pos_cursor;
    Zeta_StageVector_PeekL(sv, &pos_cursor, NULL);

    Zeta_StageVector_Insert(sv, &pos_cursor, 1);

    if (dst_cursor != NULL) {
        dst_cursor->sv = sv;
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->n = pos_cursor.n;
        dst_cursor->seg_idx = pos_cursor.seg_idx;
        dst_cursor->ele = pos_cursor.ele;
    }

    return pos_cursor.ele;
}

void* Zeta_StageVector_PushR(void* sv_, void* dst_cursor_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_StageVector_Cursor pos_cursor;
    Zeta_StageVector_GetRBCursor(sv, &pos_cursor);

    Zeta_StageVector_Insert(sv, &pos_cursor, 1);

    if (dst_cursor != NULL) {
        dst_cursor->sv = sv;
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->n = pos_cursor.n;
        dst_cursor->seg_idx = pos_cursor.seg_idx;
        dst_cursor->ele = pos_cursor.ele;
    }

    return pos_cursor.ele;
}

void PushRefL_(Zeta_StageVector* sv, Zeta_CircularVector* cv, size_t beg,
               size_t size) {
    Zeta_CircularVector_Cursor cv_cursor;
    Zeta_CircularVector_PeekL(cv, &cv_cursor, NULL);
    Zeta_CircularVector_Insert(cv, &cv_cursor, size);

    Zeta_Cursor origin_cursor;
    sv->origin->Access(sv->origin->context, &origin_cursor, NULL, beg);

    for (size_t i = 0; i < size; ++i) {
        sv->origin->Read(sv->origin->context, &origin_cursor, 1,
                         Zeta_CircularVector_Access(cv, NULL, NULL, i),
                         &origin_cursor);
    }
}

void PushRefR_(Zeta_StageVector* sv, Zeta_CircularVector* cv, size_t beg,
               size_t size) {
    size_t cv_origin_size = cv->size;

    Zeta_CircularVector_Cursor cv_cursor;
    Zeta_CircularVector_GetRBCursor(cv, &cv_cursor);
    Zeta_CircularVector_Insert(cv, &cv_cursor, size);

    Zeta_Cursor origin_cursor;
    sv->origin->Access(sv->origin->context, &origin_cursor, NULL, beg);

    for (size_t i = 0; i < size; ++i) {
        sv->origin->Read(
            sv->origin->context, &origin_cursor, 1,
            Zeta_CircularVector_Access(cv, NULL, NULL, cv_origin_size + i),
            &origin_cursor);
    }
}

/*
 * ret  0: no merge
 * ret -1: merge to l_n, r_n is empty.
 * ret  1: merge to r_n, l_n is empty.
 */
int TryMerge_(Zeta_StageVector* sv, size_t width, size_t stride, void* l_n,
              void* r_n) {
    if (sv->lb == l_n || sv->rb == r_n) { return 0; }

    size_t seg_capacity = sv->seg_capacity;

    Zeta_StageVector_Node* l_node =
        ZETA_MemberToStruct(Zeta_StageVector_Node, n, l_n);

    Zeta_StageVector_Node* r_node =
        ZETA_MemberToStruct(Zeta_StageVector_Node, n, r_n);

    int l_c = GetNodeColor(l_n);
    int r_c = GetNodeColor(r_n);

    size_t l_size = l_c == ref_color ? l_node->ref.size : l_node->dat.size;
    size_t r_size = r_c == ref_color ? r_node->ref.size : r_node->dat.size;

    if (seg_capacity < l_size + r_size) { return 0; }

    size_t rand_seed = ZETA_PtrToAddr(l_n) + ZETA_PtrToAddr(r_n);

    bool_t l_to_r = l_c != r_c ? l_c == ref_color
                    : l_size != r_size
                        ? l_size < r_size
                        : Zeta_SimpleRandomRotate(&rand_seed) % 2 == 0;

    if (l_c == ref_color && r_c == ref_color) {
        unsigned char* data = AllocateData_(sv);

        Zeta_Cursor origin_cursor;

        sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                           l_node->ref.beg);
        sv->origin->Read(sv->origin->context, &origin_cursor, l_node->ref.size,
                         data, NULL);

        sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                           r_node->ref.beg);
        sv->origin->Read(sv->origin->context, &origin_cursor, r_node->ref.size,
                         data + stride * l_node->ref.size, NULL);

        if (l_to_r) {
            SetNodeColor(r_n, dat_color);

            r_node->dat.size = l_size + r_size;
            r_node->dat.data = data;
            r_node->dat.offset = 0;

            l_node->ref.size = 0;
        } else {
            SetNodeColor(l_n, dat_color);

            l_node->dat.size = l_size + r_size;
            l_node->dat.data = data;
            l_node->dat.offset = 0;

            r_node->ref.size = 0;
        }

        return l_to_r ? 1 : -1;
    }

    Zeta_CircularVector l_cv;
    l_cv.width = width;
    l_cv.stride = stride;

    if (l_c == ref_color) {
        l_cv.data = NULL;
        l_cv.offset = 0;
        l_cv.size = 0;
        l_cv.capacity = 0;
    } else {
        l_cv.data = l_node->dat.data;
        l_cv.offset = l_node->dat.offset;
        l_cv.size = l_node->dat.size;
        l_cv.capacity = seg_capacity;
    }

    Zeta_CircularVector r_cv;
    r_cv.width = width;
    r_cv.stride = stride;

    if (r_c == ref_color) {
        r_cv.data = NULL;
        r_cv.offset = 0;
        r_cv.size = 0;
        r_cv.capacity = 0;
    } else {
        r_cv.data = r_node->dat.data;
        r_cv.offset = r_node->dat.offset;
        r_cv.size = r_node->dat.size;
        r_cv.capacity = seg_capacity;
    }

    if (l_to_r) {
        if (l_c == dat_color) {  // l_n is dat, r_n is dat
            SegShoveR(&l_cv, &r_cv, 0, 0, l_size);

            l_node->dat.offset = 0;
            l_node->dat.size = 0;
        } else {  // l_n is ref, r_n is dat
            Zeta_CircularVector_Cursor cv_cursor;
            Zeta_CircularVector_PeekL(&r_cv, &cv_cursor, NULL);
            Zeta_CircularVector_Insert(&r_cv, &cv_cursor, l_size);

            Zeta_Cursor origin_cursor;

            sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                               l_node->ref.beg);

            for (size_t i = 0; i < l_size; ++i) {
                sv->origin->Read(
                    sv->origin->context, &origin_cursor, 1,
                    Zeta_CircularVector_Access(&r_cv, NULL, NULL, i),
                    &origin_cursor);
            }

            l_node->ref.size = 0;
        }

        r_node->dat.offset = r_cv.offset;
        r_node->dat.size = r_cv.size;
    } else {
        if (r_c == dat_color) {  // l_n is dat, r_n is dat
            SegShoveL(&l_cv, &r_cv, 0, 0, r_size);

            r_node->dat.offset = 0;
            r_node->dat.size = 0;
        } else {  // l_n is dat, r_n is ref
            Zeta_CircularVector_Cursor cv_cursor;
            Zeta_CircularVector_GetRBCursor(&l_cv, &cv_cursor);
            Zeta_CircularVector_Insert(&l_cv, &cv_cursor, r_size);

            Zeta_Cursor origin_cursor;

            sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                               r_node->ref.beg);

            for (size_t i = 0; i < r_size; ++i) {
                sv->origin->Read(
                    sv->origin->context, &origin_cursor, 1,
                    Zeta_CircularVector_Access(&l_cv, NULL, NULL, l_size + i),
                    &origin_cursor);
            }

            r_node->ref.size = 0;
        }

        l_node->dat.offset = l_cv.offset;
        l_node->dat.size = l_cv.size;
    }

    return l_to_r ? 1 : -1;
}

static void Merge2_(Zeta_StageVector* sv, size_t width, size_t stride,
                    Zeta_StageVector_Node* a_node,
                    Zeta_StageVector_Node* b_node) {
    int a_c = GetNodeColor(&a_node->n);
    int b_c = GetNodeColor(&b_node->n);

    size_t a_size = a_c == ref_color ? a_node->ref.size : a_node->dat.size;
    size_t b_size = b_c == ref_color ? b_node->ref.size : b_node->dat.size;

    size_t total_size = a_size + b_size;

    size_t seg_capacity = sv->seg_capacity;

    size_t rand_seed = ZETA_PtrToAddr(a_node) + ZETA_PtrToAddr(b_node);

    if (a_c == ref_color && b_c == ref_color) {
        unsigned char* data = AllocateData_(sv);

        Zeta_Cursor origin_cursor;

        sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                           a_node->ref.beg);
        sv->origin->Read(sv->origin->context, &origin_cursor, a_size, data,
                         NULL);
        data += stride * a_size;

        sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                           b_node->ref.beg);
        sv->origin->Read(sv->origin->context, &origin_cursor, b_node->ref.size,
                         data, NULL);

        a_node->ref.size = 0;
        b_node->ref.size = 0;

        Zeta_StageVector_Node* dst_node;

        switch (Zeta_SimpleRandomRotate(&rand_seed) % 2) {
            case 0: dst_node = a_node; break;
            case 1: dst_node = b_node; break;
        }

        SetNodeColor(&dst_node->n, dat_color);
        dst_node->dat.data = data;
        dst_node->dat.size = total_size;

        return;
    }

    Zeta_CircularVector a_cv;
    a_cv.data = a_node->dat.data;
    a_cv.width = width;
    a_cv.stride = stride;
    a_cv.offset = a_node->dat.offset;
    a_cv.size = a_node->dat.size;
    a_cv.capacity = seg_capacity;

    Zeta_CircularVector b_cv;
    b_cv.data = b_node->dat.data;
    b_cv.width = width;
    b_cv.stride = stride;
    b_cv.offset = b_node->dat.offset;
    b_cv.size = b_node->dat.size;
    b_cv.capacity = seg_capacity;

    size_t a_score = a_c == ref_color ? 0 : a_size;
    size_t b_score = b_c == ref_color ? 0 : b_size;

    if (a_score < b_score) { goto B_MERGE; }
    if (b_score < a_score) { goto A_MERGE; }

    if (Zeta_SimpleRandomRotate(&rand_seed) % 2 == 0) {
        goto A_MERGE;
    } else {
        goto B_MERGE;
    }

A_MERGE: {
    if (b_c == ref_color) {
        PushRefR_(sv, &a_cv, b_node->ref.beg, b_size);
    } else {
        SegShoveL(&a_cv, &b_cv, 0, 0, b_size);
    }

    goto MERGE_END;
}

B_MERGE: {
    if (a_c == ref_color) {
        PushRefL_(sv, &b_cv, a_node->ref.beg, a_size);
    } else {
        SegShoveR(&a_cv, &b_cv, 0, 0, a_size);
    }

    goto MERGE_END;
}

MERGE_END:;

    if (a_c == ref_color) {
        a_node->ref.size = 0;
    } else {
        a_node->dat.offset = a_cv.offset;
        a_node->dat.size = a_cv.size;
    }

    if (b_c == ref_color) {
        b_node->ref.size = 0;
    } else {
        b_node->dat.offset = b_cv.offset;
        b_node->dat.size = b_cv.size;
    }
}

void* Zeta_StageVector_Insert(void* sv_, void* pos_cursor_, size_t cnt) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* pos_cursor = pos_cursor_;
    Zeta_StageVector_Cursor_Check(sv, pos_cursor);

    if (cnt == 0) { return pos_cursor->ele; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = Zeta_StageVector_GetWidth(sv);
    size_t stride = Zeta_StageVector_GetStride(sv);
    size_t seg_capacity = sv->seg_capacity;

    void* m_n = pos_cursor->n;
    size_t seg_idx = pos_cursor->seg_idx;

    ZETA_DebugAssert(sv->lb != m_n);

    size_t rand_seed = ZETA_PtrToAddr(m_n);

    Zeta_StageVector_Node* m_node;

    if (sv->rb != m_n && GetNodeColor(m_n) == dat_color) {
        m_node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, m_n);
    } else {
        void* l_n = Zeta_BinTree_StepL(&btn_opr, m_n);

        if (0 < seg_idx) {
            m_node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, m_n);

            l_n =
                SplitRefSeg_(sv, &btn_opr, width, stride, l_n, m_node, seg_idx);

            seg_idx = 0;

            Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->ref.size);
        }

        if (sv->lb == l_n || GetNodeColor(l_n) == ref_color) {
            if (sv->lb != l_n) {
                Zeta_BinTree_SetSize(
                    &btn_opr, l_n,
                    ZETA_MemberToStruct(Zeta_StageVector_Node, n, l_n)
                        ->ref.size);
            }

            if (sv->rb != m_n) {
                size_t post_cnt = cnt % ZETA_CeilIntDiv(cnt, seg_capacity);

                if (post_cnt + m_node->ref.size <= seg_capacity) {
                    TransRefSegToDatSeg_(sv, m_node);

                    m_node->dat.offset =
                        (m_node->dat.offset + seg_capacity - post_cnt) %
                        seg_capacity;

                    m_node->dat.size += post_cnt;

                    Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->dat.size);

                    cnt -= post_cnt;
                }
            }

            while (0 < cnt) {
                size_t cur_cnt = GetAvgCnt(cnt);

                Zeta_StageVector_Node* new_node = AllocateDatNode_(sv);
                sv->root = Zeta_RBTree_Insert(&btn_opr, l_n, m_n, &new_node->n);

                m_node = new_node;
                m_n = &m_node->n;

                m_node->dat.size = cur_cnt;
                Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->dat.size);

                cnt -= cur_cnt;
            }

            pos_cursor->n = m_n;
            pos_cursor->seg_idx = 0;
            pos_cursor->ele = m_node->dat.data;

            return pos_cursor->ele;
        }

        m_n = l_n;
        m_node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, m_n);

        seg_idx = m_node->dat.size;
    }

    Zeta_CircularVector_Cursor cv_cursor;

    Zeta_CircularVector m_cv;
    m_cv.data = m_node->dat.data;
    m_cv.width = width;
    m_cv.stride = stride;
    m_cv.offset = m_node->dat.offset;
    m_cv.size = m_node->dat.size;
    m_cv.capacity = seg_capacity;

    size_t m_vacant = seg_capacity - m_cv.size;

    if (cnt <= m_vacant) {
        Zeta_CircularVector_Access(&m_cv, &cv_cursor, NULL, seg_idx);

        void* ele = Zeta_CircularVector_Insert(&m_cv, &cv_cursor, cnt);

        m_node->dat.offset = m_cv.offset;
        m_node->dat.size = m_cv.size;
        Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->dat.size);

        pos_cursor->n = m_n;
        pos_cursor->seg_idx = seg_idx;
        pos_cursor->ele = ele;

        return ele;
    }

    size_t ml_cnt = seg_idx;
    size_t mr_cnt = m_cv.size - seg_idx;

    void* l_n = Zeta_BinTree_StepL(&btn_opr, m_n);
    void* r_n = Zeta_BinTree_StepR(&btn_opr, m_n);

    Zeta_StageVector_Node* l_node;
    Zeta_StageVector_Node* r_node;

    size_t l_vacant;
    size_t r_vacant;

    Zeta_CircularVector l_cv;
    l_cv.width = width;
    l_cv.stride = stride;
    l_cv.capacity = seg_capacity;

    Zeta_CircularVector r_cv;
    r_cv.width = width;
    r_cv.stride = stride;
    r_cv.capacity = seg_capacity;

    if (sv->lb == l_n) {
        l_node = NULL;
        l_cv.size = 0;
        l_vacant = 0;
    } else {
        l_node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, l_n);

        if (GetNodeColor(l_n) == ref_color) {
            l_cv.size = l_node->ref.size;

            l_vacant = seg_capacity - ZETA_GetMinOf(seg_capacity, l_cv.size);
        } else {
            l_cv.data = l_node->dat.data;
            l_cv.offset = l_node->dat.offset;
            l_cv.size = l_node->dat.size;

            l_vacant = seg_capacity - l_cv.size;
        }
    }

    if (sv->rb == r_n) {
        r_node = NULL;
        r_cv.size = 0;
        r_vacant = 0;
    } else {
        r_node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, r_n);

        if (GetNodeColor(r_n) == ref_color) {
            r_cv.size = r_node->ref.size;

            r_vacant = seg_capacity - ZETA_GetMinOf(seg_capacity, r_cv.size);
        } else {
            r_cv.data = r_node->dat.data;
            r_cv.offset = r_node->dat.offset;
            r_cv.size = r_node->dat.size;

            r_vacant = seg_capacity - r_cv.size;
        }
    }

    if (r_vacant < l_vacant) { goto TRY_L_SHOVE; }
    if (l_vacant < r_vacant) { goto TRY_R_SHOVE; }

    if (ml_cnt < mr_cnt) { goto TRY_L_SHOVE; }
    if (mr_cnt < ml_cnt) { goto TRY_R_SHOVE; }

    if (Zeta_SimpleRandomRotate(&rand_seed) % 2 == 0) {
        goto TRY_L_SHOVE;
    } else {
        goto TRY_R_SHOVE;
    }

TRY_L_SHOVE: {
    if (l_vacant + m_vacant < cnt) { goto SHOVE_FAILED; }

    if (GetNodeColor(l_n) == ref_color) {
        TransRefSegToDatSeg_(sv, l_node);
        l_cv.data = l_node->dat.data;
        l_cv.offset = l_node->dat.offset;
    }

    size_t ret_seg_idx = l_cv.size + ml_cnt;

    size_t new_lm_size = l_cv.size + m_cv.size + cnt;
    size_t new_l_size = (new_lm_size + (m_cv.size < l_cv.size)) / 2;

    SegShoveL(&l_cv, &m_cv, ml_cnt, cnt, new_l_size - l_cv.size);

    l_node->dat.offset = l_cv.offset;
    l_node->dat.size = l_cv.size;
    Zeta_BinTree_SetSize(&btn_opr, l_n, l_cv.size);

    m_node->dat.offset = m_cv.offset;
    m_node->dat.size = m_cv.size;
    Zeta_BinTree_SetSize(&btn_opr, m_n, m_cv.size);

    if (ret_seg_idx < l_cv.size) {
        pos_cursor->n = l_n;
        pos_cursor->seg_idx = ret_seg_idx;
        pos_cursor->ele =
            Zeta_CircularVector_Access(&l_cv, NULL, NULL, ret_seg_idx);
    } else {
        ret_seg_idx -= l_cv.size;

        pos_cursor->n = m_n;
        pos_cursor->seg_idx = ret_seg_idx;
        pos_cursor->ele =
            Zeta_CircularVector_Access(&m_cv, NULL, NULL, ret_seg_idx);
    }

    return pos_cursor->ele;
}

TRY_R_SHOVE: {
    if (m_vacant + r_vacant < cnt) { goto SHOVE_FAILED; }

    if (GetNodeColor(r_n) == ref_color) {
        TransRefSegToDatSeg_(sv, r_node);
        r_cv.data = r_node->dat.data;
        r_cv.offset = r_node->dat.offset;
    }

    size_t ret_seg_idx = ml_cnt;

    size_t new_mr_size = r_cv.size + m_cv.size + cnt;
    size_t new_r_size = (new_mr_size + (m_cv.size < r_cv.size)) / 2;

    SegShoveR(&m_cv, &r_cv, mr_cnt, cnt, new_r_size - r_cv.size);

    m_node->dat.offset = m_cv.offset;
    m_node->dat.size = m_cv.size;
    Zeta_BinTree_SetSize(&btn_opr, m_n, m_cv.size);

    r_node->dat.offset = r_cv.offset;
    r_node->dat.size = r_cv.size;
    Zeta_BinTree_SetSize(&btn_opr, r_n, r_cv.size);

    if (ret_seg_idx < m_cv.size) {
        pos_cursor->n = m_n;
        pos_cursor->seg_idx = ret_seg_idx;
        pos_cursor->ele =
            Zeta_CircularVector_Access(&m_cv, NULL, NULL, ret_seg_idx);
    } else {
        ret_seg_idx -= m_cv.size;

        pos_cursor->n = r_n;
        pos_cursor->seg_idx = ret_seg_idx;
        pos_cursor->ele =
            Zeta_CircularVector_Access(&r_cv, NULL, NULL, ret_seg_idx);
    }

    return pos_cursor->ele;
}

SHOVE_FAILED:;

    bool_t l_is_ref = sv->lb != l_n && GetNodeColor(l_n) == ref_color;
    bool_t r_is_ref = sv->rb != r_n && GetNodeColor(r_n) == ref_color;

    bool_t l_shove;
    bool_t r_shove;

    {
        size_t res_size = seg_capacity - 1 -
                          (m_cv.size + cnt + seg_capacity - 1) % seg_capacity;

        if (l_cv.size + r_cv.size <= res_size) {
            l_shove = TRUE;
            r_shove = TRUE;
        } else if (l_cv.size != r_cv.size
                       ? l_cv.size < r_cv.size
                       : Zeta_SimpleRandomRotate(&rand_seed) % 2 == 0) {
            l_shove = l_cv.size <= res_size;
            r_shove = FALSE;
        } else {
            l_shove = FALSE;
            r_shove = r_cv.size <= res_size;
        }

        if (l_cv.size == 0) { l_shove = FALSE; }
        if (r_cv.size == 0) { r_shove = FALSE; }
    }

    if (l_is_ref && l_shove) {
        TransRefSegToDatSeg_(sv, l_node);
        l_cv.data = l_node->dat.data;
        l_cv.offset = l_node->dat.offset;
    }

    if (r_is_ref && r_shove) {
        TransRefSegToDatSeg_(sv, r_node);
        r_cv.data = r_node->dat.data;
        r_cv.offset = r_node->dat.offset;
    }

    size_t total_size =
        (l_shove ? l_cv.size : 0) + m_cv.size + cnt + (r_shove ? r_cv.size : 0);

    size_t ret_seg_idx = (l_shove ? l_cv.size : 0) + ml_cnt;

    size_t new_l_size;

    if (l_shove) {
        new_l_size = GetAvgCnt(total_size);
        total_size -= new_l_size;
    } else {
        new_l_size = l_cv.size;
    }

    size_t new_r_size;

    if (r_shove) {
        new_r_size = GetAvgCnt(total_size);
        total_size -= new_r_size;
    } else {
        new_r_size = r_cv.size;
    }

    if (l_cv.size < new_l_size) {
        size_t shove_cnt = new_l_size - l_cv.size;
        size_t k = ZETA_GetMaxOf(ml_cnt, shove_cnt);
        size_t cur_cnt = k - ml_cnt;
        SegShoveL(&l_cv, &m_cv, ml_cnt, cur_cnt, shove_cnt);
        ml_cnt = k - shove_cnt;
        cnt -= cur_cnt;

        l_node->dat.offset = l_cv.offset;
        l_node->dat.size = l_cv.size;
        Zeta_BinTree_SetSize(&btn_opr, l_n, l_node->dat.size);
    }

    if (r_cv.size < new_r_size) {
        size_t shove_cnt = new_r_size - r_cv.size;
        size_t k = ZETA_GetMaxOf(mr_cnt, shove_cnt);
        size_t cur_cnt = k - mr_cnt;
        SegShoveR(&m_cv, &r_cv, mr_cnt, cur_cnt, shove_cnt);
        mr_cnt = k - shove_cnt;
        cnt -= cur_cnt;

        r_node->dat.offset = r_cv.offset;
        r_node->dat.size = r_cv.size;
        Zeta_BinTree_SetSize(&btn_opr, r_n, r_node->dat.size);
    }

    if (total_size <= seg_capacity) {
        Zeta_CircularVector_Access(&m_cv, &cv_cursor, NULL, ml_cnt);
        Zeta_CircularVector_Insert(&m_cv, &cv_cursor, cnt);

        m_node->dat.offset = m_cv.offset;
        m_node->dat.size = m_cv.size;
        Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->dat.size);

        if (l_shove && ret_seg_idx < l_cv.size) {
            pos_cursor->n = l_n;
            pos_cursor->seg_idx = ret_seg_idx;
            pos_cursor->ele =
                Zeta_CircularVector_Access(&l_cv, NULL, NULL, ret_seg_idx);
        } else {
            if (l_shove) { ret_seg_idx -= l_cv.size; }

            pos_cursor->n = m_n;
            pos_cursor->seg_idx = ret_seg_idx;
            pos_cursor->ele =
                Zeta_CircularVector_Access(&m_cv, NULL, NULL, ret_seg_idx);
        }

        return pos_cursor->ele;
    }

    Zeta_StageVector_Node* ml_node;
    Zeta_StageVector_Node* mr_node;

    Zeta_CircularVector ml_cv;
    ml_cv.width = width;
    ml_cv.stride = stride;
    ml_cv.capacity = seg_capacity;

    Zeta_CircularVector mr_cv;
    mr_cv.width = width;
    mr_cv.stride = stride;
    mr_cv.capacity = seg_capacity;

    if (ml_cnt < mr_cnt) { goto L_SPLIT; }
    if (mr_cnt < ml_cnt) { goto R_SPLIT; }

    if (Zeta_SimpleRandomRotate(&rand_seed) % 2 == 0) {
        goto L_SPLIT;
    } else {
        goto R_SPLIT;
    }

L_SPLIT: {
    ml_node = AllocateDatNode_(sv);
    mr_node = m_node;

    sv->root = Zeta_RBTree_Insert(&btn_opr, l_n, &mr_node->n, &ml_node->n);

    ml_cv.data = ml_node->dat.data;
    ml_cv.offset = 0;
    ml_cv.size = 0;

    mr_cv.data = m_cv.data;
    mr_cv.offset = m_cv.offset;
    mr_cv.size = m_cv.size;

    SegShoveL(&ml_cv, &mr_cv, 0, 0, ml_cnt);

    goto SPLIT_END;
}

R_SPLIT: {
    ml_node = m_node;
    mr_node = AllocateDatNode_(sv);

    sv->root = Zeta_RBTree_Insert(&btn_opr, &ml_node->n, r_n, &mr_node->n);

    ml_cv.data = m_cv.data;
    ml_cv.offset = m_cv.offset;
    ml_cv.size = m_cv.size;

    mr_cv.data = mr_node->dat.data;
    mr_cv.offset = 0;
    mr_cv.size = 0;

    SegShoveR(&ml_cv, &mr_cv, 0, 0, mr_cnt);

    goto SPLIT_END;
}

SPLIT_END:;

    size_t new_ml_size = GetAvgCnt(total_size);
    total_size -= new_ml_size;

    size_t new_mr_size = GetAvgCnt(total_size);
    total_size -= new_mr_size;

    if (mr_cv.size <= new_mr_size) {
    } else if (total_size == 0) {
        SegShoveL(&ml_cv, &mr_cv, 0, cnt, cnt + mr_cv.size - new_mr_size);
        cnt = 0;
    } else {
        Zeta_StageVector_Node* new_mr_node = AllocateDatNode_(sv);
        sv->root = Zeta_RBTree_Insert(&btn_opr, &ml_node->n, &mr_node->n,
                                      &new_mr_node->n);

        Zeta_CircularVector new_mr_cv;
        new_mr_cv.data = new_mr_node->dat.data;
        new_mr_cv.width = width;
        new_mr_cv.stride = stride;
        new_mr_cv.offset = 0;
        new_mr_cv.size = 0;
        new_mr_cv.capacity = seg_capacity;

        SegShoveL(&new_mr_cv, &mr_cv, 0, 0, mr_cv.size - new_mr_size);

        mr_node->dat.offset = mr_cv.offset;
        mr_node->dat.size = mr_cv.size;
        Zeta_BinTree_SetSize(&btn_opr, &mr_node->n, mr_node->dat.size);

        mr_node = new_mr_node;

        mr_cv.data = new_mr_cv.data;
        mr_cv.offset = new_mr_cv.offset;
        mr_cv.size = new_mr_cv.size;

        new_mr_size = GetAvgCnt(total_size);
        total_size -= new_mr_size;
    }

    while (0 < total_size) {
        size_t cur_cnt = ZETA_GetMinOf(cnt, new_mr_size - mr_cv.size);
        cnt -= cur_cnt;

        Zeta_CircularVector_PeekL(&mr_cv, &cv_cursor, NULL);
        Zeta_CircularVector_Insert(&mr_cv, &cv_cursor, cur_cnt);

        mr_node->dat.offset = mr_cv.offset;
        mr_node->dat.size = mr_cv.size;
        Zeta_BinTree_SetSize(&btn_opr, &mr_node->n, mr_node->dat.size);

        Zeta_StageVector_Node* new_mr_node = AllocateDatNode_(sv);
        sv->root = Zeta_RBTree_Insert(&btn_opr, &ml_node->n, &mr_node->n,
                                      &new_mr_node->n);

        mr_node = new_mr_node;

        mr_cv.data = mr_node->dat.data;
        mr_cv.offset = 0;
        mr_cv.size = 0;

        new_mr_size = GetAvgCnt(total_size);
        total_size -= new_mr_size;
    }

    if (ml_cv.size <= new_ml_size) {
        SegShoveL(&ml_cv, &mr_cv, 0, cnt, new_ml_size - ml_cv.size);
    } else {
        SegShoveR(&ml_cv, &mr_cv, 0, cnt, cnt + ml_cv.size - new_ml_size);
    }

    ml_node->dat.offset = ml_cv.offset;
    ml_node->dat.size = ml_cv.size;
    Zeta_BinTree_SetSize(&btn_opr, &ml_node->n, ml_node->dat.size);

    mr_node->dat.offset = mr_cv.offset;
    mr_node->dat.size = mr_cv.size;
    Zeta_BinTree_SetSize(&btn_opr, &mr_node->n, mr_node->dat.size);

    if (l_shove) {
        if (ret_seg_idx < l_cv.size) {
            pos_cursor->n = l_n;
            pos_cursor->seg_idx = ret_seg_idx;
            pos_cursor->ele =
                Zeta_CircularVector_Access(&l_cv, NULL, NULL, ret_seg_idx);

            return pos_cursor->ele;
        }

        ret_seg_idx -= l_cv.size;
    }

    if (ret_seg_idx < ml_cv.size) {
        pos_cursor->n = &ml_node->n;
        pos_cursor->seg_idx = ret_seg_idx;
        pos_cursor->ele =
            Zeta_CircularVector_Access(&ml_cv, NULL, NULL, ret_seg_idx);

        return pos_cursor->ele;
    }

    ret_seg_idx -= ml_cv.size;

    pos_cursor->n = &mr_node->n;
    pos_cursor->seg_idx = ret_seg_idx;
    pos_cursor->ele =
        Zeta_CircularVector_Access(&mr_cv, NULL, NULL, ret_seg_idx);

    return pos_cursor->ele;
}

void Zeta_StageVector_PopL(void* sv) {
    Zeta_StageVector_Cursor pos_cursor;
    Zeta_StageVector_PeekL(sv, &pos_cursor, NULL);

    Zeta_StageVector_Erase(sv, &pos_cursor, 1);
}

void Zeta_StageVector_PopR(void* sv) {
    Zeta_StageVector_Cursor pos_cursor;
    Zeta_StageVector_PeekR(sv, &pos_cursor, NULL);

    Zeta_StageVector_Erase(sv, &pos_cursor, 1);
}

void Zeta_StageVector_Erase(void* sv_, void* pos_cursor_, size_t cnt) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* pos_cursor = pos_cursor_;
    Zeta_StageVector_Cursor_Check(sv, pos_cursor);

    if (cnt == 0) { return; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = Zeta_StageVector_GetWidth(sv);
    size_t stride = Zeta_StageVector_GetStride(sv);
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_StageVector_GetSize(sv);

    ZETA_DebugAssert(pos_cursor->idx <= size);
    ZETA_DebugAssert(pos_cursor->idx + cnt <= size);

    void* m_n = pos_cursor->n;
    size_t seg_idx = pos_cursor->seg_idx;

    void* l_n = Zeta_BinTree_StepL(&btn_opr, m_n);

    Zeta_StageVector_Node* m_node =
        ZETA_MemberToStruct(Zeta_StageVector_Node, n, m_n);

    if (GetNodeColor(m_n) == ref_color && 0 < seg_idx) {
        l_n = SplitRefSeg_(sv, &btn_opr, width, stride, l_n, m_node, seg_idx);
        seg_idx = 0;
    }

    Zeta_StageVector_Node* first_node =
        ZETA_MemberToStruct(Zeta_StageVector_Node, n, m_n);
    size_t first_seg_idx = seg_idx;

    Zeta_StageVector_Node* last_node;

    Zeta_CircularVector m_cv;
    m_cv.width = width;
    m_cv.stride = stride;
    m_cv.capacity = seg_capacity;

    while (0 < cnt) {
        m_node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, m_n);
        last_node = m_node;

        if (GetNodeColor(m_n) == ref_color) {
            if (m_node->ref.size <= cnt) {
                cnt -= m_node->ref.size;
                m_n = EraseSeg_(sv, &btn_opr, m_node);
                continue;
            }

            m_node->ref.beg += cnt;
            m_node->ref.size -= cnt;

            break;
        }

        if (seg_idx == 0 && m_node->dat.size <= cnt) {
            cnt -= m_node->dat.size;
            m_n = EraseSeg_(sv, &btn_opr, m_node);
            continue;
        }

        size_t cur_cnt = ZETA_GetMinOf(cnt, m_node->dat.size - seg_idx);

        m_cv.data = m_node->dat.data;
        m_cv.offset = m_node->dat.offset;
        m_cv.size = m_node->dat.size;

        Zeta_CircularVector_Cursor cv_cursor;
        Zeta_CircularVector_Access(&m_cv, &cv_cursor, NULL, seg_idx);
        Zeta_CircularVector_Erase(&m_cv, &cv_cursor, cur_cnt);

        m_node->dat.offset = m_cv.offset;
        m_node->dat.size = m_cv.size;

        cnt -= cur_cnt;

        if (seg_idx == m_node->dat.size) {
            m_n = Zeta_BinTree_StepR(&btn_opr, m_n);
            seg_idx = 0;
        }
    }

    void* ret_n = m_n;
    size_t ret_seg_idx = seg_idx;

    size_t rand_seed = ZETA_PtrToAddr(m_n);

    // if(first_n)

    /*

    4 cases:
        l_n     first_n       last_n (=m_n)      r_n
        l_n     last_n (=m_n)      r_n
        l_n     first_n       m_n
        l_n     first_n(=last_n)      r_n
        l_n     r_n

    first_node <= last_node <= m_n

    first_n exists if and only if
        first_seg_idx != 0 || first_n == m_n

    last_n exists if and only if
        last_n == m_n

    */

    bool_t first_exist = first_seg_idx != 0 || &first_node->n == m_n;
    bool_t last_exist = &last_node->n == m_n;

    Zeta_StageVector_Node* a_node;
    Zeta_StageVector_Node* b_node;
    Zeta_StageVector_Node* c_node;
    Zeta_StageVector_Node* d_node;

    size_t a_size;
    size_t b_size;
    size_t c_size;
    size_t d_size;

    size_t a_vacant;
    size_t b_vacant;
    size_t c_vacant;
    size_t d_vacant;

    void* r_n = last_exist ? Zeta_BinTree_StepR(&btn_opr, m_n) : m_n;

    {
        Zeta_StageVector_Node* nodes_buffer[4] = { NULL, NULL, NULL, NULL };
        size_t nodes_buffer_i = 0;

        if (sv->lb != l_n) {
            nodes_buffer[nodes_buffer_i++] =
                ZETA_MemberToStruct(Zeta_StageVector_Node, n, l_n);
        }

        if (first_exist) { nodes_buffer[nodes_buffer_i++] = first_node; }

        if (last_exist && first_node != last_node) {
            nodes_buffer[nodes_buffer_i++] = last_node;
        }

        if (sv->rb != r_n) {
            nodes_buffer[nodes_buffer_i++] =
                ZETA_MemberToStruct(Zeta_StageVector_Node, n, r_n);
        }

        a_node = nodes_buffer[0];
        b_node = nodes_buffer[1];
        c_node = nodes_buffer[2];
        d_node = nodes_buffer[3];
    }

    if (a_node == NULL) { goto UPDATE; }

    if (GetNodeColor(&a_node->n) == ref_color) {
        a_size = a_node->ref.size;
        a_vacant = seg_capacity - ZETA_GetMinOf(a_size, seg_capacity);
    } else {
        a_size = a_node->dat.size;
        a_vacant = seg_capacity - a_size;
    }

    if (ret_n == &a_node->n) { ret_n = NULL; }

    if (b_node == NULL) { goto UPDATE; }

    if (GetNodeColor(&b_node->n) == ref_color) {
        b_size = b_node->ref.size;
        b_vacant = seg_capacity - ZETA_GetMinOf(b_size, seg_capacity);
    } else {
        b_size = b_node->dat.size;
        b_vacant = seg_capacity - b_size;
    }

    if (ret_n == &b_node->n) {
        ret_n = NULL;
        ret_seg_idx += a_size;
    }

    if (c_node == NULL) { goto MERGE_2; }

    if (GetNodeColor(&c_node->n) == ref_color) {
        c_size = c_node->ref.size;
        c_vacant = seg_capacity - ZETA_GetMinOf(c_size, seg_capacity);
    } else {
        c_size = c_node->dat.size;
        c_vacant = seg_capacity - c_size;
    }

    if (ret_n == &c_node->n) {
        ret_n = NULL;
        ret_seg_idx += a_size + b_size;
    }

    if (d_node == NULL) { goto MERGE_3; }

    if (GetNodeColor(&d_node->n) == ref_color) {
        d_size = d_node->ref.size;
        d_vacant = seg_capacity - ZETA_GetMinOf(d_size, seg_capacity);
    } else {
        d_size = d_node->dat.size;
        d_vacant = seg_capacity - d_size;
    }

    if (ret_n == &d_node->n) {
        ret_n = NULL;
        ret_seg_idx += a_size + b_size + c_size;
    }

    goto MERGE_4;

MERGE_2: {
    size_t ab_vacant = a_vacant + b_vacant;

    if (seg_capacity <= ab_vacant) {
        Merge2_(sv, width, stride, a_node, b_node);
    }

    goto UPDATE;
}

MERGE_3: {
    size_t ab_vacant = a_vacant + b_vacant;
    size_t bc_vacant = b_vacant + c_vacant;

    bool_t ab_score =
        ab_vacant < seg_capacity ? 0 : ab_vacant - seg_capacity + 1;
    bool_t bc_score =
        bc_vacant < seg_capacity ? 0 : bc_vacant - seg_capacity + 1;

    size_t max_score = ZETA_GetMaxOf(ab_score, bc_score);

    if (0 < max_score) {
        bool_t x_cond = ab_score == max_score;
        bool_t y_cond = bc_score == max_score;

        if (x_cond == y_cond ? Zeta_SimpleRandomRotate(&rand_seed) % 2 == 0
                             : x_cond) {
            Merge2_(sv, width, stride, a_node, b_node);
        } else {
            Merge2_(sv, width, stride, b_node, c_node);
        }
    }

    goto UPDATE;
}

MERGE_4: {
    size_t ab_vacant = a_vacant + b_vacant;
    size_t bc_vacant = b_vacant + c_vacant;
    size_t cd_vacant = c_vacant + d_vacant;

    if (seg_capacity <= ab_vacant && seg_capacity <= cd_vacant) {
        Merge2_(sv, width, stride, a_node, b_node);
        Merge2_(sv, width, stride, c_node, d_node);
    } else {
        size_t max_vacant =
            ZETA_GetMaxOf(ZETA_GetMaxOf(ab_vacant, bc_vacant), cd_vacant);

        if (seg_capacity <= max_vacant) {
            switch (Zeta_Choose3(ab_vacant == max_vacant,
                                 bc_vacant == max_vacant,
                                 cd_vacant == max_vacant, &rand_seed)) {
                case 0: Merge2_(sv, width, stride, a_node, b_node); break;
                case 1: Merge2_(sv, width, stride, b_node, c_node); break;
                case 2: Merge2_(sv, width, stride, c_node, d_node); break;
            }
        }
    }

    goto UPDATE;
}

UPDATE: {
    if (a_node == NULL) { goto UPDATE_END; }

    a_size = GetNodeColor(&a_node->n) == ref_color ? a_node->ref.size
                                                   : a_node->dat.size;

    if (a_size == 0) {
        EraseSeg_(sv, &btn_opr, a_node);
    } else {
        Zeta_BinTree_SetSize(&btn_opr, &a_node->n, a_size);
    }

    if (ret_n != NULL) {
    } else if (ret_seg_idx < a_size) {
        ret_n = &a_node->n;
    } else {
        ret_seg_idx -= a_size;
    }

    if (b_node == NULL) { goto UPDATE_END; }

    b_size = GetNodeColor(&b_node->n) == ref_color ? b_node->ref.size
                                                   : b_node->dat.size;

    if (b_size == 0) {
        EraseSeg_(sv, &btn_opr, b_node);
    } else {
        Zeta_BinTree_SetSize(&btn_opr, &b_node->n, b_size);
    }

    if (ret_n != NULL) {
    } else if (ret_seg_idx < b_size) {
        ret_n = &b_node->n;
    } else {
        ret_seg_idx -= b_size;
    }

    if (c_node == NULL) { goto UPDATE_END; }

    c_size = GetNodeColor(&c_node->n) == ref_color ? c_node->ref.size
                                                   : c_node->dat.size;

    if (c_size == 0) {
        EraseSeg_(sv, &btn_opr, c_node);
    } else {
        Zeta_BinTree_SetSize(&btn_opr, &c_node->n, c_size);
    }

    if (ret_n != NULL) {
    } else if (ret_seg_idx < c_size) {
        ret_n = &c_node->n;
    } else {
        ret_seg_idx -= c_size;
    }

    if (d_node == NULL) { goto UPDATE_END; }

    d_size = GetNodeColor(&d_node->n) == ref_color ? d_node->ref.size
                                                   : d_node->dat.size;

    if (d_size == 0) {
        EraseSeg_(sv, &btn_opr, d_node);
    } else {
        Zeta_BinTree_SetSize(&btn_opr, &d_node->n, d_size);
    }

    if (ret_n == NULL) { ret_n = &d_node->n; }
}

UPDATE_END:;

    pos_cursor->n = ret_n;
    pos_cursor->seg_idx = ret_seg_idx;

    if (sv->rb == pos_cursor->n || GetNodeColor(pos_cursor->n) == ref_color) {
        pos_cursor->ele = NULL;
        return;
    }

    ZETA_CheckAssert(sv->lb != pos_cursor->n);
    ZETA_CheckAssert(GetNodeColor(pos_cursor->n) == dat_color);

    Zeta_StageVector_Node* node =
        ZETA_MemberToStruct(Zeta_StageVector_Node, n, pos_cursor->n);

    m_cv.data = node->dat.data;
    m_cv.offset = node->dat.offset;
    m_cv.size = node->dat.size;

    pos_cursor->ele =
        Zeta_CircularVector_Access(&m_cv, NULL, NULL, pos_cursor->seg_idx);
}

static void EraseAll_(Zeta_StageVector* sv, Zeta_BinTreeNodeOperator* btn_opr,
                      void* n) {
    void* nl = Zeta_OrdCnt3RBTreeNode_GetL(NULL, n);
    void* nr = Zeta_OrdCnt3RBTreeNode_GetR(NULL, n);

    if (sv->lb != n && sv->rb != n) {
        EraseSeg_(sv, btn_opr,
                  ZETA_MemberToStruct(Zeta_StageVector_Node, n, n));
    }

    if (nl != NULL) { EraseAll_(sv, btn_opr, nl); }

    if (nr != NULL) { EraseAll_(sv, btn_opr, nr); }
}

void Zeta_StageVector_EraseAll(void* sv_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    EraseAll_(sv, &btn_opr, sv->root);

    InitTree_(sv);
}

static void Check_(Zeta_StageVector* sv, Zeta_DebugHashMap* dst_node_hm,
                   Zeta_DebugHashMap* dst_seg_hm,
                   Zeta_BinTreeNodeOperator* btn_opr, void* n) {
    size_t stride = Zeta_StageVector_GetStride(sv);
    size_t origin_size = sv->origin->GetSize(sv->origin->context);

    void* l_n = Zeta_BinTree_StepL(btn_opr, n);
    void* r_n = Zeta_BinTree_StepR(btn_opr, n);

    if (sv->lb == n) {
        ZETA_DebugAssert(l_n == NULL);
        ZETA_DebugAssert(r_n != NULL);

        ZETA_DebugAssert(Zeta_BinTree_GetSize(btn_opr, n) == 1);

        Zeta_DebugHashMap_KeyValPair p =
            Zeta_DebugHashMap_Insert(dst_node_hm, ZETA_PtrToAddr(sv->lb));
        ZETA_DebugAssert(p.b);
        *p.val = sizeof(Zeta_OrdCnt3RBTreeNode);
    } else if (sv->rb == n) {
        ZETA_DebugAssert(l_n != NULL);
        ZETA_DebugAssert(r_n == NULL);

        ZETA_DebugAssert(Zeta_BinTree_GetSize(btn_opr, n) == 1);

        Zeta_DebugHashMap_KeyValPair p =
            Zeta_DebugHashMap_Insert(dst_node_hm, ZETA_PtrToAddr(sv->rb));
        ZETA_DebugAssert(p.b);
        *p.val = sizeof(Zeta_OrdCnt3RBTreeNode);
    } else if (GetNodeColor(n) == ref_color) {
        ZETA_DebugAssert(l_n != NULL);
        ZETA_DebugAssert(r_n != NULL);

        Zeta_StageVector_Node* node =
            ZETA_MemberToStruct(Zeta_StageVector_Node, n, n);

        {
            Zeta_DebugHashMap_KeyValPair p =
                Zeta_DebugHashMap_Insert(dst_node_hm, ZETA_PtrToAddr(node));
            ZETA_DebugAssert(p.b);
            *p.val = sizeof(Zeta_StageVector_Node);
        }

        ZETA_DebugAssert(0 < node->ref.size);

        ZETA_DebugAssert(node->ref.beg < origin_size);
        ZETA_DebugAssert(node->ref.beg + node->ref.size <= origin_size);

        size_t size = node->ref.size;

        ZETA_DebugAssert(size == Zeta_BinTree_GetSize(btn_opr, n));

        if (sv->lb != l_n) {
            Zeta_StageVector_Node* l_node =
                ZETA_MemberToStruct(Zeta_StageVector_Node, n, l_n);

            if (GetNodeColor(&l_node->n) == ref_color) {
                ZETA_DebugAssert(l_node->ref.beg + l_node->ref.size <
                                 node->ref.beg);
            }

            /*
            if (GetNodeColor(&l_node->n) == ref_color) {
                ZETA_DebugAssert(sv->seg_capacity <
                                 l_node->ref.size + node->ref.size);
            } else {
                ZETA_DebugAssert(sv->seg_capacity <
                                 l_node->dat.size + node->ref.size);
            }
            */
        }

        if (sv->rb != r_n) {
            Zeta_StageVector_Node* r_node =
                ZETA_MemberToStruct(Zeta_StageVector_Node, n, r_n);

            if (GetNodeColor(&r_node->n) == ref_color) {
                ZETA_DebugAssert(node->ref.beg + node->ref.size <
                                 r_node->ref.beg);
            }

            /*
            if (GetNodeColor(&r_node->n) == ref_color) {
                ZETA_DebugAssert(sv->seg_capacity <
                                 node->ref.size + r_node->ref.size);
            } else {
                ZETA_DebugAssert(sv->seg_capacity <
                                 node->ref.size + r_node->dat.size);
            }
            */
        }
    } else {
        ZETA_DebugAssert(GetNodeColor(n) == dat_color);

        ZETA_DebugAssert(l_n != NULL);
        ZETA_DebugAssert(r_n != NULL);

        Zeta_StageVector_Node* node =
            ZETA_MemberToStruct(Zeta_StageVector_Node, n, n);

        {
            Zeta_DebugHashMap_KeyValPair p =
                Zeta_DebugHashMap_Insert(dst_node_hm, ZETA_PtrToAddr(node));
            ZETA_DebugAssert(p.b);
            *p.val = sizeof(Zeta_StageVector_Node);
        }

        size_t size = node->dat.size;

        ZETA_DebugAssert(size == Zeta_BinTree_GetSize(btn_opr, n));

        ZETA_DebugAssert(0 < size);
        ZETA_DebugAssert(size <= sv->seg_capacity);
        ZETA_DebugAssert(node->dat.offset < sv->seg_capacity);

        {
            Zeta_DebugHashMap_KeyValPair p = Zeta_DebugHashMap_Insert(
                dst_seg_hm, ZETA_PtrToAddr(node->dat.data));
            ZETA_DebugAssert(p.b);
            *p.val = stride * sv->seg_capacity;
        }

        size_t l_vacant;
        size_t r_vacant;

        if (sv->lb == l_n) {
            l_vacant = 0;
        } else {
            Zeta_StageVector_Node* l_node =
                ZETA_MemberToStruct(Zeta_StageVector_Node, n, l_n);

            l_vacant = sv->seg_capacity -
                       (GetNodeColor(l_n) == ref_color
                            ? ZETA_GetMinOf(l_node->ref.size, sv->seg_capacity)
                            : l_node->dat.size);
        }

        if (sv->rb == r_n) {
            r_vacant = 0;
        } else {
            Zeta_StageVector_Node* r_node =
                ZETA_MemberToStruct(Zeta_StageVector_Node, n, r_n);

            r_vacant = sv->seg_capacity -
                       (GetNodeColor(r_n) == ref_color
                            ? ZETA_GetMinOf(r_node->ref.size, sv->seg_capacity)
                            : r_node->dat.size);
        }

        ZETA_DebugAssert(0 <= l_vacant);
        ZETA_DebugAssert(0 <= r_vacant);
    }

    void* sub_l_n = Zeta_OrdCnt3RBTreeNode_GetL(NULL, n);
    if (sub_l_n != NULL) {
        Check_(sv, dst_node_hm, dst_seg_hm, btn_opr, sub_l_n);
    }

    void* sub_r_n = Zeta_OrdCnt3RBTreeNode_GetR(NULL, n);
    if (sub_r_n != NULL) {
        Check_(sv, dst_node_hm, dst_seg_hm, btn_opr, sub_r_n);
    }
}

void Zeta_StageVector_Check(void* sv_, Zeta_DebugHashMap* dst_node_hm,
                            Zeta_DebugHashMap* dst_seg_hm) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Check_(sv, dst_node_hm, dst_seg_hm, &btn_opr, sv->root);
}

static void PrintState_(Zeta_StageVector* sv, Zeta_BinTreeNodeOperator* btn_opr,
                        void* n) {
    void* l_n = Zeta_OrdCnt3RBTreeNode_GetL(NULL, n);
    void* r_n = Zeta_OrdCnt3RBTreeNode_GetR(NULL, n);

    if (l_n != NULL) { PrintState_(sv, btn_opr, l_n); }

    if (sv->lb != n && sv->rb != n) {
        Zeta_StageVector_Node* node =
            ZETA_MemberToStruct(Zeta_StageVector_Node, n, n);

        if (GetNodeColor(&node->n) == ref_color) {
            printf("ref %8zu [%8zu, %8zu]\n", node->ref.size, node->ref.beg,
                   node->ref.beg + node->ref.size);
        } else {
            printf("dat %8zu\n", node->ref.size);
        }
    }

    if (r_n != NULL) { PrintState_(sv, btn_opr, r_n); }
}

void Zeta_StageVector_PrintState(void* sv_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    PrintState_(sv, &btn_opr, sv->root);
}

static Zeta_StageVector_Stats GetStats_(Zeta_StageVector* sv,
                                        Zeta_BinTreeNodeOperator* btn_opr,
                                        void* n) {
    Zeta_StageVector_Stats ret;
    ret.ref_seg_cnt = 0;
    ret.dat_seg_cnt = 0;
    ret.ref_size = 0;
    ret.dat_size = 0;

    if (n == NULL) { return ret; }

    void* l_n = Zeta_OrdCnt3RBTreeNode_GetL(NULL, n);
    void* r_n = Zeta_OrdCnt3RBTreeNode_GetR(NULL, n);

    Zeta_StageVector_Stats l_stats = GetStats_(sv, btn_opr, l_n);
    Zeta_StageVector_Stats r_stats = GetStats_(sv, btn_opr, r_n);

    if (sv->lb != n && sv->rb != n) {
        Zeta_StageVector_Node* node =
            ZETA_MemberToStruct(Zeta_StageVector_Node, n, n);

        if (GetNodeColor(n) == ref_color) {
            ++ret.ref_seg_cnt;
            ret.ref_size += node->ref.size;
        } else {
            ++ret.dat_seg_cnt;
            ret.dat_size += node->dat.size;
        }
    }

    ret.ref_seg_cnt += l_stats.ref_seg_cnt + r_stats.ref_seg_cnt;
    ret.dat_seg_cnt += l_stats.dat_seg_cnt + r_stats.dat_seg_cnt;
    ret.ref_size += l_stats.ref_size + r_stats.ref_size;
    ret.dat_size += l_stats.dat_size + r_stats.dat_size;

    return ret;
}

Zeta_StageVector_Stats Zeta_StageVector_GetStats(void* sv_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    return GetStats_(sv, &btn_opr, sv->root);
}

bool_t Zeta_StageVector_Cursor_IsEqual(void* sv_, void const* cursor_a_,
                                       void const* cursor_b_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_StageVector_Cursor const* cursor_a = cursor_a_;
    Zeta_StageVector_Cursor const* cursor_b = cursor_b_;

    Zeta_StageVector_Cursor_Check(sv, cursor_a);
    Zeta_StageVector_Cursor_Check(sv, cursor_b);

    return cursor_a->idx == cursor_b->idx;
}

int Zeta_StageVector_Cursor_Compare(void* sv_, void const* cursor_a_,
                                    void const* cursor_b_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_StageVector_Cursor const* cursor_a = cursor_a_;
    Zeta_StageVector_Cursor const* cursor_b = cursor_b_;

    Zeta_StageVector_Cursor_Check(sv, cursor_a);
    Zeta_StageVector_Cursor_Check(sv, cursor_b);

    size_t ka = cursor_a->idx + 1;
    size_t kb = cursor_b->idx + 1;

    if (ka < kb) { return -1; }
    if (kb < ka) { return 1; }
    return 0;
}

size_t Zeta_StageVector_Cursor_GetDist(void* sv_, void const* cursor_a_,
                                       void const* cursor_b_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_StageVector_Cursor const* cursor_a = cursor_a_;
    Zeta_StageVector_Cursor const* cursor_b = cursor_b_;

    Zeta_StageVector_Cursor_Check(sv, cursor_a);
    Zeta_StageVector_Cursor_Check(sv, cursor_b);

    return cursor_a->idx - cursor_b->idx;
}

size_t Zeta_StageVector_Cursor_GetIdx(void* sv_, void const* cursor_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_StageVector_Cursor const* cursor = cursor_;
    Zeta_StageVector_Cursor_Check(sv, cursor);

    return cursor->idx;
}

void Zeta_StageVector_Cursor_StepL(void* sv, void* cursor) {
    Zeta_StageVector_Cursor_AdvanceL(sv, cursor, 1);
}

void Zeta_StageVector_Cursor_StepR(void* sv, void* cursor) {
    Zeta_StageVector_Cursor_AdvanceR(sv, cursor, 1);
}

void Zeta_StageVector_Cursor_AdvanceL(void* sv_, void* cursor_, size_t step) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* cursor = cursor_;
    Zeta_StageVector_Cursor_Check(sv, cursor);

    if (step == 0) { return; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t size = Zeta_StageVector_GetSize(sv);

    ZETA_DebugAssert(cursor->idx - step + 1 < size + 2);

    void* dst_n;
    size_t dst_seg_idx;

    void* n = cursor->n;

    Zeta_StageVector_Node* node =
        ZETA_MemberToStruct(Zeta_StageVector_Node, n, n);

    size_t n_size =
        (sv->lb == n || sv->rb == n)
            ? 1
            : (GetNodeColor(n) == ref_color ? node->ref.size : node->dat.size);

    Zeta_BinTree_AdvanceL(&dst_n, &dst_seg_idx, &btn_opr, n,
                          n_size - 1 - cursor->seg_idx + step);

    ZETA_DebugAssert(dst_n != NULL);

    cursor->idx -= step;
    cursor->n = dst_n;

    if (sv->lb == dst_n) {
        cursor->seg_idx = 0;
        cursor->ele = 0;
        return;
    }

    Zeta_StageVector_Node* dst_node =
        ZETA_MemberToStruct(Zeta_StageVector_Node, n, dst_n);

    cursor->seg_idx = (GetNodeColor(dst_n) == ref_color ? dst_node->ref.size
                                                        : dst_node->dat.size) -
                      1 - dst_seg_idx;

    if (GetNodeColor(dst_n) == ref_color) {
        cursor->ele = NULL;
        return;
    }

    Zeta_CircularVector cv;
    cv.data = dst_node->dat.data;
    cv.width = Zeta_StageVector_GetWidth(sv);
    cv.stride = Zeta_StageVector_GetStride(sv);
    cv.offset = dst_node->dat.offset;
    cv.size = dst_node->dat.size;
    cv.capacity = sv->seg_capacity;

    cursor->ele = Zeta_CircularVector_Access(&cv, NULL, NULL, cursor->seg_idx);
}

void Zeta_StageVector_Cursor_AdvanceR(void* sv_, void* cursor_, size_t step) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* cursor = cursor_;
    Zeta_StageVector_Cursor_Check(sv, cursor);

    if (step == 0) { return; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t size = Zeta_StageVector_GetSize(sv);

    ZETA_DebugAssert(cursor->idx + step + 1 < size + 2);

    void* dst_n;
    size_t dst_seg_idx;

    Zeta_BinTree_AdvanceR(&dst_n, &dst_seg_idx, &btn_opr, cursor->n,
                          cursor->seg_idx + step);

    ZETA_DebugAssert(dst_n != NULL);

    cursor->idx += step;
    cursor->n = dst_n;
    cursor->seg_idx = dst_seg_idx;

    if (sv->rb == dst_n || GetNodeColor(dst_n) == ref_color) {
        cursor->ele = NULL;
        return;
    }

    Zeta_StageVector_Node* dst_node =
        ZETA_MemberToStruct(Zeta_StageVector_Node, n, dst_n);

    Zeta_CircularVector cv;
    cv.data = dst_node->dat.data;
    cv.width = Zeta_StageVector_GetWidth(sv);
    cv.stride = Zeta_StageVector_GetStride(sv);
    cv.offset = dst_node->dat.offset;
    cv.size = dst_node->dat.size;
    cv.capacity = sv->seg_capacity;

    cursor->ele = Zeta_CircularVector_Access(&cv, NULL, NULL, cursor->seg_idx);
}

void Zeta_StageVector_Cursor_Check(void* sv_, void const* cursor_) {
    if (!ZETA_IsDebug) { return; }

    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    Zeta_StageVector_Cursor re_cursor;
    Zeta_StageVector_Access(sv, &re_cursor, NULL, cursor->idx);

    ZETA_DebugAssert(re_cursor.sv == cursor->sv);
    ZETA_DebugAssert(re_cursor.idx == cursor->idx);
    ZETA_DebugAssert(re_cursor.n == cursor->n);
    ZETA_DebugAssert(re_cursor.seg_idx == cursor->seg_idx);
    ZETA_DebugAssert(re_cursor.ele == cursor->ele);
}

void Zeta_StageVector_DeploySeqContainer(void* sv_,
                                         Zeta_SeqContainer* seq_cntr) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    ZETA_DebugAssert(seq_cntr != NULL);

    seq_cntr->context = sv;

    seq_cntr->cursor_width = sizeof(Zeta_StageVector_Cursor);

    seq_cntr->GetWidth = Zeta_StageVector_GetWidth;

    seq_cntr->GetSize = Zeta_StageVector_GetSize;

    seq_cntr->GetLBCursor = Zeta_StageVector_GetLBCursor;

    seq_cntr->GetRBCursor = Zeta_StageVector_GetRBCursor;

    seq_cntr->PeekL = Zeta_StageVector_PeekL;

    seq_cntr->PeekR = Zeta_StageVector_PeekR;

    seq_cntr->Access = Zeta_StageVector_Access;

    seq_cntr->Refer = Zeta_StageVector_Refer;

    seq_cntr->Read = Zeta_StageVector_Read;

    seq_cntr->Write = Zeta_StageVector_Write;

    seq_cntr->PushL = Zeta_StageVector_PushL;

    seq_cntr->PushR = Zeta_StageVector_PushR;

    seq_cntr->Insert = Zeta_StageVector_Insert;

    seq_cntr->PopL = Zeta_StageVector_PopL;

    seq_cntr->PopR = Zeta_StageVector_PopR;

    seq_cntr->Erase = Zeta_StageVector_Erase;

    seq_cntr->EraseAll = Zeta_StageVector_EraseAll;

    seq_cntr->Cursor_IsEqual = Zeta_StageVector_Cursor_IsEqual;

    seq_cntr->Cursor_Compare = Zeta_StageVector_Cursor_Compare;

    seq_cntr->Cursor_GetDist = Zeta_StageVector_Cursor_GetDist;

    seq_cntr->Cursor_GetIdx = Zeta_StageVector_Cursor_GetIdx;

    seq_cntr->Cursor_StepL = Zeta_StageVector_Cursor_StepL;

    seq_cntr->Cursor_StepR = Zeta_StageVector_Cursor_StepR;

    seq_cntr->Cursor_AdvanceL = Zeta_StageVector_Cursor_AdvanceL;

    seq_cntr->Cursor_AdvanceR = Zeta_StageVector_Cursor_AdvanceR;
}
