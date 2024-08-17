#include "RadixVector.h"

#include "Debugger.h"
#include "MemCheck.h"
#include "RadixVector.h"
#include "utils.h"

#if ZETA_IsDebug

#define CheckRV_(rv) Zeta_RadixVector_Check((rv))

#define CheckRVCursor_(rv, cursor) Zeta_RadixVector_Cursor_Check((rv), (cursor))

#else

#define CheckRV_(rv)

#define CheckRVCursor_(rv, cursor)

#endif

static size_t CalcCapacity_(size_t seg_capacity, size_t branch_num,
                            unsigned order) {
    /*

    seg_capacity * (branch_num * 2) * (
        1 +
        branch_num +
        branch_num**2 +
        branch_num**3 +
        branch_num**(order)
    )

    */

    size_t ret = 0;

    size_t k = seg_capacity * branch_num * 2;

    for (unsigned order_i = 0;;) {
        if (ZETA_RangeMaxOf(size_t) - k < ret) {
            return ZETA_RangeMaxOf(size_t);
        }

        ret += k;

        if (order_i == order) { break; }

        if (ZETA_RangeMaxOf(size_t) / branch_num < k) {
            return ZETA_RangeMaxOf(size_t);
        }

        ++order_i;
        k *= branch_num;
    }

    return ret;
}

static void* AllocateNode_(Zeta_RadixVector* rv) {
    ZETA_DebugAssert(rv->node_allocator->Allocate != NULL);

    void* node = rv->node_allocator->Allocate(rv->node_allocator->context,
                                              sizeof(void*) * rv->branch_num);

    ZETA_DebugAssert(node != NULL);
    ZETA_DebugAssert(__builtin_is_aligned(node, sizeof(void*)));

    return node;
}

static void* AllocateSeg_(Zeta_RadixVector* rv) {
    ZETA_DebugAssert(rv->seg_allocator->Allocate != NULL);

    void* seg = rv->seg_allocator->Allocate(rv->seg_allocator->context,
                                            rv->stride * rv->seg_capacity);

    ZETA_DebugAssert(seg != NULL);

    return seg;
}

static void EraseTree_(Zeta_RadixVector* rv, void* n, unsigned order) {
    if (order == 0) {
        rv->seg_allocator->Deallocate(rv->seg_allocator->context, n);
        return;
    }

    size_t branch_num = rv->branch_num;

    for (unsigned child_i = 0; child_i < branch_num; ++child_i) {
        EraseTree_(rv, ((void**)n)[child_i], order - 1);
    }

    rv->node_allocator->Deallocate(rv->node_allocator->context, n);
}

static void* AccessSeg_(void* root, size_t branch_num, unsigned order_i,
                        size_t idx) {
    unsigned idxes[ZETA_RadixVector_max_order];

    for (unsigned i = 0; i < order_i; ++i) {
        idxes[i] = idx % branch_num;
        idx /= branch_num;
    }

    void* n = root;

    while (0 < order_i--) { n = ((void**)n)[idxes[order_i]]; }

    return n;
}

static void ReadSeg_(Zeta_RadixVector* rv, void* n, unsigned order_i,
                     size_t idx, size_t cnt, unsigned char* dst) {
    size_t seg_capacity = rv->seg_capacity;
    size_t branch_num = rv->branch_num;

    if (order_i == 0) {
        ZETA_CheckAssert(idx <= seg_capacity);
        ZETA_CheckAssert(cnt <= seg_capacity - idx);
        Zeta_MemCopy(dst, (unsigned char*)n + rv->stride * idx,
                     rv->stride * cnt);
        return;
    }

    size_t subtree_size = seg_capacity * Zeta_GetPower(branch_num, order_i - 1);

    size_t child_i = idx / subtree_size;
    size_t child_idx = idx % subtree_size;

    while (0 < cnt) {
        size_t cur_cnt = ZETA_GetMinOf(cnt, subtree_size - child_idx);
        cnt -= cur_cnt;

        ReadSeg_(rv, ((void**)n)[child_i], order_i - 1, child_idx, cur_cnt,
                 dst);

        ++child_i;
        child_idx = 0;

        dst += rv->stride * cur_cnt;
    }
}

