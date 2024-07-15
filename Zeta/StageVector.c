#include "StageVector.h"

#include "CircularVector.h"
#include "Debugger.h"
#include "RBTree.h"
#include "SegUtils.h"
#include "utils.h"

#define ref_color (ZETA_StageVector_ref_color)
#define dat_color (ZETA_StageVector_dat_color)

#define GetSegColor(n)                                \
    ({                                                \
        ZETA_DebugAssert(sv->lb != n && sv->rb != n); \
        Zeta_OrdCnt3RBTreeNode_GetLColor(NULL, (n));  \
    })

#define SetSegColor(n, c)                             \
    ZETA_DebugAssert(sv->lb != n && sv->rb != n);     \
    Zeta_OrdCnt3RBTreeNode_SetLColor(NULL, (n), (c)); \
    ZETA_StaticAssert(TRUE)

#define GetAvgCnt(x)                                                    \
    ({                                                                  \
        size_t x_ = (x);                                                \
        ZETA_CheckAssert(0 < x);                                        \
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

    ZETA_DebugAssert(sv->seg_allocator != NULL);
    ZETA_DebugAssert(sv->seg_allocator->Allocate != NULL);
    ZETA_DebugAssert(sv->seg_allocator->Deallocate != NULL);

    ZETA_DebugAssert(sv->data_allocator != NULL);
    ZETA_DebugAssert(sv->data_allocator->Allocate != NULL);
    ZETA_DebugAssert(sv->data_allocator->Deallocate != NULL);
}

static Zeta_StageVector_Seg* AllocateNode_(Zeta_StageVector* sv) {
    ZETA_DebugAssert(sv->seg_allocator != NULL);
    ZETA_DebugAssert(sv->seg_allocator->Allocate != NULL);

    void* node_ = sv->seg_allocator->Allocate(sv->seg_allocator->context,
                                              sizeof(Zeta_StageVector_Seg));
    ZETA_DebugAssert(node_ != NULL);
    ZETA_DebugAssert(
        __builtin_is_aligned(node_, alignof(Zeta_StageVector_Seg)));

    Zeta_StageVector_Seg* seg = node_;

    Zeta_OrdCnt3RBTreeNode_Init(NULL, &seg->n);

    return seg;
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

static Zeta_StageVector_Seg* AllocateRefSeg_(Zeta_StageVector* sv) {
    Zeta_StageVector_Seg* seg = AllocateNode_(sv);

    Zeta_OrdCnt3RBTreeNode_Init(NULL, &seg->n);
    Zeta_OrdCnt3RBTreeNode_SetAccSize(NULL, &seg->n, 0);

    SetSegColor(&seg->n, ref_color);

    seg->ref.beg = 0;
    seg->ref.size = 0;

    return seg;
}

static Zeta_StageVector_Seg* AllocateDatSeg_(Zeta_StageVector* sv) {
    Zeta_StageVector_Seg* seg = AllocateNode_(sv);

    Zeta_OrdCnt3RBTreeNode_Init(NULL, &seg->n);
    Zeta_OrdCnt3RBTreeNode_SetAccSize(NULL, &seg->n, 0);

    SetSegColor(&seg->n, dat_color);

    seg->dat.data = AllocateData_(sv);

    seg->dat.offset = 0;
    seg->dat.size = 0;

    return seg;
}

static void TransRefSegToDatSeg_(Zeta_StageVector* sv,
                                 Zeta_StageVector_Seg* seg) {
    unsigned char* data = AllocateData_(sv);

    size_t size = seg->ref.size;

    Zeta_Cursor origin_cursor;
    sv->origin->Access(sv->origin->context, &origin_cursor, NULL, seg->ref.beg);
    sv->origin->Read(sv->origin->context, &origin_cursor, size, data, NULL);

    SetSegColor(&seg->n, dat_color);

    seg->dat.data = data;
    seg->dat.offset = 0;
    seg->dat.size = size;
}

static void* SplitRefSeg_(Zeta_StageVector* sv,
                          Zeta_BinTreeNodeOperator* btn_opr, size_t width,
                          size_t stride, void* l_n, Zeta_StageVector_Seg* seg,
                          size_t seg_idx) {
    if (seg_idx == 0) { return l_n; }

    size_t seg_capacity = sv->seg_capacity;

    Zeta_StageVector_Seg* l_node;
    size_t l_size;

    if (sv->lb == l_n) { goto L1; }

    l_node = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, l_n);
    l_size =
        GetSegColor(l_n) == ref_color ? l_node->ref.size : l_node->dat.size;

    if (seg_capacity < l_size + seg_idx) { goto L1; }

    goto L2;

L1:;

    Zeta_StageVector_Seg* new_node = AllocateRefSeg_(sv);
    sv->root = Zeta_RBTree_Insert(btn_opr, l_n, &seg->n, &new_node->n);

    new_node->ref.beg = seg->ref.beg;
    new_node->ref.size = seg_idx;

    seg->ref.beg += seg_idx;
    seg->ref.size -= seg_idx;

    return &new_node->n;

L2:;

    if (GetSegColor(l_n) == ref_color) {
        unsigned char* data = AllocateData_(sv);

        Zeta_Cursor origin_cursor;
        sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                           l_node->ref.beg);
        sv->origin->Read(sv->origin->context, &origin_cursor, l_size, data,
                         NULL);
        sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                           seg->ref.beg);
        sv->origin->Read(sv->origin->context, &origin_cursor, seg_idx,
                         data + stride * l_size, NULL);

        SetSegColor(l_n, dat_color);

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
                           seg->ref.beg);

        for (size_t i = 0; i < seg_idx; ++i) {
            sv->origin->Read(
                sv->origin->context, &origin_cursor, 1,
                Zeta_CircularVector_Access(&cv, NULL, NULL, l_size + i),
                &origin_cursor);
        }

        l_node->dat.offset = cv.offset;
        l_node->dat.size = cv.size;
    }

    seg->ref.beg += seg_idx;
    seg->ref.size -= seg_idx;

    return l_n;
}

static void InitTree_(Zeta_StageVector* sv, Zeta_BinTreeNodeOperator* btn_opr) {
    Zeta_OrdCnt3RBTreeNode_Init(NULL, sv->lb);
    Zeta_OrdCnt3RBTreeNode_Init(NULL, sv->rb);

    Zeta_OrdCnt3RBTreeNode_SetAccSize(NULL, sv->lb, 1);
    Zeta_OrdCnt3RBTreeNode_SetAccSize(NULL, sv->rb, 1);

    void* root = NULL;

    root = Zeta_RBTree_InsertR(btn_opr, root, sv->lb);
    root = Zeta_RBTree_InsertR(btn_opr, root, sv->rb);

    sv->root = root;
}

static void RefOrigin_(Zeta_StageVector* sv,
                       Zeta_BinTreeNodeOperator* btn_opr) {
    size_t origin_size = sv->origin->GetSize(sv->origin->context);

    if (origin_size == 0) { return; }

    Zeta_StageVector_Seg* seg = AllocateRefSeg_(sv);
    sv->root = Zeta_RBTree_Insert(btn_opr, sv->lb, sv->rb, &seg->n);

    seg->ref.beg = 0;
    seg->ref.size = origin_size;

    Zeta_BinTree_SetSize(btn_opr, &seg->n, origin_size);
}

