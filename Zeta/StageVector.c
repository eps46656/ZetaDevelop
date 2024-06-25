#include "StageVector.h"

#include "CheckPointRecorder.h"
#include "CircularVector.h"
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

static Zeta_StageVector_Node* InsertRefSeg_(Zeta_StageVector* sv,
                                            Zeta_BinTreeNodeOperator* btn_opr,
                                            void* pos) {
    Zeta_StageVector_Node* node = AllocateNode_(sv);

    Zeta_OrdCnt3RBTreeNode_Init(NULL, &node->n);
    Zeta_OrdCnt3RBTreeNode_SetAccSize(NULL, &node->n, 0);

    SetNodeColor(&node->n, ref_color);

    node->ref.beg = 0;
    node->ref.size = 0;

    sv->root = Zeta_RBTree_InsertL(btn_opr, pos, &node->n);

    return node;
}

static Zeta_StageVector_Node* InsertDatSeg_(Zeta_StageVector* sv,
                                            Zeta_BinTreeNodeOperator* btn_opr,
                                            void* pos) {
    Zeta_StageVector_Node* node = AllocateNode_(sv);

    Zeta_OrdCnt3RBTreeNode_Init(NULL, &node->n);
    Zeta_OrdCnt3RBTreeNode_SetAccSize(NULL, &node->n, 0);

    SetNodeColor(&node->n, dat_color);

    node->dat.data = AllocateData_(sv);

    node->dat.offset = 0;
    node->dat.size = 0;

    sv->root = Zeta_RBTree_InsertL(btn_opr, pos, &node->n);

    return node;
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

    Zeta_StageVector_Node* new_node = InsertRefSeg_(sv, btn_opr, &node->n);

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

    Zeta_StageVector_Node* node = InsertRefSeg_(sv, &btn_opr, sv->rb);

    SetNodeColor(&node->n, ref_color);

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

    if (GetNodeColor(n) == ref_color) {
        void* l_n =
            SplitRefSeg_(sv, &btn_opr, width, stride,
                         Zeta_BinTree_StepL(&btn_opr, n), node, seg_idx);

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
                n = Zeta_BinTree_StepR(&btn_opr, n);
                seg_idx = 0;
            }

            continue;
        }

        void* r_n = NULL;

        size_t node_size = node->ref.size;

        if (seg_capacity < node_size) {
            size_t k = (node_size + seg_capacity - 1) / seg_capacity;
            size_t split_cnt = (node_size + k - 1) / k;

            Zeta_StageVector_Node* new_node = InsertRefSeg_(sv, &btn_opr, n);

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

    Zeta_StageVector_Node* m_node;

    ZETA_RecordStdCheckPoint;

    if (sv->rb == m_n || GetNodeColor(m_n) == ref_color) {
        ZETA_RecordStdCheckPoint;

        void* l_n = Zeta_BinTree_StepL(&btn_opr, m_n);

        if (0 < seg_idx) {
            ZETA_RecordStdCheckPoint;

            m_node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, m_n);

            l_n =
                SplitRefSeg_(sv, &btn_opr, width, stride, l_n, m_node, seg_idx);

            seg_idx = 0;

            Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->ref.size);
        }

        ZETA_RecordStdCheckPoint;

        if (sv->lb == l_n || GetNodeColor(l_n) == ref_color) {
            ZETA_RecordStdCheckPoint;

            if (sv->lb != l_n) {
                ZETA_RecordStdCheckPoint;

                Zeta_BinTree_SetSize(
                    &btn_opr, l_n,
                    ZETA_MemberToStruct(Zeta_StageVector_Node, n, l_n)
                        ->ref.size);
            }

            ZETA_RecordStdCheckPoint;

            while (0 < cnt) {
                size_t cur_cnt =
                    cnt / ((cnt + seg_capacity - 1) / seg_capacity);

                m_node = InsertDatSeg_(sv, &btn_opr, m_n);
                m_n = &m_node->n;

                m_node->dat.size = cur_cnt;
                Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->dat.size);

                cnt -= cur_cnt;
            }

            ZETA_RecordStdCheckPoint;

            pos_cursor->n = m_n;
            pos_cursor->seg_idx = 0;
            pos_cursor->ele = m_node->dat.data;

            return pos_cursor->ele;
        }

        ZETA_RecordStdCheckPoint;

        m_n = l_n;
        m_node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, m_n);

        seg_idx = m_node->dat.size;
    } else {
        m_node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, m_n);
    }

    ZETA_RecordStdCheckPoint;

    Zeta_CircularVector m_cv;
    m_cv.data = m_node->dat.data;
    m_cv.width = width;
    m_cv.stride = stride;
    m_cv.offset = m_node->dat.offset;
    m_cv.size = m_node->dat.size;
    m_cv.capacity = seg_capacity;

    size_t m_vacant = seg_capacity - m_cv.size;

    ZETA_RecordStdCheckPoint;

    if (cnt <= m_vacant) {
        ZETA_RecordStdCheckPoint;

        Zeta_CircularVector_Cursor cv_cursor;
        Zeta_CircularVector_Access(&m_cv, &cv_cursor, NULL, seg_idx);

        void* ele = Zeta_CircularVector_Insert(&m_cv, &cv_cursor, cnt);

        m_node->dat.offset = m_cv.offset;
        m_node->dat.size = m_cv.size;
        Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->dat.size);

        pos_cursor->n = m_n;
        pos_cursor->seg_idx = seg_idx;
        pos_cursor->ele = ele;

        ZETA_RecordStdCheckPoint;

        return ele;
    }

    ZETA_RecordStdCheckPoint;

    void* l_n = Zeta_BinTree_StepL(&btn_opr, m_n);
    void* r_n = Zeta_BinTree_StepR(&btn_opr, m_n);

    ZETA_RecordStdCheckPoint;

    Zeta_StageVector_Node* l_node;
    Zeta_StageVector_Node* r_node;

    size_t l_vacant;
    size_t r_vacant;

    if (sv->lb == l_n) {
        ZETA_RecordStdCheckPoint;

        l_node = NULL;
        l_vacant = 0;
    } else {
        ZETA_RecordStdCheckPoint;

        l_node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, l_n);
        l_vacant = GetNodeColor(l_n) == ref_color
                       ? 0
                       : seg_capacity - l_node->dat.size;
    }

    if (sv->rb == r_n) {
        ZETA_RecordStdCheckPoint;

        r_node = NULL;
        r_vacant = 0;
    } else {
        ZETA_RecordStdCheckPoint;

        r_node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, r_n);
        r_vacant = GetNodeColor(r_n) == ref_color
                       ? 0
                       : seg_capacity - r_node->dat.size;
    }

    size_t m_l_move_cnt = seg_idx;
    size_t m_r_move_cnt = m_cv.size - seg_idx;

    ZETA_RecordStdCheckPoint;

    size_t rand_seed =
        ZETA_PtrToAddr(l_n) + ZETA_PtrToAddr(m_n) + ZETA_PtrToAddr(r_n);

    if (r_vacant < l_vacant) { goto TRY_L_SHOVE; }
    if (l_vacant < r_vacant) { goto TRY_R_SHOVE; }

    if (m_l_move_cnt < m_r_move_cnt) { goto TRY_L_SHOVE; }
    if (m_r_move_cnt < m_l_move_cnt) { goto TRY_R_SHOVE; }

    ZETA_RecordStdCheckPoint;

    if (Zeta_SimpleRandomRotate(&rand_seed) % 2 == 0) {
        ZETA_RecordStdCheckPoint;

        goto TRY_L_SHOVE;
    } else {
        ZETA_RecordStdCheckPoint;

        goto TRY_R_SHOVE;
    }

    ZETA_RecordStdCheckPoint;