static void WriteSeg_(Zeta_RadixVector* rv, void* n, unsigned order_i,
                      size_t idx, size_t cnt, unsigned char const* src) {
    size_t seg_capacity = rv->seg_capacity;
    size_t branch_num = rv->branch_num;

    if (order_i == 0) {
        ZETA_DebugAssert(idx <= seg_capacity);
        ZETA_DebugAssert(cnt <= seg_capacity - idx);
        Zeta_MemCopy((unsigned char*)n + rv->stride * idx, src,
                     rv->stride * cnt);
        return;
    }

    size_t subtree_size = seg_capacity * Zeta_GetPower(branch_num, order_i - 1);

    size_t child_i = idx / subtree_size;
    size_t child_idx = idx % subtree_size;

    while (0 < cnt) {
        size_t cur_cnt = ZETA_GetMinOf(cnt, subtree_size - child_idx);
        cnt -= cur_cnt;

        WriteSeg_(rv, ((void**)n)[child_i], order_i - 1, child_idx, cur_cnt,
                  src);

        ++child_i;
        child_idx = 0;

        src += rv->stride * cur_cnt;
    }
}

static Zeta_RadixVector_Cursor Access_(Zeta_RadixVector* rv, size_t idx) {
    if (idx == (size_t)(-1) || idx == rv->size) {
        return (Zeta_RadixVector_Cursor){ rv, idx, NULL, 0, NULL };
    }

    size_t seg_capacity = rv->seg_capacity;
    size_t branch_num = rv->branch_num;
    unsigned order = rv->order;

    size_t offset = idx;
    size_t seg_idx = offset % seg_capacity;

    size_t seg_offset = offset / seg_capacity;

    size_t tree_sizes[ZETA_RadixVector_max_order + 1];
    tree_sizes[0] = 1;

    for (unsigned order_i = 1; order_i <= order; ++order_i) {
        tree_sizes[order_i] = tree_sizes[order_i - 1] * branch_num;
    }

    unsigned order_i = order;

    void* ret_seg;

    for (;; --order_i) {
        size_t total_size = tree_sizes[order_i] * rv->roots_cnt[order_i];

        if (total_size <= seg_offset) {
            seg_offset -= total_size;
            continue;
        }

        size_t k = seg_offset / tree_sizes[order_i];

        ret_seg = AccessSeg_(
            k < branch_num ? rv->roots[order_i][k]
                           : rv->redundant_roots[order_i][k - branch_num],
            branch_num, order_i, seg_offset % tree_sizes[order_i]);

        goto RET;
    }

RET:;

    return (Zeta_RadixVector_Cursor){ rv, idx, ret_seg, seg_idx,
                                      (unsigned char*)ret_seg +
                                          rv->stride * seg_idx };
}

static void PushSeg_(Zeta_RadixVector* rv, size_t cnt) {
    size_t branch_num = rv->branch_num;
    unsigned order = rv->order;

    while (0 < cnt--) {
        void* new_root = AllocateSeg_(rv);

        ZETA_DebugAssert(rv->roots_cnt[0] < branch_num * 2);

        for (unsigned order_i = 0;; ++order_i) {
            unsigned root_cnt = rv->roots_cnt[order_i] + 1;

            if (root_cnt <= branch_num) {
                ZETA_CheckAssert(root_cnt - 1 < branch_num);

                rv->roots[order_i][root_cnt - 1] = new_root;

                rv->roots_cnt[order_i] = root_cnt;

                break;
            }

            ZETA_CheckAssert(root_cnt - branch_num - 1 < branch_num);

            rv->redundant_roots[order_i][root_cnt - branch_num - 1] = new_root;

            if (root_cnt < branch_num * 2 || order_i == order ||
                rv->roots_cnt[order_i + 1] == branch_num * 2) {
                rv->roots_cnt[order_i] = root_cnt;
                break;
            }

            rv->roots_cnt[order_i] = branch_num;

            new_root = rv->roots[order_i];

            rv->roots[order_i] = rv->redundant_roots[order_i];

            rv->redundant_roots[order_i] = AllocateNode_(rv);
        }
    }
}

