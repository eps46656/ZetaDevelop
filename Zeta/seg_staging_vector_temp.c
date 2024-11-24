#include "seg_staging_vector_temp.h"

#include "circular_array.h"
#include "debugger.h"
#include "dynamic_hash_table.h"
#include "mem_check_utils.h"
#include "memory.h"
#include "pool_allocator.h"
#include "ptr_utils.h"
#include "rbtree.h"
#include "seg_utils.h"
#include "utils.h"

#if ZETA_IsDebug

#define CheckCntr_(cntr) Cntr_(Check)((cntr))

#define CheckCursor_(cntr, cursor) Cntr_(Cursor_Check)((cntr), (cursor))

#else

#define CheckCntr_(cntr)

#define CheckCursor_(cntr, cursor)

#endif

#define Cntr_(x) ZETA_Concat(Cntr, ZETA_Concat(_, x))

#if STAGING

#define Cntr Zeta_StagingVector

#define GetWidth_(cntr) ZETA_SeqCntr_GetWidth((cntr)->origin)

#define ref_color (ZETA_StagingVector_ref_color)
#define dat_color (ZETA_StagingVector_dat_color)

#define TreeNode Zeta_OrdCnt3RBTreeNode

#define TreeNodeOpr (&zeta_ord_cnt_3rb_tree_node_opr)

#define GetNColor__(tmp, n)                                     \
    ({                                                          \
        int tmp = ZETA_Concat(TreeNode, _GetLColor)(NULL, (n)); \
        ZETA_DebugAssert(tmp == ref_color || tmp == dat_color); \
        tmp;                                                    \
    })

#define DirectlySetNColor_(n, c) \
    ZETA_Concat(TreeNode, _SetLColor)(NULL, (n), (c))

#define StagingThreeWay(cond, x, y) ((cond) ? (x) : (y))

#else

#define Cntr Zeta_SegVector

#define GetWidth_(cntr) (cntr)->width

#define ref_color 0
#define dat_color 1

#define TreeNode Zeta_OrdCntRBTreeNode

#define TreeNodeOpr &zeta_ord_cnt_rb_tree_node_opr

#define GetNColor__(tmp, n) dat_color

#define DirectlySetNColor_(n, c)

#define StagingThreeWay(cond, x, y) (y)

#endif

#define GetNColor_(n) GetNColor__(ZETA_TmpName, n)

#define NToSeg__(tmp_n, n_)                        \
    ({                                             \
        ZETA_AutoVar(tmp_n, n_);                   \
        ZETA_DebugAssert(0 <= GetNColor_(tmp_n));  \
        ZETA_MemberToStruct(Cntr_(Seg), n, tmp_n); \
    })

#define NToSeg_(n) NToSeg__(ZETA_TmpName, (n))

#define SetNColor__(tmp_n, n, c)                  \
    {                                             \
        ZETA_AutoVar(tmp_n, n);                   \
        ZETA_DebugAssert(0 <= GetNColor_(tmp_n)); \
        DirectlySetNColor_(tmp_n, (c));           \
    }                                             \
    ZETA_StaticAssert(TRUE)

#define SetNColor_(n, c) SetNColor__(ZETA_TmpName, (n), (c))

#define GetAvgCnt_(x)                                                   \
    ({                                                                  \
        size_t x_ = (x);                                                \
        size_t seg_cnt = ZETA_CeilIntDiv(x_, seg_capacity);             \
        (x_ + Zeta_SimpleRandomRotate(&rand_seed) % seg_cnt) / seg_cnt; \
    })

static void InitTree_(Cntr* cntr) {
    Zeta_BinTreeNodeOperator const* btn_opr = TreeNodeOpr;

    ZETA_Concat(TreeNode, _Init)(NULL, cntr->lb);
    ZETA_Concat(TreeNode, _Init)(NULL, cntr->rb);

    ZETA_Concat(TreeNode, _SetAccSize)(NULL, cntr->lb, 1);
    ZETA_Concat(TreeNode, _SetAccSize)(NULL, cntr->rb, 1);

    void* root = NULL;

    root = Zeta_RBTree_InsertR(btn_opr, root, cntr->lb);
    root = Zeta_RBTree_InsertR(btn_opr, root, cntr->rb);

    cntr->root = root;
}

static void* AllocateData_(Cntr* cntr, Zeta_PoolAllocator* datas) {
    void* data = NULL;

    if (datas != NULL) { data = Zeta_PoolAllocator_Allocate(datas, 0); }

    if (data == NULL) {
        data = ZETA_Allocator_SafeAllocate(
            cntr->data_allocator, 1, GetWidth_(cntr) * cntr->seg_capacity);
    }

    return data;
}

static void* AllocateSeg_(Cntr* cntr, Zeta_PoolAllocator* segs) {
    Cntr_(Seg)* seg = NULL;

    if (segs != NULL) { seg = Zeta_PoolAllocator_Allocate(segs, 0); }

    if (seg == NULL) {
        seg = ZETA_Allocator_SafeAllocate(
            cntr->seg_allocator, alignof(Cntr_(Seg)), sizeof(Cntr_(Seg)));
    }

    ZETA_Concat(TreeNode, _Init)(NULL, &seg->n);

    return seg;
}

#if STAGING

static Cntr_(Seg) * AllocateRefSeg_(Cntr* cntr) {
    Cntr_(Seg)* seg = AllocateSeg_(cntr, NULL);

    ZETA_Concat(TreeNode, _Init)(NULL, &seg->n);
    ZETA_Concat(TreeNode, _SetAccSize)(NULL, &seg->n, 0);

    DirectlySetNColor_(&seg->n, ref_color);

    seg->ref.beg = 0;
    seg->ref.size = 0;

    return seg;
}

#endif

static Cntr_(Seg) * AllocateDatSeg_(Cntr* cntr) {
    Cntr_(Seg)* seg = AllocateSeg_(cntr, NULL);

    ZETA_Concat(TreeNode, _Init)(NULL, &seg->n);
    ZETA_Concat(TreeNode, _SetAccSize)(NULL, &seg->n, 0);

    DirectlySetNColor_(&seg->n, dat_color);

    seg->dat.data = AllocateData_(cntr, NULL);

    seg->dat.offset = 0;
    seg->dat.size = 0;

    return seg;
}

#if STAGING

static void TransRefSegToDatSeg_(Cntr* cntr, Cntr_(Seg) * seg) {
    void* data = AllocateData_(cntr, NULL);

    size_t size = seg->ref.size;

    void* origin_cursor = ZETA_SeqCntr_AllocaCursor(cntr->origin);

    ZETA_SeqCntr_Access(cntr->origin, seg->ref.beg, origin_cursor, NULL);
    ZETA_SeqCntr_Read(cntr->origin, origin_cursor, size, data, NULL);

    SetNColor_(&seg->n, dat_color);

    seg->dat.data = data;
    seg->dat.offset = 0;
    seg->dat.size = size;
}

#endif

static void EraseSeg_(Cntr* cntr, Cntr_(Seg) * seg) {
    Zeta_BinTreeNodeOperator const* btn_opr = TreeNodeOpr;

    int color = GetNColor_(&seg->n);
    ZETA_DebugAssert(color == ref_color || color == dat_color);

    cntr->root = Zeta_RBTree_Extract(btn_opr, &seg->n);

    if (color == dat_color) {
        ZETA_Allocator_Deallocate(cntr->data_allocator, seg->dat.data);
    }

    ZETA_Allocator_Deallocate(cntr->seg_allocator, seg);
}

static void EraseAllSeg_(Cntr* cntr, void* n) {
    void* nl = ZETA_Concat(TreeNode, _GetL)(NULL, n);
    void* nr = ZETA_Concat(TreeNode, _GetR)(NULL, n);

    if (cntr->lb != n && cntr->rb != n) {
        Cntr_(Seg)* seg = NToSeg_(n);

        if (GetNColor_(n) == dat_color) {
            ZETA_Allocator_Deallocate(cntr->data_allocator, seg->dat.data);
        }

        ZETA_Allocator_Deallocate(cntr->seg_allocator, seg);
    }

    if (nl != NULL) { EraseAllSeg_(cntr, nl); }
    if (nr != NULL) { EraseAllSeg_(cntr, nr); }
}

#if STAGING

static void RefOrigin_(Cntr* cntr) {
    ZETA_Unused(cntr);

    Zeta_BinTreeNodeOperator const* btn_opr = TreeNodeOpr;

    size_t origin_size = ZETA_SeqCntr_GetSize(cntr->origin);

    if (origin_size == 0) { return; }

    Cntr_(Seg)* seg = AllocateRefSeg_(cntr);
    cntr->root = Zeta_RBTree_Insert(btn_opr, cntr->lb, cntr->rb, &seg->n);

    seg->ref.beg = 0;
    seg->ref.size = origin_size;

    Zeta_BinTree_SetSize(btn_opr, &seg->n, origin_size);
}

#else

#define RefOrigin_(cntr)

#endif

#if STAGING

static void PushRefL_(Cntr* cntr, Zeta_CircularArray* ca, size_t beg,
                      size_t size) {
    Zeta_CircularArray_Cursor ca_cursor;
    Zeta_CircularArray_PeekL(ca, &ca_cursor, NULL);
    Zeta_CircularArray_Insert(ca, &ca_cursor, size);

    void* origin_cursor = ZETA_SeqCntr_AllocaCursor(cntr->origin);

    ZETA_SeqCntr_Access(cntr->origin, beg, origin_cursor, NULL);

    Zeta_CircularArray_AssignFromSeqCntr(ca, &ca_cursor, cntr->origin,
                                         origin_cursor, size);
}

static void PushRefR_(Cntr* cntr, Zeta_CircularArray* ca, size_t beg,
                      size_t size) {
    Zeta_CircularArray_Cursor ca_cursor;
    Zeta_CircularArray_GetRBCursor(ca, &ca_cursor);
    Zeta_CircularArray_Insert(ca, &ca_cursor, size);

    void* origin_cursor = ZETA_SeqCntr_AllocaCursor(cntr->origin);

    ZETA_SeqCntr_Access(cntr->origin, beg, origin_cursor, NULL);

    Zeta_CircularArray_AssignFromSeqCntr(ca, &ca_cursor, cntr->origin,
                                         origin_cursor, size);
}

#else

#define PushRefL_(cntr, ca, beg, size)

#define PushRefR_(cntr, ca, beg, size)

#endif

static void Merge2_(Cntr* cntr, size_t width, Cntr_(Seg) * a_seg,
                    Cntr_(Seg) * b_seg) {
#if STAGING
    int a_c = GetNColor_(&a_seg->n);
    int b_c = GetNColor_(&b_seg->n);
#endif

    size_t a_size =
        StagingThreeWay(a_c == ref_color, a_seg->ref.size, a_seg->dat.size);
    size_t b_size =
        StagingThreeWay(b_c == ref_color, b_seg->ref.size, b_seg->dat.size);

#if STAGING
    size_t total_size = a_size + b_size;
#endif

    size_t seg_capacity = cntr->seg_capacity;

    unsigned long long rand_seed =
        ZETA_PtrToAddr(a_seg) + ZETA_PtrToAddr(b_seg);

#if STAGING
    void* origin_cursor = ZETA_SeqCntr_AllocaCursor(cntr->origin);

    if (a_c == ref_color && b_c == ref_color) {
        void* data = AllocateData_(cntr, NULL);

        ZETA_SeqCntr_Access(cntr->origin, a_seg->ref.beg, origin_cursor, NULL);
        ZETA_SeqCntr_Read(cntr->origin, origin_cursor, a_size, data, NULL);

        ZETA_SeqCntr_Access(cntr->origin, b_seg->ref.beg, origin_cursor, NULL);
        ZETA_SeqCntr_Read(cntr->origin, origin_cursor, b_seg->ref.size,
                          data + width * a_size, NULL);

        a_seg->ref.size = 0;
        b_seg->ref.size = 0;

        Cntr_(Seg) * dst_seg;

        switch (Zeta_SimpleRandomRotate(&rand_seed) % 2) {
            case 0: dst_seg = a_seg; break;
            case 1: dst_seg = b_seg; break;
        }

        SetNColor_(&dst_seg->n, dat_color);
        dst_seg->dat.data = data;
        dst_seg->dat.size = total_size;

        return;
    }
#endif

    Zeta_CircularArray a_ca;
    a_ca.data = a_seg->dat.data;
    a_ca.width = width;
    a_ca.offset = a_seg->dat.offset;
    a_ca.size = a_seg->dat.size;
    a_ca.capacity = seg_capacity;

    Zeta_CircularArray b_ca;
    b_ca.data = b_seg->dat.data;
    b_ca.width = width;
    b_ca.offset = b_seg->dat.offset;
    b_ca.size = b_seg->dat.size;
    b_ca.capacity = seg_capacity;

    size_t merge_a_cost = StagingThreeWay(a_c == ref_color, a_size, 0) + b_size;
    size_t merge_b_cost = StagingThreeWay(b_c == ref_color, b_size, 0) + a_size;

    if (Zeta_Choose2(merge_a_cost <= merge_b_cost, merge_b_cost <= merge_a_cost,
                     &rand_seed) == 0) {
#if STAGING
        if (b_c == ref_color) {
            PushRefR_(cntr, &a_ca, b_seg->ref.beg, b_size);
        } else
#endif
        {
            SegShoveL(&a_ca, &b_ca, 0, 0, b_size);
        }
    } else {
#if STAGING
        if (a_c == ref_color) {
            PushRefL_(cntr, &b_ca, a_seg->ref.beg, a_size);
        } else
#endif
        {
            SegShoveR(&a_ca, &b_ca, 0, 0, a_size);
        }
    }

#if STAGING
    if (a_c == ref_color) {
        a_seg->ref.size = 0;
    } else
#endif
    {
        a_seg->dat.offset = a_ca.offset;
        a_seg->dat.size = a_ca.size;
    }

#if STAGING
    if (b_c == ref_color) {
        b_seg->ref.size = 0;
    } else
#endif
    {
        b_seg->dat.offset = b_ca.offset;
        b_seg->dat.size = b_ca.size;
    }
}

#if STAGING