TRY_L_SHOVE:;

    ZETA_RecordStdCheckPoint;

    if (l_vacant + m_vacant < cnt) {
        ZETA_RecordStdCheckPoint;

        goto TRY_SPLIT;
    }

    ZETA_RecordStdCheckPoint;

    Zeta_CircularVector l_cv;
    l_cv.data = l_node->dat.data;
    l_cv.width = width;
    l_cv.stride = stride;
    l_cv.offset = l_node->dat.offset;
    l_cv.size = l_node->dat.size;
    l_cv.capacity = seg_capacity;

    // l_size + m_l_move_cnt + cnt + m_r_move_cnt

    seg_idx += l_node->dat.size;

    ZETA_RecordStdCheckPoint;

    size_t old_l_size = l_node->dat.size;
    size_t new_lm_size = old_l_size + m_cv.size + cnt;
    size_t new_l_size = (new_lm_size + (m_cv.size < old_l_size)) / 2;

    SegShoveL(&l_cv, &m_cv, m_l_move_cnt, cnt, new_l_size - old_l_size);

    ZETA_RecordStdCheckPoint;

    l_node->dat.offset = l_cv.offset;
    l_node->dat.size = l_cv.size;
    Zeta_BinTree_SetSize(&btn_opr, l_n, l_node->dat.size);

    ZETA_RecordStdCheckPoint;

    m_node->dat.offset = m_cv.offset;
    m_node->dat.size = m_cv.size;
    Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->dat.size);

    ZETA_RecordStdCheckPoint;

    if (seg_idx < l_node->dat.size) {
        ZETA_RecordStdCheckPoint;

        pos_cursor->n = l_n;
        pos_cursor->seg_idx = seg_idx;
        pos_cursor->ele =
            Zeta_CircularVector_Access(&l_cv, NULL, NULL, seg_idx);
    } else {
        ZETA_RecordStdCheckPoint;

        seg_idx -= l_node->dat.size;

        pos_cursor->n = m_n;
        pos_cursor->seg_idx = seg_idx;
        pos_cursor->ele =
            Zeta_CircularVector_Access(&m_cv, NULL, NULL, seg_idx);
    }

    ZETA_RecordStdCheckPoint;

    return pos_cursor->ele;