static void PopSeg_(Zeta_RadixVector* rv, size_t cnt) {
    size_t branch_num = rv->branch_num;
    unsigned order = rv->order;

    size_t tree_sizes[ZETA_RadixVector_max_order + 1];
    tree_sizes[0] = 1;

    for (unsigned order_i = 1; order_i <= order; ++order_i) {
        tree_sizes[order_i] = tree_sizes[order_i - 1] * branch_num;
    }

    unsigned order_i = 0;

    for (;;) {
        size_t root_cnt = rv->roots_cnt[order_i];

        if (0 < root_cnt) {
            size_t cur_cnt = ZETA_GetMinOf(root_cnt, cnt / tree_sizes[order_i]);
            cnt -= tree_sizes[order_i] * cur_cnt;

            root_cnt -= cur_cnt;

            if (cnt == 0) {
                rv->roots_cnt[order_i] = root_cnt;
                return;
            }
        }

        if (root_cnt == 0) {
            rv->roots_cnt[order_i] = 0;

            if (order_i == order) { break; }

            ++order_i;
            continue;
        }

        ZETA_CheckAssert(0 < order_i);

        rv->node_allocator->Deallocate(rv->node_allocator->context,
                                       rv->roots[order_i - 1]);

        --root_cnt;

        size_t k = root_cnt;

        rv->roots[order_i - 1] =
            k < branch_num
                ? rv->roots[order_i][root_cnt]
                : rv->redundant_roots[order_i][root_cnt - branch_num];

        rv->roots_cnt[order_i] = root_cnt;
        rv->roots_cnt[order_i - 1] = branch_num;

        --order_i;
    }
}

void Zeta_RadixVector_Init(void* rv_) {
    Zeta_RadixVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);

    size_t width = rv->width;
    size_t stride = rv->stride;
    size_t seg_capacity = rv->seg_capacity;

    size_t branch_num = rv->branch_num;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    ZETA_DebugAssert(0 < seg_capacity);
    ZETA_DebugAssert(seg_capacity <= ZETA_RadixVector_max_seg_capacity);

    ZETA_DebugAssert(ZETA_RadixVector_min_branch_num <= branch_num);
    ZETA_DebugAssert(branch_num <= ZETA_RadixVector_max_branch_num);

    unsigned order = rv->order = ZETA_GetMinOf(
        rv->order,
        Zeta_GetFloorLog(ZETA_RangeMaxOf(size_t) / seg_capacity, branch_num));

    ZETA_DebugAssert(0 <= order);
    ZETA_DebugAssert(order <= ZETA_RadixVector_max_order);

    ZETA_DebugAssert(rv->node_allocator != NULL);
    ZETA_DebugAssert(rv->node_allocator->Allocate != NULL);
    ZETA_DebugAssert(rv->node_allocator->Deallocate != NULL);
    ZETA_DebugAssert(rv->node_allocator->GetAlign(rv->node_allocator->context) %
                         alignof(void*) ==
                     0);

    ZETA_DebugAssert(rv->seg_allocator != NULL);
    ZETA_DebugAssert(rv->seg_allocator->Allocate != NULL);
    ZETA_DebugAssert(rv->seg_allocator->Deallocate != NULL);
    ZETA_DebugAssert(rv->seg_allocator->GetAlign(rv->seg_allocator->context) %
                         alignof(void*) ==
                     0);

    rv->size = 0;

    for (unsigned order_i = 0; order_i <= order; ++order_i) {
        rv->roots[order_i] = AllocateNode_(rv);
        rv->redundant_roots[order_i] = AllocateNode_(rv);
    }

    for (unsigned order_i = 0; order_i <= order; ++order_i) {
        rv->roots_cnt[order_i] = 0;
    }
}

void Zeta_RadixVector_Deinit(void* rv_) {
    Zeta_RadixVector* rv = rv_;
    CheckRV_(rv);

    Zeta_RadixVector_EraseAll(rv);

    unsigned order = rv->order;

    for (unsigned order_i = 0; order_i <= order; ++order_i) {
        rv->node_allocator->Deallocate(rv->node_allocator->context,
                                       rv->roots[order_i]);

        rv->node_allocator->Deallocate(rv->node_allocator->context,
                                       rv->redundant_roots[order_i]);
    }
}

size_t Zeta_RadixVector_GetWidth(void* rv_) {
    Zeta_RadixVector* rv = rv_;
    CheckRV_(rv);

    return rv->width;
}

size_t Zeta_RadixVector_GetStride(void* rv_) {
    Zeta_RadixVector* rv = rv_;
    CheckRV_(rv);

    return rv->stride;
}

