#include "StageVector.h"

#include "CircularArray.h"
#include "Debugger.h"
#include "MemCheck.h"
#include "RBTree.h"
#include "SegUtils.h"
#include "utils.h"

#if ZETA_IsDebug

#define CheckSV_(sv) Zeta_StageVector_Check((sv))

#define CheckSVCursor_(sv, cursor) Zeta_StageVector_Cursor_Check((sv), (cursor))

#else

#define CheckSV_(dv)

#define CheckSVCursor_(dv, cursor)

#endif

#define ref_color (ZETA_StageVector_ref_color)
#define dat_color (ZETA_StageVector_dat_color)

#define GetNColor_(n)                                 \
    ({                                                \
        ZETA_DebugAssert(sv->lb != n && sv->rb != n); \
        Zeta_OrdCnt3RBTreeNode_GetLColor(NULL, (n));  \
    })

#define SetNColor_(n, c)                              \
    ZETA_DebugAssert(sv->lb != n && sv->rb != n);     \
    Zeta_OrdCnt3RBTreeNode_SetLColor(NULL, (n), (c)); \
    ZETA_StaticAssert(TRUE)

#define GetAvgCnt_(x)                                                   \
    ({                                                                  \
        size_t x_ = (x);                                                \
        size_t seg_cnt = ZETA_CeilIntDiv(x_, seg_capacity);             \
        (x_ + Zeta_SimpleRandomRotate(&rand_seed) % seg_cnt) / seg_cnt; \
    })

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

static void* AllocateData_(Zeta_StageVector* sv) {
    ZETA_DebugAssert(sv->data_allocator != NULL);
    ZETA_DebugAssert(sv->data_allocator->Allocate != NULL);

    void* data = sv->data_allocator->Allocate(
        sv->data_allocator->context,
        Zeta_StageVector_GetStride(sv) * sv->seg_capacity);

    ZETA_DebugAssert(data != NULL);

    return data;
}

static Zeta_StageVector_Seg* AllocateNode_(Zeta_StageVector* sv) {
    ZETA_DebugAssert(sv->seg_allocator != NULL);
    ZETA_DebugAssert(sv->seg_allocator->Allocate != NULL);

    Zeta_StageVector_Seg* seg = sv->seg_allocator->Allocate(
        sv->seg_allocator->context, sizeof(Zeta_StageVector_Seg));

    ZETA_DebugAssert(seg != NULL);
    ZETA_DebugAssert(__builtin_is_aligned(seg, alignof(Zeta_StageVector_Seg)));

    Zeta_OrdCnt3RBTreeNode_Init(NULL, &seg->n);

    return seg;
}

static Zeta_StageVector_Seg* AllocateRefSeg_(Zeta_StageVector* sv) {
    Zeta_StageVector_Seg* seg = AllocateNode_(sv);

    Zeta_OrdCnt3RBTreeNode_Init(NULL, &seg->n);
    Zeta_OrdCnt3RBTreeNode_SetAccSize(NULL, &seg->n, 0);

    SetNColor_(&seg->n, ref_color);

    seg->ref.beg = 0;
    seg->ref.size = 0;

    return seg;
}

static Zeta_StageVector_Seg* AllocateDatSeg_(Zeta_StageVector* sv) {
    Zeta_StageVector_Seg* seg = AllocateNode_(sv);

    Zeta_OrdCnt3RBTreeNode_Init(NULL, &seg->n);
    Zeta_OrdCnt3RBTreeNode_SetAccSize(NULL, &seg->n, 0);

    SetNColor_(&seg->n, dat_color);

    seg->dat.data = AllocateData_(sv);

    seg->dat.offset = 0;
    seg->dat.size = 0;

    return seg;
}

static void TransRefSegToDatSeg_(Zeta_StageVector* sv,
                                 Zeta_StageVector_Seg* seg) {
    unsigned char* data = AllocateData_(sv);

    size_t size = seg->ref.size;

    void* origin_cursor = __builtin_alloca_with_align(
        sv->origin->cursor_size, CHAR_BIT * alignof(max_align_t));

    sv->origin->Access(sv->origin->context, origin_cursor, NULL, seg->ref.beg);
    sv->origin->Read(sv->origin->context, origin_cursor, size, data, NULL);

    SetNColor_(&seg->n, dat_color);

    seg->dat.data = data;
    seg->dat.offset = 0;
    seg->dat.size = size;
}

static void EraseSeg_(Zeta_StageVector* sv, Zeta_BinTreeNodeOperator* btn_opr,
                      Zeta_StageVector_Seg* seg) {
    void* n = &seg->n;

    sv->root = Zeta_RBTree_Extract(btn_opr, n);

    if (GetNColor_(n) == dat_color) {
        sv->data_allocator->Deallocate(sv->data_allocator->context,
                                       seg->dat.data);
    }

    sv->seg_allocator->Deallocate(sv->seg_allocator->context, seg);
}