TRY_R_SHOVE:;

    if (m_vacant + r_vacant < cnt) {
        ZETA_RecordStdCheckPoint;
        goto TRY_SPLIT;
    }

    ZETA_RecordStdCheckPoint;

    Zeta_CircularVector r_cv;
    r_cv.data = r_node->dat.data;
    r_cv.width = width;
    r_cv.stride = stride;
    r_cv.offset = r_node->dat.offset;
    r_cv.size = r_node->dat.size;
    r_cv.capacity = seg_capacity;

    // m_l_move_cnt + cnt + m_r_move_cnt + r_size

    size_t old_r_size = r_node->dat.size;
    size_t new_mr_size = old_r_size + m_cv.size + cnt;
    size_t new_r_size = (new_mr_size + (m_cv.size < old_r_size)) / 2;

    ZETA_RecordStdCheckPoint;

    SegShoveR(&m_cv, &r_cv, m_r_move_cnt, cnt, new_r_size - old_r_size);

    ZETA_RecordStdCheckPoint;

    m_node->dat.offset = m_cv.offset;
    m_node->dat.size = m_cv.size;
    Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->dat.size);

    ZETA_RecordStdCheckPoint;

    r_node->dat.offset = r_cv.offset;
    r_node->dat.size = r_cv.size;
    Zeta_BinTree_SetSize(&btn_opr, r_n, r_node->dat.size);

    ZETA_RecordStdCheckPoint;

    if (seg_idx < m_node->dat.size) {
        ZETA_RecordStdCheckPoint;
        pos_cursor->n = m_n;
        pos_cursor->seg_idx = seg_idx;
        pos_cursor->ele =
            Zeta_CircularVector_Access(&m_cv, NULL, NULL, seg_idx);
    } else {
        ZETA_RecordStdCheckPoint;
        seg_idx -= m_node->dat.size;

        pos_cursor->n = r_n;
        pos_cursor->seg_idx = seg_idx;
        pos_cursor->ele =
            Zeta_CircularVector_Access(&r_cv, NULL, NULL, seg_idx);
    }

    ZETA_RecordStdCheckPoint;

    return pos_cursor->ele;