#define SetSegState_(seg_tmp, ca_tmp, vacant_tmp, seg, ca, vacant)        \
    {                                                                     \
        Cntr_(Seg)* seg_tmp = (seg);                                      \
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

#else

#define SetSegState_(seg_tmp, ca_tmp, vacant_tmp, seg, ca, vacant) \
    {                                                              \
        Cntr_(Seg)* seg_tmp = (seg);                               \
        Zeta_CircularArray* ca_tmp = (ca);                         \
        size_t* vacant_tmp = (vacant);                             \
                                                                   \
        {                                                          \
            ca_tmp->data = seg_tmp->dat.data;                      \
            ca_tmp->offset = seg_tmp->dat.offset;                  \
            ca_tmp->size = seg_tmp->dat.size;                      \
            *vacant_tmp = seg_capacity - ca_tmp->size;             \
        }                                                          \
    }

#endif

#define SetSegState(seg, ca, vacant) \
    SetSegState_(ZETA_TmpName, ZETA_TmpName, ZETA_TmpName, seg, ca, vacant)

#if STAGING

static void RefShoveL_(Cntr* cntr, Zeta_CircularArray* l_ca, Cntr_(Seg) * r_seg,
                       size_t rl_size, size_t ins_cnt, size_t shove_cnt) {
    size_t width = l_ca->width;

    size_t cnt_a = ZETA_GetMinOf(rl_size, shove_cnt);
    size_t cnt_b = ZETA_GetMinOf(ins_cnt, shove_cnt - cnt_a);
    size_t cnt_c = shove_cnt - cnt_a - cnt_b;

    size_t l_i = l_ca->size;

    Zeta_CircularArray_Cursor l_cursor;
    void* r_cursor = ZETA_SeqCntr_AllocaCursor(cntr->origin);

    Zeta_CircularArray_GetRBCursor(l_ca, &l_cursor);

    Zeta_CircularArray_Insert(l_ca, &l_cursor, shove_cnt);

    ZETA_SeqCntr_Access(cntr->origin, r_seg->ref.beg, r_cursor, NULL);

    for (size_t i = cnt_a; 0 < i;) {
        size_t cur_i =
            ZETA_GetMinOf(i, Zeta_CircularArray_GetLongestContSucr(l_ca, l_i));
        i -= cur_i;

        ZETA_SeqCntr_Read(cntr->origin, r_cursor, cur_i,
                          Zeta_CircularArray_Access(l_ca, l_i, NULL, NULL),
                          r_cursor);

        l_i += cur_i;
    }

    l_i += cnt_b;

    for (size_t i = cnt_c; 0 < i;) {
        size_t cur_i =
            ZETA_GetMinOf(i, Zeta_CircularArray_GetLongestContSucr(l_ca, l_i));
        i -= cur_i;

        ZETA_SeqCntr_Read(cntr->origin, r_cursor, cur_i,
                          Zeta_CircularArray_Access(l_ca, l_i, NULL, NULL),
                          r_cursor);

        l_i += cur_i;
    }

    size_t new_r_size = r_seg->ref.size + ins_cnt - shove_cnt;

    if (cnt_b < ins_cnt) {
        void* data = AllocateData_(cntr, NULL);
        void* data_i = data;

        if (cnt_a < rl_size) {
            ZETA_SeqCntr_Read(cntr->origin, r_cursor, rl_size - cnt_a, data_i,
                              r_cursor);
        }

        data_i += width * (rl_size - cnt_a + ins_cnt - cnt_b);

        if (rl_size < r_seg->ref.size) {
            ZETA_SeqCntr_Read(cntr->origin, r_cursor, r_seg->ref.size - rl_size,
                              data_i, r_cursor);
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

static void RefShoveR_(Cntr* cntr, Cntr_(Seg) * l_seg, Zeta_CircularArray* r_ca,
                       size_t lr_size, size_t ins_cnt, size_t shove_cnt) {
    size_t width = r_ca->width;

    size_t cnt_a = ZETA_GetMinOf(lr_size, shove_cnt);
    size_t cnt_b = ZETA_GetMinOf(ins_cnt, shove_cnt - cnt_a);
    size_t cnt_c = shove_cnt - cnt_a - cnt_b;

    size_t r_i = 0;

    void* l_cursor = ZETA_SeqCntr_AllocaCursor(cntr->origin);

    Zeta_CircularArray_Cursor r_cursor;

    Zeta_CircularArray_PeekL(r_ca, &r_cursor, NULL);

    Zeta_CircularArray_Insert(r_ca, &r_cursor, shove_cnt);

    ZETA_SeqCntr_Access(cntr->origin,
                        l_seg->ref.beg + l_seg->ref.size - cnt_a - cnt_c,
                        l_cursor, NULL);

    for (size_t i = cnt_c; 0 < i;) {
        size_t cur_i =
            ZETA_GetMinOf(i, Zeta_CircularArray_GetLongestContSucr(r_ca, r_i));
        i -= cur_i;

        ZETA_SeqCntr_Read(cntr->origin, l_cursor, cur_i,
                          Zeta_CircularArray_Access(r_ca, r_i, NULL, NULL),
                          l_cursor);

        r_i += cur_i;
    }

    r_i += cnt_b;

    for (size_t i = cnt_a; 0 < i;) {
        size_t cur_i =
            ZETA_GetMinOf(i, Zeta_CircularArray_GetLongestContSucr(r_ca, r_i));
        i -= cur_i;

        ZETA_SeqCntr_Read(cntr->origin, l_cursor, cur_i,
                          Zeta_CircularArray_Access(r_ca, r_i, NULL, NULL),
                          l_cursor);

        r_i += cur_i;
    }

    size_t new_l_size = l_seg->ref.size + ins_cnt - shove_cnt;

    if (cnt_b < ins_cnt) {
        void* data = AllocateData_(cntr, NULL);
        void* data_i = data;

        ZETA_SeqCntr_Access(cntr->origin, l_seg->ref.beg, l_cursor, NULL);

        if (lr_size < l_seg->ref.size) {
            ZETA_SeqCntr_Read(cntr->origin, l_cursor, l_seg->ref.size - lr_size,
                              data_i, l_cursor);
        }

        data_i += width * (l_seg->ref.size - lr_size + ins_cnt - cnt_b);

        if (cnt_a < lr_size) {
            ZETA_SeqCntr_Read(cntr->origin, l_cursor, lr_size - cnt_a, data_i,
                              l_cursor);
        }

        SetNColor_(&l_seg->n, dat_color);

        l_seg->dat.data = data;
        l_seg->dat.offset = 0;
        l_seg->dat.size = new_l_size;
    } else {
        l_seg->ref.size = new_l_size;
    }
}

#else

#define RefShoveL_(cntr, l_ca, r_seg, rl_size, ins_cnt, shove_cnt)

#define RefShoveR_(cntr, l_seg, r_ca, lr_size, ins_cnt, shove_cnt)

#endif

void Cntr_(Init)(void* cntr_) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    cntr->lb = ZETA_Allocator_SafeAllocate(cntr->seg_allocator,
                                           alignof(TreeNode), sizeof(TreeNode));

    cntr->rb = ZETA_Allocator_SafeAllocate(cntr->seg_allocator,
                                           alignof(TreeNode), sizeof(TreeNode));

    InitTree_(cntr);
    RefOrigin_(cntr);
}

void Cntr_(Deinit)(void* cntr_) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    EraseAllSeg_(cntr, cntr->root);

    ZETA_Allocator_Deallocate(cntr->seg_allocator, cntr->lb);
    ZETA_Allocator_Deallocate(cntr->seg_allocator, cntr->rb);
}

size_t Cntr_(GetWidth)(void* cntr_) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    return GetWidth_(cntr);
}

size_t Cntr_(GetSize)(void* cntr_) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    return ZETA_Concat(TreeNode, _GetAccSize)(NULL, cntr->root) - 2;
}

size_t Cntr_(GetCapacity)(void* cntr_) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    return ZETA_RangeMaxOf(size_t);
}

void Cntr_(GetLBCursor)(void* cntr_, void* dst_cursor_) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    Cntr_(Cursor)* dst_cursor = dst_cursor_;

    if (dst_cursor == NULL) { return; }

    dst_cursor->cntr = cntr;
    dst_cursor->idx = -1;
    dst_cursor->n = cntr->lb;
    dst_cursor->seg_idx = 0;
    dst_cursor->ref = NULL;
}

void Cntr_(GetRBCursor)(void* cntr_, void* dst_cursor_) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    Cntr_(Cursor)* dst_cursor = dst_cursor_;

    if (dst_cursor == NULL) { return; }

    dst_cursor->cntr = cntr;
    dst_cursor->idx = Cntr_(GetSize)(cntr);
    dst_cursor->n = cntr->rb;
    dst_cursor->seg_idx = 0;
    dst_cursor->ref = NULL;
}

void* Cntr_(PeekL)(void* cntr_, void* dst_cursor_, void* dst_elem) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    Cntr_(Cursor)* dst_cursor = dst_cursor_;

    Zeta_BinTreeNodeOperator const* btn_opr = TreeNodeOpr;

    size_t width = GetWidth_(cntr);
    size_t seg_capacity = cntr->seg_capacity;

    void* n = Zeta_BinTree_StepR(btn_opr, cntr->lb);

    if (cntr->rb == n) {
        if (dst_cursor != NULL) {
            dst_cursor->cntr = cntr;
            dst_cursor->idx = 0;
            dst_cursor->n = n;
            dst_cursor->seg_idx = 0;
            dst_cursor->ref = NULL;
        }

        return NULL;
    }

    Cntr_(Seg)* seg = NToSeg_(n);

    void* ref = NULL;

#if STAGING
    if (GetNColor_(n) == ref_color) {
        ZETA_SeqCntr_Access(cntr->origin, seg->ref.beg, NULL, dst_elem);
    } else
#endif
    {
        Zeta_CircularArray ca;
        ca.data = seg->dat.data;
        ca.width = width;
        ca.offset = seg->dat.offset;
        ca.size = seg->dat.size;
        ca.capacity = seg_capacity;

        ref = Zeta_CircularArray_PeekL(&ca, NULL, dst_elem);
    }

    if (dst_cursor != NULL) {
        dst_cursor->cntr = cntr;
        dst_cursor->idx = 0;
        dst_cursor->n = n;
        dst_cursor->seg_idx = 0;
        dst_cursor->ref = ref;
    }

    return ref;
}

void* Cntr_(PeekR)(void* cntr_, void* dst_cursor_, void* dst_elem) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    Cntr_(Cursor)* dst_cursor = dst_cursor_;

    Zeta_BinTreeNodeOperator const* btn_opr = TreeNodeOpr;

    size_t width = GetWidth_(cntr);
    size_t seg_capacity = cntr->seg_capacity;
    size_t size = Cntr_(GetSize)(cntr);

    void* n = Zeta_BinTree_StepL(btn_opr, cntr->rb);

    if (cntr->lb == n) {
        if (dst_cursor != NULL) {
            dst_cursor->cntr = cntr;
            dst_cursor->idx = -1;
            dst_cursor->n = n;
            dst_cursor->seg_idx = 0;
            dst_cursor->ref = NULL;
        }

        return NULL;
    }

    Cntr_(Seg)* seg = NToSeg_(n);

    void* ref = NULL;

    size_t seg_size;

#if STAGING
    if (GetNColor_(n) == ref_color) {
        seg_size = seg->ref.size;

        ZETA_SeqCntr_Access(cntr->origin, seg->ref.beg + seg_size - 1, NULL,
                            dst_elem);
    } else
#endif
    {
        seg_size = seg->dat.size;

        Zeta_CircularArray ca;
        ca.data = seg->dat.data;
        ca.width = width;
        ca.offset = seg->dat.offset;
        ca.size = seg_size;
        ca.capacity = seg_capacity;

        ref = Zeta_CircularArray_PeekR(&ca, NULL, dst_elem);
    }

    if (dst_cursor != NULL) {
        dst_cursor->cntr = cntr;
        dst_cursor->idx = size - 1;
        dst_cursor->n = n;
        dst_cursor->seg_idx = seg_size - 1;
        dst_cursor->ref = ref;
    }

    return ref;
}

void* Cntr_(Access)(void* cntr_, size_t idx, void* dst_cursor_,
                    void* dst_elem) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    Cntr_(Cursor)* dst_cursor = dst_cursor_;

    Zeta_BinTreeNodeOperator const* btn_opr = TreeNodeOpr;

    size_t width = GetWidth_(cntr);
    size_t seg_capacity = cntr->seg_capacity;
    size_t size = Cntr_(GetSize)(cntr);

    ZETA_DebugAssert(idx + 1 < size + 2);

    void* n;
    Cntr_(Seg) * seg;
    size_t seg_idx;

    Zeta_BinTree_AccessL(&n, &seg_idx, btn_opr, cntr->root, idx + 1);

    void* ref = NULL;

    if (idx < size) {
        seg = NToSeg_(n);

        if (GetNColor_(n) == dat_color) {
            Zeta_CircularArray ca;
            ca.data = seg->dat.data;
            ca.width = width;
            ca.offset = seg->dat.offset;
            ca.size = seg->dat.size;
            ca.capacity = seg_capacity;

            ref = Zeta_CircularArray_Access(&ca, seg_idx, NULL, dst_elem);
        }
#if STAGING
        else if (dst_elem != NULL) {
            ZETA_SeqCntr_Access(cntr->origin, seg->ref.beg + seg_idx, NULL,
                                dst_elem);
        }
#endif
    }

    if (dst_cursor != NULL) {
        dst_cursor->cntr = cntr;
        dst_cursor->idx = idx;
        dst_cursor->n = n;
        dst_cursor->seg_idx = seg_idx;
        dst_cursor->ref = ref;
    }

    return ref;
}

void* Cntr_(Refer)(void* cntr_, void const* pos_cursor_) {
    Cntr* cntr = cntr_;
    Cntr_(Cursor) const* pos_cursor = pos_cursor_;
    Cntr_(Cursor_Check)(cntr, pos_cursor);

    return pos_cursor->ref;
}