static void EraseAllSeg_(Zeta_StageVector* sv,
                         Zeta_BinTreeNodeOperator* btn_opr, void* n) {
    void* nl = Zeta_OrdCnt3RBTreeNode_GetL(NULL, n);
    void* nr = Zeta_OrdCnt3RBTreeNode_GetR(NULL, n);

    if (sv->lb != n && sv->rb != n) {
        Zeta_StageVector_Seg* seg =
            ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        if (GetNColor_(n) == dat_color) {
            sv->data_allocator->Deallocate(sv->data_allocator->context,
                                           seg->dat.data);
        }

        sv->seg_allocator->Deallocate(sv->seg_allocator->context, seg);
    }

    if (nl != NULL) { EraseAllSeg_(sv, btn_opr, nl); }
    if (nr != NULL) { EraseAllSeg_(sv, btn_opr, nr); }
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

static void PushRefL_(Zeta_StageVector* sv, Zeta_CircularArray* ca, size_t beg,
                      size_t size) {
    Zeta_CircularArray_Cursor ca_cursor;
    Zeta_CircularArray_PeekL(ca, &ca_cursor, NULL);
    Zeta_CircularArray_Insert(ca, &ca_cursor, size);

    void* origin_cursor = __builtin_alloca_with_align(
        sv->origin->cursor_size, CHAR_BIT * alignof(max_align_t));

    sv->origin->Access(sv->origin->context, origin_cursor, NULL, beg);

    Zeta_CircularArray_AssignFromSeqContainer(ca, &ca_cursor, sv->origin,
                                              origin_cursor, size);
}

static void PushRefR_(Zeta_StageVector* sv, Zeta_CircularArray* ca, size_t beg,
                      size_t size) {
    Zeta_CircularArray_Cursor ca_cursor;
    Zeta_CircularArray_GetRBCursor(ca, &ca_cursor);
    Zeta_CircularArray_Insert(ca, &ca_cursor, size);

    void* origin_cursor = __builtin_alloca_with_align(
        sv->origin->cursor_size, CHAR_BIT * alignof(max_align_t));

    sv->origin->Access(sv->origin->context, origin_cursor, NULL, beg);

    Zeta_CircularArray_AssignFromSeqContainer(ca, &ca_cursor, sv->origin,
                                              origin_cursor, size);
}

static void Merge2_(Zeta_StageVector* sv, size_t width, size_t stride,
                    Zeta_StageVector_Seg* a_seg, Zeta_StageVector_Seg* b_seg) {
    int a_c = GetNColor_(&a_seg->n);
    int b_c = GetNColor_(&b_seg->n);

    size_t a_size = a_c == ref_color ? a_seg->ref.size : a_seg->dat.size;
    size_t b_size = b_c == ref_color ? b_seg->ref.size : b_seg->dat.size;

    size_t total_size = a_size + b_size;

    size_t seg_capacity = sv->seg_capacity;

    unsigned long long rand_seed =
        ZETA_PtrToAddr(a_seg) + ZETA_PtrToAddr(b_seg);

    void* origin_cursor = __builtin_alloca_with_align(
        sv->origin->cursor_size, CHAR_BIT * alignof(max_align_t));

    if (a_c == ref_color && b_c == ref_color) {
        unsigned char* data = AllocateData_(sv);

        sv->origin->Access(sv->origin->context, origin_cursor, NULL,
                           a_seg->ref.beg);
        sv->origin->Read(sv->origin->context, origin_cursor, a_size, data,
                         NULL);

        sv->origin->Access(sv->origin->context, origin_cursor, NULL,
                           b_seg->ref.beg);
        sv->origin->Read(sv->origin->context, origin_cursor, b_seg->ref.size,
                         data + stride * a_size, NULL);

        a_seg->ref.size = 0;
        b_seg->ref.size = 0;

        Zeta_StageVector_Seg* dst_seg;

        switch (Zeta_SimpleRandomRotate(&rand_seed) % 2) {
            case 0: dst_seg = a_seg; break;
            case 1: dst_seg = b_seg; break;
        }

        SetNColor_(&dst_seg->n, dat_color);
        dst_seg->dat.data = data;
        dst_seg->dat.size = total_size;

        return;
    }

    Zeta_CircularArray a_ca;
    a_ca.data = a_seg->dat.data;
    a_ca.width = width;
    a_ca.stride = stride;
    a_ca.offset = a_seg->dat.offset;
    a_ca.size = a_seg->dat.size;
    a_ca.capacity = seg_capacity;

    Zeta_CircularArray b_ca;
    b_ca.data = b_seg->dat.data;
    b_ca.width = width;
    b_ca.stride = stride;
    b_ca.offset = b_seg->dat.offset;
    b_ca.size = b_seg->dat.size;
    b_ca.capacity = seg_capacity;

    size_t merge_a_cost = (a_c == ref_color ? a_size : 0) + b_size;
    size_t merge_b_cost = (b_c == ref_color ? b_size : 0) + a_size;

    if (Zeta_Choose2(merge_a_cost <= merge_b_cost, merge_b_cost <= merge_a_cost,
                     &rand_seed) == 0) {
        if (b_c == ref_color) {
            PushRefR_(sv, &a_ca, b_seg->ref.beg, b_size);
        } else {
            SegShoveL(&a_ca, &b_ca, 0, 0, b_size);
        }
    } else {
        if (a_c == ref_color) {
            PushRefL_(sv, &b_ca, a_seg->ref.beg, a_size);
        } else {
            SegShoveR(&a_ca, &b_ca, 0, 0, a_size);
        }
    }

    if (a_c == ref_color) {
        a_seg->ref.size = 0;
    } else {
        a_seg->dat.offset = a_ca.offset;
        a_seg->dat.size = a_ca.size;
    }

    if (b_c == ref_color) {
        b_seg->ref.size = 0;
    } else {
        b_seg->dat.offset = b_ca.offset;
        b_seg->dat.size = b_ca.size;
    }
}

#define SetSegState_(seg_tmp, ca_tmp, vacant_tmp, seg, ca, vacant)        \
    {                                                                     \
        Zeta_StageVector_Seg* seg_tmp = (seg);                            \
        Zeta_CircularArray* ca_tmp = (ca);                                \
        size_t* vacant_tmp = (vacant);                                    \
                                                                          \
        if (GetNColor_(&seg_tmp->n) == ref_color) {                       \
            ca_tmp->size = seg_tmp->ref.size;                             \
            *vacant_tmp =                                                 \
                seg_capacity - ZETA_GetMinOf(ca_tmp->size, seg_capacity); \
        } else {                                                          \
            ca_tmp->data = seg_tmp->dat.data;                             \
            ca_tmp->offset = seg_tmp->dat.offset;                         \
            ca_tmp->size = seg_tmp->dat.size;                             \
            *vacant_tmp = seg_capacity - ca_tmp->size;                    \
        }                                                                 \
    }

#define SetSegState(seg, ca, vacant) \
    SetSegState_(ZETA_TmpName, ZETA_TmpName, ZETA_TmpName, seg, ca, vacant)

static void RefShoveL_(Zeta_StageVector* sv, Zeta_CircularArray* l_ca,
                       Zeta_StageVector_Seg* r_seg, size_t rl_size,
                       size_t ins_cnt, size_t shove_cnt) {
    size_t stride = l_ca->stride;

    size_t cnt_a = ZETA_GetMinOf(rl_size, shove_cnt);
    size_t cnt_b = ZETA_GetMinOf(ins_cnt, shove_cnt - cnt_a);
    size_t cnt_c = shove_cnt - cnt_a - cnt_b;

    size_t l_i = l_ca->size;

    Zeta_CircularArray_Cursor l_cursor;
    void* r_cursor = __builtin_alloca_with_align(
        sv->origin->cursor_size, CHAR_BIT * alignof(max_align_t));

    Zeta_CircularArray_GetRBCursor(l_ca, &l_cursor);

    Zeta_CircularArray_Insert(l_ca, &l_cursor, shove_cnt);

    sv->origin->Access(sv->origin->context, r_cursor, NULL, r_seg->ref.beg);

    for (size_t i = cnt_a; 0 < i;) {
        size_t cur_i =
            ZETA_GetMinOf(i, Zeta_CircularArray_GetLongestContSucr(l_ca, l_i));
        i -= cur_i;

        sv->origin->Read(sv->origin->context, r_cursor, cur_i,
                         Zeta_CircularArray_Access(l_ca, NULL, NULL, l_i),
                         r_cursor);

        l_i += cur_i;
    }

    l_i += cnt_b;

    for (size_t i = cnt_c; 0 < i;) {
        size_t cur_i =
            ZETA_GetMinOf(i, Zeta_CircularArray_GetLongestContSucr(l_ca, l_i));
        i -= cur_i;

        sv->origin->Read(sv->origin->context, r_cursor, cur_i,
                         Zeta_CircularArray_Access(l_ca, NULL, NULL, l_i),
                         r_cursor);

        l_i += cur_i;
    }

    size_t new_r_size = r_seg->ref.size + ins_cnt - shove_cnt;

    if (cnt_b < ins_cnt) {
        unsigned char* data = AllocateData_(sv);
        unsigned char* data_i = data;

        if (cnt_a < rl_size) {
            sv->origin->Read(sv->origin->context, r_cursor, rl_size - cnt_a,
                             data_i, r_cursor);
        }

        data_i += stride * (rl_size - cnt_a + ins_cnt - cnt_b);

        if (rl_size < r_seg->ref.size) {
            sv->origin->Read(sv->origin->context, r_cursor,
                             r_seg->ref.size - rl_size, data_i, r_cursor);
        }

        SetNColor_(&r_seg->n, dat_color);

        r_seg->dat.data = data;
        r_seg->dat.offset = 0;
        r_seg->dat.size = new_r_size;
    } else {
        r_seg->ref.beg += cnt_a + cnt_c;
        r_seg->ref.size = new_r_size;
    }
}

static void RefShoveR_(Zeta_StageVector* sv, Zeta_StageVector_Seg* l_seg,
                       Zeta_CircularArray* r_ca, size_t lr_size, size_t ins_cnt,
                       size_t shove_cnt) {
    size_t stride = r_ca->stride;

    size_t cnt_a = ZETA_GetMinOf(lr_size, shove_cnt);
    size_t cnt_b = ZETA_GetMinOf(ins_cnt, shove_cnt - cnt_a);
    size_t cnt_c = shove_cnt - cnt_a - cnt_b;

    size_t r_i = 0;

    void* l_cursor = __builtin_alloca_with_align(
        sv->origin->cursor_size, CHAR_BIT * alignof(max_align_t));

    Zeta_CircularArray_Cursor r_cursor;

    Zeta_CircularArray_PeekL(r_ca, &r_cursor, NULL);

    Zeta_CircularArray_Insert(r_ca, &r_cursor, shove_cnt);

    sv->origin->Access(sv->origin->context, l_cursor, NULL,
                       l_seg->ref.beg + l_seg->ref.size - cnt_a - cnt_c);

    for (size_t i = cnt_c; 0 < i;) {
        size_t cur_i =
            ZETA_GetMinOf(i, Zeta_CircularArray_GetLongestContSucr(r_ca, r_i));
        i -= cur_i;

        sv->origin->Read(sv->origin->context, l_cursor, cur_i,
                         Zeta_CircularArray_Access(r_ca, NULL, NULL, r_i),
                         l_cursor);

        r_i += cur_i;
    }

    r_i += cnt_b;

    for (size_t i = cnt_a; 0 < i;) {
        size_t cur_i =
            ZETA_GetMinOf(i, Zeta_CircularArray_GetLongestContSucr(r_ca, r_i));
        i -= cur_i;

        sv->origin->Read(sv->origin->context, l_cursor, cur_i,
                         Zeta_CircularArray_Access(r_ca, NULL, NULL, r_i),
                         l_cursor);

        r_i += cur_i;
    }

    size_t new_l_size = l_seg->ref.size + ins_cnt - shove_cnt;

    if (cnt_b < ins_cnt) {
        unsigned char* data = AllocateData_(sv);
        unsigned char* data_i = data;

        sv->origin->Access(sv->origin->context, l_cursor, NULL, l_seg->ref.beg);

        if (lr_size < l_seg->ref.size) {
            sv->origin->Read(sv->origin->context, l_cursor,
                             l_seg->ref.size - lr_size, data_i, l_cursor);
        }

        data_i += stride * (l_seg->ref.size - lr_size + ins_cnt - cnt_b);

        if (cnt_a < lr_size) {
            sv->origin->Read(sv->origin->context, l_cursor, lr_size - cnt_a,
                             data_i, l_cursor);
        }

        SetNColor_(&l_seg->n, dat_color);

        l_seg->dat.data = data;
        l_seg->dat.offset = 0;
        l_seg->dat.size = new_l_size;
    } else {
        l_seg->ref.size = new_l_size;
    }
}

void Zeta_StageVector_Init(void* sv_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_OrdCnt3RBTreeNode* lb = sv->seg_allocator->Allocate(
        sv->seg_allocator->context, sizeof(Zeta_OrdCnt3RBTreeNode));
    Zeta_OrdCnt3RBTreeNode* rb = sv->seg_allocator->Allocate(
        sv->seg_allocator->context, sizeof(Zeta_OrdCnt3RBTreeNode));

    ZETA_DebugAssert(lb != NULL);
    ZETA_DebugAssert(rb != NULL);

    ZETA_DebugAssert(__builtin_is_aligned(lb, alignof(Zeta_OrdCnt3RBTreeNode)));
    ZETA_DebugAssert(__builtin_is_aligned(rb, alignof(Zeta_OrdCnt3RBTreeNode)));

    sv->lb = lb;
    sv->rb = rb;

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    InitTree_(sv, &btn_opr);
    RefOrigin_(sv, &btn_opr);
}

void Zeta_StageVector_Deinit(void* sv_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    EraseAllSeg_(sv, &btn_opr, sv->root);

    sv->seg_allocator->Deallocate(sv->seg_allocator->context, sv->lb);
    sv->seg_allocator->Deallocate(sv->seg_allocator->context, sv->rb);
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

size_t Zeta_StageVector_GetCapacity(void* sv_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    return ZETA_RangeMaxOf(size_t);
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
    dst_cursor->ref = NULL;
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
    dst_cursor->ref = NULL;
}

void* Zeta_StageVector_PeekL(void* sv_, void* dst_cursor_, void* dst_elem) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_BinTreeNodeOperator btn_opr;
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
            dst_cursor->ref = NULL;
        }

        return NULL;
    }

    Zeta_StageVector_Seg* seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

    void* ref = NULL;

    if (GetNColor_(n) == ref_color) {
        sv->origin->Access(sv->origin->context, NULL, dst_elem, seg->ref.beg);
    } else {
        Zeta_CircularArray ca;
        ca.data = seg->dat.data;
        ca.width = width;
        ca.stride = stride;
        ca.offset = seg->dat.offset;
        ca.size = seg->dat.size;
        ca.capacity = seg_capacity;

        ref = Zeta_CircularArray_PeekL(&ca, NULL, dst_elem);
    }

    if (dst_cursor != NULL) {
        dst_cursor->sv = sv;
        dst_cursor->idx = 0;
        dst_cursor->n = n;
        dst_cursor->seg_idx = 0;
        dst_cursor->ref = ref;
    }

    return ref;
}