TRY_SPLIT:;

    ZETA_RecordStdCheckPoint;

    void* ml_n;
    void* mr_n;

    Zeta_StageVector_Node* ml_node;
    Zeta_StageVector_Node* mr_node;

    bool_t l_split = m_l_move_cnt == m_r_move_cnt
                         ? Zeta_SimpleRandomRotate(&rand_seed) % 2 == 0
                         : m_l_move_cnt < m_r_move_cnt;

    if (l_split) {
        ZETA_RecordStdCheckPoint;

        if (m_l_move_cnt <= l_vacant) {
            ZETA_RecordStdCheckPoint;

            ml_n = l_n;
            ml_node = l_node;

            seg_idx += sv->lb == l_n ? 0
                                     : (GetNodeColor(l_n) == ref_color
                                            ? l_node->ref.size
                                            : l_node->dat.size);
        } else {
            ZETA_RecordStdCheckPoint;

            ml_node = InsertDatSeg_(sv, &btn_opr, m_n);
            ml_n = &ml_node->n;
        }

        mr_n = m_n;
        mr_node = m_node;
    } else {
        ZETA_RecordStdCheckPoint;

        ml_n = m_n;
        ml_node = m_node;

        if (m_r_move_cnt <= r_vacant) {
            mr_n = r_n;
            mr_node = r_node;
        } else {
            mr_node = InsertDatSeg_(sv, &btn_opr, r_n);
            mr_n = &mr_node->n;
        }
    }

    ZETA_RecordStdCheckPoint;

    if (0 < m_l_move_cnt && 0 < m_r_move_cnt) {
        ZETA_RecordStdCheckPoint;

        Zeta_CircularVector ml_cv;
        ml_cv.width = width;
        ml_cv.stride = stride;

        if (sv->lb == ml_n) {
            ZETA_RecordStdCheckPoint;

            ml_cv.data = NULL;
            ml_cv.offset = 0;
            ml_cv.size = 0;
            ml_cv.capacity = 0;
        } else {
            ZETA_RecordStdCheckPoint;

            ml_cv.data = ml_node->dat.data;
            ml_cv.offset = ml_node->dat.offset;
            ml_cv.size = ml_node->dat.size;
            ml_cv.capacity = seg_capacity;
        }

        ZETA_RecordStdCheckPoint;

        Zeta_CircularVector mr_cv;
        mr_cv.width = width;
        mr_cv.stride = stride;

        if (sv->rb == mr_n) {
            ZETA_RecordStdCheckPoint;

            mr_cv.data = NULL;
            mr_cv.offset = 0;
            mr_cv.size = 0;
            mr_cv.capacity = 0;
        } else {
            ZETA_RecordStdCheckPoint;

            mr_cv.data = mr_node->dat.data;
            mr_cv.offset = mr_node->dat.offset;
            mr_cv.size = mr_node->dat.size;
            mr_cv.capacity = seg_capacity;
        }

        ZETA_RecordStdCheckPoint;

        // m_l_move_cnt + m_r_move_cnt

        if (l_split) {
            ZETA_RecordStdCheckPoint;

            SegShoveL(&ml_cv, &mr_cv, m_l_move_cnt, 0, m_l_move_cnt);
        } else {
            ZETA_RecordStdCheckPoint;

            SegShoveR(&ml_cv, &mr_cv, m_r_move_cnt, 0, m_r_move_cnt);
        }

        ZETA_RecordStdCheckPoint;

        if (sv->lb != ml_n) {
            ZETA_RecordStdCheckPoint;

            ml_node->dat.offset = ml_cv.offset;
            ml_node->dat.size = ml_cv.size;
        }

        if (sv->rb != mr_n) {
            ZETA_RecordStdCheckPoint;

            mr_node->dat.offset = mr_cv.offset;
            mr_node->dat.size = mr_cv.size;
            Zeta_BinTree_SetSize(&btn_opr, mr_n, mr_node->dat.size);
        }
    }

    ZETA_RecordStdCheckPoint;

    while (0 < cnt) {
        ZETA_RecordStdCheckPoint;

        size_t cur_cnt = cnt / ((cnt + seg_capacity - 1) / seg_capacity);

        mr_node = InsertDatSeg_(sv, &btn_opr, mr_n);
        mr_n = &mr_node->n;

        mr_node->dat.size = cur_cnt;
        Zeta_BinTree_SetSize(&btn_opr, mr_n, mr_node->dat.size);

        cnt -= cur_cnt;
    }

    ZETA_RecordStdCheckPoint;

    if (sv->lb != l_n) {
        ZETA_RecordStdCheckPoint;

        Zeta_BinTree_SetSize(&btn_opr, l_n,
                             GetNodeColor(l_n) == ref_color ? l_node->ref.size
                                                            : l_node->dat.size);
    }

    if (sv->lb == ml_n) {
        ZETA_RecordStdCheckPoint;

        pos_cursor->n = mr_n;
        pos_cursor->seg_idx = 0;
        pos_cursor->ele = mr_node->dat.data;

        return pos_cursor->ele;
    }

    size_t ml_size =
        GetNodeColor(ml_n) == ref_color ? ml_node->ref.size : ml_node->dat.size;

    Zeta_BinTree_SetSize(&btn_opr, ml_n, ml_size);

    if (seg_idx < ml_size) {
        ZETA_RecordStdCheckPoint;

        pos_cursor->n = ml_n;
        pos_cursor->seg_idx = seg_idx;

        if (GetNodeColor(ml_n) == ref_color) {
            ZETA_RecordStdCheckPoint;

            pos_cursor->ele = NULL;
        } else {
            ZETA_RecordStdCheckPoint;

            m_cv.data = ml_node->dat.data;
            m_cv.offset = ml_node->dat.offset;
            m_cv.size = ml_size;

            pos_cursor->ele =
                Zeta_CircularVector_Access(&m_cv, NULL, NULL, seg_idx);
        }
    } else {
        ZETA_RecordStdCheckPoint;

        seg_idx -= ml_size;

        pos_cursor->n = mr_n;
        pos_cursor->seg_idx = seg_idx;
        pos_cursor->ele = ZETA_AddrToPtr(ZETA_PtrToAddr(mr_node->dat.data) +
                                         stride * seg_idx);
    }

    ZETA_RecordStdCheckPoint;

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

    void* l_n = NULL;

    Zeta_StageVector_Node* m_node =
        ZETA_MemberToStruct(Zeta_StageVector_Node, n, m_n);

    ZETA_RecordStdCheckPoint;

    if (GetNodeColor(m_n) == ref_color && 0 < seg_idx) {
        ZETA_RecordStdCheckPoint;

        l_n = SplitRefSeg_(sv, &btn_opr, width, stride,
                           Zeta_BinTree_StepL(&btn_opr, m_n), m_node, seg_idx);

        seg_idx = 0;
    }

    ZETA_RecordStdCheckPoint;

    void* origin_m_n = m_n;
    size_t origin_seg_idx = seg_idx;

    Zeta_CircularVector m_cv;
    m_cv.width = width;
    m_cv.stride = stride;
    m_cv.capacity = seg_capacity;

    while (0 < cnt) {
        ZETA_RecordStdCheckPoint;

        m_node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, m_n);

        if (GetNodeColor(m_n) == ref_color) {
            ZETA_RecordStdCheckPoint;

            if (m_node->ref.size <= cnt) {
                ZETA_RecordStdCheckPoint;

                cnt -= m_node->ref.size;
                m_n = EraseSeg_(sv, &btn_opr, m_node);
                continue;
            }

            ZETA_RecordStdCheckPoint;

            m_node->ref.beg += cnt;
            m_node->ref.size -= cnt;

            break;
        }

        if (seg_idx == 0 && m_node->dat.size <= cnt) {
            ZETA_RecordStdCheckPoint;

            cnt -= m_node->dat.size;
            m_n = EraseSeg_(sv, &btn_opr, m_node);
            continue;
        }

        ZETA_RecordStdCheckPoint;

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
            ZETA_RecordStdCheckPoint;

            m_n = Zeta_BinTree_StepR(&btn_opr, m_n);
            seg_idx = 0;
        }
    }

    pos_cursor->n = m_n;
    pos_cursor->seg_idx = seg_idx;

    void* r_n = NULL;

    if (origin_m_n != m_n) {
        ZETA_RecordStdCheckPoint;

        if (0 < origin_seg_idx) {
            ZETA_RecordStdCheckPoint;

            r_n = m_n;
            m_n = origin_m_n;
        } else if (sv->rb == m_n) {
            ZETA_RecordStdCheckPoint;

            goto RET;
        }
    }

    ZETA_RecordStdCheckPoint;

    m_node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, m_n);

    int m_c = GetNodeColor(m_n);

    if (m_c == dat_color && seg_capacity <= m_node->dat.size * 2) {
        ZETA_RecordStdCheckPoint;

        if (l_n != NULL && sv->lb != l_n) {
            ZETA_RecordStdCheckPoint;

            Zeta_StageVector_Node* l_node =
                ZETA_MemberToStruct(Zeta_StageVector_Node, n, l_n);

            Zeta_BinTree_SetSize(&btn_opr, l_n,
                                 GetNodeColor(l_n) == ref_color
                                     ? l_node->ref.size
                                     : l_node->dat.size);
        }

        ZETA_RecordStdCheckPoint;

        Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->dat.size);

        if (r_n != NULL && sv->rb != r_n) {
            ZETA_RecordStdCheckPoint;

            Zeta_StageVector_Node* r_node =
                ZETA_MemberToStruct(Zeta_StageVector_Node, n, r_n);

            Zeta_BinTree_SetSize(&btn_opr, r_n,
                                 GetNodeColor(r_n) == ref_color
                                     ? r_node->ref.size
                                     : r_node->dat.size);
        }

        ZETA_RecordStdCheckPoint;

        goto RET;
    }

    ZETA_RecordStdCheckPoint;

    l_n = l_n == NULL ? Zeta_BinTree_StepL(&btn_opr, m_n) : l_n;
    r_n = r_n == NULL ? Zeta_BinTree_StepR(&btn_opr, m_n) : r_n;

    ZETA_CheckAssert(l_n == Zeta_BinTree_StepL(&btn_opr, m_n));
    ZETA_CheckAssert(r_n == Zeta_BinTree_StepR(&btn_opr, m_n));

    Zeta_StageVector_Node* l_node;
    Zeta_StageVector_Node* r_node;

    size_t l_vacant;
    size_t r_vacant;

    if (sv->lb == l_n) {
        ZETA_RecordStdCheckPoint;

        l_node = NULL;
        l_vacant = 0;
    } else {
        ZETA_RecordStdCheckPoint;

        l_node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, l_n);

        l_vacant =
            seg_capacity - (GetNodeColor(l_n) == ref_color
                                ? ZETA_GetMinOf(l_node->ref.size, seg_capacity)
                                : l_node->dat.size);
    }

    if (sv->rb == r_n) {
        ZETA_RecordStdCheckPoint;

        r_node = NULL;
        r_vacant = 0;
    } else {
        ZETA_RecordStdCheckPoint;

        r_node = ZETA_MemberToStruct(Zeta_StageVector_Node, n, r_n);

        r_vacant =
            seg_capacity - (GetNodeColor(r_n) == ref_color
                                ? ZETA_GetMinOf(r_node->ref.size, seg_capacity)
                                : r_node->dat.size);
    }

    ZETA_RecordStdCheckPoint;

    size_t rand_seed =
        ZETA_PtrToAddr(l_n) + ZETA_PtrToAddr(m_n) + ZETA_PtrToAddr(r_n);

    if (m_c == ref_color) {
        ZETA_RecordStdCheckPoint;

        if (sv->lb != l_n && GetNodeColor(l_n) == ref_color &&
            l_node->ref.beg + l_node->ref.size == m_node->ref.beg) {
            ZETA_RecordStdCheckPoint;

            if (Zeta_SimpleRandomRotate(&rand_seed) % 2 == 0) {
                ZETA_RecordStdCheckPoint;

                if (pos_cursor->n == m_n) {
                    ZETA_RecordStdCheckPoint;

                    pos_cursor->n = l_n;
                    pos_cursor->seg_idx += l_node->ref.size;
                }

                ZETA_RecordStdCheckPoint;

                l_node->ref.size += m_node->ref.size;

                m_node->ref.size = 0;
            } else {
                ZETA_RecordStdCheckPoint;

                if (pos_cursor->n == l_n) {
                    ZETA_RecordStdCheckPoint;

                    pos_cursor->n = m_n;
                } else if (pos_cursor->n == m_n) {
                    ZETA_RecordStdCheckPoint;

                    pos_cursor->seg_idx += l_node->ref.size;
                }

                ZETA_RecordStdCheckPoint;

                m_node->ref.beg = l_node->ref.beg;
                m_node->ref.size += l_node->ref.size;

                l_node->ref.size = 0;
            }

            ZETA_RecordStdCheckPoint;

            goto UPDATE_RET;
        }

        if (sv->rb != r_n && GetNodeColor(r_n) == ref_color &&
            m_node->ref.beg + m_node->ref.size == r_node->ref.beg) {
            ZETA_RecordStdCheckPoint;

            if (Zeta_SimpleRandomRotate(&rand_seed) % 2 == 0) {
                ZETA_RecordStdCheckPoint;

                if (pos_cursor->n == r_n) {
                    ZETA_RecordStdCheckPoint;

                    pos_cursor->n = m_n;
                    pos_cursor->seg_idx += m_node->ref.size;
                }

                m_node->ref.size += r_node->ref.size;

                r_node->ref.size = 0;
            } else {
                ZETA_RecordStdCheckPoint;

                if (pos_cursor->n == m_n) {
                    ZETA_RecordStdCheckPoint;

                    pos_cursor->n = r_n;
                } else if (pos_cursor->n == r_n) {
                    ZETA_RecordStdCheckPoint;

                    pos_cursor->seg_idx += m_node->ref.size;
                }

                ZETA_RecordStdCheckPoint;

                r_node->ref.beg = m_node->ref.beg;
                r_node->ref.size += m_node->ref.size;

                m_node->ref.size = 0;
            }

            ZETA_RecordStdCheckPoint;

            goto UPDATE_RET;
        }
    }

    size_t m_vacant =
        seg_capacity - (m_c == ref_color
                            ? ZETA_GetMinOf(m_node->ref.size, seg_capacity)
                            : m_node->dat.size);

    ZETA_RecordStdCheckPoint;

    size_t lm_vacant = l_vacant + m_vacant;
    size_t mr_vacant = m_vacant + r_vacant;

    void* a_n;
    void* b_n;

    Zeta_StageVector_Node* a_node;
    Zeta_StageVector_Node* b_node;

    size_t a_vacant;
    size_t b_vacant;

    if (lm_vacant == mr_vacant ? (Zeta_SimpleRandomRotate(&rand_seed) % 2 == 0)
                               : lm_vacant < mr_vacant) {
        ZETA_RecordStdCheckPoint;

        a_n = m_n;
        a_node = m_node;
        a_vacant = m_vacant;

        b_n = r_n;
        b_node = r_node;
        b_vacant = r_vacant;
    } else {
        ZETA_RecordStdCheckPoint;

        a_n = l_n;
        a_node = l_node;
        a_vacant = l_vacant;

        b_n = m_n;
        b_node = m_node;
        b_vacant = m_vacant;
    }

    size_t ab_vacant = a_vacant + b_vacant;

    // if (ab_vacant < seg_capacity * 1.25) { return; }
    if (ab_vacant * 4 < seg_capacity * 5) {
        ZETA_RecordStdCheckPoint;

        goto UPDATE_RET;
    }

    int a_c = GetNodeColor(a_n);
    int b_c = GetNodeColor(b_n);

    bool_t a_to_b = a_c != b_c ? a_c == ref_color
                    : a_vacant != b_vacant
                        ? b_vacant < a_vacant
                        : Zeta_SimpleRandomRotate(&rand_seed) % 2 == 0;

    if (a_to_b) {
        ZETA_RecordStdCheckPoint;

        if (pos_cursor->n == a_n) {
            ZETA_RecordStdCheckPoint;

            pos_cursor->n = b_n;
        } else if (pos_cursor->n == b_n) {
            ZETA_RecordStdCheckPoint;

            pos_cursor->seg_idx +=
                a_c == ref_color ? a_node->ref.size : a_node->dat.size;
        }
    } else if (pos_cursor->n == b_n) {
        ZETA_RecordStdCheckPoint;

        pos_cursor->n = a_n;
        pos_cursor->seg_idx +=
            a_c == ref_color ? a_node->ref.size : a_node->dat.size;
    }

    if (a_c == ref_color && b_c == ref_color) {
        ZETA_RecordStdCheckPoint;

        unsigned char* data = AllocateData_(sv);

        Zeta_Cursor origin_cursor;

        sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                           a_node->ref.beg);
        sv->origin->Read(sv->origin->context, &origin_cursor, a_node->ref.size,
                         data, NULL);

        sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                           b_node->ref.beg);
        sv->origin->Read(sv->origin->context, &origin_cursor, b_node->ref.size,
                         data + stride * a_node->ref.size, NULL);

        if (a_to_b) {
            ZETA_RecordStdCheckPoint;

            SetNodeColor(b_n, dat_color);

            b_node->dat.size = a_node->ref.size + b_node->ref.size;
            b_node->dat.data = data;
            b_node->dat.offset = 0;

            a_node->ref.size = 0;
        } else {
            ZETA_RecordStdCheckPoint;

            SetNodeColor(a_n, dat_color);

            a_node->dat.size = a_node->ref.size + b_node->ref.size;
            a_node->dat.data = data;
            a_node->dat.offset = 0;

            b_node->ref.size = 0;
        }

        ZETA_RecordStdCheckPoint;

        goto UPDATE_RET;
    }

    Zeta_CircularVector a_cv;
    a_cv.width = width;
    a_cv.stride = stride;

    if (a_c == ref_color) {
        ZETA_RecordStdCheckPoint;

        a_cv.data = NULL;
        a_cv.offset = 0;
        a_cv.size = 0;
        a_cv.capacity = 0;
    } else {
        ZETA_RecordStdCheckPoint;

        a_cv.data = a_node->dat.data;
        a_cv.offset = a_node->dat.offset;
        a_cv.size = a_node->dat.size;
        a_cv.capacity = seg_capacity;
    }

    Zeta_CircularVector b_cv;
    b_cv.width = width;
    b_cv.stride = stride;

    if (b_c == ref_color) {
        ZETA_RecordStdCheckPoint;

        b_cv.data = NULL;
        b_cv.offset = 0;
        b_cv.size = 0;
        b_cv.capacity = 0;
    } else {
        ZETA_RecordStdCheckPoint;

        b_cv.data = b_node->dat.data;
        b_cv.offset = b_node->dat.offset;
        b_cv.size = b_node->dat.size;
        b_cv.capacity = seg_capacity;
    }

    if (a_to_b) {
        ZETA_RecordStdCheckPoint;

        if (a_c == dat_color) {  // a_n is dat, b_n is dat

            ZETA_RecordStdCheckPoint;

            SegShoveR(&a_cv, &b_cv, 0, 0, a_node->dat.size);

            a_node->dat.offset = a_cv.offset;
            a_node->dat.size = a_cv.size;
        } else {  // a_n is ref, b_n is dat

            ZETA_RecordStdCheckPoint;

            Zeta_CircularVector_Cursor cv_cursor;
            Zeta_CircularVector_PeekL(&b_cv, &cv_cursor, NULL);
            Zeta_CircularVector_Insert(&b_cv, &cv_cursor, a_node->ref.size);

            Zeta_Cursor origin_cursor;

            sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                               a_node->ref.beg);

            for (size_t i = 0; i < a_node->ref.size; ++i) {
                sv->origin->Read(
                    sv->origin->context, &origin_cursor, 1,
                    Zeta_CircularVector_Access(&b_cv, NULL, NULL, i),
                    &origin_cursor);
            }

            a_node->ref.size = 0;
        }

        b_node->dat.offset = b_cv.offset;
        b_node->dat.size = b_cv.size;
    } else {
        ZETA_RecordStdCheckPoint;

        if (b_c == dat_color) {  // a_n is dat, b_n is dat

            ZETA_RecordStdCheckPoint;

            SegShoveL(&a_cv, &b_cv, 0, 0, b_node->dat.size);

            b_node->dat.offset = b_cv.offset;
            b_node->dat.size = b_cv.size;
        } else {  // a_n is dat, b_n is ref

            ZETA_RecordStdCheckPoint;

            Zeta_CircularVector_Cursor cv_cursor;
            Zeta_CircularVector_GetRBCursor(&a_cv, &cv_cursor);
            Zeta_CircularVector_Insert(&a_cv, &cv_cursor, b_node->ref.size);

            Zeta_Cursor origin_cursor;

            sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                               b_node->ref.beg);

            for (size_t i = 0; i < b_node->ref.size; ++i) {
                sv->origin->Read(sv->origin->context, &origin_cursor, 1,
                                 Zeta_CircularVector_Access(
                                     &a_cv, NULL, NULL, a_node->dat.size + i),
                                 &origin_cursor);
            }

            b_node->ref.size = 0;
        }

        a_node->dat.offset = a_cv.offset;
        a_node->dat.size = a_cv.size;
    }