void Cntr_(Read)(void* cntr_, void const* pos_cursor_, size_t cnt, void* dst,
                 void* dst_cursor_) {
    Cntr* cntr = cntr_;
    Cntr_(Cursor) const* pos_cursor = pos_cursor_;
    Cntr_(Cursor_Check)(cntr, pos_cursor);

    Cntr_(Cursor)* dst_cursor = dst_cursor_;

    if (cnt == 0) {
        if (dst_cursor != NULL) {
            dst_cursor->cntr = cntr;
            dst_cursor->idx = pos_cursor->idx;
            dst_cursor->n = pos_cursor->n;
            dst_cursor->seg_idx = pos_cursor->seg_idx;
            dst_cursor->ref = pos_cursor->ref;
        }

        return;
    }

    Zeta_BinTreeNodeOperator const* btn_opr = TreeNodeOpr;

    size_t width = GetWidth_(cntr);
    size_t seg_capacity = cntr->seg_capacity;
    size_t size = Cntr_(GetSize)(cntr);

    size_t end = pos_cursor->idx + cnt;
    ZETA_DebugAssert(end <= size);

    ZETA_DebugAssert(dst);

    void* n = pos_cursor->n;
    Cntr_(Seg) * seg;

    size_t seg_idx = pos_cursor->seg_idx;

#if STAGING
    void* origin_cursor = ZETA_SeqCntr_AllocaCursor(cntr->origin);
#endif

    Zeta_CircularArray ca;
    ca.width = width;
    ca.capacity = seg_capacity;

    while (0 < cnt) {
        seg = NToSeg_(n);

        size_t cur_cnt;
        size_t seg_size;

#if STAGING
        if (GetNColor_(n) == ref_color) {
            seg_size = seg->ref.size;
            cur_cnt = ZETA_GetMinOf(cnt, seg_size - seg_idx);

            ZETA_SeqCntr_Access(cntr->origin, seg->ref.beg + seg_idx,
                                origin_cursor, NULL);
            ZETA_SeqCntr_Read(cntr->origin, origin_cursor, cur_cnt, dst, NULL);

        } else
#endif
        {
            seg_size = seg->dat.size;
            cur_cnt = ZETA_GetMinOf(cnt, seg_size - seg_idx);

            ca.data = seg->dat.data;
            ca.offset = seg->dat.offset;
            ca.size = seg_size;

            Zeta_CircularArray_Cursor ca_cursor;
            Zeta_CircularArray_Access(&ca, seg_idx, &ca_cursor, NULL);
            Zeta_CircularArray_Read(&ca, &ca_cursor, cur_cnt, dst, NULL);
        }

        dst += width * cur_cnt;

        seg_idx += cur_cnt;
        cnt -= cur_cnt;

        if (seg_idx == seg_size) {
            n = Zeta_BinTree_StepR(btn_opr, n);
            seg_idx = 0;
        }
    }

    if (dst_cursor == NULL) { return; }

    dst_cursor->cntr = cntr;
    dst_cursor->idx = end;
    dst_cursor->n = n;
    dst_cursor->seg_idx = seg_idx;

    if (cntr->rb == n
#if STAGING
        || GetNColor_(n) == ref_color
#endif
    ) {
        dst_cursor->ref = NULL;
        return;
    }

    seg = NToSeg_(n);

    ca.data = seg->dat.data;
    ca.offset = seg->dat.offset;
    ca.size = seg->dat.size;

    dst_cursor->ref = Zeta_CircularArray_Access(&ca, seg_idx, NULL, NULL);
}

void Cntr_(Write)(void* cntr_, void* pos_cursor_, size_t cnt, void const* src,
                  void* dst_cursor_) {
    Cntr* cntr = cntr_;
    Cntr_(Cursor)* pos_cursor = pos_cursor_;
    Cntr_(Cursor_Check)(cntr, pos_cursor);

    Cntr_(Cursor)* dst_cursor = dst_cursor_;

    if (cnt == 0) {
        if (dst_cursor != NULL) {
            dst_cursor->cntr = cntr;
            dst_cursor->idx = pos_cursor->idx;
            dst_cursor->n = pos_cursor->n;
            dst_cursor->seg_idx = pos_cursor->seg_idx;
            dst_cursor->ref = pos_cursor->ref;
        }

        return;
    }

    Zeta_BinTreeNodeOperator const* btn_opr = TreeNodeOpr;

    size_t width = GetWidth_(cntr);
    size_t seg_capacity = cntr->seg_capacity;
    size_t size = Cntr_(GetSize)(cntr);

    size_t end = pos_cursor->idx + cnt;
    ZETA_DebugAssert(end <= size);

    ZETA_DebugAssert(src != NULL);

    size_t seg_idx = pos_cursor->seg_idx;

    void* n = pos_cursor->n;

#if STAGING
    void* l_n = Zeta_BinTree_StepL(btn_opr, n);
#endif

    Cntr_(Seg) * seg;

    Zeta_CircularArray ca;
    ca.width = width;
    ca.capacity = seg_capacity;

#if STAGING
    void* origin_cursor = ZETA_SeqCntr_AllocaCursor(cntr->origin);

    unsigned long long rand_seed = ZETA_PtrToAddr(n);

    if (GetNColor_(n) == ref_color && 0 < seg_idx) {
        seg = NToSeg_(n);

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
            Cntr_(Seg)* new_seg = AllocateRefSeg_(cntr);
            cntr->root = Zeta_RBTree_Insert(btn_opr, l_n, n, &new_seg->n);

            new_seg->ref.beg = seg->ref.beg;
            new_seg->ref.size = l_seg_size;

            seg->ref.beg += l_seg_size;
            seg->ref.size -= l_seg_size;

            ZETA_CheckAssert(0 < seg->ref.size);

            Zeta_BinTree_SetSize(btn_opr, &new_seg->n, new_seg->ref.size);

            l_n = &new_seg->n;
        }

        if (seg_idx == 0) { goto WRITE_INTO_REF_SEG_END; }

        size_t cnt_a = seg_idx;
        size_t cnt_b = ZETA_GetMinOf(cnt, avg_seg_size - seg_idx);
        size_t cnt_c = avg_seg_size - cnt_a - cnt_b;

        cnt -= cnt_b;

        if (avg_seg_size == seg->ref.size) {
            void* data = AllocateData_(cntr, NULL);

            ZETA_SeqCntr_Access(cntr->origin, seg->ref.beg, origin_cursor,
                                NULL);

            ZETA_SeqCntr_Read(cntr->origin, origin_cursor, cnt_a, data, NULL);

            Zeta_MemCopy(data + width * cnt_a, src, width * cnt_b);

            if (0 < cnt_c) {
                ZETA_SeqCntr_Access(cntr->origin, seg->ref.beg + cnt_a + cnt_b,
                                    origin_cursor, NULL);

                ZETA_SeqCntr_Read(cntr->origin, origin_cursor, cnt_c,
                                  data + width * (cnt_a + cnt_b), NULL);
            }

            SetNColor_(n, dat_color);

            seg->dat.data = data;
            seg->dat.offset = 0;
            seg->dat.size = avg_seg_size;

            seg_idx += cnt_b;

            src += width * cnt_b;

            if (seg_idx == avg_seg_size) {
                ZETA_DebugLogCurPos;

                Zeta_BinTree_SetSize(btn_opr, n, avg_seg_size);

                l_n = n;
                n = Zeta_BinTree_StepR(btn_opr, n);

                seg_idx = 0;
            }
        } else {
            Cntr_(Seg)* new_seg = AllocateDatSeg_(cntr);
            cntr->root = Zeta_RBTree_Insert(btn_opr, l_n, n, &new_seg->n);

            new_seg->dat.size = avg_seg_size;

            void* data = new_seg->dat.data;

            ZETA_SeqCntr_Access(cntr->origin, seg->ref.beg, origin_cursor,
                                NULL);

            ZETA_SeqCntr_Read(cntr->origin, origin_cursor, cnt_a, data, NULL);

            Zeta_MemCopy(data + width * cnt_a, src, width * cnt_b);

            if (0 < cnt_c) {
                ZETA_SeqCntr_Access(cntr->origin, seg->ref.beg + cnt_a + cnt_b,
                                    origin_cursor, NULL);

                ZETA_SeqCntr_Read(cntr->origin, origin_cursor, cnt_c,
                                  data + width * (cnt_a + cnt_b), NULL);
            }

            seg->ref.beg += avg_seg_size;
            seg->ref.size -= avg_seg_size;

            seg_idx += cnt_b;

            src += width * cnt_b;

            if (seg_idx < new_seg->dat.size) {
                ZETA_DebugLogCurPos;

                Zeta_BinTree_SetSize(btn_opr, n, seg->ref.size);

                n = &new_seg->n;

                ZETA_CheckAssert(cnt == 0);

            } else {
                ZETA_DebugLogCurPos;

                Zeta_BinTree_SetSize(btn_opr, &new_seg->n, new_seg->dat.size);

                l_n = &new_seg->n;
                seg_idx = 0;
            }
        }
    }

WRITE_INTO_REF_SEG_END:;

#endif

    while (0 < cnt) {
        seg = NToSeg_(n);

        if (GetNColor_(n) == dat_color) {
            ca.data = seg->dat.data;
            ca.offset = seg->dat.offset;
            ca.size = seg->dat.size;

            size_t cur_cnt = ZETA_GetMinOf(cnt, ca.size - seg_idx);

            Zeta_CircularArray_Cursor ca_cursor;
            Zeta_CircularArray_Access(&ca, seg_idx, &ca_cursor, NULL);
            Zeta_CircularArray_Write(&ca, &ca_cursor, cur_cnt, src, NULL);

            src += width * cur_cnt;

            seg_idx += cur_cnt;
            cnt -= cur_cnt;

            if (seg_idx == seg->dat.size) {
#if STAGING
                l_n = n;
#endif
                n = Zeta_BinTree_StepR(btn_opr, n);
                seg_idx = 0;
            }

            continue;
        }

#if STAGING
        void* r_n = NULL;

        size_t seg_size = seg->ref.size;

        if (seg_capacity < seg_size) {
            size_t split_cnt = GetAvgCnt_(seg_size);

            Cntr_(Seg)* new_seg = AllocateRefSeg_(cntr);
            cntr->root = Zeta_RBTree_Insert(btn_opr, l_n, n, &new_seg->n);

            new_seg->ref.beg = seg->ref.beg;
            new_seg->ref.size = split_cnt;

            seg->ref.beg += split_cnt;
            seg->ref.size -= split_cnt;

            r_n = n;

            seg = new_seg;
            n = &new_seg->n;

            seg_size = seg->ref.size;
        }

        Zeta_BinTree_SetSize(btn_opr, &seg->n, seg_size);

        size_t cur_cnt = ZETA_GetMinOf(cnt, seg_size);

        void* data = AllocateData_(cntr, NULL);

        Zeta_MemCopy(data, src, width * cur_cnt);

        src += width * cur_cnt;

        SetNColor_(n, dat_color);

        if (cur_cnt == seg_size) {
            seg->dat.data = data;
            seg->dat.offset = 0;
            seg->dat.size = seg_size;

            cnt -= cur_cnt;
            l_n = n;
            n = r_n == NULL ? Zeta_BinTree_StepR(btn_opr, n) : r_n;
            continue;
        }

        ZETA_SeqCntr_Access(cntr->origin, seg->ref.beg + cur_cnt, origin_cursor,
                            NULL);

        ZETA_SeqCntr_Read(cntr->origin, origin_cursor, seg_size - cur_cnt,
                          data + width * cur_cnt, NULL);

        seg->dat.data = data;
        seg->dat.offset = 0;
        seg->dat.size = seg_size;

        if (r_n != NULL) {
            Cntr_(Seg)* r_seg = NToSeg_(r_n);

            Zeta_BinTree_SetSize(btn_opr, r_n,
                                 GetNColor_(r_n) == ref_color
                                     ? r_seg->ref.size
                                     : r_seg->dat.size);
        }

        seg_idx = cur_cnt;

        break;
#endif
    }

    if (cntr->rb != n) {
        seg = NToSeg_(n);

        Zeta_BinTree_SetSize(btn_opr, n,
                             StagingThreeWay(GetNColor_(n) == ref_color,
                                             seg->ref.size, seg->dat.size));
    }

    if (dst_cursor == NULL) { return; }

    dst_cursor->cntr = cntr;
    dst_cursor->idx = end;
    dst_cursor->n = n;
    dst_cursor->seg_idx = seg_idx;

    if (cntr->rb == n || GetNColor_(n) == ref_color) {
        dst_cursor->ref = NULL;
        return;
    }

    seg = NToSeg_(n);

    ca.data = seg->dat.data;
    ca.offset = seg->dat.offset;
    ca.size = seg->dat.size;

    dst_cursor->ref = Zeta_CircularArray_Access(&ca, seg_idx, NULL, NULL);
}

void* Cntr_(PushL)(void* cntr_, size_t cnt, void* dst_cursor_) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    Cntr_(Cursor)* dst_cursor = dst_cursor_;

    Cntr_(Cursor) pos_cursor;
    Cntr_(PeekL)(cntr, &pos_cursor, NULL);

    Cntr_(Insert)(cntr, &pos_cursor, cnt);

    if (dst_cursor != NULL) {
        dst_cursor->cntr = cntr;
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->n = pos_cursor.n;
        dst_cursor->seg_idx = pos_cursor.seg_idx;
        dst_cursor->ref = pos_cursor.ref;
    }

    return pos_cursor.ref;
}

void* Cntr_(PushR)(void* cntr_, size_t cnt, void* dst_cursor_) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    Cntr_(Cursor)* dst_cursor = dst_cursor_;

    Cntr_(Cursor) pos_cursor;
    Cntr_(GetRBCursor)(cntr, &pos_cursor);

    Cntr_(Insert)(cntr, &pos_cursor, cnt);

    if (dst_cursor != NULL) {
        dst_cursor->cntr = cntr;
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->n = pos_cursor.n;
        dst_cursor->seg_idx = pos_cursor.seg_idx;
        dst_cursor->ref = pos_cursor.ref;
    }

    return pos_cursor.ref;
}