size_t Zeta_RadixVector_GetSize(void* rv_) {
    Zeta_RadixVector* rv = rv_;
    CheckRV_(rv);

    return rv->size;
}

size_t Zeta_RadixVector_GetCapacity(void* rv_) {
    Zeta_RadixVector* rv = rv_;
    CheckRV_(rv);

    return CalcCapacity_(rv->seg_capacity, rv->branch_num, rv->order);
}

void Zeta_RadixVector_GetLBCursor(void* rv_, void* dst_cursor) {
    Zeta_RadixVector* rv = rv_;
    CheckRV_(rv);

    Zeta_RadixVector_Access(rv, dst_cursor, NULL, -1);
}

void Zeta_RadixVector_GetRBCursor(void* rv_, void* dst_cursor) {
    Zeta_RadixVector* rv = rv_;
    CheckRV_(rv);

    Zeta_RadixVector_Access(rv, dst_cursor, NULL, rv->size);
}

void* Zeta_RadixVector_PeekL(void* rv_, void* dst_cursor, void* dst_elem) {
    Zeta_RadixVector* rv = rv_;
    CheckRV_(rv);

    return Zeta_RadixVector_Access(rv, dst_cursor, dst_elem, 0);
}

void* Zeta_RadixVector_PeekR(void* rv_, void* dst_cursor, void* dst_elem) {
    Zeta_RadixVector* rv = rv_;
    CheckRV_(rv);

    return Zeta_RadixVector_Access(rv, dst_cursor, dst_elem, rv->size - 1);
}

void* Zeta_RadixVector_Refer(void* rv_, void const* pos_cursor_) {
    Zeta_RadixVector* rv = rv_;
    Zeta_RadixVector_Cursor const* pos_cursor = pos_cursor_;

    CheckRVCursor_(rv, pos_cursor);

    return pos_cursor->ref;
}

void* Zeta_RadixVector_Access(void* rv_, void* dst_cursor_, void* dst_elem,
                              size_t idx) {
    Zeta_RadixVector* rv = rv_;
    CheckRV_(rv);

    ZETA_DebugAssert(idx + 1 < rv->size + 2);

    Zeta_RadixVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_RadixVector_Cursor pos_cursor = Access_(rv, idx);

    if (dst_cursor != NULL) {
        dst_cursor->rv = pos_cursor.rv;
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->seg = pos_cursor.seg;
        dst_cursor->seg_idx = pos_cursor.seg_idx;
        dst_cursor->ref = pos_cursor.ref;
    }

    if (pos_cursor.ref != NULL && dst_elem != NULL) {
        Zeta_MemCopy(dst_elem, pos_cursor.ref, rv->width);
    }

    return pos_cursor.ref;
}

void Zeta_RadixVector_Read(void* rv_, void const* pos_cursor_, size_t cnt,
                           void* dst_, void* dst_cursor_) {
    Zeta_RadixVector* rv = rv_;
    Zeta_RadixVector_Cursor const* pos_cursor = pos_cursor_;

    CheckRVCursor_(rv, pos_cursor);

    ZETA_DebugAssert(pos_cursor->idx <= rv->size);
    ZETA_DebugAssert(cnt <= rv->size - pos_cursor->idx);

    Zeta_RadixVector_Cursor* dst_cursor = dst_cursor_;

    if (cnt == 0) {
        if (dst_cursor != NULL) {
            dst_cursor->rv = pos_cursor->rv;
            dst_cursor->idx = pos_cursor->idx;
            dst_cursor->seg = pos_cursor->seg;
            dst_cursor->seg_idx = pos_cursor->seg_idx;
            dst_cursor->ref = pos_cursor->ref;
        }

        return;
    }

    size_t stride = rv->stride;
    size_t seg_capacity = rv->seg_capacity;
    size_t branch_num = rv->branch_num;
    unsigned order = rv->order;

    size_t ret_idx = pos_cursor->idx + cnt;

    unsigned char* dst = dst_;
    ZETA_DebugAssert(dst != NULL);

    size_t offset = pos_cursor->idx;

    size_t tree_sizes[ZETA_RadixVector_max_order + 1];
    tree_sizes[0] = seg_capacity;

    for (unsigned order_i = 1; order_i <= order; ++order_i) {
        tree_sizes[order_i] = tree_sizes[order_i - 1] * branch_num;
    }

    unsigned order_i = order;

    for (; 0 < cnt; --order_i) {
        size_t total_size = tree_sizes[order_i] * rv->roots_cnt[order_i];

        if (total_size <= offset) {
            offset -= total_size;
            continue;
        }

        size_t root_i = offset / tree_sizes[order_i];
        size_t root_offset = offset % tree_sizes[order_i];

        while (root_i < rv->roots_cnt[order_i] && 0 < cnt) {
            size_t cur_cnt =
                ZETA_GetMinOf(cnt, tree_sizes[order_i] - root_offset);
            cnt -= cur_cnt;

            size_t k = root_i;

            ReadSeg_(rv,
                     k < branch_num
                         ? rv->roots[order_i][k]
                         : rv->redundant_roots[order_i][k - branch_num],
                     order_i, root_offset, cur_cnt, dst);

            ++root_i;
            root_offset = 0;

            dst += stride * cur_cnt;
        }

        offset = 0;
    }

    if (dst_cursor != NULL) {
        Zeta_RadixVector_Access(rv, dst_cursor, NULL, ret_idx);
    }
}