void* Zeta_StageVector_PeekR(void* sv_, void* dst_cursor_, void* dst_elem) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_BinTreeNodeOperator btn_opr;
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
            dst_cursor->ref = NULL;
        }

        return NULL;
    }

    Zeta_StageVector_Seg* seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

    void* ref = NULL;

    size_t seg_size;

    if (GetNColor_(n) == ref_color) {
        seg_size = seg->ref.size;

        sv->origin->Access(sv->origin->context, NULL, dst_elem,
                           seg->ref.beg + seg_size - 1);
    } else {
        seg_size = seg->dat.size;

        Zeta_CircularArray ca;
        ca.data = seg->dat.data;
        ca.width = width;
        ca.stride = stride;
        ca.offset = seg->dat.offset;
        ca.size = seg_size;
        ca.capacity = seg_capacity;

        ref = Zeta_CircularArray_PeekR(&ca, NULL, dst_elem);
    }

    if (dst_cursor != NULL) {
        dst_cursor->sv = sv;
        dst_cursor->idx = size - 1;
        dst_cursor->n = n;
        dst_cursor->seg_idx = seg_size - 1;
        dst_cursor->ref = ref;
    }

    return ref;
}

void* Zeta_StageVector_Access(void* sv_, void* dst_cursor_, void* dst_elem,
                              size_t idx) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_BinTreeNodeOperator btn_opr;
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

    void* ref = NULL;

    if (idx < size) {
        seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        if (GetNColor_(n) == dat_color) {
            Zeta_CircularArray ca;
            ca.data = seg->dat.data;
            ca.width = width;
            ca.stride = stride;
            ca.offset = seg->dat.offset;
            ca.size = seg->dat.size;
            ca.capacity = seg_capacity;

            ref = Zeta_CircularArray_Access(&ca, NULL, dst_elem, seg_idx);
        } else if (dst_elem != NULL) {
            sv->origin->Access(sv->origin->context, NULL, dst_elem,
                               seg->ref.beg + seg_idx);
        }
    }

    if (dst_cursor != NULL) {
        dst_cursor->sv = sv;
        dst_cursor->idx = idx;
        dst_cursor->n = n;
        dst_cursor->seg_idx = seg_idx;
        dst_cursor->ref = ref;
    }

    return ref;
}

void* Zeta_StageVector_Refer(void* sv_, void const* pos_cursor_) {
    Zeta_StageVector* sv = sv_;
    Zeta_StageVector_Cursor const* pos_cursor = pos_cursor_;
    Zeta_StageVector_Cursor_Check(sv, pos_cursor);

    return pos_cursor->ref;
}

void Zeta_StageVector_Read(void* sv_, void const* pos_cursor_, size_t cnt,
                           void* dst_, void* dst_cursor_) {
    Zeta_StageVector* sv = sv_;
    Zeta_StageVector_Cursor const* pos_cursor = pos_cursor_;
    Zeta_StageVector_Cursor_Check(sv, pos_cursor);

    Zeta_StageVector_Cursor* dst_cursor = dst_cursor_;

    if (cnt == 0) {
        if (dst_cursor != NULL) {
            dst_cursor->sv = sv;
            dst_cursor->idx = pos_cursor->idx;
            dst_cursor->n = pos_cursor->n;
            dst_cursor->seg_idx = pos_cursor->seg_idx;
            dst_cursor->ref = pos_cursor->ref;
        }

        return;
    }

    Zeta_BinTreeNodeOperator btn_opr;
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

    void* origin_cursor = __builtin_alloca_with_align(
        sv->origin->cursor_size, CHAR_BIT * alignof(max_align_t));

    Zeta_CircularArray ca;
    ca.width = width;
    ca.stride = stride;
    ca.capacity = seg_capacity;

    while (0 < cnt) {
        seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        size_t cur_cnt;
        size_t seg_size;

        if (GetNColor_(n) == ref_color) {
            seg_size = seg->ref.size;
            cur_cnt = ZETA_GetMinOf(cnt, seg_size - seg_idx);

            sv->origin->Access(sv->origin->context, origin_cursor, NULL,
                               seg->ref.beg + seg_idx);
            sv->origin->Read(sv->origin->context, origin_cursor, cur_cnt, dst,
                             NULL);
        } else {
            seg_size = seg->dat.size;
            cur_cnt = ZETA_GetMinOf(cnt, seg_size - seg_idx);

            ca.data = seg->dat.data;
            ca.offset = seg->dat.offset;
            ca.size = seg_size;

            Zeta_CircularArray_Cursor ca_cursor;
            Zeta_CircularArray_Access(&ca, &ca_cursor, NULL, seg_idx);
            Zeta_CircularArray_Read(&ca, &ca_cursor, cur_cnt, dst, NULL);
        }

        dst += stride * cur_cnt;

        seg_idx += cur_cnt;
        cnt -= cur_cnt;

        if (seg_idx == seg_size) {
            n = Zeta_BinTree_StepR(&btn_opr, n);
            seg_idx = 0;
        }
    }

    if (dst_cursor == NULL) { return; }

    dst_cursor->sv = sv;
    dst_cursor->idx = end;
    dst_cursor->n = n;
    dst_cursor->seg_idx = seg_idx;

    if (sv->rb == n || GetNColor_(n) == ref_color) {
        dst_cursor->ref = NULL;
        return;
    }

    seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

    ca.data = seg->dat.data;
    ca.offset = seg->dat.offset;
    ca.size = seg->dat.size;

    dst_cursor->ref = Zeta_CircularArray_Access(&ca, NULL, NULL, seg_idx);
}

void Zeta_StageVector_Write(void* sv_, void* pos_cursor_, size_t cnt,
                            void const* src_, void* dst_cursor_) {
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
            dst_cursor->ref = pos_cursor->ref;
        }

        return;
    }

    Zeta_BinTreeNodeOperator btn_opr;
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
    void* l_n = Zeta_BinTree_StepL(&btn_opr, n);

    Zeta_StageVector_Seg* seg;

    Zeta_CircularArray ca;
    ca.width = width;
    ca.stride = stride;
    ca.capacity = seg_capacity;

    void* origin_cursor = __builtin_alloca_with_align(
        sv->origin->cursor_size, CHAR_BIT * alignof(max_align_t));

    unsigned long long rand_seed = ZETA_PtrToAddr(n);

    if (GetNColor_(n) == ref_color && 0 < seg_idx) {
        seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        size_t seg_size = seg->ref.size;

        size_t seg_cnt = ZETA_CeilIntDiv(seg_size, seg_capacity);

        size_t k = (seg_idx * seg_cnt + seg_cnt - 1) / seg_size;

        size_t l_seg_size = k * seg_size / seg_cnt;

        ZETA_CheckAssert(l_seg_size <= seg_idx);
        ZETA_CheckAssert(seg_idx < (k + 1) * seg_size / seg_cnt);

        size_t avg_seg_size = (k + 1) * seg_size / seg_cnt - l_seg_size;

        ZETA_CheckAssert(seg_idx - l_seg_size < avg_seg_size);

        seg_idx -= l_seg_size;

        if (0 < l_seg_size) {
            Zeta_StageVector_Seg* new_seg = AllocateRefSeg_(sv);
            sv->root = Zeta_RBTree_Insert(&btn_opr, l_n, n, &new_seg->n);

            new_seg->ref.beg = seg->ref.beg;
            new_seg->ref.size = l_seg_size;

            seg->ref.beg += l_seg_size;
            seg->ref.size -= l_seg_size;

            ZETA_CheckAssert(0 < seg->ref.size);

            Zeta_BinTree_SetSize(&btn_opr, &new_seg->n, new_seg->ref.size);

            l_n = &new_seg->n;
        }

        if (seg_idx == 0) { goto WRITE_INTO_REF_SEG_END; }

        size_t cnt_a = seg_idx;
        size_t cnt_b = ZETA_GetMinOf(cnt, avg_seg_size - seg_idx);
        size_t cnt_c = avg_seg_size - cnt_a - cnt_b;

        cnt -= cnt_b;

        if (avg_seg_size == seg->ref.size) {
            unsigned char* data = AllocateData_(sv);

            sv->origin->Access(sv->origin->context, origin_cursor, NULL,
                               seg->ref.beg);

            sv->origin->Read(sv->origin->context, origin_cursor, cnt_a, data,
                             NULL);

            Zeta_MemCopy(data + stride * cnt_a, src, stride * cnt_b);

            if (0 < cnt_c) {
                sv->origin->Access(sv->origin->context, origin_cursor, NULL,
                                   seg->ref.beg + cnt_a + cnt_b);

                sv->origin->Read(sv->origin->context, origin_cursor, cnt_c,
                                 data + stride * (cnt_a + cnt_b), NULL);
            }

            SetNColor_(n, dat_color);

            seg->dat.data = data;
            seg->dat.offset = 0;
            seg->dat.size = avg_seg_size;

            seg_idx += cnt_b;

            src += stride * cnt_b;

            if (seg_idx == avg_seg_size) {
                ZETA_DebugLogCurPos;

                Zeta_BinTree_SetSize(&btn_opr, n, avg_seg_size);

                l_n = n;
                n = Zeta_BinTree_StepR(&btn_opr, n);

                seg_idx = 0;
            }
        } else {
            Zeta_StageVector_Seg* new_seg = AllocateDatSeg_(sv);
            sv->root = Zeta_RBTree_Insert(&btn_opr, l_n, n, &new_seg->n);

            new_seg->dat.size = avg_seg_size;

            unsigned char* data = new_seg->dat.data;

            sv->origin->Access(sv->origin->context, origin_cursor, NULL,
                               seg->ref.beg);

            sv->origin->Read(sv->origin->context, origin_cursor, cnt_a, data,
                             NULL);

            Zeta_MemCopy(data + stride * cnt_a, src, stride * cnt_b);

            if (0 < cnt_c) {
                sv->origin->Access(sv->origin->context, origin_cursor, NULL,
                                   seg->ref.beg + cnt_a + cnt_b);

                sv->origin->Read(sv->origin->context, origin_cursor, cnt_c,
                                 data + stride * (cnt_a + cnt_b), NULL);
            }

            seg->ref.beg += avg_seg_size;
            seg->ref.size -= avg_seg_size;

            seg_idx += cnt_b;

            src += stride * cnt_b;

            if (seg_idx < new_seg->dat.size) {
                ZETA_DebugLogCurPos;

                Zeta_BinTree_SetSize(&btn_opr, n, seg->ref.size);

                n = &new_seg->n;

                ZETA_CheckAssert(cnt == 0);

            } else {
                ZETA_DebugLogCurPos;

                Zeta_BinTree_SetSize(&btn_opr, &new_seg->n, new_seg->dat.size);

                l_n = &new_seg->n;
                seg_idx = 0;
            }
        }
    }