void* Cntr_(Insert)(void* cntr_, void* pos_cursor_, size_t cnt) {
    Cntr* cntr = cntr_;
    Cntr_(Cursor)* pos_cursor = pos_cursor_;
    Cntr_(Cursor_Check)(cntr, pos_cursor);

    if (cnt == 0) { return pos_cursor->ref; }

    Zeta_BinTreeNodeOperator const* btn_opr = TreeNodeOpr;

    size_t width = GetWidth_(cntr);
    size_t seg_capacity = cntr->seg_capacity;

    void* m_n = pos_cursor->n;
    size_t seg_idx = pos_cursor->seg_idx;

    ZETA_DebugAssert(cntr->lb != m_n);

    unsigned long long rand_seed = ZETA_PtrToAddr(m_n);

    void* l_n;
    void* r_n;

    Cntr_(Seg) * l_seg;
    Cntr_(Seg) * m_seg;
    Cntr_(Seg) * r_seg;

    size_t l_vacant;
    size_t m_vacant;
    size_t r_vacant;

    Zeta_CircularArray l_ca;
    l_ca.width = width;
    l_ca.capacity = seg_capacity;

    Zeta_CircularArray m_ca;
    m_ca.width = width;
    m_ca.capacity = seg_capacity;

    Zeta_CircularArray r_ca;
    r_ca.width = width;
    r_ca.capacity = seg_capacity;

    Zeta_CircularArray_Cursor ca_cursor;

    if (seg_idx == 0) {
        l_n = Zeta_BinTree_StepL(btn_opr, m_n);

        r_n = m_n;

        m_n = NULL;
        m_seg = NULL;

        if (cntr->lb == l_n) {
            l_ca.size = 0;
            l_vacant = 0;
        } else {
            l_seg = NToSeg_(l_n);
            SetSegState(l_seg, &l_ca, &l_vacant);
        }

        if (cntr->rb == r_n) {
            r_ca.size = 0;
            r_vacant = 0;
        } else {
            r_seg = NToSeg_(r_n);
            SetSegState(r_seg, &r_ca, &r_vacant);
        }
    } else {
        m_seg = NToSeg_(m_n);
        SetSegState(m_seg, &m_ca, &m_vacant);

        if (cnt <= m_vacant) {
            void* ref;

#if STAGING
            if (GetNColor_(m_n) == ref_color) {
                m_ca.data = AllocateData_(cntr, NULL);
                m_ca.offset = 0;
                m_ca.size = 0;

                RefShoveL_(cntr, &m_ca, m_seg, seg_idx, cnt,
                           m_seg->ref.size + cnt);

                SetNColor_(&m_seg->n, dat_color);

                m_seg->dat.data = m_ca.data;

                ref = Zeta_CircularArray_Access(&m_ca, seg_idx, NULL, NULL);
            } else
#endif
            {
                Zeta_CircularArray_Access(&m_ca, seg_idx, &ca_cursor, NULL);
                ref = Zeta_CircularArray_Insert(&m_ca, &ca_cursor, cnt);
            }

            m_seg->dat.offset = m_ca.offset;
            m_seg->dat.size = m_ca.size;
            Zeta_BinTree_SetSize(btn_opr, m_n, m_ca.size);

            pos_cursor->n = m_n;
            pos_cursor->seg_idx = seg_idx;
            pos_cursor->ref = ref;

            return ref;
        }

        l_n = Zeta_BinTree_StepL(btn_opr, m_n);
        r_n = Zeta_BinTree_StepR(btn_opr, m_n);

        if (cntr->lb == l_n) {
            l_ca.size = 0;
            l_vacant = 0;
        } else {
            l_seg = NToSeg_(l_n);
            SetSegState(l_seg, &l_ca, &l_vacant);
        }

        if (cntr->rb == r_n) {
            r_ca.size = 0;
            r_vacant = 0;
        } else {
            r_seg = NToSeg_(r_n);
            SetSegState(r_seg, &r_ca, &r_vacant);
        }

        size_t ml_size = seg_idx;
        size_t mr_size = m_ca.size - seg_idx;

        size_t new_l_m_size = l_ca.size + m_ca.size + cnt;
        size_t new_m_r_size = m_ca.size + r_ca.size + cnt;

        size_t l_m_vacant = l_vacant + m_vacant;
        size_t m_r_vacant = m_vacant + r_vacant;

        bool_t l_m_ok = new_l_m_size <= seg_capacity * 2 && cnt <= l_m_vacant;

        bool_t m_r_ok = new_m_r_size <= seg_capacity * 2 && cnt <= m_r_vacant;

        if (!l_m_ok && !m_r_ok) { goto SPLIT; }

        int shove_dir = Zeta_Choose2(
            l_m_ok && (!m_r_ok || m_r_vacant <= l_m_vacant),
            m_r_ok && (!l_m_ok || l_m_vacant <= m_r_vacant), &rand_seed);

        if (shove_dir == 0) {
#if STAGING
            if (GetNColor_(l_n) == ref_color) {
                TransRefSegToDatSeg_(cntr, l_seg);
                l_ca.data = l_seg->dat.data;
                l_ca.offset = l_seg->dat.offset;
            }
#endif

            size_t ret_seg_idx = l_ca.size + ml_size;

            size_t new_l_size = (new_l_m_size + (m_ca.size <= l_ca.size)) / 2;

#if STAGING
            if (GetNColor_(m_n) == ref_color) {
                RefShoveL_(cntr, &l_ca, m_seg, ml_size, cnt,
                           new_l_size - l_ca.size);

                if (GetNColor_(m_n) == ref_color) {
                    m_ca.size = m_seg->ref.size;
                } else {
                    m_ca.data = m_seg->dat.data;
                    m_ca.offset = m_seg->dat.offset;
                    m_ca.size = m_seg->dat.size;
                }
            } else
#endif
            {
                SegShoveL(&l_ca, &m_ca, ml_size, cnt, new_l_size - l_ca.size);

                m_seg->dat.offset = m_ca.offset;
                m_seg->dat.size = m_ca.size;
            }

            Zeta_BinTree_SetSize(btn_opr, m_n, m_ca.size);

            l_seg->dat.offset = l_ca.offset;
            l_seg->dat.size = l_ca.size;
            Zeta_BinTree_SetSize(btn_opr, l_n, l_ca.size);

            if (ret_seg_idx < l_ca.size) {
                pos_cursor->n = l_n;
                pos_cursor->seg_idx = ret_seg_idx;
                pos_cursor->ref =
                    Zeta_CircularArray_Access(&l_ca, ret_seg_idx, NULL, NULL);
            } else {
                ret_seg_idx -= l_ca.size;

                pos_cursor->n = m_n;
                pos_cursor->seg_idx = ret_seg_idx;
                pos_cursor->ref = StagingThreeWay(
                    GetNColor_(m_n) == ref_color, NULL,
                    Zeta_CircularArray_Access(&m_ca, ret_seg_idx, NULL, NULL));
            }
        } else {
#if STAGING
            if (GetNColor_(r_n) == ref_color) {
                TransRefSegToDatSeg_(cntr, r_seg);
                r_ca.data = r_seg->dat.data;
                r_ca.offset = r_seg->dat.offset;
            }
#endif

            size_t ret_seg_idx = seg_idx;

            size_t new_r_size = (new_m_r_size + (m_ca.size <= r_ca.size)) / 2;

#if STAGING
            if (GetNColor_(m_n) == ref_color) {
                RefShoveR_(cntr, m_seg, &r_ca, mr_size, cnt,
                           new_r_size - r_ca.size);

                if (GetNColor_(m_n) == ref_color) {
                    m_ca.size = m_seg->ref.size;
                } else {
                    m_ca.data = m_seg->dat.data;
                    m_ca.offset = m_seg->dat.offset;
                    m_ca.size = m_seg->dat.size;
                }
            } else
#endif
            {
                SegShoveR(&m_ca, &r_ca, mr_size, cnt, new_r_size - r_ca.size);

                m_seg->dat.offset = m_ca.offset;
                m_seg->dat.size = m_ca.size;
            }

            Zeta_BinTree_SetSize(btn_opr, m_n, m_ca.size);

            r_seg->dat.offset = r_ca.offset;
            r_seg->dat.size = r_ca.size;
            Zeta_BinTree_SetSize(btn_opr, r_n, r_ca.size);

            if (ret_seg_idx < m_ca.size) {
                pos_cursor->n = m_n;
                pos_cursor->seg_idx = ret_seg_idx;
                pos_cursor->ref = StagingThreeWay(
                    GetNColor_(m_n) == ref_color, NULL,
                    Zeta_CircularArray_Access(&m_ca, ret_seg_idx, NULL, NULL));
            } else {
                ret_seg_idx -= m_ca.size;

                pos_cursor->n = r_n;
                pos_cursor->seg_idx = ret_seg_idx;
                pos_cursor->ref =
                    Zeta_CircularArray_Access(&r_ca, ret_seg_idx, NULL, NULL);
            }
        }

        return pos_cursor->ref;

    SPLIT:;

        if (ml_size <= l_vacant) {
#if STAGING
            if (GetNColor_(l_n) == ref_color) {
                TransRefSegToDatSeg_(cntr, l_seg);

                l_ca.data = l_seg->dat.data;
                l_ca.offset = l_seg->dat.offset;
            }
#endif

#if STAGING
            if (GetNColor_(m_n) == ref_color) {
                PushRefR_(cntr, &l_ca, m_seg->ref.beg, ml_size);

                m_seg->ref.beg += ml_size;
                m_ca.size -= ml_size;
            } else
#endif
            {
                SegShoveL(&l_ca, &m_ca, 0, 0, ml_size);
            }

            ml_size = 0;
            l_vacant = seg_capacity - l_ca.size;
        }

        if (mr_size <= r_vacant) {
#if STAGING
            if (GetNColor_(r_n) == ref_color) {
                TransRefSegToDatSeg_(cntr, r_seg);

                r_ca.data = r_seg->dat.data;
                r_ca.offset = r_seg->dat.offset;
            }
#endif

#if STAGING
            if (GetNColor_(m_n) == ref_color) {
                PushRefL_(cntr, &r_ca, m_seg->ref.beg + ml_size, mr_size);

                m_ca.size -= mr_size;
            } else
#endif
            {
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
#if STAGING
            if (GetNColor_(m_n) == ref_color) {
                Cntr_(Seg)* new_l_seg = AllocateRefSeg_(cntr);
                cntr->root =
                    Zeta_RBTree_Insert(btn_opr, l_n, m_n, &new_l_seg->n);

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
            } else
#endif
            {
                Cntr_(Seg)* new_l_seg = AllocateDatSeg_(cntr);
                cntr->root =
                    Zeta_RBTree_Insert(btn_opr, l_n, m_n, &new_l_seg->n);

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
#if STAGING
            if (GetNColor_(m_n) == ref_color) {
                Cntr_(Seg)* new_r_seg = AllocateRefSeg_(cntr);
                cntr->root =
                    Zeta_RBTree_Insert(btn_opr, m_n, r_n, &new_r_seg->n);

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
            } else
#endif
            {
                Cntr_(Seg)* new_r_seg = AllocateDatSeg_(cntr);
                cntr->root =
                    Zeta_RBTree_Insert(btn_opr, m_n, r_n, &new_r_seg->n);

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

#if STAGING
    bool_t l_is_ref = cntr->lb != l_n && GetNColor_(l_n) == ref_color;
    bool_t r_is_ref = cntr->rb != r_n && GetNColor_(r_n) == ref_color;
#endif

    bool_t l_shove;
    bool_t r_shove;

    {
        size_t res_size =
            seg_capacity - 1 - (cnt + seg_capacity - 1) % seg_capacity;

        if (l_ca.size + r_ca.size <= res_size) {
            l_shove = TRUE;
            r_shove = TRUE;
        } else if (Zeta_Choose2(l_ca.size <= r_ca.size, r_ca.size <= l_ca.size,
                                &rand_seed) == 0) {
            l_shove = l_ca.size <= res_size;
            r_shove = FALSE;
        } else {
            l_shove = FALSE;
            r_shove = r_ca.size <= res_size;
        }

        if (l_ca.size == 0) { l_shove = FALSE; }
        if (r_ca.size == 0) { r_shove = FALSE; }
    }

#if STAGING
    if (l_is_ref && l_shove) {
        l_seg->ref.size = l_ca.size;

        TransRefSegToDatSeg_(cntr, l_seg);

        l_ca.data = l_seg->dat.data;
        l_ca.offset = 0;
    }
#endif

#if STAGING
    if (r_is_ref && r_shove) {
        r_seg->ref.size = r_ca.size;

        TransRefSegToDatSeg_(cntr, r_seg);

        r_ca.data = r_seg->dat.data;
        r_ca.offset = 0;
    }
#endif

    size_t total_size =
        (l_shove ? l_ca.size : 0) + cnt + (r_shove ? r_ca.size : 0);

    size_t ret_seg_idx = l_ca.size;

    if (l_shove && r_shove && total_size <= seg_capacity) {
        if (Zeta_Choose2(r_ca.size <= l_ca.size, l_ca.size <= r_ca.size,
                         &rand_seed) == 0) {
            SegShoveL(&l_ca, &r_ca, 0, cnt, cnt + r_ca.size);

            EraseSeg_(cntr, r_seg);

            l_seg->dat.offset = l_ca.offset;
            l_seg->dat.size = l_ca.size;
            Zeta_BinTree_SetSize(btn_opr, l_n, l_ca.size);

            pos_cursor->n = l_n;
            pos_cursor->seg_idx = ret_seg_idx;
            pos_cursor->ref =
                Zeta_CircularArray_Access(&l_ca, ret_seg_idx, NULL, NULL);
        } else {
            SegShoveR(&l_ca, &r_ca, 0, cnt, l_ca.size + cnt);

            EraseSeg_(cntr, l_seg);

            r_seg->dat.offset = r_ca.offset;
            r_seg->dat.size = r_ca.size;
            Zeta_BinTree_SetSize(btn_opr, r_n, r_ca.size);

            pos_cursor->n = r_n;
            pos_cursor->seg_idx = ret_seg_idx;
            pos_cursor->ref =
                Zeta_CircularArray_Access(&r_ca, ret_seg_idx, NULL, NULL);
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

    if (cntr->lb != l_n) {
#if STAGING
        if (GetNColor_(l_n) == ref_color) {
            l_seg->ref.size = l_ca.size;
        } else
#endif
        {
            l_seg->dat.offset = l_ca.offset;
            l_seg->dat.size = l_ca.size;
        }

        Zeta_BinTree_SetSize(btn_opr, l_n, l_ca.size);
    }

    if (cntr->rb != r_n) {
#if STAGING
        if (GetNColor_(r_n) == ref_color) {
            r_seg->ref.size = r_ca.size;
        } else
#endif
        {
            r_seg->dat.offset = r_ca.offset;
            r_seg->dat.size = r_ca.size;
        }

        Zeta_BinTree_SetSize(btn_opr, r_n, r_ca.size);
    }

    if (0 < total_size && m_n != NULL) {
        size_t cur_size = GetAvgCnt_(total_size);
        total_size -= cur_size;

#if STAGING
        if (GetNColor_(m_n) == ref_color) {
            SetNColor_(m_n, dat_color);
            m_seg->dat.data = AllocateData_(cntr, NULL);
        }
#endif

        m_seg->dat.offset = 0;
        m_seg->dat.size = cur_size;
        Zeta_BinTree_SetSize(btn_opr, m_n, cur_size);

        r_n = m_n;
        r_seg = m_seg;

        m_n = NULL;
        m_seg = NULL;
    }

    while (0 < total_size) {
        size_t cur_size = GetAvgCnt_(total_size);
        total_size -= cur_size;

        Cntr_(Seg)* new_r_seg = AllocateDatSeg_(cntr);

        cntr->root = Zeta_RBTree_Insert(btn_opr, l_n, r_n, &new_r_seg->n);

        new_r_seg->dat.size = cur_size;
        Zeta_BinTree_SetSize(btn_opr, &new_r_seg->n, cur_size);

        r_n = &new_r_seg->n;
        r_seg = new_r_seg;
    }

    if (m_n != NULL) { EraseSeg_(cntr, m_seg); }

    if (ret_seg_idx < l_ca.size) {
        pos_cursor->n = l_n;
        pos_cursor->seg_idx = ret_seg_idx;

#if STAGING
        if (GetNColor_(l_n) == ref_color) {
            pos_cursor->ref = NULL;
        } else
#endif
        {
            l_ca.data = l_seg->dat.data;
            l_ca.offset = l_seg->dat.offset;
            l_ca.size = l_seg->dat.size;

            pos_cursor->ref =
                Zeta_CircularArray_Access(&l_ca, ret_seg_idx, NULL, NULL);
        }

        return pos_cursor->ref;
    }

    ret_seg_idx -= l_ca.size;

    pos_cursor->n = r_n;
    pos_cursor->seg_idx = ret_seg_idx;

#if STAGING
    if (GetNColor_(r_n) == ref_color) {
        pos_cursor->ref = NULL;
    } else
#endif
    {
        r_ca.data = r_seg->dat.data;
        r_ca.offset = r_seg->dat.offset;
        r_ca.size = r_seg->dat.size;

        pos_cursor->ref =
            Zeta_CircularArray_Access(&r_ca, ret_seg_idx, NULL, NULL);
    }

    return pos_cursor->ref;
}

void Cntr_(PopL)(void* cntr, size_t cnt) {
    Cntr_(Cursor) pos_cursor;
    Cntr_(PeekL)(cntr, &pos_cursor, NULL);

    Cntr_(Erase)(cntr, &pos_cursor, cnt);
}

void Cntr_(PopR)(void* cntr, size_t cnt) {
    size_t size = Cntr_(GetSize)(cntr);

    ZETA_DebugAssert(cnt <= size);

    Cntr_(Cursor) pos_cursor;
    Cntr_(Access)(cntr, size - cnt, &pos_cursor, NULL);

    Cntr_(Erase)(cntr, &pos_cursor, cnt);
}

void Cntr_(Erase)(void* cntr_, void* pos_cursor_, size_t cnt) {
    Cntr* cntr = cntr_;
    Cntr_(Cursor)* pos_cursor = pos_cursor_;
    Cntr_(Cursor_Check)(cntr, pos_cursor);

    if (cnt == 0) { return; }

    Zeta_BinTreeNodeOperator const* btn_opr = TreeNodeOpr;

    size_t width = GetWidth_(cntr);
    size_t seg_capacity = cntr->seg_capacity;
    size_t size = Cntr_(GetSize)(cntr);

    ZETA_DebugAssert(pos_cursor->idx <= size);
    ZETA_DebugAssert(pos_cursor->idx + cnt <= size);

    void* m_n = pos_cursor->n;
    size_t seg_idx = pos_cursor->seg_idx;

    void* l_n = Zeta_BinTree_StepL(btn_opr, m_n);

    Cntr_(Seg)* m_seg = NToSeg_(m_n);

    Cntr_(Seg)* first_seg = m_seg;
    size_t first_seg_idx = seg_idx;

    Cntr_(Seg)* last_seg = m_n;

    Zeta_CircularArray m_ca;
    m_ca.width = width;
    m_ca.capacity = seg_capacity;

#if STAGING
    if (GetNColor_(m_n) == ref_color && 0 < seg_idx) {
        ZETA_DebugLogCurPos;

        m_seg = NToSeg_(m_n);

        size_t ref_beg = m_seg->ref.beg;
        size_t ref_size = m_seg->ref.size;

        if (ref_size - seg_idx <= cnt) {
            ZETA_DebugLogCurPos;

            m_seg->ref.size = seg_idx;
            cnt -= ref_size - seg_idx;
            m_n = Zeta_BinTree_StepR(btn_opr, m_n);
        } else {
            ZETA_DebugLogCurPos;

            Cntr_(Seg)* pre_m_seg = AllocateRefSeg_(cntr);
            cntr->root =
                Zeta_RBTree_Insert(btn_opr, l_n, &m_seg->n, &pre_m_seg->n);

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
#endif

    while (0 < cnt) {
        m_seg = NToSeg_(m_n);
        last_seg = m_seg;

#if STAGING
        if (GetNColor_(m_n) == ref_color) {
            if (m_seg->ref.size <= cnt) {
                cnt -= m_seg->ref.size;
                void* nxt_m_n = Zeta_BinTree_StepR(btn_opr, m_n);
                EraseSeg_(cntr, m_seg);
                m_n = nxt_m_n;

                continue;
            }

            m_seg->ref.beg += cnt;
            m_seg->ref.size -= cnt;

            break;
        }
#endif

        if (seg_idx == 0 && m_seg->dat.size <= cnt) {
            cnt -= m_seg->dat.size;
            void* nxt_m_n = Zeta_BinTree_StepR(btn_opr, m_n);
            EraseSeg_(cntr, m_seg);
            m_n = nxt_m_n;

            continue;
        }

        size_t cur_cnt = ZETA_GetMinOf(cnt, m_seg->dat.size - seg_idx);

        m_ca.data = m_seg->dat.data;
        m_ca.offset = m_seg->dat.offset;
        m_ca.size = m_seg->dat.size;

        Zeta_CircularArray_Cursor ca_cursor;
        Zeta_CircularArray_Access(&m_ca, seg_idx, &ca_cursor, NULL);
        Zeta_CircularArray_Erase(&m_ca, &ca_cursor, cur_cnt);

        m_seg->dat.offset = m_ca.offset;
        m_seg->dat.size = m_ca.size;

        cnt -= cur_cnt;

        if (seg_idx == m_seg->dat.size) {
            m_n = Zeta_BinTree_StepR(btn_opr, m_n);
            seg_idx = 0;
        }
    }

    void* ret_n = m_n;
    size_t ret_seg_idx = seg_idx;

    unsigned long long rand_seed = ZETA_PtrToAddr(m_n);

    bool_t first_exist = first_seg_idx != 0 || &first_seg->n == m_n;
    bool_t last_exist = &last_seg->n == m_n;

    Cntr_(Seg) * a_seg;
    Cntr_(Seg) * b_seg;
    Cntr_(Seg) * c_seg;
    Cntr_(Seg) * d_seg;

    size_t a_size;
    size_t b_size;
    size_t c_size;
    size_t d_size;

    size_t a_vacant;
    size_t b_vacant;
    size_t c_vacant;
    size_t d_vacant;

    void* r_n = last_exist ? Zeta_BinTree_StepR(btn_opr, m_n) : m_n;

    {
        Cntr_(Seg) * segs_buffer[4] = { NULL, NULL, NULL, NULL };
        size_t segs_buffer_i = 0;

        if (cntr->lb != l_n) { segs_buffer[segs_buffer_i++] = NToSeg_(l_n); }

        if (first_exist) { segs_buffer[segs_buffer_i++] = first_seg; }

        if (last_exist && first_seg != last_seg) {
            segs_buffer[segs_buffer_i++] = last_seg;
        }

        if (cntr->rb != r_n) { segs_buffer[segs_buffer_i++] = NToSeg_(r_n); }

        a_seg = segs_buffer[0];
        b_seg = segs_buffer[1];
        c_seg = segs_buffer[2];
        d_seg = segs_buffer[3];
    }

    if (a_seg == NULL) {
    }
#if STAGING
    else if (GetNColor_(&a_seg->n) == ref_color) {
        a_size = a_seg->ref.size;
        a_vacant = seg_capacity - ZETA_GetMinOf(a_size, seg_capacity);
    }
#endif
    else {
        a_size = a_seg->dat.size;
        a_vacant = seg_capacity - a_size;
    }

    if (b_seg == NULL) {
    }
#if STAGING
    else if (GetNColor_(&b_seg->n) == ref_color) {
        b_size = b_seg->ref.size;
        b_vacant = seg_capacity - ZETA_GetMinOf(b_size, seg_capacity);
    }
#endif
    else {
        b_size = b_seg->dat.size;
        b_vacant = seg_capacity - b_size;
    }

    if (c_seg == NULL) {
    }
#if STAGING
    else if (GetNColor_(&c_seg->n) == ref_color) {
        c_size = c_seg->ref.size;
        c_vacant = seg_capacity - ZETA_GetMinOf(c_size, seg_capacity);
    }
#endif
    else {
        c_size = c_seg->dat.size;
        c_vacant = seg_capacity - c_size;
    }

    if (d_seg == NULL) {
    }
#if STAGING
    else if (GetNColor_(&d_seg->n) == ref_color) {
        d_size = d_seg->ref.size;
        d_vacant = seg_capacity - ZETA_GetMinOf(d_size, seg_capacity);
    }
#endif
    else {
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

#if STAGING
    if (GetNColor_(&a_seg->n) == ref_color &&
        GetNColor_(&b_seg->n) == ref_color &&
        a_seg->ref.beg + a_seg->ref.size == b_seg->ref.beg) {
        a_seg->ref.size += b_seg->ref.size;

        EraseSeg_(cntr, b_seg);
        b_seg = NULL;

        if (c_seg == NULL) { goto UPDATE; }

        b_seg = c_seg;
        c_seg = d_seg;
        d_seg = NULL;
    }
#endif

    if (c_seg == NULL) { goto MERGE_2; }

    if (ret_n == &c_seg->n) {
        ret_n = NULL;
        ret_seg_idx += a_size + b_size;
    }

#if STAGING
    if (GetNColor_(&b_seg->n) == ref_color &&
        GetNColor_(&c_seg->n) == ref_color &&
        b_seg->ref.beg + b_seg->ref.size == c_seg->ref.beg) {
        b_seg->ref.size += c_seg->ref.size;

        EraseSeg_(cntr, c_seg);
        c_seg = NULL;

        if (d_seg == NULL) { goto MERGE_2; }

        c_seg = d_seg;
        d_seg = NULL;

        goto MERGE_3;
    }
#endif

    if (d_seg == NULL) { goto MERGE_3; }

    if (ret_n == &d_seg->n) {
        ret_n = NULL;
        ret_seg_idx += a_size + b_size + c_size;
    }

#if STAGING
    if (GetNColor_(&c_seg->n) == ref_color &&
        GetNColor_(&d_seg->n) == ref_color &&
        c_seg->ref.beg + c_seg->ref.size == d_seg->ref.beg) {
        c_seg->ref.size += d_seg->ref.size;

        EraseSeg_(cntr, d_seg);
        d_seg = NULL;

        goto MERGE_3;
    }
#endif

    goto MERGE_4;

MERGE_2: {
    size_t ab_vacant = a_vacant + b_vacant;

    if (seg_capacity <= ab_vacant) { Merge2_(cntr, width, a_seg, b_seg); }

    goto UPDATE;
}

MERGE_3: {
    size_t ab_vacant = a_vacant + b_vacant;
    size_t bc_vacant = b_vacant + c_vacant;

    size_t max_vacant = ZETA_GetMaxOf(ab_vacant, bc_vacant);

    if (seg_capacity < max_vacant) {
        if (Zeta_Choose2(bc_vacant <= ab_vacant, ab_vacant <= bc_vacant,
                         &rand_seed) == 0) {
            Merge2_(cntr, width, a_seg, b_seg);
        } else {
            Merge2_(cntr, width, b_seg, c_seg);
        }
    }

    goto UPDATE;
}

MERGE_4: {
    size_t ab_vacant = a_vacant + b_vacant;
    size_t bc_vacant = b_vacant + c_vacant;
    size_t cd_vacant = c_vacant + d_vacant;

    if (seg_capacity <= ab_vacant && seg_capacity <= cd_vacant) {
        Merge2_(cntr, width, a_seg, b_seg);
        Merge2_(cntr, width, c_seg, d_seg);
    } else {
        size_t max_vacant =
            ZETA_GetMaxOf(ZETA_GetMaxOf(ab_vacant, bc_vacant), cd_vacant);

        if (seg_capacity <= max_vacant) {
            switch (Zeta_Choose3(ab_vacant == max_vacant,
                                 bc_vacant == max_vacant,
                                 cd_vacant == max_vacant, &rand_seed)) {
                case 0: Merge2_(cntr, width, a_seg, b_seg); break;
                case 1: Merge2_(cntr, width, b_seg, c_seg); break;
                case 2: Merge2_(cntr, width, c_seg, d_seg); break;
            }
        }
    }

    goto UPDATE;
}

UPDATE: {
    if (a_seg == NULL) { goto UPDATE_END; }

    a_size = StagingThreeWay(GetNColor_(&a_seg->n) == ref_color,
                             a_seg->ref.size, a_seg->dat.size);

    if (a_size == 0) {
        EraseSeg_(cntr, a_seg);
    } else {
        Zeta_BinTree_SetSize(btn_opr, &a_seg->n, a_size);
    }

    if (ret_n != NULL) {
    } else if (ret_seg_idx < a_size) {
        ret_n = &a_seg->n;
    } else {
        ret_seg_idx -= a_size;
    }

    if (b_seg == NULL) { goto UPDATE_END; }

    b_size = StagingThreeWay(GetNColor_(&b_seg->n) == ref_color,
                             b_seg->ref.size, b_seg->dat.size);

    if (b_size == 0) {
        EraseSeg_(cntr, b_seg);
    } else {
        Zeta_BinTree_SetSize(btn_opr, &b_seg->n, b_size);
    }

    if (ret_n != NULL) {
    } else if (ret_seg_idx < b_size) {
        ret_n = &b_seg->n;
    } else {
        ret_seg_idx -= b_size;
    }

    if (c_seg == NULL) { goto UPDATE_END; }

    c_size = StagingThreeWay(GetNColor_(&c_seg->n) == ref_color,
                             c_seg->ref.size, c_seg->dat.size);

    if (c_size == 0) {
        EraseSeg_(cntr, c_seg);
    } else {
        Zeta_BinTree_SetSize(btn_opr, &c_seg->n, c_size);
    }

    if (ret_n != NULL) {
    } else if (ret_seg_idx < c_size) {
        ret_n = &c_seg->n;
    } else {
        ret_seg_idx -= c_size;
    }

    if (d_seg == NULL) { goto UPDATE_END; }

    d_size = StagingThreeWay(GetNColor_(&d_seg->n) == ref_color,
                             d_seg->ref.size, d_seg->dat.size);

    if (d_size == 0) {
        EraseSeg_(cntr, d_seg);
    } else {
        Zeta_BinTree_SetSize(btn_opr, &d_seg->n, d_size);
    }

    if (ret_n == NULL) { ret_n = &d_seg->n; }
}

UPDATE_END:;

    pos_cursor->n = ret_n;
    pos_cursor->seg_idx = ret_seg_idx;

    if (cntr->rb == pos_cursor->n
#if STAGING
        || GetNColor_(pos_cursor->n) == ref_color
#endif
    ) {
        pos_cursor->ref = NULL;
        return;
    }

    Cntr_(Seg)* seg = NToSeg_(pos_cursor->n);

    m_ca.data = seg->dat.data;
    m_ca.offset = seg->dat.offset;
    m_ca.size = seg->dat.size;

    pos_cursor->ref =
        Zeta_CircularArray_Access(&m_ca, pos_cursor->seg_idx, NULL, NULL);
}

void Cntr_(EraseAll)(void* cntr_) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    EraseAllSeg_(cntr, cntr->root);

    InitTree_(cntr);
}

void Cntr_(Reset)(void* cntr_) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    EraseAllSeg_(cntr, cntr->root);

    InitTree_(cntr);
    RefOrigin_(cntr);
}

static void CollectAllResources_(Cntr* cntr, void* n,
                                 Zeta_PoolAllocator* dst_segs,
                                 Zeta_PoolAllocator* dst_datas) {
    void* nl = ZETA_Concat(TreeNode, _GetL)(NULL, n);
    void* nr = ZETA_Concat(TreeNode, _GetR)(NULL, n);

    if (nl != NULL) { CollectAllResources_(cntr, nl, dst_segs, dst_datas); }
    if (nr != NULL) { CollectAllResources_(cntr, nr, dst_segs, dst_datas); }

    if (cntr->lb == n || cntr->rb == n) { return; }

    Cntr_(Seg)* seg = NToSeg_(n);

    if (GetNColor_(n) == dat_color) {
        if (dst_datas == NULL) {
            ZETA_Allocator_Deallocate(cntr->data_allocator, seg->dat.data);
        } else {
            Zeta_PoolAllocator_Deallocate(dst_datas, seg->dat.data);
        }
    }

    Zeta_PoolAllocator_Deallocate(dst_segs, seg);
}

static inline void* CopySegs_(Cntr* cntr, Cntr* src_cntr, void* src_n,
                              Zeta_PoolAllocator* segs,
                              Zeta_PoolAllocator* datas) {
    Zeta_BinTreeNodeOperator const* btn_opr = TreeNodeOpr;

    void* src_nl = ZETA_Concat(TreeNode, _GetL)(NULL, src_n);
    void* src_nr = ZETA_Concat(TreeNode, _GetR)(NULL, src_n);

    void* nl =
        src_nl == NULL ? NULL : CopySegs_(cntr, src_cntr, src_nl, segs, datas);

    void* nr =
        src_nr == NULL ? NULL : CopySegs_(cntr, src_cntr, src_nr, segs, datas);

    void* n = NULL;

    if (src_cntr->lb == src_n) {
        n = cntr->lb;
        ZETA_Concat(TreeNode, _Init)(NULL, n);
        Zeta_BinTree_SetSize(btn_opr, n, 1);
    } else if (src_cntr->rb == src_n) {
        n = cntr->rb;
        ZETA_Concat(TreeNode, _Init)(NULL, n);
        Zeta_BinTree_SetSize(btn_opr, n, 1);
    } else {
        Cntr_(Seg)* seg = AllocateSeg_(cntr, segs);
        n = &seg->n;

        ZETA_Concat(TreeNode, _Init)(NULL, n);

        Cntr_(Seg)* src_seg = NToSeg_(src_n);

#if STAGING
        int color = GetNColor_(src_n);
        DirectlySetNColor_(n, color);

        if (color == ref_color) {
            seg->ref.beg = src_seg->ref.beg;
            seg->ref.size = src_seg->ref.size;

            Zeta_BinTree_SetSize(btn_opr, n, seg->ref.size);
        } else
#endif
        {
            seg->dat.data = AllocateData_(cntr, datas);
            seg->dat.offset = 0;
            seg->dat.size = src_seg->dat.size;

            Zeta_CircularArray ca;
            ca.data = src_seg->dat.data;
            ca.width = GetWidth_(cntr);
            ca.offset = src_seg->dat.offset;
            ca.size = src_seg->dat.size;
            ca.capacity = src_cntr->seg_capacity;

            Zeta_CircularArray_Cursor ca_cursor;

            Zeta_CircularArray_PeekL(&ca, &ca_cursor, NULL);

            Zeta_CircularArray_Read(&ca, &ca_cursor, ca.size, seg->dat.data,
                                    NULL);

            Zeta_BinTree_SetSize(btn_opr, n, ca.size);
        }
    }

    ZETA_Concat(TreeNode, _SetPColor)(
        NULL, n, ZETA_Concat(TreeNode, _GetPColor)(NULL, src_n));

    Zeta_BinTree_AttatchL(btn_opr, n, nl);
    Zeta_BinTree_AttatchR(btn_opr, n, nr);

    return n;
}

void Cntr_(Copy)(void* cntr_, void* src_cntr_) {
    Cntr* cntr = cntr_;
    Cntr* src_cntr = src_cntr_;

    CheckCntr_(cntr);
    CheckCntr_(src_cntr);

    if (cntr == src_cntr) { return; }

    size_t width = GetWidth_(cntr);
    size_t seg_capacity = cntr->seg_capacity;

    size_t src_width = GetWidth_(src_cntr);
    size_t src_seg_capacity = cntr->seg_capacity;

    bool_t same_data_size =
        width * seg_capacity == src_width * src_seg_capacity;

    Zeta_PoolAllocator segs;
    Zeta_PoolAllocator datas;

    Zeta_PoolAllocator_Init(&segs);
    Zeta_PoolAllocator_Init(&datas);

    CollectAllResources_(cntr, cntr->root, &segs,
                         same_data_size ? &datas : NULL);

#if STAGING
    cntr->origin = src_cntr->origin;
#endif

    cntr->seg_capacity = src_seg_capacity;

    cntr->root = CopySegs_(cntr, src_cntr, src_cntr->root, &segs,
                           same_data_size ? &datas : NULL);

    for (;;) {
        void* seg = Zeta_PoolAllocator_Allocate(&segs, 0);
        if (seg == NULL) { break; }
        ZETA_Allocator_Deallocate(cntr->seg_allocator, seg);
    }

    for (;;) {
        void* data = Zeta_PoolAllocator_Allocate(&datas, 0);
        if (data == NULL) { break; }
        ZETA_Allocator_Deallocate(cntr->data_allocator, data);
    }
}

#if STAGING

void Cntr_(Collapse)(void* cntr_) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    ZETA_DebugAssert(cntr->origin->GetWidth == Cntr_(GetWidth));

    Cntr* origin = cntr->origin->context;
    cntr->origin = origin->origin;

    size_t seg_capacity = cntr->seg_capacity;

    Zeta_BinTreeNodeOperator const* btn_opr = TreeNodeOpr;

    TreeNode* n = Zeta_BinTree_StepR(btn_opr, cntr->lb);

    Cntr_(Cursor) origin_cursor;
    Cntr_(Access)(origin, 0, &origin_cursor, NULL);

    unsigned long long rand_seed = ZETA_PtrToAddr(cntr);

    Zeta_CircularArray origin_ca;
    origin_ca.width = ZETA_SeqCntr_GetWidth(origin->origin);
    origin_ca.capacity = origin->seg_capacity;

    Zeta_CircularArray_Cursor origin_ca_cursor;

    while (n != cntr->rb) {
        Cntr_(Seg)* seg = NToSeg_(n);

        if (GetNColor_(n) == dat_color) { goto NEXT; }

        Cntr_(Cursor_AdvanceR)(origin, &origin_cursor,
                               seg->ref.beg - origin_cursor.idx);

        TreeNode* origin_n = origin_cursor.n;
        size_t origin_seg_idx = origin_cursor.seg_idx;

        Cntr_(Seg)* origin_seg = NToSeg_(origin_n);

        int origin_seg_color = GetNColor_(&origin_seg->n);

        size_t origin_seg_size = origin_seg_color == ref_color
                                     ? origin_seg->ref.size
                                     : origin_seg->dat.size;

        size_t origin_res_size = origin_seg_size - origin_seg_idx;

        if (origin_seg_color == ref_color) {
            if (seg->ref.size <= origin_res_size) {
                seg->ref.beg = origin_seg->ref.beg + origin_seg_idx;
                goto NEXT;
            }

            Cntr_(Seg)* new_ref_seg = AllocateRefSeg_(cntr);

            new_ref_seg->ref.beg = origin_seg->ref.beg + origin_seg_idx;
            new_ref_seg->ref.size = origin_res_size;

            Zeta_BinTree_SetSize(btn_opr, &new_ref_seg->n,
                                 new_ref_seg->ref.size);

            seg->ref.beg += origin_res_size;
            seg->ref.size -= origin_res_size;

            Zeta_BinTree_SetSize(btn_opr, seg, seg->ref.size);

            cntr->root = Zeta_RBTree_InsertL(btn_opr, n, &new_ref_seg->n);

            continue;
        }

        size_t cur_cnt =
            GetAvgCnt_(ZETA_GetMinOf(seg->ref.size, origin_res_size));

        if (cur_cnt == seg->ref.size) {
            SetNColor_(n, dat_color);
            seg->dat.data = AllocateData_(cntr, NULL);
            seg->dat.offset = 0;
            seg->dat.size = cur_cnt;
        } else {
            Cntr_(Seg)* new_dat_seg = AllocateDatSeg_(cntr);

            new_dat_seg->dat.size = cur_cnt;

            Zeta_BinTree_SetSize(btn_opr, &new_dat_seg->n, cur_cnt);

            seg->ref.beg += origin_res_size;
            seg->ref.size -= origin_res_size;

            Zeta_BinTree_SetSize(btn_opr, n, seg->ref.size);

            cntr->root = Zeta_RBTree_InsertL(btn_opr, n, &new_dat_seg->n);

            n = &new_dat_seg->n;
            seg = new_dat_seg;
        }

        origin_ca.data = origin_seg->dat.data;
        origin_ca.offset = origin_seg->dat.offset;
        origin_ca.size = origin_seg_size;

        Zeta_CircularArray_Access(&origin_ca, origin_seg_idx, &origin_ca_cursor,
                                  NULL);

        Zeta_CircularArray_Read(&origin_ca, &origin_ca_cursor, cur_cnt,
                                seg->dat.data, NULL);

    NEXT: { n = Zeta_BinTree_StepR(btn_opr, n); }
    }
}

#endif

#if STAGING

ZETA_DeclareStruct(WBSeg);

struct WBSeg {
    size_t beg;
    size_t size;

    size_t dst_idx;
    size_t acc_ref;

    void* data;
    unsigned short offset;
};

#endif

#if STAGING

static inline WBSeg* ToWBSeg_(Cntr* cntr, TreeNode* n, size_t ref_offset,
                              WBSeg* dst) {
    while (n != NULL) {
        TreeNode* nl = ZETA_Concat(TreeNode, _GetL)(NULL, n);
        TreeNode* nr = ZETA_Concat(TreeNode, _GetR)(NULL, n);

        dst = ToWBSeg_(cntr, nl, ref_offset, dst);

        if (cntr->lb == n || cntr->rb == n) {
            n = nr;
            continue;
        }

        Cntr_(Seg)* seg = NToSeg_(n);

        if (GetNColor_(n) == ref_color) {
            dst->beg = ref_offset + seg->ref.beg;
            dst->size = seg->ref.size;
        } else {
            dst->beg = ZETA_RangeMaxOf(size_t);
            dst->size = seg->dat.size;
            dst->data = seg->dat.data;
            dst->offset = seg->dat.offset;
        }

        ZETA_Allocator_Deallocate(cntr->seg_allocator, seg);

        WBSeg* prv = dst - 1;

        dst->dst_idx = prv->dst_idx + prv->size;

        dst->acc_ref = prv->beg == ZETA_RangeMaxOf(size_t)
                           ? prv->acc_ref
                           : prv->beg + prv->size;

        ++dst;

        n = nr;
    }

    return dst;
}

#endif

#if STAGING

static inline void WriteWBSeg_(Cntr* cntr, Zeta_SeqCntr* ca_seq_cntr,
                               WBSeg* wb_segs, size_t segs_cnt) {
    Zeta_SeqCntr* origin = cntr->origin;

    for (;;) {
        if (segs_cnt == 1) {
            if (wb_segs->beg == ZETA_RangeMaxOf(size_t)) {
                Zeta_CircularArray* ca = ca_seq_cntr->context;
                ca->data = wb_segs->data;
                ca->offset = wb_segs->offset;
                ca->size = wb_segs->size;

                Zeta_SeqCntr_RangeAssign(origin, ca_seq_cntr, wb_segs->dst_idx,
                                         0, wb_segs->size);

                ZETA_Allocator_Deallocate(cntr->data_allocator, ca->data);
            } else {
                Zeta_SeqCntr_RangeAssign(origin, origin, wb_segs->dst_idx,
                                         wb_segs->beg, wb_segs->size);
            }

            return;
        }

        size_t half_segs_cnt = segs_cnt / 2;

        WBSeg* mid_wb_segs = wb_segs + half_segs_cnt;

        size_t dst_pivot = mid_wb_segs->dst_idx;
        size_t src_pivot = mid_wb_segs->acc_ref;

        if (dst_pivot <= src_pivot) {
            WriteWBSeg_(cntr, ca_seq_cntr, wb_segs, half_segs_cnt);
            wb_segs = mid_wb_segs;
            segs_cnt -= half_segs_cnt;
        } else {
            WriteWBSeg_(cntr, ca_seq_cntr, mid_wb_segs,
                        segs_cnt - half_segs_cnt);
            segs_cnt = half_segs_cnt;
        }
    }
}

#endif

#if STAGING

ZETA_DeclareStruct(OffsetPair);

struct OffsetPair {
    size_t offset;
    size_t cnt;
};

static unsigned long long OffsetPairHash(void* context, void const* a,
                                         unsigned long long salt) {
    ZETA_Unused(context);
    return Zeta_ULLHash(((OffsetPair*)a)->offset, salt);
}

static unsigned long long OffsetHash(void* context, void const* a,
                                     unsigned long long salt) {
    ZETA_Unused(context);
    return Zeta_ULLHash(*(unsigned long long*)a, salt);
}

static int OffsetPairCompare(void* context, void const* a, void const* b) {
    ZETA_Unused(context);

    return ZETA_ThreeWayCompare(((OffsetPair*)a)->offset,
                                ((OffsetPair*)b)->offset);
}

static int OffsetOffsetPairCompare(void* context, void const* a,
                                   void const* b) {
    ZETA_Unused(context);

    return ZETA_ThreeWayCompare(*(unsigned long long*)a,
                                ((OffsetPair*)b)->offset);
}

#endif

#if STAGING

static size_t RecordOffset_(Cntr* cntr, TreeNode* n, size_t dst_idx,
                            Zeta_DynamicHashTable* dht) {
    while (n != NULL) {
        TreeNode* nl = ZETA_Concat(TreeNode, _GetL)(NULL, n);
        TreeNode* nr = ZETA_Concat(TreeNode, _GetR)(NULL, n);

        if (nl != NULL) { dst_idx = RecordOffset_(cntr, nl, dst_idx, dht); }

        if (cntr->lb == n || cntr->rb == n) {
            n = nr;
            continue;
        }

        if (GetNColor_(n) == dat_color) {
            Cntr_(Seg)* seg = NToSeg_(n);
            dst_idx += seg->dat.size;
            n = nr;
            continue;
        }

        Cntr_(Seg)* seg = NToSeg_(n);

        size_t offset = seg->ref.beg - dst_idx;

        OffsetPair* offset_pair =
            Zeta_DynamicHashTable_Find(dht, &offset, NULL);

        if (offset_pair == NULL) {
            OffsetPair tmp_offset_pair = { .offset = offset, .cnt = 0 };

            offset_pair =
                Zeta_DynamicHashTable_Insert(dht, &tmp_offset_pair, NULL);
        }

        offset_pair->cnt += seg->ref.size;

        dst_idx += seg->ref.size;

        n = nr;
    }

    return dst_idx;
}

#endif

#if STAGING

static void WriteBack_LR_(Cntr* cntr, int write_back_strategy,
                          unsigned long long cost_coeff_read,
                          unsigned long long cost_coeff_write,
                          unsigned long long cost_coeff_insert,
                          unsigned long long cost_coeff_erase) {
    size_t size = Cntr_(GetSize)(cntr);

    Zeta_SeqCntr* origin = cntr->origin;
    size_t origin_size = ZETA_SeqCntr_GetSize(origin);

    size_t del_l_cnt;
    size_t del_r_cnt;

    switch (write_back_strategy) {
        case ZETA_StagingVector_WriteBackStrategy_L:
            del_l_cnt = size - origin_size;
            del_r_cnt = 0;
            break;

        case ZETA_StagingVector_WriteBackStrategy_R:
            del_l_cnt = 0;
            del_r_cnt = size - origin_size;
            break;

        case ZETA_StagingVector_WriteBackStrategy_LR: {
            Zeta_DynamicHashTable dht;

            dht.width = sizeof(OffsetPair);

            dht.elem_hash_context = NULL;
            dht.ElemHash = OffsetPairHash;

            dht.key_hash_context = NULL;
            dht.KeyHash = OffsetHash;

            dht.elem_cmp_context = NULL;
            dht.ElemCompare = OffsetPairCompare;

            dht.key_elem_cmp_context = NULL;
            dht.KeyElemCompare = OffsetOffsetPairCompare;

            dht.node_allocator = zeta_cas_allocator;

            dht.ght.table_allocator = zeta_cas_allocator;

            Zeta_DynamicHashTable_Init(&dht);

            RecordOffset_(cntr, cntr->root, 0, &dht);

            void* cursor = __builtin_alloca_with_align(
                sizeof(Zeta_DynamicHashTable_Cursor),
                CHAR_BIT * alignof(Zeta_DynamicHashTable_Cursor));

            Zeta_DynamicHashTable_PeekL(&dht, cursor);

            unsigned long long best_cost = ZETA_ULLONG_MAX;

            del_l_cnt = 0;
            del_r_cnt = size - origin_size;

            for (;;) {
                OffsetPair* offset_pair =
                    Zeta_DynamicHashTable_Refer(&dht, cursor);

                if (offset_pair == NULL) { break; }

                size_t cur_offset = offset_pair->offset;

                size_t cur_del_l_cnt = 0;
                size_t cur_del_r_cnt = size - origin_size;

                if (cur_offset <= ZETA_RangeMaxOf(size_t) / 2) {
                    cur_del_l_cnt -= cur_offset;
                    cur_del_r_cnt += cur_offset;
                } else {
                    cur_del_l_cnt += cur_offset;
                    cur_del_r_cnt -= cur_offset;
                }

                unsigned long long cur_cost =
                    -((cost_coeff_read + cost_coeff_write) * offset_pair->cnt);

                if (cur_del_l_cnt <= ZETA_RangeMaxOf(size_t) / 2) {
                    cur_cost += cost_coeff_insert * cur_del_l_cnt;
                } else {
                    cur_cost += cost_coeff_erase * -cur_del_l_cnt;
                }

                if (cur_del_r_cnt <= ZETA_RangeMaxOf(size_t) / 2) {
                    cur_cost += cost_coeff_insert * cur_del_r_cnt;
                } else {
                    cur_cost += cost_coeff_erase * -cur_del_r_cnt;
                }

                if (ZETA_ULLONG_MAX / 2 < cur_cost - best_cost) {
                    best_cost = cur_cost;
                    del_l_cnt = cur_del_l_cnt;
                    del_r_cnt = cur_del_r_cnt;
                }

                Zeta_DynamicHashTable_Cursor_StepR(&dht, cursor);
            }

            Zeta_DynamicHashTable_Deinit(&dht);

            break;
        }
    }

    ZETA_CheckAssert(origin_size + del_l_cnt + del_r_cnt == size);

    size_t push_l_cnt;
    size_t pop_l_cnt;

    size_t push_r_cnt;
    size_t pop_r_cnt;

    if (del_l_cnt <= ZETA_RangeMaxOf(size_t) / 2) {
        push_l_cnt = del_l_cnt;
        pop_l_cnt = 0;
    } else {
        push_l_cnt = 0;
        pop_l_cnt = -del_l_cnt;
    }

    if (del_r_cnt <= ZETA_RangeMaxOf(size_t) / 2) {
        push_r_cnt = del_r_cnt;
        pop_r_cnt = 0;
    } else {
        push_r_cnt = 0;
        pop_r_cnt = -del_r_cnt;
    }

    if (0 < push_l_cnt) { ZETA_SeqCntr_PushL(origin, push_l_cnt, NULL); }

    if (0 < push_r_cnt) { ZETA_SeqCntr_PushR(origin, push_r_cnt, NULL); }

    size_t segs_cnt = Zeta_BinTree_Count(TreeNodeOpr, cntr->root) - 2;

    WBSeg* wb_segs =
        ZETA_Allocator_SafeAllocate(zeta_cas_allocator, alignof(WBSeg),
                                    sizeof(WBSeg) * (segs_cnt + 1)) +
        sizeof(WBSeg);

    WBSeg* prv_wb_seg = wb_segs - 1;

    prv_wb_seg->beg = ZETA_RangeMaxOf(size_t);
    prv_wb_seg->size = 0;

    prv_wb_seg->dst_idx = pop_l_cnt;
    prv_wb_seg->acc_ref = pop_l_cnt;

    ToWBSeg_(cntr, cntr->root, push_l_cnt, wb_segs);

    Zeta_CircularArray ca;
    ca.data = &ca;
    ca.width = GetWidth_(cntr);
    ca.offset = 0;
    ca.size = 0;
    ca.capacity = cntr->seg_capacity;

    Zeta_SeqCntr ca_seq_cntr;
    Zeta_CircularArray_DeploySeqCntr(&ca, &ca_seq_cntr);

    WriteWBSeg_(cntr, &ca_seq_cntr, wb_segs, segs_cnt);

    ZETA_Allocator_Deallocate(zeta_cas_allocator, prv_wb_seg);

    if (0 < pop_l_cnt) { ZETA_SeqCntr_PopL(origin, pop_l_cnt); }

    if (0 < pop_r_cnt) { ZETA_SeqCntr_PopR(origin, pop_r_cnt); }

    InitTree_(cntr);
    RefOrigin_(cntr);
}

#endif

#if STAGING

void Cntr_(WriteBack)(void* cntr_, int write_back_strategy,
                      unsigned long long cost_coeff_read,
                      unsigned long long cost_coeff_write,
                      unsigned long long cost_coeff_insert,
                      unsigned long long cost_coeff_erase) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    ZETA_DebugAssert(
        write_back_strategy == ZETA_StagingVector_WriteBackStrategy_L ||    //
        write_back_strategy == ZETA_StagingVector_WriteBackStrategy_R ||    //
        write_back_strategy == ZETA_StagingVector_WriteBackStrategy_LR ||   //
        write_back_strategy == ZETA_StagingVector_WriteBackStrategy_Random  //
    );

    if (write_back_strategy != ZETA_StagingVector_WriteBackStrategy_Random) {
        WriteBack_LR_(cntr, write_back_strategy, cost_coeff_read,
                      cost_coeff_write, cost_coeff_insert, cost_coeff_erase);
        return;
    }
}

#endif

void Cntr_(Check)(void* cntr_) {
    Cntr* cntr = cntr_;
    ZETA_DebugAssert(cntr != NULL);

#if STAGING
    ZETA_DebugAssert(cntr->origin != NULL);
    ZETA_DebugAssert(cntr->origin->GetWidth != NULL);
    ZETA_DebugAssert(cntr->origin->GetSize != NULL);
    ZETA_DebugAssert(cntr->origin->Access != NULL);
    ZETA_DebugAssert(cntr->origin->Read != NULL);
#endif

    size_t width = GetWidth_(cntr);

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(cntr->seg_capacity <= ZETA_RangeMaxOf(unsigned short));

    ZETA_DebugAssert(cntr->seg_allocator != NULL);
    ZETA_DebugAssert(cntr->seg_allocator->GetAlign != NULL);
    ZETA_DebugAssert(cntr->seg_allocator->Allocate != NULL);
    ZETA_DebugAssert(cntr->seg_allocator->Deallocate != NULL);
    ZETA_DebugAssert(ZETA_Allocator_GetAlign(cntr->seg_allocator) %
                         alignof(Cntr_(Seg)) ==
                     0);

    ZETA_DebugAssert(cntr->data_allocator != NULL);
    ZETA_DebugAssert(cntr->data_allocator->Allocate != NULL);
    ZETA_DebugAssert(cntr->data_allocator->Deallocate != NULL);
}

ZETA_DeclareStruct(SanitizeRet);

struct SanitizeRet {
    bool_t b;
    size_t ref_beg;
    size_t ref_end;
};

static SanitizeRet Sanitize_(Cntr* cntr, Zeta_MemRecorder* dst_seg,
                             Zeta_MemRecorder* dst_data, void* n) {
    Zeta_BinTreeNodeOperator const* btn_opr = TreeNodeOpr;

    size_t width = GetWidth_(cntr);

#if STAGING
    size_t origin_size = ZETA_SeqCntr_GetSize(cntr->origin);
#endif

    void* l_n = Zeta_BinTree_StepL(btn_opr, n);
    void* r_n = Zeta_BinTree_StepR(btn_opr, n);

    if (cntr->lb == n) {
        ZETA_DebugAssert(l_n == NULL);
        ZETA_DebugAssert(r_n != NULL);

        ZETA_DebugAssert(Zeta_BinTree_GetSize(btn_opr, n) == 1);

        if (dst_seg != NULL) {
            Zeta_MemRecorder_Record(dst_seg, cntr->lb, sizeof(TreeNode));
        }
    } else if (cntr->rb == n) {
        ZETA_DebugAssert(l_n != NULL);
        ZETA_DebugAssert(r_n == NULL);

        ZETA_DebugAssert(Zeta_BinTree_GetSize(btn_opr, n) == 1);

        if (dst_seg != NULL) {
            Zeta_MemRecorder_Record(dst_seg, cntr->rb, sizeof(TreeNode));
        }
    }
#if STAGING
    else if (GetNColor_(n) == ref_color) {
        ZETA_DebugAssert(l_n != NULL);
        ZETA_DebugAssert(r_n != NULL);

        Cntr_(Seg)* seg = NToSeg_(n);

        if (dst_seg != NULL) {
            Zeta_MemRecorder_Record(dst_seg, seg, sizeof(Cntr_(Seg)));
        }

        ZETA_DebugAssert(0 < seg->ref.size);

        ZETA_DebugAssert(seg->ref.beg < origin_size);
        ZETA_DebugAssert(seg->ref.beg + seg->ref.size <= origin_size);

        size_t size = seg->ref.size;

        ZETA_DebugAssert(size == Zeta_BinTree_GetSize(btn_opr, n));

        if (cntr->lb != l_n) {
            Cntr_(Seg)* l_seg = NToSeg_(l_n);

            if (GetNColor_(&l_seg->n) == ref_color) {
                ZETA_DebugAssert(l_seg->ref.beg + l_seg->ref.size <
                                 seg->ref.beg);
            }
        }

        if (cntr->rb != r_n) {
            Cntr_(Seg)* r_seg = NToSeg_(r_n);

            if (GetNColor_(&r_seg->n) == ref_color) {
                ZETA_DebugAssert(seg->ref.beg + seg->ref.size < r_seg->ref.beg);
            }
        }
    }
#endif
    else {
        ZETA_DebugAssert(GetNColor_(n) == dat_color);

        ZETA_DebugAssert(l_n != NULL);
        ZETA_DebugAssert(r_n != NULL);

        Cntr_(Seg)* seg = NToSeg_(n);

        if (dst_seg != NULL) {
            Zeta_MemRecorder_Record(dst_seg, seg, sizeof(Cntr_(Seg)));
        }

        size_t size = seg->dat.size;

        ZETA_DebugAssert(size == Zeta_BinTree_GetSize(btn_opr, n));

        ZETA_DebugAssert(0 < size);
        ZETA_DebugAssert(size <= cntr->seg_capacity);
        ZETA_DebugAssert(seg->dat.offset < cntr->seg_capacity);

        if (dst_data != NULL) {
            Zeta_MemRecorder_Record(dst_data, seg->dat.data,
                                    width * cntr->seg_capacity);
        }

        size_t l_vacant;
        size_t r_vacant;

        if (cntr->lb == l_n) {
            l_vacant = 0;
        } else {
            Cntr_(Seg)* l_seg = NToSeg_(l_n);

            l_vacant = cntr->seg_capacity -
                       StagingThreeWay(
                           GetNColor_(l_n) == ref_color,
                           ZETA_GetMinOf(l_seg->ref.size, cntr->seg_capacity),
                           l_seg->dat.size);
        }

        if (cntr->rb == r_n) {
            r_vacant = 0;
        } else {
            Cntr_(Seg)* r_seg = NToSeg_(r_n);

            r_vacant = cntr->seg_capacity -
                       StagingThreeWay(
                           GetNColor_(r_n) == ref_color,
                           ZETA_GetMinOf(r_seg->ref.size, cntr->seg_capacity),
                           r_seg->dat.size);
        }

        ZETA_DebugAssert(0 <= l_vacant);
        ZETA_DebugAssert(0 <= r_vacant);
    }

    void* sub_l_n = ZETA_Concat(TreeNode, _GetL)(NULL, n);
    void* sub_r_n = ZETA_Concat(TreeNode, _GetR)(NULL, n);

    SanitizeRet l_check_ret = (SanitizeRet){ FALSE, 0, 0 };
    SanitizeRet r_check_ret = (SanitizeRet){ FALSE, 0, 0 };

    if (sub_l_n != NULL) {
        l_check_ret = Sanitize_(cntr, dst_seg, dst_data, sub_l_n);
    }

    if (sub_r_n != NULL) {
        r_check_ret = Sanitize_(cntr, dst_seg, dst_data, sub_r_n);
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

    if (cntr->lb == n || cntr->rb == n) { return ret; }

#if STAGING
    Cntr_(Seg)* seg = NToSeg_(n);

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
#endif

    return ret;
}

void Cntr_(Sanitize)(void* cntr_, Zeta_MemRecorder* dst_seg,
                     Zeta_MemRecorder* dst_data) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    Sanitize_(cntr, dst_seg, dst_data, cntr->root);
}

static void PrintState_(Cntr* cntr, void* n) {
    void* l_n = ZETA_Concat(TreeNode, _GetL)(NULL, n);
    void* r_n = ZETA_Concat(TreeNode, _GetR)(NULL, n);

    if (l_n != NULL) { PrintState_(cntr, l_n); }

    if (cntr->lb != n && cntr->rb != n) {
        Cntr_(Seg)* seg = NToSeg_(n);

#if STAGING
        if (GetNColor_(&seg->n) == ref_color) {
            printf("ref %8zu [%8zu, %8zu]\n", seg->ref.size, seg->ref.beg,
                   seg->ref.beg + seg->ref.size);
        } else
#endif
        {
            printf("dat %8zu\n", (size_t)seg->dat.size);
        }
    }

    if (r_n != NULL) { PrintState_(cntr, r_n); }
}

void Cntr_(PrintState)(void* cntr_) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    PrintState_(cntr, cntr->root);
}

static Cntr_(Stats) GetStats_(Cntr* cntr, void* n) {
    Cntr_(Stats) ret;
    ret.ref_seg_cnt = 0;
    ret.dat_seg_cnt = 0;
    ret.ref_size = 0;
    ret.dat_size = 0;

    if (n == NULL) { return ret; }

    void* l_n = ZETA_Concat(TreeNode, _GetL)(NULL, n);
    void* r_n = ZETA_Concat(TreeNode, _GetR)(NULL, n);

    Cntr_(Stats) l_stats = GetStats_(cntr, l_n);
    Cntr_(Stats) r_stats = GetStats_(cntr, r_n);

    if (cntr->lb != n && cntr->rb != n) {
        Cntr_(Seg)* seg = NToSeg_(n);

#if STAGING
        if (GetNColor_(n) == ref_color) {
            ++ret.ref_seg_cnt;
            ret.ref_size += seg->ref.size;
        } else
#endif
        {
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

Cntr_(Stats) Cntr_(GetStats)(void* cntr_) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    return GetStats_(cntr, cntr->root);
}

bool_t Cntr_(Cursor_AreEqual)(void* cntr_, void const* cursor_a_,
                              void const* cursor_b_) {
    Cntr* cntr = cntr_;

    Cntr_(Cursor) const* cursor_a = cursor_a_;
    Cntr_(Cursor) const* cursor_b = cursor_b_;

    CheckCursor_(cntr, cursor_a);
    CheckCursor_(cntr, cursor_b);

    return cursor_a->idx == cursor_b->idx;
}

int Cntr_(Cursor_Compare)(void* cntr_, void const* cursor_a_,
                          void const* cursor_b_) {
    Cntr* cntr = cntr_;

    Cntr_(Cursor) const* cursor_a = cursor_a_;
    Cntr_(Cursor) const* cursor_b = cursor_b_;

    CheckCursor_(cntr, cursor_a);
    CheckCursor_(cntr, cursor_b);

    return ZETA_ThreeWayCompare(cursor_a->idx + 1, cursor_b->idx + 1);
}

size_t Cntr_(Cursor_GetDist)(void* cntr_, void const* cursor_a_,
                             void const* cursor_b_) {
    Cntr* cntr = cntr_;

    Cntr_(Cursor) const* cursor_a = cursor_a_;
    Cntr_(Cursor) const* cursor_b = cursor_b_;

    CheckCursor_(cntr, cursor_a);
    CheckCursor_(cntr, cursor_b);

    return cursor_b->idx - cursor_a->idx;
}

size_t Cntr_(Cursor_GetIdx)(void* cntr_, void const* cursor_) {
    Cntr* cntr = cntr_;
    Cntr_(Cursor) const* cursor = cursor_;
    CheckCursor_(cntr, cursor);

    return cursor->idx;
}

void Cntr_(Cursor_StepL)(void* cntr, void* cursor) {
    Cntr_(Cursor_AdvanceL)(cntr, cursor, 1);
}

void Cntr_(Cursor_StepR)(void* cntr, void* cursor) {
    Cntr_(Cursor_AdvanceR)(cntr, cursor, 1);
}

void Cntr_(Cursor_AdvanceL)(void* cntr_, void* cursor_, size_t step) {
    Cntr* cntr = cntr_;
    Cntr_(Cursor)* cursor = cursor_;
    CheckCursor_(cntr, cursor);

    if (step == 0) { return; }

    Zeta_BinTreeNodeOperator const* btn_opr = TreeNodeOpr;

    ZETA_DebugAssert(step <= cursor->idx + 1);

    void* dst_n;
    size_t dst_seg_idx;

    void* n = cursor->n;

    size_t n_size = n == cntr->rb ? 1 : ({
        Cntr_(Seg)* seg = NToSeg_(n);

        (cntr->lb == n || cntr->rb == n)
            ? 1
            : StagingThreeWay(GetNColor_(n) == ref_color, seg->ref.size,
                              seg->dat.size);
    });

    Zeta_BinTree_AdvanceL(&dst_n, &dst_seg_idx, btn_opr, n,
                          n_size - 1 - cursor->seg_idx + step);

    ZETA_DebugAssert(dst_n != NULL);

    cursor->idx -= step;
    cursor->n = dst_n;

    if (cntr->lb == dst_n) {
        cursor->seg_idx = 0;
        cursor->ref = 0;
        return;
    }

    Cntr_(Seg)* dst_seg = NToSeg_(dst_n);

    cursor->seg_idx = StagingThreeWay(GetNColor_(dst_n) == ref_color,
                                      dst_seg->ref.size, dst_seg->dat.size) -
                      1 - dst_seg_idx;

#if STAGING
    if (GetNColor_(dst_n) == ref_color) {
        cursor->ref = NULL;
        return;
    }
#endif

    Zeta_CircularArray ca;
    ca.data = dst_seg->dat.data;
    ca.width = GetWidth_(cntr);
    ca.offset = dst_seg->dat.offset;
    ca.size = dst_seg->dat.size;
    ca.capacity = cntr->seg_capacity;

    cursor->ref = Zeta_CircularArray_Access(&ca, cursor->seg_idx, NULL, NULL);
}

void Cntr_(Cursor_AdvanceR)(void* cntr_, void* cursor_, size_t step) {
    Cntr* cntr = cntr_;
    Cntr_(Cursor)* cursor = cursor_;
    CheckCursor_(cntr, cursor);

    if (step == 0) { return; }

    Zeta_BinTreeNodeOperator const* btn_opr = TreeNodeOpr;

    size_t size = Cntr_(GetSize)(cntr);

    ZETA_DebugAssert(step <= size - cursor->idx);

    void* dst_n;
    size_t dst_seg_idx;

    Zeta_BinTree_AdvanceR(&dst_n, &dst_seg_idx, btn_opr, cursor->n,
                          cursor->seg_idx + step);

    ZETA_DebugAssert(dst_n != NULL);

    cursor->idx += step;
    cursor->n = dst_n;
    cursor->seg_idx = dst_seg_idx;

    if (cntr->rb == dst_n
#if STAGING
        || GetNColor_(dst_n) == ref_color
#endif
    ) {
        cursor->ref = NULL;
        return;
    }

    Cntr_(Seg)* dst_seg = NToSeg_(dst_n);

    Zeta_CircularArray ca;
    ca.data = dst_seg->dat.data;
    ca.width = GetWidth_(cntr);
    ca.offset = dst_seg->dat.offset;
    ca.size = dst_seg->dat.size;
    ca.capacity = cntr->seg_capacity;

    cursor->ref = Zeta_CircularArray_Access(&ca, cursor->seg_idx, NULL, NULL);
}

void Cntr_(Cursor_Check)(void* cntr_, void const* cursor_) {
    if (!ZETA_IsDebug) { return; }

    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    Cntr_(Cursor) const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    Cntr_(Cursor) re_cursor;
    Cntr_(Access)(cntr, cursor->idx, &re_cursor, NULL);

    ZETA_DebugAssert(re_cursor.cntr == cursor->cntr);
    ZETA_DebugAssert(re_cursor.idx == cursor->idx);
    ZETA_DebugAssert(re_cursor.n == cursor->n);
    ZETA_DebugAssert(re_cursor.seg_idx == cursor->seg_idx);
    ZETA_DebugAssert(re_cursor.ref == cursor->ref);
}

void Cntr_(DeploySeqCntr)(void* cntr_, Zeta_SeqCntr* seq_cntr) {
    Cntr* cntr = cntr_;
    CheckCntr_(cntr);

    Zeta_SeqCntr_Init(seq_cntr);

    seq_cntr->context = cntr;

    seq_cntr->cursor_size = sizeof(Cntr_(Cursor));

    seq_cntr->GetWidth = Cntr_(GetWidth);

    seq_cntr->GetSize = Cntr_(GetSize);

    seq_cntr->GetCapacity = Cntr_(GetCapacity);

    seq_cntr->GetLBCursor = Cntr_(GetLBCursor);

    seq_cntr->GetRBCursor = Cntr_(GetRBCursor);

    seq_cntr->PeekL = Cntr_(PeekL);

    seq_cntr->PeekR = Cntr_(PeekR);

    seq_cntr->Access = Cntr_(Access);

    seq_cntr->Refer = Cntr_(Refer);

    seq_cntr->Read = Cntr_(Read);

    seq_cntr->Write = Cntr_(Write);

    seq_cntr->PushL = Cntr_(PushL);

    seq_cntr->PushR = Cntr_(PushR);

    seq_cntr->Insert = Cntr_(Insert);

    seq_cntr->PopL = Cntr_(PopL);

    seq_cntr->PopR = Cntr_(PopR);

    seq_cntr->Erase = Cntr_(Erase);

    seq_cntr->EraseAll = Cntr_(EraseAll);

    seq_cntr->Cursor_AreEqual = Cntr_(Cursor_AreEqual);

    seq_cntr->Cursor_Compare = Cntr_(Cursor_Compare);

    seq_cntr->Cursor_GetDist = Cntr_(Cursor_GetDist);

    seq_cntr->Cursor_GetIdx = Cntr_(Cursor_GetIdx);

    seq_cntr->Cursor_StepL = Cntr_(Cursor_StepL);

    seq_cntr->Cursor_StepR = Cntr_(Cursor_StepR);

    seq_cntr->Cursor_AdvanceL = Cntr_(Cursor_AdvanceL);

    seq_cntr->Cursor_AdvanceR = Cntr_(Cursor_AdvanceR);
}