void Zeta_RadixVector_Write(void* rv_, void* pos_cursor_, size_t cnt,
                            void const* src_, void* dst_cursor_) {
    Zeta_RadixVector* rv = rv_;
    Zeta_RadixVector_Cursor const* pos_cursor = pos_cursor_;

    CheckRVCursor_(rv, pos_cursor);

    ZETA_DebugAssert(pos_cursor->idx <= rv->size);
    ZETA_DebugAssert(cnt <= rv->size - pos_cursor->idx);

    Zeta_RadixVector_Cursor* dst_cursor = dst_cursor_;

    if (cnt == 0) {
        if (dst_cursor != NULL) {
            dst_cursor->rv = pos_cursor->rv;
            dst_cursor->idx = pos_cursor->idx;
            dst_cursor->seg = pos_cursor->seg;
            dst_cursor->seg_idx = pos_cursor->seg_idx;
            dst_cursor->ref = pos_cursor->ref;
        }

        return;
    }

    size_t stride = rv->stride;
    size_t seg_capacity = rv->seg_capacity;
    size_t branch_num = rv->branch_num;
    unsigned order = rv->order;

    size_t ret_idx = pos_cursor->idx + cnt;

    unsigned char const* src = src_;
    ZETA_DebugAssert(src != NULL);

    size_t offset = pos_cursor->idx;

    size_t tree_sizes[ZETA_RadixVector_max_order + 1];
    tree_sizes[0] = seg_capacity;

    for (unsigned order_i = 1; order_i <= order; ++order_i) {
        tree_sizes[order_i] = tree_sizes[order_i - 1] * branch_num;
    }

    unsigned order_i = order;

    for (; 0 < cnt; --order_i) {
        size_t total_size = tree_sizes[order_i] * rv->roots_cnt[order_i];

        if (total_size <= offset) {
            offset -= total_size;
            continue;
        }

        size_t root_i = offset / tree_sizes[order_i];
        size_t root_offset = offset % tree_sizes[order_i];

        while (root_i < rv->roots_cnt[order_i] && 0 < cnt) {
            size_t cur_cnt =
                ZETA_GetMinOf(cnt, tree_sizes[order_i] - root_offset);
            cnt -= cur_cnt;

            size_t k = root_i;

            WriteSeg_(rv,
                      k < branch_num
                          ? rv->roots[order_i][k]
                          : rv->redundant_roots[order_i][k - branch_num],
                      order_i, root_offset, cur_cnt, src);

            ++root_i;
            root_offset = 0;

            src += stride * cur_cnt;
        }

        offset = 0;
    }

    if (dst_cursor != NULL) {
        Zeta_RadixVector_Access(rv, dst_cursor, NULL, ret_idx);
    }
}