WRITE_INTO_REF_SEG_END:;

    while (0 < cnt) {
        seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        if (GetNColor_(n) == dat_color) {
            ca.data = seg->dat.data;
            ca.offset = seg->dat.offset;
            ca.size = seg->dat.size;

            size_t cur_cnt = ZETA_GetMinOf(cnt, ca.size - seg_idx);

            Zeta_CircularArray ca_cursor;
            Zeta_CircularArray_Access(&ca, &ca_cursor, NULL, seg_idx);
            Zeta_CircularArray_Write(&ca, &ca_cursor, cur_cnt, src, NULL);

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

        size_t seg_size = seg->ref.size;

        if (seg_capacity < seg_size) {
            size_t split_cnt = GetAvgCnt_(seg_size);

            Zeta_StageVector_Seg* new_seg = AllocateRefSeg_(sv);
            sv->root = Zeta_RBTree_Insert(&btn_opr, l_n, n, &new_seg->n);

            new_seg->ref.beg = seg->ref.beg;
            new_seg->ref.size = split_cnt;

            seg->ref.beg += split_cnt;
            seg->ref.size -= split_cnt;

            r_n = n;

            seg = new_seg;
            n = &new_seg->n;

            seg_size = seg->ref.size;
        }

        Zeta_BinTree_SetSize(&btn_opr, &seg->n, seg_size);

        size_t cur_cnt = ZETA_GetMinOf(cnt, seg_size);

        unsigned char* data = AllocateData_(sv);

        Zeta_MemCopy(data, src, stride * cur_cnt);

        src += stride * cur_cnt;

        SetNColor_(n, dat_color);

        if (cur_cnt == seg_size) {
            seg->dat.data = data;
            seg->dat.offset = 0;
            seg->dat.size = seg_size;

            cnt -= cur_cnt;
            l_n = n;
            n = r_n == NULL ? Zeta_BinTree_StepR(&btn_opr, n) : r_n;
            continue;
        }

        sv->origin->Access(sv->origin->context, origin_cursor, NULL,
                           seg->ref.beg + cur_cnt);

        sv->origin->Read(sv->origin->context, origin_cursor, seg_size - cur_cnt,
                         data + stride * cur_cnt, NULL);

        seg->dat.data = data;
        seg->dat.offset = 0;
        seg->dat.size = seg_size;

        if (r_n != NULL) {
            Zeta_StageVector_Seg* r_seg =
                ZETA_MemberToStruct(Zeta_StageVector_Seg, n, r_n);

            Zeta_BinTree_SetSize(&btn_opr, r_n,
                                 GetNColor_(r_n) == ref_color
                                     ? r_seg->ref.size
                                     : r_seg->dat.size);
        }

        seg_idx = cur_cnt;

        break;
    }

    if (sv->rb != n) {
        seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        Zeta_BinTree_SetSize(
            &btn_opr, n,
            GetNColor_(n) == ref_color ? seg->ref.size : seg->dat.size);
    }

    if (dst_cursor == NULL) { return; }

    dst_cursor->sv = sv;
    dst_cursor->idx = end;
    dst_cursor->n = n;
    dst_cursor->seg_idx = seg_idx;

    if (sv->rb == n || GetNColor_(n) == ref_color) {
        dst_cursor->ref = NULL;
        return;
    }

    seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

    ca.data = seg->dat.data;
    ca.offset = seg->dat.offset;
    ca.size = seg->dat.size;

    dst_cursor->ref = Zeta_CircularArray_Access(&ca, NULL, NULL, seg_idx);
}

void* Zeta_StageVector_PushL(void* sv_, void* dst_cursor_, size_t cnt) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_StageVector_Cursor pos_cursor;
    Zeta_StageVector_PeekL(sv, &pos_cursor, NULL);

    Zeta_StageVector_Insert(sv, &pos_cursor, cnt);

    if (dst_cursor != NULL) {
        dst_cursor->sv = sv;
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->n = pos_cursor.n;
        dst_cursor->seg_idx = pos_cursor.seg_idx;
        dst_cursor->ref = pos_cursor.ref;
    }

    return pos_cursor.ref;
}

void* Zeta_StageVector_PushR(void* sv_, void* dst_cursor_, size_t cnt) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_StageVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_StageVector_Cursor pos_cursor;
    Zeta_StageVector_GetRBCursor(sv, &pos_cursor);

    Zeta_StageVector_Insert(sv, &pos_cursor, cnt);

    if (dst_cursor != NULL) {
        dst_cursor->sv = sv;
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->n = pos_cursor.n;
        dst_cursor->seg_idx = pos_cursor.seg_idx;
        dst_cursor->ref = pos_cursor.ref;
    }

    return pos_cursor.ref;
}