void Zeta_StageVector_Init(void* sv_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    void* lb_ = sv->seg_allocator->Allocate(sv->seg_allocator->context,
                                            sizeof(Zeta_OrdCnt3RBTreeNode));
    void* rb_ = sv->seg_allocator->Allocate(sv->seg_allocator->context,
                                            sizeof(Zeta_OrdCnt3RBTreeNode));

    ZETA_DebugAssert(lb_ != NULL);
    ZETA_DebugAssert(rb_ != NULL);

    ZETA_DebugAssert(
        __builtin_is_aligned(lb_, alignof(Zeta_OrdCnt3RBTreeNode)));
    ZETA_DebugAssert(
        __builtin_is_aligned(rb_, alignof(Zeta_OrdCnt3RBTreeNode)));

    sv->lb = lb_;
    sv->rb = rb_;

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    InitTree_(sv, &btn_opr);
    RefOrigin_(sv, &btn_opr);
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

    Zeta_StageVector_Seg* seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

    void* ele = NULL;

    if (GetSegColor(n) == ref_color) {
        sv->origin->Access(sv->origin->context, NULL, dst_ele, seg->ref.beg);
    } else {
        Zeta_CircularVector cv;
        cv.data = seg->dat.data;
        cv.width = width;
        cv.stride = stride;
        cv.offset = seg->dat.offset;
        cv.size = seg->dat.size;
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

    Zeta_StageVector_Seg* seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

    void* ele = NULL;

    size_t node_size;

    if (GetSegColor(n) == ref_color) {
        node_size = seg->ref.size;

        sv->origin->Access(sv->origin->context, NULL, dst_ele,
                           seg->ref.beg + node_size - 1);
    } else {
        node_size = seg->dat.size;

        Zeta_CircularVector cv;
        cv.data = seg->dat.data;
        cv.width = width;
        cv.stride = stride;
        cv.offset = seg->dat.offset;
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
    Zeta_StageVector_Seg* seg;
    size_t seg_idx;

    Zeta_BinTree_AccessL(&n, &seg_idx, &btn_opr, sv->root, idx + 1);

    void* ele = NULL;

    if (idx < size) {
        seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        if (GetSegColor(n) == dat_color) {
            Zeta_CircularVector cv;
            cv.data = seg->dat.data;
            cv.width = width;
            cv.stride = stride;
            cv.offset = seg->dat.offset;
            cv.size = seg->dat.size;
            cv.capacity = seg_capacity;

            ele = Zeta_CircularVector_Access(&cv, NULL, dst_ele, seg_idx);
        } else if (dst_ele != NULL) {
            sv->origin->Access(sv->origin->context, NULL, dst_ele,
                               seg->ref.beg + seg_idx);
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
    Zeta_StageVector_Seg* seg;

    size_t seg_idx = pos_cursor->seg_idx;

    Zeta_Cursor origin_cursor;

    Zeta_CircularVector cv;
    cv.width = width;
    cv.stride = stride;
    cv.capacity = seg_capacity;

    while (0 < cnt) {
        seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        size_t cur_cnt;
        size_t node_size;

        if (GetSegColor(n) == ref_color) {
            node_size = seg->ref.size;
            cur_cnt = ZETA_GetMinOf(cnt, node_size - seg_idx);

            sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                               seg->ref.beg + seg_idx);
            sv->origin->Read(sv->origin->context, &origin_cursor, cur_cnt, dst,
                             NULL);
        } else {
            node_size = seg->dat.size;
            cur_cnt = ZETA_GetMinOf(cnt, node_size - seg_idx);

            cv.data = seg->dat.data;
            cv.offset = seg->dat.offset;
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

    if (sv->rb == n || GetSegColor(n) == ref_color) {
        dst_cursor->ele = NULL;
        return;
    }

    seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

    cv.data = seg->dat.data;
    cv.offset = seg->dat.offset;
    cv.size = seg->dat.size;

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

    Zeta_StageVector_Seg* seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

    void* l_n = Zeta_BinTree_StepL(&btn_opr, n);

    if (GetSegColor(n) == ref_color) {
        l_n = SplitRefSeg_(sv, &btn_opr, width, stride, l_n, seg, seg_idx);

        if (sv->lb != l_n) {
            Zeta_StageVector_Seg* l_node =
                ZETA_MemberToStruct(Zeta_StageVector_Seg, n, l_n);

            Zeta_BinTree_SetSize(&btn_opr, l_n,
                                 GetSegColor(l_n) == ref_color
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
        seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        if (GetSegColor(n) == dat_color) {
            cv.data = seg->dat.data;
            cv.offset = seg->dat.offset;
            cv.size = seg->dat.size;

            size_t cur_cnt = ZETA_GetMinOf(cnt, cv.size - seg_idx);

            Zeta_CircularVector cv_cursor;
            Zeta_CircularVector_Access(&cv, &cv_cursor, NULL, seg_idx);
            Zeta_CircularVector_Write(&cv, &cv_cursor, cur_cnt, src, NULL);

            src += stride * cur_cnt;

            seg_idx += cur_cnt;
            cnt -= cur_cnt;

            if (seg_idx == seg->dat.size) {
                l_n = n;
                n = Zeta_BinTree_StepR(&btn_opr, n);
                seg_idx = 0;
            }

            continue;
        }

        void* r_n = NULL;

        size_t node_size = seg->ref.size;

        if (seg_capacity < node_size) {
            size_t split_cnt = ZETA_CeilIntDiv(
                node_size, ZETA_CeilIntDiv(node_size, seg_capacity));

            Zeta_StageVector_Seg* new_node = AllocateRefSeg_(sv);

            sv->root = Zeta_RBTree_Insert(&btn_opr, l_n, n, &new_node->n);

            new_node->ref.beg = seg->ref.beg;
            new_node->ref.size = split_cnt;

            seg->ref.beg += split_cnt;
            seg->ref.size -= split_cnt;

            r_n = n;

            seg = new_node;
            n = &new_node->n;

            node_size = seg->ref.size;
        }

        Zeta_BinTree_SetSize(&btn_opr, &seg->n, node_size);

        size_t cur_cnt = ZETA_GetMinOf(cnt, node_size);

        unsigned char* data = AllocateData_(sv);

        Zeta_MemCopy(data, src, stride * cur_cnt);

        src += stride * cur_cnt;

        SetSegColor(n, dat_color);

        if (cur_cnt == node_size) {
            seg->dat.data = data;
            seg->dat.offset = 0;
            seg->dat.size = node_size;

            cnt -= cur_cnt;
            l_n = n;
            n = r_n == NULL ? Zeta_BinTree_StepR(&btn_opr, n) : r_n;
            continue;
        }

        Zeta_Cursor origin_cursor;
        sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                           seg->ref.beg + cur_cnt);

        sv->origin->Read(sv->origin->context, &origin_cursor,
                         node_size - cur_cnt, data + stride * cur_cnt, NULL);

        seg->dat.data = data;
        seg->dat.offset = 0;
        seg->dat.size = node_size;

        if (r_n != NULL) {
            Zeta_StageVector_Seg* r_node =
                ZETA_MemberToStruct(Zeta_StageVector_Seg, n, r_n);

            Zeta_BinTree_SetSize(&btn_opr, r_n,
                                 GetSegColor(r_n) == ref_color
                                     ? r_node->ref.size
                                     : r_node->dat.size);
        }

        seg_idx = cur_cnt;

        break;
    }

    if (sv->rb != n) {
        Zeta_StageVector_Seg* seg =
            ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        Zeta_BinTree_SetSize(
            &btn_opr, n,
            GetSegColor(n) == ref_color ? seg->ref.size : seg->dat.size);
    }

    if (dst_cursor == NULL) { return; }

    dst_cursor->sv = sv;
    dst_cursor->idx = end;
    dst_cursor->n = n;
    dst_cursor->seg_idx = seg_idx;

    if (sv->rb == n || GetSegColor(n) == ref_color) {
        dst_cursor->ele = NULL;
        return;
    }

    seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

    cv.data = seg->dat.data;
    cv.offset = seg->dat.offset;
    cv.size = seg->dat.size;

    dst_cursor->ele = Zeta_CircularVector_Access(&cv, NULL, NULL, seg_idx);
}

static void EraseSeg_(Zeta_StageVector* sv, Zeta_BinTreeNodeOperator* btn_opr,
                      Zeta_StageVector_Seg* seg) {
    ZETA_DebugAssert(sv->seg_allocator != NULL);
    ZETA_DebugAssert(sv->data_allocator != NULL);

    ZETA_DebugAssert(sv->seg_allocator->Deallocate != NULL);
    ZETA_DebugAssert(sv->data_allocator->Deallocate != NULL);

    void* n = &seg->n;

    sv->root = Zeta_RBTree_Extract(btn_opr, n);

    if (GetSegColor(n) == dat_color) {
        sv->data_allocator->Deallocate(sv->data_allocator->context,
                                       seg->dat.data);
    }

    sv->seg_allocator->Deallocate(sv->seg_allocator->context, seg);
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

static void PushRefL_(Zeta_StageVector* sv, Zeta_CircularVector* cv, size_t beg,
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

static void PushRefR_(Zeta_StageVector* sv, Zeta_CircularVector* cv, size_t beg,
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

static void Merge2_(Zeta_StageVector* sv, size_t width, size_t stride,
                    Zeta_StageVector_Seg* a_seg, Zeta_StageVector_Seg* b_seg) {
    int a_c = GetSegColor(&a_seg->n);
    int b_c = GetSegColor(&b_seg->n);

    size_t a_size = a_c == ref_color ? a_seg->ref.size : a_seg->dat.size;
    size_t b_size = b_c == ref_color ? b_seg->ref.size : b_seg->dat.size;

    size_t total_size = a_size + b_size;

    size_t seg_capacity = sv->seg_capacity;

    ZETA_CheckAssert(total_size <= seg_capacity);

    unsigned long long rand_seed =
        ZETA_PtrToAddr(a_seg) + ZETA_PtrToAddr(b_seg);

    if (a_c == ref_color && b_c == ref_color) {
        unsigned char* data = AllocateData_(sv);

        Zeta_Cursor origin_cursor;

        sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                           a_seg->ref.beg);
        sv->origin->Read(sv->origin->context, &origin_cursor, a_size, data,
                         NULL);

        sv->origin->Access(sv->origin->context, &origin_cursor, NULL,
                           b_seg->ref.beg);
        sv->origin->Read(sv->origin->context, &origin_cursor, b_seg->ref.size,
                         data + stride * a_size, NULL);

        a_seg->ref.size = 0;
        b_seg->ref.size = 0;

        Zeta_StageVector_Seg* dst_node;

        switch (Zeta_SimpleRandomRotate(&rand_seed) % 2) {
            case 0: dst_node = a_seg; break;
            case 1: dst_node = b_seg; break;
        }

        SetSegColor(&dst_node->n, dat_color);
        dst_node->dat.data = data;
        dst_node->dat.size = total_size;

        return;
    }

    Zeta_CircularVector a_cv;
    a_cv.data = a_seg->dat.data;
    a_cv.width = width;
    a_cv.stride = stride;
    a_cv.offset = a_seg->dat.offset;
    a_cv.size = a_seg->dat.size;
    a_cv.capacity = seg_capacity;

    Zeta_CircularVector b_cv;
    b_cv.data = b_seg->dat.data;
    b_cv.width = width;
    b_cv.stride = stride;
    b_cv.offset = b_seg->dat.offset;
    b_cv.size = b_seg->dat.size;
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
        PushRefR_(sv, &a_cv, b_seg->ref.beg, b_size);
    } else {
        SegShoveL(&a_cv, &b_cv, 0, 0, b_size);
    }

    goto MERGE_END;
}

B_MERGE: {
    if (a_c == ref_color) {
        PushRefL_(sv, &b_cv, a_seg->ref.beg, a_size);
    } else {
        SegShoveR(&a_cv, &b_cv, 0, 0, a_size);
    }

    goto MERGE_END;
}

MERGE_END:;

    if (a_c == ref_color) {
        a_seg->ref.size = 0;
    } else {
        a_seg->dat.offset = a_cv.offset;
        a_seg->dat.size = a_cv.size;
    }

    if (b_c == ref_color) {
        b_seg->ref.size = 0;
    } else {
        b_seg->dat.offset = b_cv.offset;
        b_seg->dat.size = b_cv.size;
    }
}

#define SetSegState_(seg_tmp, cv_tmp, vacant_tmp, seg, cv, vacant)        \
    {                                                                     \
        Zeta_StageVector_Seg* seg_tmp = (seg);                            \
        Zeta_CircularVector* cv_tmp = (cv);                               \
        size_t* vacant_tmp = (vacant);                                    \
                                                                          \
        if (GetSegColor(&seg_tmp->n) == ref_color) {                      \
            cv_tmp->size = seg_tmp->ref.size;                             \
            *vacant_tmp =                                                 \
                seg_capacity - ZETA_GetMinOf(cv_tmp->size, seg_capacity); \
        } else {                                                          \
            cv_tmp->data = seg_tmp->dat.data;                             \
            cv_tmp->offset = seg_tmp->dat.offset;                         \
            cv_tmp->size = seg_tmp->dat.size;                             \
            *vacant_tmp = seg_capacity - cv_tmp->size;                    \
        }                                                                 \
    }

#define SetSegState(seg, cv, vacant)                                   \
    SetSegState_(ZETA_UniqueName(ZETA_tmp), ZETA_UniqueName(ZETA_tmp), \
                 ZETA_UniqueName(ZETA_tmp), seg, cv, vacant)

static void RefShoveL_(Zeta_StageVector* sv, Zeta_CircularVector* l_cv,
                       Zeta_StageVector_Seg* r_seg, size_t rl_size,
                       size_t ins_cnt, size_t shove_cnt) {
    ZETA_CheckAssert(rl_size <= r_seg->ref.size);
    ZETA_CheckAssert(shove_cnt <= l_cv->capacity - l_cv->size);
    ZETA_CheckAssert(shove_cnt <= r_seg->ref.size + ins_cnt);

    ZETA_CheckAssert(GetSegColor(&r_seg->n) == ref_color);

    size_t origin_l_size = l_cv->size;
    size_t origin_r_size = r_seg->ref.size;

    size_t stride = l_cv->stride;

    size_t cnt_a = ZETA_GetMinOf(rl_size, shove_cnt);
    size_t cnt_b = ZETA_GetMinOf(ins_cnt, shove_cnt - cnt_a);
    size_t cnt_c = shove_cnt - cnt_a - cnt_b;

    size_t l_i = l_cv->size;

    Zeta_CircularVector_Cursor l_cursor;
    Zeta_Cursor r_cursor;

    Zeta_CircularVector_GetRBCursor(l_cv, &l_cursor);

    Zeta_CircularVector_Insert(l_cv, &l_cursor, shove_cnt);

    sv->origin->Access(sv->origin->context, &r_cursor, NULL, r_seg->ref.beg);

    for (size_t i = cnt_a; 0 < i--; ++l_i) {
        sv->origin->Read(sv->origin->context, &r_cursor, 1,
                         Zeta_CircularVector_Access(l_cv, NULL, NULL, l_i),
                         &r_cursor);
    }

    l_i += cnt_b;

    for (size_t i = cnt_c; 0 < i--; ++l_i) {
        sv->origin->Read(sv->origin->context, &r_cursor, 1,
                         Zeta_CircularVector_Access(l_cv, NULL, NULL, l_i),
                         &r_cursor);
    }

    ZETA_CheckAssert(
        sv->origin->Cursor_GetIdx(sv->origin->context, &r_cursor) ==
        r_seg->ref.beg + cnt_a + cnt_c);

    size_t new_r_size = r_seg->ref.size + ins_cnt - shove_cnt;

    if (cnt_b < ins_cnt) {
        ZETA_CheckAssert(cnt_c == 0);

        ZETA_CheckAssert(new_r_size <= sv->seg_capacity);

        unsigned char* data = AllocateData_(sv);
        unsigned char* data_i = data;

        if (cnt_a < rl_size) {
            sv->origin->Read(sv->origin->context, &r_cursor, rl_size - cnt_a,
                             data_i, &r_cursor);
        }

        data_i += stride * (rl_size - cnt_a + ins_cnt - cnt_b);

        if (rl_size < r_seg->ref.size) {
            sv->origin->Read(sv->origin->context, &r_cursor,
                             r_seg->ref.size - rl_size, data_i, &r_cursor);
        }

        ZETA_CheckAssert(rl_size - cnt_a + ins_cnt - cnt_b + r_seg->ref.size -
                             rl_size ==
                         new_r_size);

        SetSegColor(&r_seg->n, dat_color);

        r_seg->dat.data = data;
        r_seg->dat.offset = 0;
        r_seg->dat.size = new_r_size;
    } else {
        r_seg->ref.beg += cnt_a + cnt_c;
        r_seg->ref.size = new_r_size;
    }

    ZETA_CheckAssert(l_cv->size == origin_l_size + shove_cnt);
    ZETA_CheckAssert(new_r_size == origin_r_size + ins_cnt - shove_cnt);
    ZETA_CheckAssert(new_r_size ==
                     origin_r_size - cnt_a - cnt_c + ins_cnt - cnt_b);
}

static void RefShoveR_(Zeta_StageVector* sv, Zeta_StageVector_Seg* l_seg,
                       Zeta_CircularVector* r_cv, size_t lr_size,
                       size_t ins_cnt, size_t shove_cnt) {
    ZETA_CheckAssert(lr_size <= l_seg->ref.size);
    ZETA_CheckAssert(shove_cnt <= r_cv->capacity - r_cv->size);
    ZETA_CheckAssert(shove_cnt <= l_seg->ref.size + ins_cnt);

    ZETA_CheckAssert(GetSegColor(&l_seg->n) == ref_color);

    size_t origin_l_size = l_seg->ref.size;
    size_t origin_r_size = r_cv->size;

    size_t stride = r_cv->stride;

    size_t cnt_a = ZETA_GetMinOf(lr_size, shove_cnt);
    size_t cnt_b = ZETA_GetMinOf(ins_cnt, shove_cnt - cnt_a);
    size_t cnt_c = shove_cnt - cnt_a - cnt_b;

    size_t r_i = 0;

    Zeta_Cursor l_cursor;
    Zeta_CircularVector_Cursor r_cursor;

    Zeta_CircularVector_PeekL(r_cv, &r_cursor, NULL);

    Zeta_CircularVector_Insert(r_cv, &r_cursor, shove_cnt);

    sv->origin->Access(sv->origin->context, &l_cursor, NULL,
                       l_seg->ref.beg + l_seg->ref.size - cnt_a - cnt_c);

    for (size_t i = cnt_c; 0 < i--; ++r_i) {
        sv->origin->Read(sv->origin->context, &l_cursor, 1,
                         Zeta_CircularVector_Access(r_cv, NULL, NULL, r_i),
                         &l_cursor);
    }

    r_i += cnt_b;

    for (size_t i = cnt_a; 0 < i--; ++r_i) {
        sv->origin->Read(sv->origin->context, &l_cursor, 1,
                         Zeta_CircularVector_Access(r_cv, NULL, NULL, r_i),
                         &l_cursor);
    }

    size_t new_l_size = l_seg->ref.size + ins_cnt - shove_cnt;

    if (cnt_b < ins_cnt) {
        ZETA_CheckAssert(cnt_c == 0);

        ZETA_CheckAssert(new_l_size <= sv->seg_capacity);

        unsigned char* data = AllocateData_(sv);
        unsigned char* data_i = data;

        sv->origin->Access(sv->origin->context, &l_cursor, NULL,
                           l_seg->ref.beg);

        if (lr_size < l_seg->ref.size) {
            sv->origin->Read(sv->origin->context, &l_cursor,
                             l_seg->ref.size - lr_size, data_i, &l_cursor);
        }

        data_i += stride * (l_seg->ref.size - lr_size + ins_cnt - cnt_b);

        if (cnt_a < lr_size) {
            sv->origin->Read(sv->origin->context, &l_cursor, lr_size - cnt_a,
                             data_i, &l_cursor);
        }

        ZETA_CheckAssert(l_seg->ref.size - lr_size + ins_cnt - cnt_b + lr_size -
                             cnt_a ==
                         new_l_size);

        SetSegColor(&l_seg->n, dat_color);

        l_seg->dat.data = data;
        l_seg->dat.offset = 0;
        l_seg->dat.size = new_l_size;
    } else {
        l_seg->ref.size = new_l_size;
    }

    ZETA_CheckAssert(new_l_size == origin_l_size + ins_cnt - shove_cnt);
    ZETA_CheckAssert(r_cv->size == origin_r_size + shove_cnt);
}

void* Zeta_StageVector_Insert(void* sv_, void* pos_cursor_, size_t ins_cnt) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* pos_cursor = pos_cursor_;
    Zeta_StageVector_Cursor_Check(sv, pos_cursor);

    if (ins_cnt == 0) { return pos_cursor->ele; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = Zeta_StageVector_GetWidth(sv);
    size_t stride = Zeta_StageVector_GetStride(sv);
    size_t seg_capacity = sv->seg_capacity;

    void* m_n = pos_cursor->n;
    size_t seg_idx = pos_cursor->seg_idx;

    ZETA_DebugAssert(sv->lb != m_n);

    unsigned long long rand_seed = ZETA_PtrToAddr(m_n);

    void* l_n;
    void* r_n;

    Zeta_StageVector_Seg* l_seg;
    Zeta_StageVector_Seg* m_seg;
    Zeta_StageVector_Seg* r_seg;

    size_t l_vacant;
    size_t m_vacant;
    size_t r_vacant;

    Zeta_CircularVector l_cv;
    l_cv.width = width;
    l_cv.stride = stride;
    l_cv.capacity = seg_capacity;

    Zeta_CircularVector m_cv;
    m_cv.width = width;
    m_cv.stride = stride;
    m_cv.capacity = seg_capacity;

    Zeta_CircularVector r_cv;
    r_cv.width = width;
    r_cv.stride = stride;
    r_cv.capacity = seg_capacity;

    Zeta_CircularVector cv_cursor;

    if (seg_idx == 0) {
        l_n = Zeta_BinTree_StepL(&btn_opr, m_n);

        r_n = m_n;

        m_n = NULL;
        m_seg = NULL;

        if (sv->lb == l_n) {
            l_cv.size = 0;
            l_vacant = 0;
        } else {
            l_seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, l_n);
            SetSegState(l_seg, &l_cv, &l_vacant);
        }

        if (sv->rb == r_n) {
            r_cv.size = 0;
            r_vacant = 0;
        } else {
            r_seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, r_n);
            SetSegState(r_seg, &r_cv, &r_vacant);
        }
    } else {
        m_seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, m_n);
        SetSegState(m_seg, &m_cv, &m_vacant);

        if (ins_cnt <= m_vacant) {
            void* ele;

            if (GetSegColor(m_n) == ref_color) {
                m_cv.data = AllocateData_(sv);
                m_cv.offset = 0;
                m_cv.size = 0;

                RefShoveL_(sv, &m_cv, m_seg, seg_idx, ins_cnt,
                           m_seg->ref.size + ins_cnt);

                ZETA_CheckAssert(m_seg->ref.size == 0);

                SetSegColor(&m_seg->n, dat_color);

                m_seg->dat.data = m_cv.data;

                ele = Zeta_CircularVector_Access(&m_cv, NULL, NULL, seg_idx);
            } else {
                Zeta_CircularVector_Access(&m_cv, &cv_cursor, NULL, seg_idx);
                ele = Zeta_CircularVector_Insert(&m_cv, &cv_cursor, ins_cnt);
            }

            m_seg->dat.offset = m_cv.offset;
            m_seg->dat.size = m_cv.size;
            Zeta_BinTree_SetSize(&btn_opr, m_n, m_cv.size);

            pos_cursor->n = m_n;
            pos_cursor->seg_idx = seg_idx;
            pos_cursor->ele = ele;

            return ele;
        }

        l_n = Zeta_BinTree_StepL(&btn_opr, m_n);
        r_n = Zeta_BinTree_StepR(&btn_opr, m_n);

        if (sv->lb == l_n) {
            l_cv.size = 0;
            l_vacant = 0;
        } else {
            l_seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, l_n);
            SetSegState(l_seg, &l_cv, &l_vacant);
        }

        if (sv->rb == r_n) {
            r_cv.size = 0;
            r_vacant = 0;
        } else {
            r_seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, r_n);
            SetSegState(r_seg, &r_cv, &r_vacant);
        }

        size_t ml_size = seg_idx;
        size_t mr_size = m_cv.size - seg_idx;

        size_t new_l_m_size = l_cv.size + m_cv.size + ins_cnt;
        size_t new_m_r_size = m_cv.size + r_cv.size + ins_cnt;

        size_t l_m_vacant = l_vacant + m_vacant;
        size_t m_r_vacant = m_vacant + r_vacant;

        bool_t l_m_ok =
            new_l_m_size <= seg_capacity * 2 && ins_cnt <= l_m_vacant;

        bool_t m_r_ok =
            new_m_r_size <= seg_capacity * 2 && ins_cnt <= m_r_vacant;

        if (!l_m_ok && !m_r_ok) { goto SPLIT; }

        int shove_dir =
            Zeta_Choose2(l_m_ok && m_r_vacant <= l_m_vacant,
                         m_r_ok && l_m_vacant <= m_r_vacant, &rand_seed);

        if (shove_dir == 0) {
            if (GetSegColor(l_n) == ref_color) {
                TransRefSegToDatSeg_(sv, l_seg);
                l_cv.data = l_seg->dat.data;
                l_cv.offset = l_seg->dat.offset;
            }

            size_t ret_seg_idx = l_cv.size + ml_size;

            size_t new_l_size = (new_l_m_size + (m_cv.size <= l_cv.size)) / 2;

            if (GetSegColor(m_n) == ref_color) {
                RefShoveL_(sv, &l_cv, m_seg, ml_size, ins_cnt,
                           new_l_size - l_cv.size);

                if (GetSegColor(m_n) == ref_color) {
                    m_cv.size = m_seg->ref.size;
                } else {
                    m_cv.data = m_seg->dat.data;
                    m_cv.offset = m_seg->dat.offset;
                    m_cv.size = m_seg->dat.size;
                }
            } else {
                SegShoveL(&l_cv, &m_cv, ml_size, ins_cnt,
                          new_l_size - l_cv.size);

                m_seg->dat.offset = m_cv.offset;
                m_seg->dat.size = m_cv.size;
            }

            Zeta_BinTree_SetSize(&btn_opr, m_n, m_cv.size);

            l_seg->dat.offset = l_cv.offset;
            l_seg->dat.size = l_cv.size;
            Zeta_BinTree_SetSize(&btn_opr, l_n, l_cv.size);

            if (ret_seg_idx < l_cv.size) {
                pos_cursor->n = l_n;
                pos_cursor->seg_idx = ret_seg_idx;
                pos_cursor->ele =
                    Zeta_CircularVector_Access(&l_cv, NULL, NULL, ret_seg_idx);
            } else {
                ret_seg_idx -= l_cv.size;

                pos_cursor->n = m_n;
                pos_cursor->seg_idx = ret_seg_idx;
                pos_cursor->ele = GetSegColor(m_n) == ref_color
                                      ? NULL
                                      : Zeta_CircularVector_Access(
                                            &m_cv, NULL, NULL, ret_seg_idx);
            }
        } else {
            if (GetSegColor(r_n) == ref_color) {
                TransRefSegToDatSeg_(sv, r_seg);
                r_cv.data = r_seg->dat.data;
                r_cv.offset = r_seg->dat.offset;
            }

            size_t ret_seg_idx = seg_idx;

            size_t new_r_size = (new_m_r_size + (m_cv.size <= r_cv.size)) / 2;

            if (GetSegColor(m_n) == ref_color) {
                RefShoveR_(sv, m_seg, &r_cv, mr_size, ins_cnt,
                           new_r_size - r_cv.size);

                if (GetSegColor(m_n) == ref_color) {
                    m_cv.size = m_seg->ref.size;
                } else {
                    m_cv.data = m_seg->dat.data;
                    m_cv.offset = m_seg->dat.offset;
                    m_cv.size = m_seg->dat.size;
                }
            } else {
                SegShoveR(&m_cv, &r_cv, mr_size, ins_cnt,
                          new_r_size - r_cv.size);

                m_seg->dat.offset = m_cv.offset;
                m_seg->dat.size = m_cv.size;
            }

            Zeta_BinTree_SetSize(&btn_opr, m_n, m_cv.size);

            r_seg->dat.offset = r_cv.offset;
            r_seg->dat.size = r_cv.size;
            Zeta_BinTree_SetSize(&btn_opr, r_n, r_cv.size);

            if (ret_seg_idx < m_cv.size) {
                pos_cursor->n = m_n;
                pos_cursor->seg_idx = ret_seg_idx;
                pos_cursor->ele = GetSegColor(m_n) == ref_color
                                      ? NULL
                                      : Zeta_CircularVector_Access(
                                            &m_cv, NULL, NULL, ret_seg_idx);
            } else {
                ret_seg_idx -= m_cv.size;

                pos_cursor->n = r_n;
                pos_cursor->seg_idx = ret_seg_idx;
                pos_cursor->ele =
                    Zeta_CircularVector_Access(&r_cv, NULL, NULL, ret_seg_idx);
            }
        }

        return pos_cursor->ele;

    SPLIT:;

        if (ml_size <= l_vacant) {
            if (GetSegColor(l_n) == ref_color) {
                TransRefSegToDatSeg_(sv, l_seg);

                l_cv.data = l_seg->dat.data;
                l_cv.offset = l_seg->dat.offset;
            }

            if (GetSegColor(m_n) == ref_color) {
                PushRefR_(sv, &l_cv, m_seg->ref.beg, ml_size);

                m_seg->ref.beg += ml_size;
                m_cv.size -= ml_size;
            } else {
                SegShoveL(&l_cv, &m_cv, 0, 0, ml_size);
            }

            ml_size = 0;
            l_vacant = seg_capacity - l_cv.size;
        }

        if (mr_size <= r_vacant) {
            if (GetSegColor(r_n) == ref_color) {
                TransRefSegToDatSeg_(sv, r_seg);

                r_cv.data = r_seg->dat.data;
                r_cv.offset = r_seg->dat.offset;
            }

            if (GetSegColor(m_n) == ref_color) {
                PushRefL_(sv, &r_cv, m_seg->ref.beg + ml_size, mr_size);

                m_cv.size -= mr_size;
            } else {
                SegShoveR(&m_cv, &r_cv, 0, 0, mr_size);
            }

            mr_size = 0;
            r_vacant = seg_capacity - r_cv.size;
        }

        if (ml_size == 0 && mr_size == 0) { goto INS; }

        if (ml_size == 0) {
            r_n = m_n;
            r_seg = m_seg;
            r_cv.data = m_cv.data;
            r_cv.offset = m_cv.offset;
            r_cv.size = m_cv.size;
            r_vacant = seg_capacity - ZETA_GetMinOf(r_cv.size, seg_capacity);

            m_n = NULL;
            m_seg = NULL;

            goto INS;
        }

        if (mr_size == 0) {
            l_n = m_n;
            l_seg = m_seg;
            l_cv.data = m_cv.data;
            l_cv.offset = m_cv.offset;
            l_cv.size = m_cv.size;
            l_vacant = seg_capacity - ZETA_GetMinOf(l_cv.size, seg_capacity);

            m_n = NULL;
            m_seg = NULL;

            goto INS;
        }

        int split_dir =
            Zeta_Choose2(ml_size <= mr_size, mr_size <= ml_size, &rand_seed);

        if (split_dir == 0) {
            if (GetSegColor(m_n) == ref_color) {
                Zeta_StageVector_Seg* new_l_seg = AllocateRefSeg_(sv);
                sv->root =
                    Zeta_RBTree_Insert(&btn_opr, l_n, m_n, &new_l_seg->n);

                l_n = &new_l_seg->n;
                l_seg = new_l_seg;
                l_seg->ref.beg = m_seg->ref.beg;
                l_cv.size = ml_size;
                l_vacant =
                    seg_capacity - ZETA_GetMinOf(l_cv.size, seg_capacity);

                r_n = m_n;
                r_seg = m_seg;
                r_seg->ref.beg += ml_size;
                r_cv.size = mr_size;
                r_vacant =
                    seg_capacity - ZETA_GetMinOf(r_cv.size, seg_capacity);
            } else {
                Zeta_StageVector_Seg* new_l_seg = AllocateDatSeg_(sv);
                sv->root =
                    Zeta_RBTree_Insert(&btn_opr, l_n, m_n, &new_l_seg->n);

                l_n = &new_l_seg->n;
                l_seg = new_l_seg;
                l_cv.data = new_l_seg->dat.data;
                l_cv.offset = 0;
                l_cv.size = 0;

                r_n = m_n;
                r_seg = m_seg;
                r_cv.data = m_cv.data;
                r_cv.offset = m_cv.offset;
                r_cv.size = m_cv.size;

                SegShoveL(&l_cv, &r_cv, 0, 0, ml_size);

                l_vacant = seg_capacity - l_cv.size;
                r_vacant = seg_capacity - r_cv.size;
            }
        } else {
            if (GetSegColor(m_n) == ref_color) {
                Zeta_StageVector_Seg* new_r_seg = AllocateRefSeg_(sv);
                sv->root =
                    Zeta_RBTree_Insert(&btn_opr, m_n, r_n, &new_r_seg->n);

                l_n = m_n;
                l_seg = m_seg;
                l_cv.size = ml_size;
                l_vacant =
                    seg_capacity - ZETA_GetMinOf(l_cv.size, seg_capacity);

                r_n = &new_r_seg->n;
                r_seg = new_r_seg;
                r_seg->ref.beg = m_seg->ref.beg + ml_size;
                r_cv.size = mr_size;
                r_vacant =
                    seg_capacity - ZETA_GetMinOf(r_cv.size, seg_capacity);
            } else {
                Zeta_StageVector_Seg* new_r_seg = AllocateDatSeg_(sv);
                sv->root =
                    Zeta_RBTree_Insert(&btn_opr, m_n, r_n, &new_r_seg->n);

                l_n = m_n;
                l_seg = m_seg;
                l_cv.data = m_cv.data;
                l_cv.offset = m_cv.offset;
                l_cv.size = m_cv.size;

                r_n = &new_r_seg->n;
                r_seg = new_r_seg;
                r_cv.data = r_seg->dat.data;
                r_cv.offset = 0;
                r_cv.size = 0;

                SegShoveR(&l_cv, &r_cv, 0, 0, mr_size);

                l_vacant = seg_capacity - l_cv.size;
                r_vacant = seg_capacity - r_cv.size;
            }
        }

        m_n = NULL;
        m_seg = NULL;
    }

INS:;

    if (sv->lb != l_n) {
        ZETA_CheckAssert(l_vacant ==
                         seg_capacity - ZETA_GetMinOf(l_cv.size, seg_capacity));
    }

    if (sv->rb != r_n) {
        ZETA_CheckAssert(r_vacant ==
                         seg_capacity - ZETA_GetMinOf(r_cv.size, seg_capacity));
    }

    bool_t l_is_ref = sv->lb != l_n && GetSegColor(l_n) == ref_color;
    bool_t r_is_ref = sv->rb != r_n && GetSegColor(r_n) == ref_color;

    bool_t l_shove;
    bool_t r_shove;

    {
        size_t res_size =
            seg_capacity - 1 - (ins_cnt + seg_capacity - 1) % seg_capacity;

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
        l_seg->ref.size = l_cv.size;

        TransRefSegToDatSeg_(sv, l_seg);

        l_cv.data = l_seg->dat.data;
        l_cv.offset = 0;
    }

    if (r_is_ref && r_shove) {
        r_seg->ref.size = r_cv.size;

        TransRefSegToDatSeg_(sv, r_seg);

        r_cv.data = r_seg->dat.data;
        r_cv.offset = 0;
    }

    size_t total_size =
        (l_shove ? l_cv.size : 0) + ins_cnt + (r_shove ? r_cv.size : 0);

    size_t ret_seg_idx = l_cv.size;

    if (l_shove && r_shove && total_size <= seg_capacity) {
        if (Zeta_Choose2(r_cv.size <= l_cv.size, l_cv.size <= r_cv.size,
                         &rand_seed) == 0) {
            SegShoveL(&l_cv, &r_cv, 0, ins_cnt, ins_cnt + r_cv.size);

            EraseSeg_(sv, &btn_opr, r_seg);

            l_seg->dat.offset = l_cv.offset;
            l_seg->dat.size = l_cv.size;
            Zeta_BinTree_SetSize(&btn_opr, l_n, l_cv.size);

            pos_cursor->n = l_n;
            pos_cursor->seg_idx = ret_seg_idx;
            pos_cursor->ele =
                Zeta_CircularVector_Access(&l_cv, NULL, NULL, ret_seg_idx);
        } else {
            SegShoveR(&l_cv, &r_cv, 0, ins_cnt, l_cv.size + ins_cnt);

            EraseSeg_(sv, &btn_opr, l_seg);

            r_seg->dat.offset = r_cv.offset;
            r_seg->dat.size = r_cv.size;
            Zeta_BinTree_SetSize(&btn_opr, r_n, r_cv.size);

            pos_cursor->n = r_n;
            pos_cursor->seg_idx = ret_seg_idx;
            pos_cursor->ele =
                Zeta_CircularVector_Access(&r_cv, NULL, NULL, ret_seg_idx);
        }

        return pos_cursor->ele;
    }

    size_t new_l_size;
    size_t new_r_size;

    if (l_shove) {
        new_l_size = GetAvgCnt(total_size);
        total_size -= new_l_size;
    } else {
        new_l_size = l_cv.size;
    }

    if (r_shove) {
        new_r_size = GetAvgCnt(total_size);
        total_size -= new_r_size;
    } else {
        new_r_size = r_cv.size;
    }

    ZETA_CheckAssert(l_cv.size <= new_l_size);
    ZETA_CheckAssert(r_cv.size <= new_r_size);

    if (l_cv.size < new_l_size) {
        Zeta_CircularVector_Cursor cv_cursor;
        Zeta_CircularVector_GetRBCursor(&l_cv, &cv_cursor);
        Zeta_CircularVector_Insert(&l_cv, &cv_cursor, new_l_size - l_cv.size);
    }

    if (r_cv.size < new_r_size) {
        Zeta_CircularVector_Cursor cv_cursor;
        Zeta_CircularVector_PeekL(&r_cv, &cv_cursor, NULL);
        Zeta_CircularVector_Insert(&r_cv, &cv_cursor, new_r_size - r_cv.size);
    }

    if (sv->lb != l_n) {
        if (GetSegColor(l_n) == ref_color) {
            l_seg->ref.size = l_cv.size;
        } else {
            l_seg->dat.offset = l_cv.offset;
            l_seg->dat.size = l_cv.size;
        }

        Zeta_BinTree_SetSize(&btn_opr, l_n, l_cv.size);
    }

    if (sv->rb != r_n) {
        if (GetSegColor(r_n) == ref_color) {
            r_seg->ref.size = r_cv.size;
        } else {
            r_seg->dat.offset = r_cv.offset;
            r_seg->dat.size = r_cv.size;
        }

        Zeta_BinTree_SetSize(&btn_opr, r_n, r_cv.size);
    }

    if (0 < total_size && m_n != NULL) {
        ZETA_CheckAssert(Zeta_BinTree_StepR(&btn_opr, l_n) == m_n);
        ZETA_CheckAssert(Zeta_BinTree_StepR(&btn_opr, m_n) == r_n);

        size_t cur_size = GetAvgCnt(total_size);
        total_size -= cur_size;

        if (GetSegColor(m_n) == ref_color) {
            SetSegColor(m_n, dat_color);
            m_seg->dat.data = AllocateData_(sv);
        }

        m_seg->dat.offset = 0;
        m_seg->dat.size = cur_size;
        Zeta_BinTree_SetSize(&btn_opr, m_n, cur_size);

        ZETA_CheckAssert(&m_seg->n == m_n);

        r_n = m_n;
        r_seg = m_seg;

        m_n = NULL;
        m_seg = NULL;
    }

    ZETA_CheckAssert(Zeta_BinTree_StepR(&btn_opr, l_n) == r_n);

    while (0 < total_size) {
        size_t cur_size = GetAvgCnt(total_size);
        total_size -= cur_size;

        Zeta_StageVector_Seg* new_r_seg = AllocateDatSeg_(sv);

        sv->root = Zeta_RBTree_Insert(&btn_opr, l_n, r_n, &new_r_seg->n);

        new_r_seg->dat.size = cur_size;
        Zeta_BinTree_SetSize(&btn_opr, &new_r_seg->n, cur_size);

        r_n = &new_r_seg->n;
        r_seg = new_r_seg;
    }

    if (m_n != NULL) { EraseSeg_(sv, &btn_opr, m_seg); }

    if (ret_seg_idx < l_cv.size) {
        pos_cursor->n = l_n;
        pos_cursor->seg_idx = ret_seg_idx;

        if (GetSegColor(l_n) == ref_color) {
            pos_cursor->ele = NULL;
        } else {
            l_cv.data = l_seg->dat.data;
            l_cv.offset = l_seg->dat.offset;
            l_cv.size = l_seg->dat.size;

            pos_cursor->ele =
                Zeta_CircularVector_Access(&l_cv, NULL, NULL, ret_seg_idx);
        }

        return pos_cursor->ele;
    }

    ret_seg_idx -= l_cv.size;

    pos_cursor->n = r_n;
    pos_cursor->seg_idx = ret_seg_idx;

    if (GetSegColor(r_n) == ref_color) {
        pos_cursor->ele = NULL;
    } else {
        r_cv.data = r_seg->dat.data;
        r_cv.offset = r_seg->dat.offset;
        r_cv.size = r_seg->dat.size;

        pos_cursor->ele =
            Zeta_CircularVector_Access(&r_cv, NULL, NULL, ret_seg_idx);
    }

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

    Zeta_StageVector_Seg* m_node =
        ZETA_MemberToStruct(Zeta_StageVector_Seg, n, m_n);

    if (GetSegColor(m_n) == ref_color && 0 < seg_idx) {
        l_n = SplitRefSeg_(sv, &btn_opr, width, stride, l_n, m_node, seg_idx);
        seg_idx = 0;
    }

    Zeta_StageVector_Seg* first_node =
        ZETA_MemberToStruct(Zeta_StageVector_Seg, n, m_n);
    size_t first_seg_idx = seg_idx;

    Zeta_StageVector_Seg* last_node;

    Zeta_CircularVector m_cv;
    m_cv.width = width;
    m_cv.stride = stride;
    m_cv.capacity = seg_capacity;

    while (0 < cnt) {
        m_node = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, m_n);
        last_node = m_node;

        if (GetSegColor(m_n) == ref_color) {
            if (m_node->ref.size <= cnt) {
                cnt -= m_node->ref.size;
                void* nxt_m_n = Zeta_BinTree_StepR(&btn_opr, m_n);
                EraseSeg_(sv, &btn_opr, m_node);
                m_n = nxt_m_n;

                continue;
            }

            m_node->ref.beg += cnt;
            m_node->ref.size -= cnt;

            break;
        }

        if (seg_idx == 0 && m_node->dat.size <= cnt) {
            cnt -= m_node->dat.size;
            void* nxt_m_n = Zeta_BinTree_StepR(&btn_opr, m_n);
            EraseSeg_(sv, &btn_opr, m_node);
            m_n = nxt_m_n;

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

    unsigned long long rand_seed = ZETA_PtrToAddr(m_n);

    bool_t first_exist = first_seg_idx != 0 || &first_node->n == m_n;
    bool_t last_exist = &last_node->n == m_n;

    Zeta_StageVector_Seg* a_seg;
    Zeta_StageVector_Seg* b_seg;
    Zeta_StageVector_Seg* c_seg;
    Zeta_StageVector_Seg* d_seg;

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
        Zeta_StageVector_Seg* segs_buffer[4] = { NULL, NULL, NULL, NULL };
        size_t segs_buffer_i = 0;

        if (sv->lb != l_n) {
            segs_buffer[segs_buffer_i++] =
                ZETA_MemberToStruct(Zeta_StageVector_Seg, n, l_n);
        }

        if (first_exist) { segs_buffer[segs_buffer_i++] = first_node; }

        if (last_exist && first_node != last_node) {
            segs_buffer[segs_buffer_i++] = last_node;
        }

        if (sv->rb != r_n) {
            segs_buffer[segs_buffer_i++] =
                ZETA_MemberToStruct(Zeta_StageVector_Seg, n, r_n);
        }

        a_seg = segs_buffer[0];
        b_seg = segs_buffer[1];
        c_seg = segs_buffer[2];
        d_seg = segs_buffer[3];
    }

    if (a_seg == NULL) {
    } else if (GetSegColor(&a_seg->n) == ref_color) {
        a_size = a_seg->ref.size;
        a_vacant = seg_capacity - ZETA_GetMinOf(a_size, seg_capacity);
    } else {
        a_size = a_seg->dat.size;
        a_vacant = seg_capacity - a_size;
    }

    if (b_seg == NULL) {
    } else if (GetSegColor(&b_seg->n) == ref_color) {
        b_size = b_seg->ref.size;
        b_vacant = seg_capacity - ZETA_GetMinOf(b_size, seg_capacity);
    } else {
        b_size = b_seg->dat.size;
        b_vacant = seg_capacity - b_size;
    }

    if (c_seg == NULL) {
    } else if (GetSegColor(&c_seg->n) == ref_color) {
        c_size = c_seg->ref.size;
        c_vacant = seg_capacity - ZETA_GetMinOf(c_size, seg_capacity);
    } else {
        c_size = c_seg->dat.size;
        c_vacant = seg_capacity - c_size;
    }

    if (d_seg == NULL) {
    } else if (GetSegColor(&d_seg->n) == ref_color) {
        d_size = d_seg->ref.size;
        d_vacant = seg_capacity - ZETA_GetMinOf(d_size, seg_capacity);
    } else {
        d_size = d_seg->dat.size;
        d_vacant = seg_capacity - d_size;
    }

    //

    //

    if (a_seg == NULL) { goto UPDATE; }

    if (ret_n == &a_seg->n) { ret_n = NULL; }

    if (b_seg == NULL) { goto UPDATE; }

    if (ret_n == &b_seg->n) {
        ret_n = NULL;
        ret_seg_idx += a_size;
    }

    if (GetSegColor(&a_seg->n) == ref_color &&
        GetSegColor(&b_seg->n) == ref_color &&
        a_seg->ref.beg + a_seg->ref.size == b_seg->ref.beg) {
        a_seg->ref.size += b_seg->ref.size;

        EraseSeg_(sv, &btn_opr, b_seg);

        if (c_seg == NULL) { goto UPDATE; }

        b_seg = c_seg;
        c_seg = d_seg;
        d_seg = NULL;
    }

    if (c_seg == NULL) { goto MERGE_2; }

    if (ret_n == &c_seg->n) {
        ret_n = NULL;
        ret_seg_idx += a_size + b_size;
    }

    if (GetSegColor(&b_seg->n) == ref_color &&
        GetSegColor(&c_seg->n) == ref_color &&
        b_seg->ref.beg + b_seg->ref.size == c_seg->ref.beg) {
        b_seg->ref.size += c_seg->ref.size;

        EraseSeg_(sv, &btn_opr, c_seg);

        if (d_seg == NULL) { goto MERGE_2; }

        c_seg = d_seg;
        d_seg = NULL;

        goto MERGE_3;
    }

    if (d_seg == NULL) { goto MERGE_3; }

    if (ret_n == &d_seg->n) {
        ret_n = NULL;
        ret_seg_idx += a_size + b_size + c_size;
    }

    if (GetSegColor(&c_seg->n) == ref_color &&
        GetSegColor(&d_seg->n) == ref_color &&
        c_seg->ref.beg + c_seg->ref.size == d_seg->ref.beg) {
        c_seg->ref.size += d_seg->ref.size;

        EraseSeg_(sv, &btn_opr, d_seg);

        goto MERGE_3;
    }

    goto MERGE_4;

MERGE_2: {
    size_t ab_vacant = a_vacant + b_vacant;

    if (seg_capacity <= ab_vacant) { Merge2_(sv, width, stride, a_seg, b_seg); }

    goto UPDATE;
}

MERGE_3: {
    size_t ab_vacant = a_vacant + b_vacant;
    size_t bc_vacant = b_vacant + c_vacant;

    size_t max_vacant = ZETA_GetMaxOf(ab_vacant, bc_vacant);

    if (seg_capacity < max_vacant) {
        if (Zeta_Choose2(bc_vacant <= ab_vacant, ab_vacant <= bc_vacant,
                         &rand_seed) == 0) {
            Merge2_(sv, width, stride, a_seg, b_seg);
        } else {
            Merge2_(sv, width, stride, b_seg, c_seg);
        }
    }

    goto UPDATE;
}

MERGE_4: {
    size_t ab_vacant = a_vacant + b_vacant;
    size_t bc_vacant = b_vacant + c_vacant;
    size_t cd_vacant = c_vacant + d_vacant;

    if (seg_capacity <= ab_vacant && seg_capacity <= cd_vacant) {
        Merge2_(sv, width, stride, a_seg, b_seg);
        Merge2_(sv, width, stride, c_seg, d_seg);
    } else {
        size_t max_vacant =
            ZETA_GetMaxOf(ZETA_GetMaxOf(ab_vacant, bc_vacant), cd_vacant);

        if (seg_capacity <= max_vacant) {
            switch (Zeta_Choose3(ab_vacant == max_vacant,
                                 bc_vacant == max_vacant,
                                 cd_vacant == max_vacant, &rand_seed)) {
                case 0: Merge2_(sv, width, stride, a_seg, b_seg); break;
                case 1: Merge2_(sv, width, stride, b_seg, c_seg); break;
                case 2: Merge2_(sv, width, stride, c_seg, d_seg); break;
            }
        }
    }

    goto UPDATE;
}

UPDATE: {
    if (a_seg == NULL) { goto UPDATE_END; }

    a_size =
        GetSegColor(&a_seg->n) == ref_color ? a_seg->ref.size : a_seg->dat.size;

    if (a_size == 0) {
        EraseSeg_(sv, &btn_opr, a_seg);
    } else {
        Zeta_BinTree_SetSize(&btn_opr, &a_seg->n, a_size);
    }

    if (ret_n != NULL) {
    } else if (ret_seg_idx < a_size) {
        ret_n = &a_seg->n;
    } else {
        ret_seg_idx -= a_size;
    }

    if (b_seg == NULL) { goto UPDATE_END; }

    b_size =
        GetSegColor(&b_seg->n) == ref_color ? b_seg->ref.size : b_seg->dat.size;

    if (b_size == 0) {
        EraseSeg_(sv, &btn_opr, b_seg);
    } else {
        Zeta_BinTree_SetSize(&btn_opr, &b_seg->n, b_size);
    }

    if (ret_n != NULL) {
    } else if (ret_seg_idx < b_size) {
        ret_n = &b_seg->n;
    } else {
        ret_seg_idx -= b_size;
    }

    if (c_seg == NULL) { goto UPDATE_END; }

    c_size =
        GetSegColor(&c_seg->n) == ref_color ? c_seg->ref.size : c_seg->dat.size;

    if (c_size == 0) {
        EraseSeg_(sv, &btn_opr, c_seg);
    } else {
        Zeta_BinTree_SetSize(&btn_opr, &c_seg->n, c_size);
    }

    if (ret_n != NULL) {
    } else if (ret_seg_idx < c_size) {
        ret_n = &c_seg->n;
    } else {
        ret_seg_idx -= c_size;
    }

    if (d_seg == NULL) { goto UPDATE_END; }

    d_size =
        GetSegColor(&d_seg->n) == ref_color ? d_seg->ref.size : d_seg->dat.size;

    if (d_size == 0) {
        EraseSeg_(sv, &btn_opr, d_seg);
    } else {
        Zeta_BinTree_SetSize(&btn_opr, &d_seg->n, d_size);
    }

    if (ret_n == NULL) { ret_n = &d_seg->n; }
}

UPDATE_END:;

    pos_cursor->n = ret_n;
    pos_cursor->seg_idx = ret_seg_idx;

    if (sv->rb == pos_cursor->n || GetSegColor(pos_cursor->n) == ref_color) {
        pos_cursor->ele = NULL;
        return;
    }

    ZETA_CheckAssert(sv->lb != pos_cursor->n);
    ZETA_CheckAssert(GetSegColor(pos_cursor->n) == dat_color);

    Zeta_StageVector_Seg* seg =
        ZETA_MemberToStruct(Zeta_StageVector_Seg, n, pos_cursor->n);

    m_cv.data = seg->dat.data;
    m_cv.offset = seg->dat.offset;
    m_cv.size = seg->dat.size;

    pos_cursor->ele =
        Zeta_CircularVector_Access(&m_cv, NULL, NULL, pos_cursor->seg_idx);
}

static void EraseAll_(Zeta_StageVector* sv, Zeta_BinTreeNodeOperator* btn_opr,
                      void* n) {
    void* nl = Zeta_OrdCnt3RBTreeNode_GetL(NULL, n);
    void* nr = Zeta_OrdCnt3RBTreeNode_GetR(NULL, n);

    if (sv->lb != n && sv->rb != n) {
        Zeta_StageVector_Seg* seg =
            ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        if (GetSegColor(n) == dat_color) {
            sv->data_allocator->Deallocate(sv->data_allocator->context,
                                           seg->dat.data);
        }

        sv->seg_allocator->Deallocate(sv->seg_allocator->context, seg);
    }

    if (nl != NULL) { EraseAll_(sv, btn_opr, nl); }

    if (nr != NULL) { EraseAll_(sv, btn_opr, nr); }
}

void Zeta_StageVector_EraseAll(void* sv_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    ZETA_DebugAssert(sv->seg_allocator != NULL);
    ZETA_DebugAssert(sv->data_allocator != NULL);

    ZETA_DebugAssert(sv->seg_allocator->Deallocate != NULL);
    ZETA_DebugAssert(sv->data_allocator->Deallocate != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    EraseAll_(sv, &btn_opr, sv->root);

    InitTree_(sv, &btn_opr);
}

void Zeta_StageVector_Reset(void* sv_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    ZETA_DebugAssert(sv->seg_allocator != NULL);
    ZETA_DebugAssert(sv->data_allocator != NULL);

    ZETA_DebugAssert(sv->seg_allocator->Deallocate != NULL);
    ZETA_DebugAssert(sv->data_allocator->Deallocate != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    EraseAll_(sv, &btn_opr, sv->root);

    InitTree_(sv, &btn_opr);
    RefOrigin_(sv, &btn_opr);
}

ZETA_DeclareStruct(CheckRet);

struct CheckRet {
    bool_t b;
    size_t ref_beg;
    size_t ref_end;
};

static CheckRet Check_(Zeta_StageVector* sv, Zeta_DebugHashMap* dst_node_hm,
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
    } else if (GetSegColor(n) == ref_color) {
        ZETA_DebugAssert(l_n != NULL);
        ZETA_DebugAssert(r_n != NULL);

        Zeta_StageVector_Seg* seg =
            ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        {
            Zeta_DebugHashMap_KeyValPair p =
                Zeta_DebugHashMap_Insert(dst_node_hm, ZETA_PtrToAddr(seg));
            ZETA_DebugAssert(p.b);
            *p.val = sizeof(Zeta_StageVector_Seg);
        }

        ZETA_DebugAssert(0 < seg->ref.size);

        ZETA_DebugAssert(seg->ref.beg < origin_size);
        ZETA_DebugAssert(seg->ref.beg + seg->ref.size <= origin_size);

        size_t size = seg->ref.size;

        ZETA_DebugAssert(size == Zeta_BinTree_GetSize(btn_opr, n));

        if (sv->lb != l_n) {
            Zeta_StageVector_Seg* l_node =
                ZETA_MemberToStruct(Zeta_StageVector_Seg, n, l_n);

            if (GetSegColor(&l_node->n) == ref_color) {
                ZETA_DebugAssert(l_node->ref.beg + l_node->ref.size <
                                 seg->ref.beg);
            }

            /*
            if (GetSegColor(&l_node->n) == ref_color) {
                ZETA_DebugAssert(sv->seg_capacity <
                                 l_node->ref.size + seg->ref.size);
            } else {
                ZETA_DebugAssert(sv->seg_capacity <
                                 l_node->dat.size + seg->ref.size);
            }
            */
        }

        if (sv->rb != r_n) {
            Zeta_StageVector_Seg* r_node =
                ZETA_MemberToStruct(Zeta_StageVector_Seg, n, r_n);

            if (GetSegColor(&r_node->n) == ref_color) {
                ZETA_DebugAssert(seg->ref.beg + seg->ref.size <
                                 r_node->ref.beg);
            }

            /*
            if (GetSegColor(&r_node->n) == ref_color) {
                ZETA_DebugAssert(sv->seg_capacity <
                                 seg->ref.size + r_node->ref.size);
            } else {
                ZETA_DebugAssert(sv->seg_capacity <
                                 seg->ref.size + r_node->dat.size);
            }
            */
        }
    } else {
        ZETA_DebugAssert(GetSegColor(n) == dat_color);

        ZETA_DebugAssert(l_n != NULL);
        ZETA_DebugAssert(r_n != NULL);

        Zeta_StageVector_Seg* seg =
            ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        {
            Zeta_DebugHashMap_KeyValPair p =
                Zeta_DebugHashMap_Insert(dst_node_hm, ZETA_PtrToAddr(seg));
            ZETA_DebugAssert(p.b);
            *p.val = sizeof(Zeta_StageVector_Seg);
        }

        size_t size = seg->dat.size;

        ZETA_DebugAssert(size == Zeta_BinTree_GetSize(btn_opr, n));

        ZETA_DebugAssert(0 < size);
        ZETA_DebugAssert(size <= sv->seg_capacity);
        ZETA_DebugAssert(seg->dat.offset < sv->seg_capacity);

        {
            Zeta_DebugHashMap_KeyValPair p = Zeta_DebugHashMap_Insert(
                dst_seg_hm, ZETA_PtrToAddr(seg->dat.data));
            ZETA_DebugAssert(p.b);
            *p.val = stride * sv->seg_capacity;
        }

        size_t l_vacant;
        size_t r_vacant;

        if (sv->lb == l_n) {
            l_vacant = 0;
        } else {
            Zeta_StageVector_Seg* l_node =
                ZETA_MemberToStruct(Zeta_StageVector_Seg, n, l_n);

            l_vacant = sv->seg_capacity -
                       (GetSegColor(l_n) == ref_color
                            ? ZETA_GetMinOf(l_node->ref.size, sv->seg_capacity)
                            : l_node->dat.size);
        }

        if (sv->rb == r_n) {
            r_vacant = 0;
        } else {
            Zeta_StageVector_Seg* r_node =
                ZETA_MemberToStruct(Zeta_StageVector_Seg, n, r_n);

            r_vacant = sv->seg_capacity -
                       (GetSegColor(r_n) == ref_color
                            ? ZETA_GetMinOf(r_node->ref.size, sv->seg_capacity)
                            : r_node->dat.size);
        }

        ZETA_DebugAssert(0 <= l_vacant);
        ZETA_DebugAssert(0 <= r_vacant);
    }

    void* sub_l_n = Zeta_OrdCnt3RBTreeNode_GetL(NULL, n);
    void* sub_r_n = Zeta_OrdCnt3RBTreeNode_GetR(NULL, n);

    CheckRet l_check_ret = (CheckRet){ FALSE, 0, 0 };
    CheckRet r_check_ret = (CheckRet){ FALSE, 0, 0 };

    if (sub_l_n != NULL) {
        l_check_ret = Check_(sv, dst_node_hm, dst_seg_hm, btn_opr, sub_l_n);
    }

    if (sub_r_n != NULL) {
        r_check_ret = Check_(sv, dst_node_hm, dst_seg_hm, btn_opr, sub_r_n);
    }

    CheckRet ret = (CheckRet){ FALSE, 0, 0 };

    if (l_check_ret.b && r_check_ret.b) {
        ZETA_DebugAssert(l_check_ret.ref_end <= r_check_ret.ref_beg);

        ret.b = TRUE;
        ret.ref_beg = l_check_ret.ref_beg;
        ret.ref_end = r_check_ret.ref_end;
    } else if (l_check_ret.b) {
        ret = l_check_ret;
    } else if (r_check_ret.b) {
        ret = r_check_ret;
    }

    if (sv->lb == n || sv->rb == n) { return ret; }

    Zeta_StageVector_Seg* seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

    if (GetSegColor(n) == dat_color) { return ret; }

    size_t cur_ref_beg = seg->ref.beg;
    size_t cur_ref_end = seg->ref.beg + seg->ref.size;

    if (l_check_ret.b) { ZETA_DebugAssert(l_check_ret.ref_end <= cur_ref_beg); }
    if (r_check_ret.b) { ZETA_DebugAssert(cur_ref_end <= r_check_ret.ref_beg); }

    if (ret.b) {
        ret.ref_beg = ZETA_GetMinOf(ret.ref_beg, cur_ref_beg);
        ret.ref_end = ZETA_GetMaxOf(ret.ref_end, cur_ref_end);
    } else {
        ret.b = TRUE;
        ret.ref_beg = cur_ref_beg;
        ret.ref_end = cur_ref_end;
    }

    return ret;
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
        Zeta_StageVector_Seg* seg =
            ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        if (GetSegColor(&seg->n) == ref_color) {
            printf("ref %8zu [%8zu, %8zu]\n", seg->ref.size, seg->ref.beg,
                   seg->ref.beg + seg->ref.size);
        } else {
            printf("dat %8zu\n", seg->ref.size);
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
        Zeta_StageVector_Seg* seg =
            ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        if (GetSegColor(n) == ref_color) {
            ++ret.ref_seg_cnt;
            ret.ref_size += seg->ref.size;
        } else {
            ++ret.dat_seg_cnt;
            ret.dat_size += seg->dat.size;
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

    Zeta_StageVector_Seg* seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

    size_t n_size =
        (sv->lb == n || sv->rb == n)
            ? 1
            : (GetSegColor(n) == ref_color ? seg->ref.size : seg->dat.size);

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

    Zeta_StageVector_Seg* dst_node =
        ZETA_MemberToStruct(Zeta_StageVector_Seg, n, dst_n);

    cursor->seg_idx = (GetSegColor(dst_n) == ref_color ? dst_node->ref.size
                                                       : dst_node->dat.size) -
                      1 - dst_seg_idx;

    if (GetSegColor(dst_n) == ref_color) {
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

    if (sv->rb == dst_n || GetSegColor(dst_n) == ref_color) {
        cursor->ele = NULL;
        return;
    }

    Zeta_StageVector_Seg* dst_node =
        ZETA_MemberToStruct(Zeta_StageVector_Seg, n, dst_n);

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