void* Zeta_RadixVector_PushR(void* rv_, void* dst_cursor_, size_t cnt) {
    Zeta_RadixVector* rv = rv_;
    CheckRV_(rv);

    Zeta_RadixVector_Cursor* dst_cursor = dst_cursor_;

    size_t origin_size = rv->size;

    if (0 < cnt) {
        size_t seg_capacity = rv->seg_capacity;

        size_t size = rv->size;
        size_t capacity =
            CalcCapacity_(rv->seg_capacity, rv->branch_num, rv->order);

        ZETA_DebugAssert(cnt <= capacity - size);

        PushSeg_(rv, ZETA_CeilIntDiv(size + cnt, seg_capacity) -
                         ZETA_CeilIntDiv(size, seg_capacity));

        rv->size += cnt;
    }

    Zeta_RadixVector_Cursor pos_cursor = Access_(rv, origin_size);

    if (dst_cursor != NULL) {
        dst_cursor->rv = pos_cursor.rv;
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->seg = pos_cursor.seg;
        dst_cursor->seg_idx = pos_cursor.seg_idx;
        dst_cursor->ref = pos_cursor.ref;
    }

    return pos_cursor.ref;
}

void Zeta_RadixVector_PopR(void* rv_, size_t cnt) {
    Zeta_RadixVector* rv = rv_;
    CheckRV_(rv);

    size_t size = rv->size;
    ZETA_DebugAssert(cnt <= size);

    if (cnt == 0) { return; }

    if (cnt == size) {
        Zeta_RadixVector_EraseAll(rv);
        return;
    }

    size_t seg_capacity = rv->seg_capacity;

    PopSeg_(rv, ZETA_CeilIntDiv(size, seg_capacity) -
                    ZETA_CeilIntDiv(size - cnt, seg_capacity));

    rv->size -= cnt;
}

void Zeta_RadixVector_EraseAll(void* rv_) {
    Zeta_RadixVector* rv = rv_;
    CheckRV_(rv);

    if (rv->size == 0) { return; }

    size_t branch_num = rv->branch_num;
    unsigned order = rv->order;

    for (unsigned order_i = 0; order_i <= order; ++order_i) {
        unsigned roots_cnt = ZETA_GetMinOf(branch_num, rv->roots_cnt[order_i]);
        unsigned redundant_cnt = rv->roots_cnt[order_i] - roots_cnt;

        for (unsigned root_i = 0; root_i < roots_cnt; ++root_i) {
            EraseTree_(rv, rv->roots[order_i][root_i], order_i);
        }

        for (unsigned root_i = 0; root_i < redundant_cnt; ++root_i) {
            EraseTree_(rv, rv->redundant_roots[order_i][root_i], order_i);
        }
    }

    rv->size = 0;
}

void Zeta_RadixVector_Check(void* rv_) {
    Zeta_RadixVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);

    size_t width = rv->width;
    size_t stride = rv->stride;
    size_t seg_capacity = rv->seg_capacity;
    size_t branch_num = rv->branch_num;
    unsigned order = rv->order;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    ZETA_DebugAssert(0 < seg_capacity);
    ZETA_DebugAssert(seg_capacity <= ZETA_RadixVector_max_seg_capacity);

    ZETA_DebugAssert(ZETA_RadixVector_min_branch_num <= branch_num);
    ZETA_DebugAssert(branch_num <= ZETA_RadixVector_max_branch_num);

    ZETA_DebugAssert(order <= ZETA_RadixVector_max_order);

    size_t size = rv->size;

    size_t capacity =
        CalcCapacity_(rv->seg_capacity, rv->branch_num, rv->order);

    ZETA_DebugAssert(size <= capacity);

    ZETA_DebugAssert(rv->node_allocator != NULL);
    ZETA_DebugAssert(rv->node_allocator->Allocate != NULL);
    ZETA_DebugAssert(rv->node_allocator->Deallocate != NULL);

    ZETA_DebugAssert(rv->seg_allocator != NULL);
    ZETA_DebugAssert(rv->seg_allocator->Allocate != NULL);
    ZETA_DebugAssert(rv->seg_allocator->Deallocate != NULL);

    size_t tree_size = seg_capacity;

    size_t size_acc = 0;

    for (unsigned order_i = 0; order_i <= order;
         ++order_i, tree_size *= branch_num) {
        size_acc += tree_size * rv->roots_cnt[order_i];
    }

    size_t last_seg_r_vacant = ZETA_ModAddInv(size, seg_capacity);

    ZETA_DebugAssert(size_acc == size + last_seg_r_vacant);
}