void* Zeta_StageVector_Insert(void* sv_, void* pos_cursor_, size_t ins_cnt) {
    Zeta_StageVector* sv = sv_;
    Zeta_StageVector_Cursor* pos_cursor = pos_cursor_;
    Zeta_StageVector_Cursor_Check(sv, pos_cursor);

    if (ins_cnt == 0) { return pos_cursor->ref; }

    Zeta_BinTreeNodeOperator btn_opr;
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

    Zeta_CircularArray l_ca;
    l_ca.width = width;
    l_ca.stride = stride;
    l_ca.capacity = seg_capacity;

    Zeta_CircularArray m_ca;
    m_ca.width = width;
    m_ca.stride = stride;
    m_ca.capacity = seg_capacity;

    Zeta_CircularArray r_ca;
    r_ca.width = width;
    r_ca.stride = stride;
    r_ca.capacity = seg_capacity;

    Zeta_CircularArray ca_cursor;

    if (seg_idx == 0) {
        l_n = Zeta_BinTree_StepL(&btn_opr, m_n);

        r_n = m_n;

        m_n = NULL;
        m_seg = NULL;

        if (sv->lb == l_n) {
            l_ca.size = 0;
            l_vacant = 0;
        } else {
            l_seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, l_n);
            SetSegState(l_seg, &l_ca, &l_vacant);
        }

        if (sv->rb == r_n) {
            r_ca.size = 0;
            r_vacant = 0;
        } else {
            r_seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, r_n);
            SetSegState(r_seg, &r_ca, &r_vacant);
        }
    } else {
        m_seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, m_n);
        SetSegState(m_seg, &m_ca, &m_vacant);

        if (ins_cnt <= m_vacant) {
            void* ref;

            if (GetNColor_(m_n) == ref_color) {
                m_ca.data = AllocateData_(sv);
                m_ca.offset = 0;
                m_ca.size = 0;

                RefShoveL_(sv, &m_ca, m_seg, seg_idx, ins_cnt,
                           m_seg->ref.size + ins_cnt);

                SetNColor_(&m_seg->n, dat_color);

                m_seg->dat.data = m_ca.data;

                ref = Zeta_CircularArray_Access(&m_ca, NULL, NULL, seg_idx);
            } else {
                Zeta_CircularArray_Access(&m_ca, &ca_cursor, NULL, seg_idx);
                ref = Zeta_CircularArray_Insert(&m_ca, &ca_cursor, ins_cnt);
            }

            m_seg->dat.offset = m_ca.offset;
            m_seg->dat.size = m_ca.size;
            Zeta_BinTree_SetSize(&btn_opr, m_n, m_ca.size);

            pos_cursor->n = m_n;
            pos_cursor->seg_idx = seg_idx;
            pos_cursor->ref = ref;

            return ref;
        }

        l_n = Zeta_BinTree_StepL(&btn_opr, m_n);
        r_n = Zeta_BinTree_StepR(&btn_opr, m_n);

        if (sv->lb == l_n) {
            l_ca.size = 0;
            l_vacant = 0;
        } else {
            l_seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, l_n);
            SetSegState(l_seg, &l_ca, &l_vacant);
        }

        if (sv->rb == r_n) {
            r_ca.size = 0;
            r_vacant = 0;
        } else {
            r_seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, r_n);
            SetSegState(r_seg, &r_ca, &r_vacant);
        }

        size_t ml_size = seg_idx;
        size_t mr_size = m_ca.size - seg_idx;

        size_t new_l_m_size = l_ca.size + m_ca.size + ins_cnt;
        size_t new_m_r_size = m_ca.size + r_ca.size + ins_cnt;

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
            if (GetNColor_(l_n) == ref_color) {
                TransRefSegToDatSeg_(sv, l_seg);
                l_ca.data = l_seg->dat.data;
                l_ca.offset = l_seg->dat.offset;
            }

            size_t ret_seg_idx = l_ca.size + ml_size;

            size_t new_l_size = (new_l_m_size + (m_ca.size <= l_ca.size)) / 2;

            if (GetNColor_(m_n) == ref_color) {
                RefShoveL_(sv, &l_ca, m_seg, ml_size, ins_cnt,
                           new_l_size - l_ca.size);

                if (GetNColor_(m_n) == ref_color) {
                    m_ca.size = m_seg->ref.size;
                } else {
                    m_ca.data = m_seg->dat.data;
                    m_ca.offset = m_seg->dat.offset;
                    m_ca.size = m_seg->dat.size;
                }
            } else {
                SegShoveL(&l_ca, &m_ca, ml_size, ins_cnt,
                          new_l_size - l_ca.size);

                m_seg->dat.offset = m_ca.offset;
                m_seg->dat.size = m_ca.size;
            }

            Zeta_BinTree_SetSize(&btn_opr, m_n, m_ca.size);

            l_seg->dat.offset = l_ca.offset;
            l_seg->dat.size = l_ca.size;
            Zeta_BinTree_SetSize(&btn_opr, l_n, l_ca.size);

            if (ret_seg_idx < l_ca.size) {
                pos_cursor->n = l_n;
                pos_cursor->seg_idx = ret_seg_idx;
                pos_cursor->ref =
                    Zeta_CircularArray_Access(&l_ca, NULL, NULL, ret_seg_idx);
            } else {
                ret_seg_idx -= l_ca.size;

                pos_cursor->n = m_n;
                pos_cursor->seg_idx = ret_seg_idx;
                pos_cursor->ref = GetNColor_(m_n) == ref_color
                                      ? NULL
                                      : Zeta_CircularArray_Access(
                                            &m_ca, NULL, NULL, ret_seg_idx);
            }
        } else {
            if (GetNColor_(r_n) == ref_color) {
                TransRefSegToDatSeg_(sv, r_seg);
                r_ca.data = r_seg->dat.data;
                r_ca.offset = r_seg->dat.offset;
            }

            size_t ret_seg_idx = seg_idx;

            size_t new_r_size = (new_m_r_size + (m_ca.size <= r_ca.size)) / 2;

            if (GetNColor_(m_n) == ref_color) {
                RefShoveR_(sv, m_seg, &r_ca, mr_size, ins_cnt,
                           new_r_size - r_ca.size);

                if (GetNColor_(m_n) == ref_color) {
                    m_ca.size = m_seg->ref.size;
                } else {
                    m_ca.data = m_seg->dat.data;
                    m_ca.offset = m_seg->dat.offset;
                    m_ca.size = m_seg->dat.size;
                }
            } else {
                SegShoveR(&m_ca, &r_ca, mr_size, ins_cnt,
                          new_r_size - r_ca.size);

                m_seg->dat.offset = m_ca.offset;
                m_seg->dat.size = m_ca.size;
            }

            Zeta_BinTree_SetSize(&btn_opr, m_n, m_ca.size);

            r_seg->dat.offset = r_ca.offset;
            r_seg->dat.size = r_ca.size;
            Zeta_BinTree_SetSize(&btn_opr, r_n, r_ca.size);

            if (ret_seg_idx < m_ca.size) {
                pos_cursor->n = m_n;
                pos_cursor->seg_idx = ret_seg_idx;
                pos_cursor->ref = GetNColor_(m_n) == ref_color
                                      ? NULL
                                      : Zeta_CircularArray_Access(
                                            &m_ca, NULL, NULL, ret_seg_idx);
            } else {
                ret_seg_idx -= m_ca.size;

                pos_cursor->n = r_n;
                pos_cursor->seg_idx = ret_seg_idx;
                pos_cursor->ref =
                    Zeta_CircularArray_Access(&r_ca, NULL, NULL, ret_seg_idx);
            }
        }

        return pos_cursor->ref;

    SPLIT:;

        if (ml_size <= l_vacant) {
            if (GetNColor_(l_n) == ref_color) {
                TransRefSegToDatSeg_(sv, l_seg);

                l_ca.data = l_seg->dat.data;
                l_ca.offset = l_seg->dat.offset;
            }

            if (GetNColor_(m_n) == ref_color) {
                PushRefR_(sv, &l_ca, m_seg->ref.beg, ml_size);

                m_seg->ref.beg += ml_size;
                m_ca.size -= ml_size;
            } else {
                SegShoveL(&l_ca, &m_ca, 0, 0, ml_size);
            }

            ml_size = 0;
            l_vacant = seg_capacity - l_ca.size;
        }

        if (mr_size <= r_vacant) {
            if (GetNColor_(r_n) == ref_color) {
                TransRefSegToDatSeg_(sv, r_seg);

                r_ca.data = r_seg->dat.data;
                r_ca.offset = r_seg->dat.offset;
            }

            if (GetNColor_(m_n) == ref_color) {
                PushRefL_(sv, &r_ca, m_seg->ref.beg + ml_size, mr_size);

                m_ca.size -= mr_size;
            } else {
                SegShoveR(&m_ca, &r_ca, 0, 0, mr_size);
            }

            mr_size = 0;
            r_vacant = seg_capacity - r_ca.size;
        }

        if (ml_size == 0 && mr_size == 0) { goto INS; }

        if (ml_size == 0) {
            r_n = m_n;
            r_seg = m_seg;
            r_ca.data = m_ca.data;
            r_ca.offset = m_ca.offset;
            r_ca.size = m_ca.size;
            r_vacant = seg_capacity - ZETA_GetMinOf(r_ca.size, seg_capacity);

            m_n = NULL;
            m_seg = NULL;

            goto INS;
        }

        if (mr_size == 0) {
            l_n = m_n;
            l_seg = m_seg;
            l_ca.data = m_ca.data;
            l_ca.offset = m_ca.offset;
            l_ca.size = m_ca.size;
            l_vacant = seg_capacity - ZETA_GetMinOf(l_ca.size, seg_capacity);

            m_n = NULL;
            m_seg = NULL;

            goto INS;
        }

        int split_dir =
            Zeta_Choose2(ml_size <= mr_size, mr_size <= ml_size, &rand_seed);

        if (split_dir == 0) {
            if (GetNColor_(m_n) == ref_color) {
                Zeta_StageVector_Seg* new_l_seg = AllocateRefSeg_(sv);
                sv->root =
                    Zeta_RBTree_Insert(&btn_opr, l_n, m_n, &new_l_seg->n);

                l_n = &new_l_seg->n;
                l_seg = new_l_seg;
                l_seg->ref.beg = m_seg->ref.beg;
                l_ca.size = ml_size;
                l_vacant =
                    seg_capacity - ZETA_GetMinOf(l_ca.size, seg_capacity);

                r_n = m_n;
                r_seg = m_seg;
                r_seg->ref.beg += ml_size;
                r_ca.size = mr_size;
                r_vacant =
                    seg_capacity - ZETA_GetMinOf(r_ca.size, seg_capacity);
            } else {
                Zeta_StageVector_Seg* new_l_seg = AllocateDatSeg_(sv);
                sv->root =
                    Zeta_RBTree_Insert(&btn_opr, l_n, m_n, &new_l_seg->n);

                l_n = &new_l_seg->n;
                l_seg = new_l_seg;
                l_ca.data = new_l_seg->dat.data;
                l_ca.offset = 0;
                l_ca.size = 0;

                r_n = m_n;
                r_seg = m_seg;
                r_ca.data = m_ca.data;
                r_ca.offset = m_ca.offset;
                r_ca.size = m_ca.size;

                SegShoveL(&l_ca, &r_ca, 0, 0, ml_size);

                l_vacant = seg_capacity - l_ca.size;
                r_vacant = seg_capacity - r_ca.size;
            }
        } else {
            if (GetNColor_(m_n) == ref_color) {
                Zeta_StageVector_Seg* new_r_seg = AllocateRefSeg_(sv);
                sv->root =
                    Zeta_RBTree_Insert(&btn_opr, m_n, r_n, &new_r_seg->n);

                l_n = m_n;
                l_seg = m_seg;
                l_ca.size = ml_size;
                l_vacant =
                    seg_capacity - ZETA_GetMinOf(l_ca.size, seg_capacity);

                r_n = &new_r_seg->n;
                r_seg = new_r_seg;
                r_seg->ref.beg = m_seg->ref.beg + ml_size;
                r_ca.size = mr_size;
                r_vacant =
                    seg_capacity - ZETA_GetMinOf(r_ca.size, seg_capacity);
            } else {
                Zeta_StageVector_Seg* new_r_seg = AllocateDatSeg_(sv);
                sv->root =
                    Zeta_RBTree_Insert(&btn_opr, m_n, r_n, &new_r_seg->n);

                l_n = m_n;
                l_seg = m_seg;
                l_ca.data = m_ca.data;
                l_ca.offset = m_ca.offset;
                l_ca.size = m_ca.size;

                r_n = &new_r_seg->n;
                r_seg = new_r_seg;
                r_ca.data = r_seg->dat.data;
                r_ca.offset = 0;
                r_ca.size = 0;

                SegShoveR(&l_ca, &r_ca, 0, 0, mr_size);

                l_vacant = seg_capacity - l_ca.size;
                r_vacant = seg_capacity - r_ca.size;
            }
        }

        m_n = NULL;
        m_seg = NULL;
    }