UPDATE_RET:;

    if (sv->lb != l_n) {
        ZETA_RecordStdCheckPoint;

        size_t l_size = GetNodeColor(l_n) == ref_color ? l_node->ref.size
                                                       : l_node->dat.size;

        if (l_size == 0) {
            ZETA_RecordStdCheckPoint;

            EraseSeg_(sv, &btn_opr, l_node);
        } else {
            ZETA_RecordStdCheckPoint;

            Zeta_BinTree_SetSize(&btn_opr, l_n, l_size);
        }
    }

    {
        ZETA_RecordStdCheckPoint;

        size_t m_size = GetNodeColor(m_n) == ref_color ? m_node->ref.size
                                                       : m_node->dat.size;

        if (m_size == 0) {
            ZETA_RecordStdCheckPoint;

            EraseSeg_(sv, &btn_opr, m_node);
        } else {
            ZETA_RecordStdCheckPoint;

            Zeta_BinTree_SetSize(&btn_opr, m_n, m_size);
        }
    }

    if (sv->rb != r_n) {
        ZETA_RecordStdCheckPoint;

        size_t r_size = GetNodeColor(r_n) == ref_color ? r_node->ref.size
                                                       : r_node->dat.size;

        if (r_size == 0) {
            ZETA_RecordStdCheckPoint;

            EraseSeg_(sv, &btn_opr, r_node);
        } else {
            ZETA_RecordStdCheckPoint;

            Zeta_BinTree_SetSize(&btn_opr, r_n, r_size);
        }
    }

    ZETA_RecordStdCheckPoint;

RET:;

    if (sv->rb == pos_cursor->n || GetNodeColor(pos_cursor->n) == ref_color) {
        ZETA_RecordStdCheckPoint;

        pos_cursor->ele = NULL;
        return;
    }

    ZETA_RecordStdCheckPoint;

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

            // ZETA_DebugAssert(sv->seg_capacity < l_node->size + node->size);
        }

        if (sv->rb != r_n) {
            Zeta_StageVector_Node* r_node =
                ZETA_MemberToStruct(Zeta_StageVector_Node, n, r_n);

            if (GetNodeColor(&r_node->n) == ref_color) {
                ZETA_DebugAssert(node->ref.beg + node->ref.size <
                                 r_node->ref.beg);
            }

            // ZETA_DebugAssert(sv->seg_capacity < node->size + r_node->size);
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

        if (sv->rb == l_n) {
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