static void CheckTree_(Zeta_RadixVector* rv, Zeta_MemRecorder* dst_node,
                       Zeta_MemRecorder* dst_seg, void* n, unsigned order_i) {
    ZETA_DebugAssert(n != NULL);

    if (order_i == 0) {
        if (dst_seg != NULL) {
            Zeta_MemRecorder_Record(dst_seg, n, rv->stride * rv->seg_capacity);
        }

        return;
    }

    if (dst_node != NULL) {
        Zeta_MemRecorder_Record(dst_node, n, sizeof(void*) * rv->branch_num);
    }

    for (unsigned child_i = 0; child_i < rv->branch_num; ++child_i) {
        CheckTree_(rv, dst_node, dst_seg, ((void**)n)[child_i], order_i - 1);
    }
}

void Zeta_RadixVector_Sanitize(void* rv_, Zeta_MemRecorder* dst_node,
                               Zeta_MemRecorder* dst_seg) {
    Zeta_RadixVector* rv = rv_;
    CheckRV_(rv);

    ZETA_DebugAssert(dst_node != NULL);
    ZETA_DebugAssert(dst_seg != NULL);

    size_t branch_num = rv->branch_num;
    unsigned order = rv->order;

    for (unsigned order_i = 0; order_i <= order; ++order_i) {
        ZETA_DebugAssert(rv->roots[order_i] != NULL);
        ZETA_DebugAssert(rv->redundant_roots[order_i] != NULL);

        if (dst_node != NULL) {
            Zeta_MemRecorder_Record(dst_node, rv->roots[order_i],
                                    sizeof(void*) * branch_num);
            Zeta_MemRecorder_Record(dst_node, rv->redundant_roots[order_i],
                                    sizeof(void*) * branch_num);
        }
    }

    for (unsigned order_i = 0; order_i <= order; ++order_i) {
        unsigned cnt_a = ZETA_GetMinOf(branch_num, rv->roots_cnt[order_i]);
        unsigned cnt_b = rv->roots_cnt[order_i] - cnt_a;

        for (unsigned root_i = 0; root_i < cnt_a; ++root_i) {
            CheckTree_(rv, dst_node, dst_seg, rv->roots[order_i][root_i],
                       order_i);
        }

        for (unsigned root_i = 0; root_i < cnt_b; ++root_i) {
            CheckTree_(rv, dst_node, dst_seg,
                       rv->redundant_roots[order_i][root_i], order_i);
        }
    }
}

bool_t Zeta_RadixVector_Cursor_IsEqual(void* rv_, void const* cursor_a_,
                                       void const* cursor_b_) {
    Zeta_RadixVector* rv = rv_;

    Zeta_RadixVector_Cursor const* cursor_a = cursor_a_;
    Zeta_RadixVector_Cursor const* cursor_b = cursor_b_;

    CheckRVCursor_(rv, cursor_a);
    CheckRVCursor_(rv, cursor_b);

    return cursor_a->idx == cursor_b->idx;
}

int Zeta_RadixVector_Cursor_Compare(void* rv_, void const* cursor_a_,
                                    void const* cursor_b_) {
    Zeta_RadixVector* rv = rv_;

    Zeta_RadixVector_Cursor const* cursor_a = cursor_a_;
    Zeta_RadixVector_Cursor const* cursor_b = cursor_b_;

    CheckRVCursor_(rv, cursor_a);
    CheckRVCursor_(rv, cursor_b);

    size_t ka = cursor_a->idx + 1;
    size_t kb = cursor_b->idx + 1;

    if (ka < kb) { return -1; }
    if (kb < ka) { return 1; }
    return 0;
}

size_t Zeta_RadixVector_Cursor_GetDist(void* rv_, void const* cursor_a_,
                                       void const* cursor_b_) {
    Zeta_RadixVector* rv = rv_;

    Zeta_RadixVector_Cursor const* cursor_a = cursor_a_;
    Zeta_RadixVector_Cursor const* cursor_b = cursor_b_;

    CheckRVCursor_(rv, cursor_a);
    CheckRVCursor_(rv, cursor_b);

    return cursor_a->idx - cursor_b->idx;
}

size_t Zeta_RadixVector_Cursor_GetIdx(void* rv_, void const* cursor_) {
    Zeta_RadixVector* rv = rv_;
    Zeta_RadixVector_Cursor const* cursor = cursor_;

    CheckRVCursor_(rv, cursor);

    return cursor->idx;
}

void Zeta_RadixVector_Cursor_StepL(void* rv, void* cursor) {
    Zeta_RadixVector_Cursor_AdvanceL(rv, cursor, 1);
}