INS:;

    bool_t l_is_ref = sv->lb != l_n && GetNColor_(l_n) == ref_color;
    bool_t r_is_ref = sv->rb != r_n && GetNColor_(r_n) == ref_color;

    bool_t l_shove;
    bool_t r_shove;

    {
        size_t res_size =
            seg_capacity - 1 - (ins_cnt + seg_capacity - 1) % seg_capacity;

        if (l_ca.size + r_ca.size <= res_size) {
            l_shove = TRUE;
            r_shove = TRUE;
        } else if (l_ca.size != r_ca.size
                       ? l_ca.size < r_ca.size
                       : Zeta_SimpleRandomRotate(&rand_seed) % 2 == 0) {
            l_shove = l_ca.size <= res_size;
            r_shove = FALSE;
        } else {
            l_shove = FALSE;
            r_shove = r_ca.size <= res_size;
        }

        if (l_ca.size == 0) { l_shove = FALSE; }
        if (r_ca.size == 0) { r_shove = FALSE; }
    }

    if (l_is_ref && l_shove) {
        l_seg->ref.size = l_ca.size;

        TransRefSegToDatSeg_(sv, l_seg);

        l_ca.data = l_seg->dat.data;
        l_ca.offset = 0;
    }

    if (r_is_ref && r_shove) {
        r_seg->ref.size = r_ca.size;

        TransRefSegToDatSeg_(sv, r_seg);

        r_ca.data = r_seg->dat.data;
        r_ca.offset = 0;
    }

    size_t total_size =
        (l_shove ? l_ca.size : 0) + ins_cnt + (r_shove ? r_ca.size : 0);

    size_t ret_seg_idx = l_ca.size;

    if (l_shove && r_shove && total_size <= seg_capacity) {
        if (Zeta_Choose2(r_ca.size <= l_ca.size, l_ca.size <= r_ca.size,
                         &rand_seed) == 0) {
            SegShoveL(&l_ca, &r_ca, 0, ins_cnt, ins_cnt + r_ca.size);

            EraseSeg_(sv, &btn_opr, r_seg);

            l_seg->dat.offset = l_ca.offset;
            l_seg->dat.size = l_ca.size;
            Zeta_BinTree_SetSize(&btn_opr, l_n, l_ca.size);

            pos_cursor->n = l_n;
            pos_cursor->seg_idx = ret_seg_idx;
            pos_cursor->ref =
                Zeta_CircularArray_Access(&l_ca, NULL, NULL, ret_seg_idx);
        } else {
            SegShoveR(&l_ca, &r_ca, 0, ins_cnt, l_ca.size + ins_cnt);

            EraseSeg_(sv, &btn_opr, l_seg);

            r_seg->dat.offset = r_ca.offset;
            r_seg->dat.size = r_ca.size;
            Zeta_BinTree_SetSize(&btn_opr, r_n, r_ca.size);

            pos_cursor->n = r_n;
            pos_cursor->seg_idx = ret_seg_idx;
            pos_cursor->ref =
                Zeta_CircularArray_Access(&r_ca, NULL, NULL, ret_seg_idx);
        }

        return pos_cursor->ref;
    }

    size_t new_l_size;
    size_t new_r_size;

    if (l_shove) {
        new_l_size = GetAvgCnt_(total_size);
        total_size -= new_l_size;
    } else {
        new_l_size = l_ca.size;
    }

    if (r_shove) {
        new_r_size = GetAvgCnt_(total_size);
        total_size -= new_r_size;
    } else {
        new_r_size = r_ca.size;
    }

    if (l_ca.size < new_l_size) {
        Zeta_CircularArray_Cursor ca_cursor;
        Zeta_CircularArray_GetRBCursor(&l_ca, &ca_cursor);
        Zeta_CircularArray_Insert(&l_ca, &ca_cursor, new_l_size - l_ca.size);
    }

    if (r_ca.size < new_r_size) {
        Zeta_CircularArray_Cursor ca_cursor;
        Zeta_CircularArray_PeekL(&r_ca, &ca_cursor, NULL);
        Zeta_CircularArray_Insert(&r_ca, &ca_cursor, new_r_size - r_ca.size);
    }

    if (sv->lb != l_n) {
        if (GetNColor_(l_n) == ref_color) {
            l_seg->ref.size = l_ca.size;
        } else {
            l_seg->dat.offset = l_ca.offset;
            l_seg->dat.size = l_ca.size;
        }

        Zeta_BinTree_SetSize(&btn_opr, l_n, l_ca.size);
    }

    if (sv->rb != r_n) {
        if (GetNColor_(r_n) == ref_color) {
            r_seg->ref.size = r_ca.size;
        } else {
            r_seg->dat.offset = r_ca.offset;
            r_seg->dat.size = r_ca.size;
        }

        Zeta_BinTree_SetSize(&btn_opr, r_n, r_ca.size);
    }

    if (0 < total_size && m_n != NULL) {
        size_t cur_size = GetAvgCnt_(total_size);
        total_size -= cur_size;

        if (GetNColor_(m_n) == ref_color) {
            SetNColor_(m_n, dat_color);
            m_seg->dat.data = AllocateData_(sv);
        }

        m_seg->dat.offset = 0;
        m_seg->dat.size = cur_size;
        Zeta_BinTree_SetSize(&btn_opr, m_n, cur_size);

        r_n = m_n;
        r_seg = m_seg;

        m_n = NULL;
        m_seg = NULL;
    }

    while (0 < total_size) {
        size_t cur_size = GetAvgCnt_(total_size);
        total_size -= cur_size;

        Zeta_StageVector_Seg* new_r_seg = AllocateDatSeg_(sv);

        sv->root = Zeta_RBTree_Insert(&btn_opr, l_n, r_n, &new_r_seg->n);

        new_r_seg->dat.size = cur_size;
        Zeta_BinTree_SetSize(&btn_opr, &new_r_seg->n, cur_size);

        r_n = &new_r_seg->n;
        r_seg = new_r_seg;
    }

    if (m_n != NULL) { EraseSeg_(sv, &btn_opr, m_seg); }

    if (ret_seg_idx < l_ca.size) {
        pos_cursor->n = l_n;
        pos_cursor->seg_idx = ret_seg_idx;

        if (GetNColor_(l_n) == ref_color) {
            pos_cursor->ref = NULL;
        } else {
            l_ca.data = l_seg->dat.data;
            l_ca.offset = l_seg->dat.offset;
            l_ca.size = l_seg->dat.size;

            pos_cursor->ref =
                Zeta_CircularArray_Access(&l_ca, NULL, NULL, ret_seg_idx);
        }

        return pos_cursor->ref;
    }

    ret_seg_idx -= l_ca.size;

    pos_cursor->n = r_n;
    pos_cursor->seg_idx = ret_seg_idx;

    if (GetNColor_(r_n) == ref_color) {
        pos_cursor->ref = NULL;
    } else {
        r_ca.data = r_seg->dat.data;
        r_ca.offset = r_seg->dat.offset;
        r_ca.size = r_seg->dat.size;

        pos_cursor->ref =
            Zeta_CircularArray_Access(&r_ca, NULL, NULL, ret_seg_idx);
    }

    return pos_cursor->ref;
}

void Zeta_StageVector_PopL(void* sv, size_t cnt) {
    Zeta_StageVector_Cursor pos_cursor;
    Zeta_StageVector_PeekL(sv, &pos_cursor, NULL);

    Zeta_StageVector_Erase(sv, &pos_cursor, cnt);
}

void Zeta_StageVector_PopR(void* sv, size_t cnt) {
    Zeta_StageVector_Cursor pos_cursor;
    Zeta_StageVector_PeekR(sv, &pos_cursor, NULL);

    Zeta_StageVector_Erase(sv, &pos_cursor, cnt);
}

void Zeta_StageVector_Erase(void* sv_, void* pos_cursor_, size_t cnt) {
    Zeta_StageVector* sv = sv_;
    Zeta_StageVector_Cursor* pos_cursor = pos_cursor_;
    Zeta_StageVector_Cursor_Check(sv, pos_cursor);

    if (cnt == 0) { return; }

    Zeta_BinTreeNodeOperator btn_opr;
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

    Zeta_StageVector_Seg* m_seg =
        ZETA_MemberToStruct(Zeta_StageVector_Seg, n, m_n);

    Zeta_StageVector_Seg* first_seg = m_seg;
    size_t first_seg_idx = seg_idx;

    Zeta_StageVector_Seg* last_seg = m_n;

    Zeta_CircularArray m_ca;
    m_ca.width = width;
    m_ca.stride = stride;
    m_ca.capacity = seg_capacity;

    if (GetNColor_(m_n) == ref_color && 0 < seg_idx) {
        ZETA_DebugLogCurPos;

        m_seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, m_n);

        size_t ref_beg = m_seg->ref.beg;
        size_t ref_size = m_seg->ref.size;

        if (ref_size - seg_idx <= cnt) {
            ZETA_DebugLogCurPos;

            m_seg->ref.size = seg_idx;
            cnt -= ref_size - seg_idx;
            m_n = Zeta_BinTree_StepR(&btn_opr, m_n);
        } else {
            ZETA_DebugLogCurPos;

            Zeta_StageVector_Seg* pre_m_seg = AllocateRefSeg_(sv);
            sv->root =
                Zeta_RBTree_Insert(&btn_opr, l_n, &m_seg->n, &pre_m_seg->n);

            first_seg = pre_m_seg;

            pre_m_seg->ref.beg = ref_beg;
            pre_m_seg->ref.size = seg_idx;

            m_seg->ref.beg = ref_beg + seg_idx + cnt;
            m_seg->ref.size = ref_size - seg_idx - cnt;

            cnt = 0;
        }

        seg_idx = 0;

        ZETA_DebugLogCurPos;
    }

    while (0 < cnt) {
        m_seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, m_n);
        last_seg = m_seg;

        if (GetNColor_(m_n) == ref_color) {
            if (m_seg->ref.size <= cnt) {
                cnt -= m_seg->ref.size;
                void* nxt_m_n = Zeta_BinTree_StepR(&btn_opr, m_n);
                EraseSeg_(sv, &btn_opr, m_seg);
                m_n = nxt_m_n;

                continue;
            }

            m_seg->ref.beg += cnt;
            m_seg->ref.size -= cnt;

            break;
        }

        if (seg_idx == 0 && m_seg->dat.size <= cnt) {
            cnt -= m_seg->dat.size;
            void* nxt_m_n = Zeta_BinTree_StepR(&btn_opr, m_n);
            EraseSeg_(sv, &btn_opr, m_seg);
            m_n = nxt_m_n;

            continue;
        }

        size_t cur_cnt = ZETA_GetMinOf(cnt, m_seg->dat.size - seg_idx);

        m_ca.data = m_seg->dat.data;
        m_ca.offset = m_seg->dat.offset;
        m_ca.size = m_seg->dat.size;

        Zeta_CircularArray_Cursor ca_cursor;
        Zeta_CircularArray_Access(&m_ca, &ca_cursor, NULL, seg_idx);
        Zeta_CircularArray_Erase(&m_ca, &ca_cursor, cur_cnt);

        m_seg->dat.offset = m_ca.offset;
        m_seg->dat.size = m_ca.size;

        cnt -= cur_cnt;

        if (seg_idx == m_seg->dat.size) {
            m_n = Zeta_BinTree_StepR(&btn_opr, m_n);
            seg_idx = 0;
        }
    }

    void* ret_n = m_n;
    size_t ret_seg_idx = seg_idx;

    unsigned long long rand_seed = ZETA_PtrToAddr(m_n);

    bool_t first_exist = first_seg_idx != 0 || &first_seg->n == m_n;
    bool_t last_exist = &last_seg->n == m_n;

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

        if (first_exist) { segs_buffer[segs_buffer_i++] = first_seg; }

        if (last_exist && first_seg != last_seg) {
            segs_buffer[segs_buffer_i++] = last_seg;
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
    } else if (GetNColor_(&a_seg->n) == ref_color) {
        a_size = a_seg->ref.size;
        a_vacant = seg_capacity - ZETA_GetMinOf(a_size, seg_capacity);
    } else {
        a_size = a_seg->dat.size;
        a_vacant = seg_capacity - a_size;
    }

    if (b_seg == NULL) {
    } else if (GetNColor_(&b_seg->n) == ref_color) {
        b_size = b_seg->ref.size;
        b_vacant = seg_capacity - ZETA_GetMinOf(b_size, seg_capacity);
    } else {
        b_size = b_seg->dat.size;
        b_vacant = seg_capacity - b_size;
    }

    if (c_seg == NULL) {
    } else if (GetNColor_(&c_seg->n) == ref_color) {
        c_size = c_seg->ref.size;
        c_vacant = seg_capacity - ZETA_GetMinOf(c_size, seg_capacity);
    } else {
        c_size = c_seg->dat.size;
        c_vacant = seg_capacity - c_size;
    }

    if (d_seg == NULL) {
    } else if (GetNColor_(&d_seg->n) == ref_color) {
        d_size = d_seg->ref.size;
        d_vacant = seg_capacity - ZETA_GetMinOf(d_size, seg_capacity);
    } else {
        d_size = d_seg->dat.size;
        d_vacant = seg_capacity - d_size;
    }

    if (a_seg == NULL) { goto UPDATE; }

    if (ret_n == &a_seg->n) { ret_n = NULL; }

    if (b_seg == NULL) { goto UPDATE; }

    if (ret_n == &b_seg->n) {
        ret_n = NULL;
        ret_seg_idx += a_size;
    }

    if (GetNColor_(&a_seg->n) == ref_color &&
        GetNColor_(&b_seg->n) == ref_color &&
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

    if (GetNColor_(&b_seg->n) == ref_color &&
        GetNColor_(&c_seg->n) == ref_color &&
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

    if (GetNColor_(&c_seg->n) == ref_color &&
        GetNColor_(&d_seg->n) == ref_color &&
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
        GetNColor_(&a_seg->n) == ref_color ? a_seg->ref.size : a_seg->dat.size;

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
        GetNColor_(&b_seg->n) == ref_color ? b_seg->ref.size : b_seg->dat.size;

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
        GetNColor_(&c_seg->n) == ref_color ? c_seg->ref.size : c_seg->dat.size;

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
        GetNColor_(&d_seg->n) == ref_color ? d_seg->ref.size : d_seg->dat.size;

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

    if (sv->rb == pos_cursor->n || GetNColor_(pos_cursor->n) == ref_color) {
        pos_cursor->ref = NULL;
        return;
    }

    Zeta_StageVector_Seg* seg =
        ZETA_MemberToStruct(Zeta_StageVector_Seg, n, pos_cursor->n);

    m_ca.data = seg->dat.data;
    m_ca.offset = seg->dat.offset;
    m_ca.size = seg->dat.size;

    pos_cursor->ref =
        Zeta_CircularArray_Access(&m_ca, NULL, NULL, pos_cursor->seg_idx);
}

void Zeta_StageVector_EraseAll(void* sv_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    EraseAllSeg_(sv, &btn_opr, sv->root);

    InitTree_(sv, &btn_opr);
}

void Zeta_StageVector_Reset(void* sv_) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    EraseAllSeg_(sv, &btn_opr, sv->root);

    InitTree_(sv, &btn_opr);
    RefOrigin_(sv, &btn_opr);
}

void Zeta_StageVector_Check(void* sv_) {
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

ZETA_DeclareStruct(SanitizeRet);

struct SanitizeRet {
    bool_t b;
    size_t ref_beg;
    size_t ref_end;
};

static SanitizeRet Sanitize_(Zeta_StageVector* sv, Zeta_AssocContainer* dst_seg,
                             Zeta_AssocContainer* dst_data,
                             Zeta_BinTreeNodeOperator* btn_opr, void* n) {
    size_t stride = Zeta_StageVector_GetStride(sv);
    size_t origin_size = sv->origin->GetSize(sv->origin->context);

    void* l_n = Zeta_BinTree_StepL(btn_opr, n);
    void* r_n = Zeta_BinTree_StepR(btn_opr, n);

    if (sv->lb == n) {
        ZETA_DebugAssert(l_n == NULL);
        ZETA_DebugAssert(r_n != NULL);

        ZETA_DebugAssert(Zeta_BinTree_GetSize(btn_opr, n) == 1);

        Zeta_MemCheck_AddPtrSize(dst_seg, sv->lb,
                                 sizeof(Zeta_OrdCnt3RBTreeNode));
    } else if (sv->rb == n) {
        ZETA_DebugAssert(l_n != NULL);
        ZETA_DebugAssert(r_n == NULL);

        ZETA_DebugAssert(Zeta_BinTree_GetSize(btn_opr, n) == 1);

        Zeta_MemCheck_AddPtrSize(dst_seg, sv->rb,
                                 sizeof(Zeta_OrdCnt3RBTreeNode));
    } else if (GetNColor_(n) == ref_color) {
        ZETA_DebugAssert(l_n != NULL);
        ZETA_DebugAssert(r_n != NULL);

        Zeta_StageVector_Seg* seg =
            ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        Zeta_MemCheck_AddPtrSize(dst_seg, seg, sizeof(Zeta_StageVector_Seg))

            ZETA_DebugAssert(0 < seg->ref.size);

        ZETA_DebugAssert(seg->ref.beg < origin_size);
        ZETA_DebugAssert(seg->ref.beg + seg->ref.size <= origin_size);

        size_t size = seg->ref.size;

        ZETA_DebugAssert(size == Zeta_BinTree_GetSize(btn_opr, n));

        if (sv->lb != l_n) {
            Zeta_StageVector_Seg* l_seg =
                ZETA_MemberToStruct(Zeta_StageVector_Seg, n, l_n);

            if (GetNColor_(&l_seg->n) == ref_color) {
                ZETA_DebugAssert(l_seg->ref.beg + l_seg->ref.size <
                                 seg->ref.beg);
            }
        }

        if (sv->rb != r_n) {
            Zeta_StageVector_Seg* r_seg =
                ZETA_MemberToStruct(Zeta_StageVector_Seg, n, r_n);

            if (GetNColor_(&r_seg->n) == ref_color) {
                ZETA_DebugAssert(seg->ref.beg + seg->ref.size < r_seg->ref.beg);
            }
        }
    } else {
        ZETA_DebugAssert(GetNColor_(n) == dat_color);

        ZETA_DebugAssert(l_n != NULL);
        ZETA_DebugAssert(r_n != NULL);

        Zeta_StageVector_Seg* seg =
            ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        Zeta_MemCheck_AddPtrSize(dst_seg, seg, sizeof(Zeta_StageVector_Seg));

        size_t size = seg->dat.size;

        ZETA_DebugAssert(size == Zeta_BinTree_GetSize(btn_opr, n));

        ZETA_DebugAssert(0 < size);
        ZETA_DebugAssert(size <= sv->seg_capacity);
        ZETA_DebugAssert(seg->dat.offset < sv->seg_capacity);

        Zeta_MemCheck_AddPtrSize(dst_data, seg->dat.data,
                                 stride * sv->seg_capacity);

        size_t l_vacant;
        size_t r_vacant;

        if (sv->lb == l_n) {
            l_vacant = 0;
        } else {
            Zeta_StageVector_Seg* l_seg =
                ZETA_MemberToStruct(Zeta_StageVector_Seg, n, l_n);

            l_vacant = sv->seg_capacity -
                       (GetNColor_(l_n) == ref_color
                            ? ZETA_GetMinOf(l_seg->ref.size, sv->seg_capacity)
                            : l_seg->dat.size);
        }

        if (sv->rb == r_n) {
            r_vacant = 0;
        } else {
            Zeta_StageVector_Seg* r_seg =
                ZETA_MemberToStruct(Zeta_StageVector_Seg, n, r_n);

            r_vacant = sv->seg_capacity -
                       (GetNColor_(r_n) == ref_color
                            ? ZETA_GetMinOf(r_seg->ref.size, sv->seg_capacity)
                            : r_seg->dat.size);
        }

        ZETA_DebugAssert(0 <= l_vacant);
        ZETA_DebugAssert(0 <= r_vacant);
    }

    void* sub_l_n = Zeta_OrdCnt3RBTreeNode_GetL(NULL, n);
    void* sub_r_n = Zeta_OrdCnt3RBTreeNode_GetR(NULL, n);

    SanitizeRet l_check_ret = (SanitizeRet){ FALSE, 0, 0 };
    SanitizeRet r_check_ret = (SanitizeRet){ FALSE, 0, 0 };

    if (sub_l_n != NULL) {
        l_check_ret = Sanitize_(sv, dst_seg, dst_data, btn_opr, sub_l_n);
    }

    if (sub_r_n != NULL) {
        r_check_ret = Sanitize_(sv, dst_seg, dst_data, btn_opr, sub_r_n);
    }

    SanitizeRet ret = (SanitizeRet){ FALSE, 0, 0 };

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

    if (GetNColor_(n) == dat_color) { return ret; }

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

void Zeta_StageVector_Sanitize(void* sv_, Zeta_DebugHashMap* dst_seg,
                               Zeta_DebugHashMap* dst_data) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Sanitize_(sv, dst_seg, dst_data, &btn_opr, sv->root);
}

static void PrintState_(Zeta_StageVector* sv, Zeta_BinTreeNodeOperator* btn_opr,
                        void* n) {
    void* l_n = Zeta_OrdCnt3RBTreeNode_GetL(NULL, n);
    void* r_n = Zeta_OrdCnt3RBTreeNode_GetR(NULL, n);

    if (l_n != NULL) { PrintState_(sv, btn_opr, l_n); }

    if (sv->lb != n && sv->rb != n) {
        Zeta_StageVector_Seg* seg =
            ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

        if (GetNColor_(&seg->n) == ref_color) {
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

        if (GetNColor_(n) == ref_color) {
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
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    return GetStats_(sv, &btn_opr, sv->root);
}

bool_t Zeta_StageVector_Cursor_IsEqual(void* sv_, void const* cursor_a_,
                                       void const* cursor_b_) {
    Zeta_StageVector* sv = sv_;

    Zeta_StageVector_Cursor const* cursor_a = cursor_a_;
    Zeta_StageVector_Cursor const* cursor_b = cursor_b_;

    Zeta_StageVector_Cursor_Check(sv, cursor_a);
    Zeta_StageVector_Cursor_Check(sv, cursor_b);

    return cursor_a->idx == cursor_b->idx;
}

int Zeta_StageVector_Cursor_Compare(void* sv_, void const* cursor_a_,
                                    void const* cursor_b_) {
    Zeta_StageVector* sv = sv_;

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

    Zeta_StageVector_Cursor const* cursor_a = cursor_a_;
    Zeta_StageVector_Cursor const* cursor_b = cursor_b_;

    Zeta_StageVector_Cursor_Check(sv, cursor_a);
    Zeta_StageVector_Cursor_Check(sv, cursor_b);

    return cursor_a->idx - cursor_b->idx;
}

size_t Zeta_StageVector_Cursor_GetIdx(void* sv_, void const* cursor_) {
    Zeta_StageVector* sv = sv_;
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
    Zeta_StageVector_Cursor* cursor = cursor_;
    Zeta_StageVector_Cursor_Check(sv, cursor);

    if (step == 0) { return; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    ZETA_DebugAssert(step <= cursor->idx + 1);

    void* dst_n;
    size_t dst_seg_idx;

    void* n = cursor->n;

    Zeta_StageVector_Seg* seg = ZETA_MemberToStruct(Zeta_StageVector_Seg, n, n);

    size_t n_size =
        (sv->lb == n || sv->rb == n)
            ? 1
            : (GetNColor_(n) == ref_color ? seg->ref.size : seg->dat.size);

    Zeta_BinTree_AdvanceL(&dst_n, &dst_seg_idx, &btn_opr, n,
                          n_size - 1 - cursor->seg_idx + step);

    ZETA_DebugAssert(dst_n != NULL);

    cursor->idx -= step;
    cursor->n = dst_n;

    if (sv->lb == dst_n) {
        cursor->seg_idx = 0;
        cursor->ref = 0;
        return;
    }

    Zeta_StageVector_Seg* dst_seg =
        ZETA_MemberToStruct(Zeta_StageVector_Seg, n, dst_n);

    cursor->seg_idx = (GetNColor_(dst_n) == ref_color ? dst_seg->ref.size
                                                      : dst_seg->dat.size) -
                      1 - dst_seg_idx;

    if (GetNColor_(dst_n) == ref_color) {
        cursor->ref = NULL;
        return;
    }

    Zeta_CircularArray ca;
    ca.data = dst_seg->dat.data;
    ca.width = Zeta_StageVector_GetWidth(sv);
    ca.stride = Zeta_StageVector_GetStride(sv);
    ca.offset = dst_seg->dat.offset;
    ca.size = dst_seg->dat.size;
    ca.capacity = sv->seg_capacity;

    cursor->ref = Zeta_CircularArray_Access(&ca, NULL, NULL, cursor->seg_idx);
}

void Zeta_StageVector_Cursor_AdvanceR(void* sv_, void* cursor_, size_t step) {
    Zeta_StageVector* sv = sv_;
    Zeta_StageVector_Cursor* cursor = cursor_;
    Zeta_StageVector_Cursor_Check(sv, cursor);

    if (step == 0) { return; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t size = Zeta_StageVector_GetSize(sv);

    ZETA_DebugAssert(step <= size - cursor->idx);

    void* dst_n;
    size_t dst_seg_idx;

    Zeta_BinTree_AdvanceR(&dst_n, &dst_seg_idx, &btn_opr, cursor->n,
                          cursor->seg_idx + step);

    ZETA_DebugAssert(dst_n != NULL);

    cursor->idx += step;
    cursor->n = dst_n;
    cursor->seg_idx = dst_seg_idx;

    if (sv->rb == dst_n || GetNColor_(dst_n) == ref_color) {
        cursor->ref = NULL;
        return;
    }

    Zeta_StageVector_Seg* dst_seg =
        ZETA_MemberToStruct(Zeta_StageVector_Seg, n, dst_n);

    Zeta_CircularArray ca;
    ca.data = dst_seg->dat.data;
    ca.width = Zeta_StageVector_GetWidth(sv);
    ca.stride = Zeta_StageVector_GetStride(sv);
    ca.offset = dst_seg->dat.offset;
    ca.size = dst_seg->dat.size;
    ca.capacity = sv->seg_capacity;

    cursor->ref = Zeta_CircularArray_Access(&ca, NULL, NULL, cursor->seg_idx);
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
    ZETA_DebugAssert(re_cursor.ref == cursor->ref);
}

void Zeta_StageVector_DeploySeqContainer(void* sv_,
                                         Zeta_SeqContainer* seq_cntr) {
    Zeta_StageVector* sv = sv_;
    CheckSV_(sv);

    Zeta_SeqContainer_Init(seq_cntr);

    seq_cntr->context = sv;

    seq_cntr->cursor_size = sizeof(Zeta_StageVector_Cursor);

    seq_cntr->GetWidth = Zeta_StageVector_GetWidth;

    seq_cntr->GetStride = Zeta_StageVector_GetStride;

    seq_cntr->GetSize = Zeta_StageVector_GetSize;

    seq_cntr->GetCapacity = Zeta_StageVector_GetCapacity;

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