void Zeta_RadixVector_Cursor_StepR(void* rv, void* cursor) {
    Zeta_RadixVector_Cursor_AdvanceR(rv, cursor, 1);
}

void Zeta_RadixVector_Cursor_AdvanceL(void* rv_, void* cursor_, size_t step) {
    Zeta_RadixVector* rv = rv_;
    Zeta_RadixVector_Cursor* cursor = cursor_;

    CheckRVCursor_(rv, cursor);

    if (step == 0) { return; }

    ZETA_DebugAssert(step <= cursor->idx + 1);

    Zeta_RadixVector_Access(rv, cursor, NULL, cursor->idx - step);
}

void Zeta_RadixVector_Cursor_AdvanceR(void* rv_, void* cursor_, size_t step) {
    Zeta_RadixVector* rv = rv_;
    Zeta_RadixVector_Cursor* cursor = cursor_;

    CheckRVCursor_(rv, cursor);

    if (step == 0) { return; }

    ZETA_DebugAssert(step <= rv->size - cursor->idx);

    Zeta_RadixVector_Access(rv, cursor, NULL, cursor->idx + step);
}

void Zeta_RadixVector_Cursor_Check(void* rv_, void const* cursor_) {
    Zeta_RadixVector* rv = rv_;
    CheckRV_(rv);

    Zeta_RadixVector_Cursor const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    Zeta_RadixVector_Cursor re_cursor;
    Zeta_RadixVector_Access(rv, &re_cursor, NULL, cursor->idx);

    ZETA_DebugAssert(re_cursor.rv == cursor->rv);
    ZETA_DebugAssert(re_cursor.idx == cursor->idx);
    ZETA_DebugAssert(re_cursor.seg == cursor->seg);
    ZETA_DebugAssert(re_cursor.seg_idx == cursor->seg_idx);
    ZETA_DebugAssert(re_cursor.ref == cursor->ref);
}

void Zeta_RadixVector_DeploySeqContainer(void* rv_,
                                         Zeta_SeqContainer* seq_cntr) {
    Zeta_RadixVector* rv = rv_;
    CheckRV_(rv);

    ZETA_DebugAssert(seq_cntr != NULL);

    Zeta_SeqContainer_Init(seq_cntr);

    seq_cntr->context = rv;

    seq_cntr->cursor_size = sizeof(Zeta_RadixVector_Cursor);

    seq_cntr->GetStride = Zeta_RadixVector_GetStride;

    seq_cntr->GetWidth = Zeta_RadixVector_GetWidth;

    seq_cntr->GetSize = Zeta_RadixVector_GetSize;

    seq_cntr->GetLBCursor = Zeta_RadixVector_GetLBCursor;

    seq_cntr->GetRBCursor = Zeta_RadixVector_GetRBCursor;

    seq_cntr->PeekL = Zeta_RadixVector_PeekL;

    seq_cntr->PeekR = Zeta_RadixVector_PeekR;

    seq_cntr->Access = Zeta_RadixVector_Access;

    seq_cntr->Refer = Zeta_RadixVector_Refer;

    seq_cntr->Read = Zeta_RadixVector_Read;

    seq_cntr->Write = Zeta_RadixVector_Write;

    seq_cntr->PushR = Zeta_RadixVector_PushR;

    seq_cntr->PopR = Zeta_RadixVector_PopR;

    seq_cntr->EraseAll = Zeta_RadixVector_EraseAll;

    seq_cntr->Cursor_IsEqual = Zeta_RadixVector_Cursor_IsEqual;

    seq_cntr->Cursor_Compare = Zeta_RadixVector_Cursor_Compare;

    seq_cntr->Cursor_GetDist = Zeta_RadixVector_Cursor_GetDist;

    seq_cntr->Cursor_GetIdx = Zeta_RadixVector_Cursor_GetIdx;

    seq_cntr->Cursor_StepL = Zeta_RadixVector_Cursor_StepL;

    seq_cntr->Cursor_StepR = Zeta_RadixVector_Cursor_StepR;

    seq_cntr->Cursor_AdvanceL = Zeta_RadixVector_Cursor_AdvanceL;

    seq_cntr->Cursor_AdvanceR = Zeta_RadixVector_Cursor_AdvanceR;
}
