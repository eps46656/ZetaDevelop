#include "RadixDeque.h"

#include "Debugger.h"
#include "MemCheck.h"
#include "RadixDeque.h"
#include "utils.h"

#if ZETA_IsDebug

#define CheckRD_(rv) Zeta_RadixDeque_Check((rv))

#define CheckRDCursor_(rv, cursor) Zeta_RadixDeque_Cursor_Check((rv), (cursor))

#else

#define CheckRD_(dv)

#define CheckRDCursor_(dv, cursor)

#endif

static size_t CalcCapacity_(size_t seg_capacity, size_t branch_num,
                            unsigned order) {
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

static void* AllocateNode_(Zeta_RadixDeque* rd) {
    ZETA_DebugAssert(rd->node_allocator->Allocate != NULL);

    void* node = rd->node_allocator->Allocate(rd->node_allocator->context,
                                              sizeof(void*) * rd->branch_num);

    ZETA_DebugAssert(node != NULL);
    ZETA_DebugAssert(__builtin_is_aligned(node, sizeof(void*)));

    return node;
}

static void* AllocateSeg_(Zeta_RadixDeque* rd) {
    ZETA_DebugAssert(rd->seg_allocator->Allocate != NULL);

    void* seg = rd->seg_allocator->Allocate(rd->seg_allocator->context,
                                            rd->stride * rd->seg_capacity);

    ZETA_DebugAssert(seg != NULL);

    return seg;
}

static void EraseTree_(Zeta_RadixDeque* rd, void* n, unsigned order) {
    if (order == 0) {
        rd->seg_allocator->Deallocate(rd->seg_allocator->context, n);
        return;
    }

    size_t branch_num = rd->branch_num;

    for (unsigned child_i = 0; child_i < branch_num; ++child_i) {
        EraseTree_(rd, ((void**)n)[child_i], order - 1);
    }

    rd->node_allocator->Deallocate(rd->node_allocator->context, n);
}

static void* AccessSeg_(void* root, size_t branch_num, unsigned order_i,
                        size_t idx) {
    unsigned idxes[ZETA_RadixDeque_max_order];

    for (unsigned i = 0; i < order_i; ++i) {
        idxes[i] = idx % branch_num;
        idx /= branch_num;
    }

    void* n = root;

    while (0 < order_i--) { n = ((void**)n)[idxes[order_i]]; }

    return n;
}

static void ReadSeg_(Zeta_RadixDeque* rd, void* n, unsigned order_i, size_t idx,
                     size_t cnt, unsigned char* dst) {
    size_t seg_capacity = rd->seg_capacity;
    size_t branch_num = rd->branch_num;

    if (order_i == 0) {
        Zeta_MemCopy(dst, (unsigned char*)n + rd->stride * idx,
                     rd->stride * cnt);
        return;
    }

    size_t subtree_size = seg_capacity * Zeta_Power(branch_num, order_i - 1);

    size_t child_i = idx / subtree_size;
    size_t child_idx = idx % subtree_size;

    while (0 < cnt) {
        size_t cur_cnt = ZETA_GetMinOf(cnt, subtree_size - child_idx);
        cnt -= cur_cnt;

        ReadSeg_(rd, ((void**)n)[child_i], order_i - 1, child_idx, cur_cnt,
                 dst);

        ++child_i;
        child_idx = 0;

        dst += rd->stride * cur_cnt;
    }
}

static void WriteSeg_(Zeta_RadixDeque* rd, void* n, unsigned order_i,
                      size_t idx, size_t cnt, unsigned char const* src) {
    size_t seg_capacity = rd->seg_capacity;
    size_t branch_num = rd->branch_num;

    if (order_i == 0) {
        ZETA_DebugAssert(idx <= seg_capacity);
        ZETA_DebugAssert(cnt <= seg_capacity - idx);
        Zeta_MemCopy((unsigned char*)n + rd->stride * idx, src,
                     rd->stride * cnt);
        return;
    }

    size_t subtree_size = seg_capacity * Zeta_Power(branch_num, order_i - 1);

    size_t child_i = idx / subtree_size;
    size_t child_idx = idx % subtree_size;

    while (0 < cnt) {
        size_t cur_cnt = ZETA_GetMinOf(cnt, subtree_size - child_idx);
        cnt -= cur_cnt;

        WriteSeg_(rd, ((void**)n)[child_i], order_i - 1, child_idx, cur_cnt,
                  src);

        ++child_i;
        child_idx = 0;

        src += rd->stride * cur_cnt;
    }
}

static Zeta_RadixDeque_Cursor Access_(Zeta_RadixDeque* rd, size_t idx) {
    if (idx == (size_t)(-1) || idx == rd->size) {
        return (Zeta_RadixDeque_Cursor){ rd, idx, NULL, 0, NULL };
    }

    size_t seg_capacity = rd->seg_capacity;
    size_t branch_num = rd->branch_num;
    unsigned order = rd->order;

    size_t offset = rd->offset + idx;
    size_t seg_idx = offset % seg_capacity;

    size_t seg_offset = offset / seg_capacity;

    size_t tree_sizes[ZETA_RadixDeque_max_order + 1];
    tree_sizes[0] = 1;

    for (unsigned order_i = 1; order_i <= order; ++order_i) {
        tree_sizes[order_i] = tree_sizes[order_i - 1] * branch_num;
    }

    unsigned order_i = 0;
    void* ret_seg;

    for (;; ++order_i) {
        size_t total_size = tree_sizes[order_i] * rd->roots_cnt_lw[order_i];

        if (total_size <= seg_offset) {
            seg_offset -= total_size;
            if (order_i == order) { break; }
            continue;
        }

        size_t k = branch_num * 2 - rd->roots_cnt_lw[order_i] +
                   seg_offset / tree_sizes[order_i];

        ret_seg =
            AccessSeg_(k < branch_num ? rd->redundant_roots_lw[order_i][k]
                                      : rd->roots_lw[order_i][k - branch_num],
                       branch_num, order_i, seg_offset % tree_sizes[order_i]);

        goto RET;
    }

    for (;; --order_i) {
        size_t total_size = tree_sizes[order_i] * rd->roots_cnt_rw[order_i];

        if (total_size <= seg_offset) {
            seg_offset -= total_size;
            continue;
        }

        size_t k = seg_offset / tree_sizes[order_i];

        ret_seg = AccessSeg_(
            k < branch_num ? rd->roots_rw[order_i][k]
                           : rd->redundant_roots_rw[order_i][k - branch_num],
            branch_num, order_i, seg_offset % tree_sizes[order_i]);

        goto RET;
    }

RET:;

    return (Zeta_RadixDeque_Cursor){ rd, idx, ret_seg, seg_idx,
                                     (unsigned char*)ret_seg +
                                         rd->stride * seg_idx };
}

static void PushSeg_(Zeta_RadixDeque* rd, int dir, size_t cnt) {
    size_t branch_num = rd->branch_num;
    unsigned order = rd->order;

    void*** roots;
    void*** redundant_roots;
    unsigned char* roots_cnt;

    if (dir == 0) {
        roots = rd->roots_lw;
        redundant_roots = rd->redundant_roots_lw;
        roots_cnt = rd->roots_cnt_lw;
    } else {
        roots = rd->roots_rw;
        redundant_roots = rd->redundant_roots_rw;
        roots_cnt = rd->roots_cnt_rw;
    }

    while (0 < cnt--) {
        void* new_root = AllocateSeg_(rd);

        ZETA_DebugAssert(roots_cnt[0] < branch_num * 2);

        for (unsigned order_i = 0;; ++order_i) {
            unsigned root_cnt = roots_cnt[order_i] + 1;

            if (root_cnt <= branch_num) {
                roots[order_i][dir == 0 ? branch_num - root_cnt
                                        : root_cnt - 1] = new_root;

                roots_cnt[order_i] = root_cnt;

                break;
            }

            redundant_roots[order_i][dir == 0 ? branch_num * 2 - root_cnt
                                              : root_cnt - branch_num - 1] =
                new_root;

            if (root_cnt < branch_num * 2 || order_i == order ||
                roots_cnt[order_i + 1] == branch_num * 2) {
                roots_cnt[order_i] = root_cnt;
                break;
            }

            roots_cnt[order_i] = branch_num;

            new_root = roots[order_i];

            roots[order_i] = redundant_roots[order_i];

            redundant_roots[order_i] = AllocateNode_(rd);
        }
    }
}

static void PopSeg_(Zeta_RadixDeque* rd, int dir, size_t cnt) {
    ZETA_Unused(rd);
    ZETA_Unused(dir);
    ZETA_Unused(cnt);

    size_t branch_num = rd->branch_num;
    unsigned order = rd->order;

    size_t tree_sizes[ZETA_RadixDeque_max_order + 1];
    tree_sizes[0] = 1;

    for (unsigned order_i = 1; order_i <= order; ++order_i) {
        tree_sizes[order_i] = tree_sizes[order_i - 1] * branch_num;
    }

    unsigned order_i = 0;

    for (;;) {
        for (;;) {
            size_t root_cnt = rd->roots_cnt_lw[order_i];

            if (0 < root_cnt) {
                size_t cur_cnt =
                    ZETA_GetMinOf(root_cnt, cnt / tree_sizes[order_i]);
                cnt -= tree_sizes[order_i] * cur_cnt;

                root_cnt -= cur_cnt;

                if (cnt == 0) {
                    rd->roots_cnt_lw[order_i] = root_cnt;
                    return;
                }
            }

            if (root_cnt == 0) {
                rd->roots_cnt_lw[order_i] = 0;

                if (order_i == order) { break; }

                ++order_i;
                continue;
            }

            rd->node_allocator->Deallocate(rd->node_allocator->context,
                                           rd->roots_lw[order_i - 1]);

            --root_cnt;

            if (dir == 0) {
                size_t k = branch_num * 2 - 1 - root_cnt;

                rd->roots_lw[order_i - 1] =
                    k < branch_num ? rd->redundant_roots_lw[order_i][k]
                                   : rd->roots_lw[order_i][k - branch_num];
            } else {
                size_t k = root_cnt;

                rd->roots_lw[order_i - 1] =
                    k < branch_num
                        ? rd->roots_lw[order_i][root_cnt]
                        : rd->redundant_roots_lw[order_i]
                                                [root_cnt - branch_num];
            }

            rd->roots_cnt_lw[order_i] = root_cnt;
            rd->roots_cnt_lw[order_i - 1] = branch_num;

            --order_i;
        }

        for (;;) {
            size_t root_cnt = rd->roots_cnt_rw[order_i];

            size_t cur_cnt = ZETA_GetMinOf(root_cnt, cnt / tree_sizes[order_i]);
            cnt -= tree_sizes[order_i] * cur_cnt;

            if (root_cnt == cur_cnt) {
                rd->roots_cnt_rw[order_i] = 0;

                if (cnt == 0) { return; }

                --order_i;
                continue;
            }

            if (cnt == 0) {
                rd->roots_cnt_rw[order_i] = root_cnt - cur_cnt;
                return;
            }

            if (branch_num <= cur_cnt) {
                root_cnt -= branch_num;
                cur_cnt -= branch_num;
                ZETA_Swap(rd->roots_rw[order_i],
                          rd->redundant_roots_rw[order_i]);
            }

            size_t move_cnt = (root_cnt - cur_cnt + 1) / 2;

            if (dir == 0) {
                size_t cnt_a = ZETA_GetMinOf(move_cnt, branch_num - cur_cnt);
                size_t cnt_b = move_cnt - cnt_a;

                Zeta_MemCopy(rd->roots_lw[order_i] + branch_num - cnt_b,
                             rd->redundant_roots_rw[order_i],
                             sizeof(void*) * cnt_b);

                Zeta_MemCopy(rd->roots_lw[order_i] + branch_num - cnt_b - cnt_a,
                             rd->roots_rw[order_i] + cur_cnt,
                             sizeof(void*) * cnt_a);
            } else {
                size_t cnt_a = ZETA_GetMinOf(move_cnt, branch_num - cur_cnt);
                size_t cnt_b = move_cnt - cnt_a;

                Zeta_MemCopy(
                    rd->roots_lw[order_i],
                    rd->redundant_roots_rw[order_i] + branch_num - cnt_b,
                    sizeof(void*) * cnt_b);

                Zeta_MemCopy(
                    rd->roots_lw[order_i] + cnt_b,
                    rd->roots_rw[order_i] + branch_num - cur_cnt - cnt_a,
                    sizeof(void*) * cnt_a);
            }

            cur_cnt += move_cnt;

            rd->roots_cnt_lw[order_i] = move_cnt;
            rd->roots_cnt_rw[order_i] = root_cnt - cur_cnt;

            if (branch_num <= cur_cnt) {
                root_cnt -= branch_num;
                cur_cnt -= branch_num;
                ZETA_Swap(rd->roots_rw[order_i],
                          rd->redundant_roots_rw[order_i]);
            }

            if (dir == 0) {
                size_t cnt_a = ZETA_GetMinOf(branch_num, root_cnt) - cur_cnt;
                size_t cnt_b = root_cnt - cur_cnt - cnt_a;

                Zeta_MemMove(rd->roots_rw[order_i],
                             rd->roots_rw[order_i] + cur_cnt,
                             sizeof(void*) * cnt_a);

                Zeta_MemMove(rd->roots_rw[order_i] + cnt_a,
                             rd->redundant_roots_rw[order_i],
                             sizeof(void*) * cnt_b);
            } else {
                size_t cnt_a = ZETA_GetMinOf(branch_num, root_cnt) - cur_cnt;
                size_t cnt_b = root_cnt - cur_cnt - cnt_a;

                void** p = rd->roots_rw[order_i] + branch_num;

                Zeta_MemMove(p - cnt_a, p - cur_cnt - cnt_a,
                             sizeof(void*) * cnt_a);

                Zeta_MemMove(
                    p - cnt_a - cnt_b,
                    rd->redundant_roots_rw[order_i] + branch_num - cnt_b,
                    sizeof(void*) * cnt_b);
            }

            break;
        }
    }
}

void Zeta_RadixDeque_Init(void* rd_) {
    Zeta_RadixDeque* rd = rd_;
    ZETA_DebugAssert(rd != NULL);

    size_t width = rd->width;
    size_t stride = rd->stride;
    size_t seg_capacity = rd->seg_capacity;

    size_t branch_num = rd->branch_num;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    ZETA_DebugAssert(0 < seg_capacity);
    ZETA_DebugAssert(seg_capacity <= ZETA_RadixDeque_max_seg_capacity);

    ZETA_DebugAssert(ZETA_RadixDeque_min_branch_num <= branch_num);
    ZETA_DebugAssert(branch_num <= ZETA_RadixDeque_max_branch_num);

    unsigned order = rd->order = ZETA_GetMinOf(
        rd->order,
        Zeta_FloorLog(ZETA_RangeMaxOf(size_t) / seg_capacity, branch_num));

    ZETA_DebugAssert(0 <= order);
    ZETA_DebugAssert(order <= ZETA_RadixDeque_max_order);

    ZETA_DebugAssert(rd->node_allocator != NULL);
    ZETA_DebugAssert(rd->node_allocator->Allocate != NULL);
    ZETA_DebugAssert(rd->node_allocator->Deallocate != NULL);
    ZETA_DebugAssert(rd->node_allocator->GetAlign(rd->node_allocator->context) %
                         alignof(void*) ==
                     0);

    ZETA_DebugAssert(rd->seg_allocator != NULL);
    ZETA_DebugAssert(rd->seg_allocator->Allocate != NULL);
    ZETA_DebugAssert(rd->seg_allocator->Deallocate != NULL);
    ZETA_DebugAssert(rd->seg_allocator->GetAlign(rd->seg_allocator->context) %
                         alignof(void*) ==
                     0);

    rd->offset = 0;
    rd->size = 0;

    for (unsigned order_i = 0; order_i <= order; ++order_i) {
        rd->roots_lw[order_i] = AllocateNode_(rd);
        rd->roots_rw[order_i] = AllocateNode_(rd);
        rd->redundant_roots_lw[order_i] = AllocateNode_(rd);
        rd->redundant_roots_rw[order_i] = AllocateNode_(rd);
    }

    for (unsigned order_i = 0; order_i <= order; ++order_i) {
        rd->roots_cnt_lw[order_i] = 0;
        rd->roots_cnt_rw[order_i] = 0;
    }
}

void Zeta_RadixDeque_Deinit(void* rd_) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    Zeta_RadixDeque_EraseAll(rd);

    unsigned order = rd->order;

    for (unsigned order_i = 0; order_i <= order; ++order_i) {
        rd->node_allocator->Deallocate(rd->node_allocator->context,
                                       rd->roots_lw[order_i]);

        rd->node_allocator->Deallocate(rd->node_allocator->context,
                                       rd->roots_rw[order_i]);

        rd->node_allocator->Deallocate(rd->node_allocator->context,
                                       rd->redundant_roots_lw[order_i]);

        rd->node_allocator->Deallocate(rd->node_allocator->context,
                                       rd->redundant_roots_rw[order_i]);
    }
}

size_t Zeta_RadixDeque_GetWidth(void* rd_) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    return rd->width;
}

size_t Zeta_RadixDeque_GetStride(void* rd_) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    return rd->stride;
}

size_t Zeta_RadixDeque_GetSize(void* rd_) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    return rd->size;
}

size_t Zeta_RadixDeque_GetCapacity(void* rd_) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    return CalcCapacity_(rd->seg_capacity, rd->branch_num, rd->order);
}

void Zeta_RadixDeque_GetLBCursor(void* rd_, void* dst_cursor) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    Zeta_RadixDeque_Access(rd, dst_cursor, NULL, -1);
}

void Zeta_RadixDeque_GetRBCursor(void* rd_, void* dst_cursor) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    Zeta_RadixDeque_Access(rd, dst_cursor, NULL, rd->size);
}

void* Zeta_RadixDeque_PeekL(void* rd_, void* dst_cursor, void* dst_elem) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    return Zeta_RadixDeque_Access(rd, dst_cursor, dst_elem, 0);
}

void* Zeta_RadixDeque_PeekR(void* rd_, void* dst_cursor, void* dst_elem) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    return Zeta_RadixDeque_Access(rd, dst_cursor, dst_elem, rd->size - 1);
}

void* Zeta_RadixDeque_Refer(void* rd_, void const* pos_cursor_) {
    Zeta_RadixDeque* rd = rd_;
    Zeta_RadixDeque_Cursor const* pos_cursor = pos_cursor_;

    CheckRDCursor_(rd, pos_cursor);

    return pos_cursor->ref;
}

void* Zeta_RadixDeque_Access(void* rd_, void* dst_cursor_, void* dst_elem,
                             size_t idx) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    ZETA_DebugAssert(idx + 1 < rd->size + 2);

    Zeta_RadixDeque_Cursor* dst_cursor = dst_cursor_;

    Zeta_RadixDeque_Cursor pos_cursor = Access_(rd, idx);

    if (dst_cursor != NULL) {
        dst_cursor->rd = pos_cursor.rd;
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->seg = pos_cursor.seg;
        dst_cursor->seg_idx = pos_cursor.seg_idx;
        dst_cursor->ref = pos_cursor.ref;
    }

    if (pos_cursor.ref != NULL && dst_elem != NULL) {
        Zeta_MemCopy(dst_elem, pos_cursor.ref, rd->width);
    }

    return pos_cursor.ref;
}

void Zeta_RadixDeque_Read(void* rd_, void const* pos_cursor_, size_t cnt,
                          void* dst_, void* dst_cursor_) {
    Zeta_RadixDeque* rd = rd_;
    Zeta_RadixDeque_Cursor const* pos_cursor = pos_cursor_;

    CheckRDCursor_(rd, pos_cursor);

    ZETA_DebugAssert(pos_cursor->idx <= rd->size);
    ZETA_DebugAssert(cnt <= rd->size - pos_cursor->idx);

    Zeta_RadixDeque_Cursor* dst_cursor = dst_cursor_;

    if (cnt == 0) {
        if (dst_cursor != NULL) {
            dst_cursor->rd = pos_cursor->rd;
            dst_cursor->idx = pos_cursor->idx;
            dst_cursor->seg = pos_cursor->seg;
            dst_cursor->seg_idx = pos_cursor->seg_idx;
            dst_cursor->ref = pos_cursor->ref;
        }

        return;
    }

    size_t stride = rd->stride;
    size_t seg_capacity = rd->seg_capacity;
    size_t branch_num = rd->branch_num;
    unsigned order = rd->order;

    size_t ret_idx = pos_cursor->idx + cnt;

    unsigned char* dst = dst_;
    ZETA_DebugAssert(dst != NULL);

    size_t offset = rd->offset + pos_cursor->idx;

    size_t tree_sizes[ZETA_RadixDeque_max_order + 1];
    tree_sizes[0] = seg_capacity;

    for (unsigned order_i = 1; order_i <= order; ++order_i) {
        tree_sizes[order_i] = tree_sizes[order_i - 1] * branch_num;
    }

    unsigned order_i = 0;

    for (; 0 < cnt; ++order_i) {
        size_t total_size = tree_sizes[order_i] * rd->roots_cnt_lw[order_i];

        if (total_size <= offset) {
            offset -= total_size;
            if (order_i == order) { break; }
            continue;
        }

        size_t root_i = offset / tree_sizes[order_i];
        size_t root_offset = offset % tree_sizes[order_i];

        while (root_i < rd->roots_cnt_lw[order_i] && 0 < cnt) {
            size_t cur_cnt =
                ZETA_GetMinOf(cnt, tree_sizes[order_i] - root_offset);
            cnt -= cur_cnt;

            size_t k = branch_num * 2 - rd->roots_cnt_lw[order_i] + root_i;

            ReadSeg_(rd,
                     k < branch_num ? rd->redundant_roots_lw[order_i][k]
                                    : rd->roots_lw[order_i][k - branch_num],
                     order_i, root_offset, cur_cnt, dst);

            ++root_i;
            root_offset = 0;

            dst += stride * cur_cnt;
        }

        offset = 0;

        if (order_i == order) { break; }
    }

    for (; 0 < cnt; --order_i) {
        size_t total_size = tree_sizes[order_i] * rd->roots_cnt_rw[order_i];

        if (total_size <= offset) {
            offset -= total_size;
            continue;
        }

        size_t root_i = offset / tree_sizes[order_i];
        size_t root_offset = offset % tree_sizes[order_i];

        while (root_i < rd->roots_cnt_rw[order_i] && 0 < cnt) {
            size_t cur_cnt =
                ZETA_GetMinOf(cnt, tree_sizes[order_i] - root_offset);
            cnt -= cur_cnt;

            size_t k = root_i;

            ReadSeg_(rd,
                     k < branch_num
                         ? rd->roots_rw[order_i][k]
                         : rd->redundant_roots_rw[order_i][k - branch_num],
                     order_i, root_offset, cur_cnt, dst);

            ++root_i;
            root_offset = 0;

            dst += stride * cur_cnt;
        }

        offset = 0;
    }

    if (dst_cursor != NULL) {
        Zeta_RadixDeque_Access(rd, dst_cursor, NULL, ret_idx);
    }
}

void Zeta_RadixDeque_Write(void* rd_, void* pos_cursor_, size_t cnt,
                           void const* src_, void* dst_cursor_) {
    Zeta_RadixDeque* rd = rd_;
    Zeta_RadixDeque_Cursor const* pos_cursor = pos_cursor_;

    CheckRDCursor_(rd, pos_cursor);

    ZETA_DebugAssert(pos_cursor->idx <= rd->size);
    ZETA_DebugAssert(cnt <= rd->size - pos_cursor->idx);

    Zeta_RadixDeque_Cursor* dst_cursor = dst_cursor_;

    if (cnt == 0) {
        if (dst_cursor != NULL) {
            dst_cursor->rd = pos_cursor->rd;
            dst_cursor->idx = pos_cursor->idx;
            dst_cursor->seg = pos_cursor->seg;
            dst_cursor->seg_idx = pos_cursor->seg_idx;
            dst_cursor->ref = pos_cursor->ref;
        }

        return;
    }

    size_t stride = rd->stride;
    size_t seg_capacity = rd->seg_capacity;
    size_t branch_num = rd->branch_num;
    unsigned order = rd->order;

    size_t ret_idx = pos_cursor->idx + cnt;

    unsigned char const* src = src_;
    ZETA_DebugAssert(src != NULL);

    size_t offset = rd->offset + pos_cursor->idx;

    size_t tree_sizes[ZETA_RadixDeque_max_order + 1];
    tree_sizes[0] = seg_capacity;

    for (unsigned order_i = 1; order_i <= order; ++order_i) {
        tree_sizes[order_i] = tree_sizes[order_i - 1] * branch_num;
    }

    unsigned order_i = 0;

    for (; 0 < cnt; ++order_i) {
        size_t total_size = tree_sizes[order_i] * rd->roots_cnt_lw[order_i];

        if (total_size <= offset) {
            offset -= total_size;
            if (order_i == order) { break; }
            continue;
        }

        size_t root_i = offset / tree_sizes[order_i];
        size_t root_offset = offset % tree_sizes[order_i];

        while (root_i < rd->roots_cnt_lw[order_i] && 0 < cnt) {
            size_t cur_cnt =
                ZETA_GetMinOf(cnt, tree_sizes[order_i] - root_offset);
            cnt -= cur_cnt;

            size_t k = branch_num * 2 - rd->roots_cnt_lw[order_i] + root_i;

            WriteSeg_(rd,
                      k < branch_num ? rd->redundant_roots_lw[order_i][k]
                                     : rd->roots_lw[order_i][k - branch_num],
                      order_i, root_offset, cur_cnt, src);

            ++root_i;
            root_offset = 0;

            src += stride * cur_cnt;
        }

        offset = 0;

        if (order_i == order) { break; }
    }

    for (; 0 < cnt; --order_i) {
        size_t total_size = tree_sizes[order_i] * rd->roots_cnt_rw[order_i];

        if (total_size <= offset) {
            offset -= total_size;
            continue;
        }

        size_t root_i = offset / tree_sizes[order_i];
        size_t root_offset = offset % tree_sizes[order_i];

        while (root_i < rd->roots_cnt_rw[order_i] && 0 < cnt) {
            size_t cur_cnt =
                ZETA_GetMinOf(cnt, tree_sizes[order_i] - root_offset);
            cnt -= cur_cnt;

            size_t k = root_i;

            WriteSeg_(rd,
                      k < branch_num
                          ? rd->roots_rw[order_i][k]
                          : rd->redundant_roots_rw[order_i][k - branch_num],
                      order_i, root_offset, cur_cnt, src);

            ++root_i;
            root_offset = 0;

            src += stride * cur_cnt;
        }

        offset = 0;
    }

    if (dst_cursor != NULL) {
        Zeta_RadixDeque_Access(rd, dst_cursor, NULL, ret_idx);
    }
}

void* Zeta_RadixDeque_PushL(void* rd_, void* dst_cursor_, size_t cnt) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    Zeta_RadixDeque_Cursor* dst_cursor = dst_cursor_;

    if (0 < cnt) {
        size_t seg_capacity = rd->seg_capacity;

        size_t offset = rd->offset;
        size_t size = rd->size;
        size_t capacity =
            CalcCapacity_(seg_capacity, rd->branch_num, rd->order);

        ZETA_DebugAssert(cnt <= capacity - size);

        PushSeg_(
            rd, 0,
            ZETA_CeilIntDiv(cnt + seg_capacity - offset, seg_capacity) - 1);

        rd->offset =
            (offset + seg_capacity - cnt % seg_capacity) % seg_capacity;

        rd->size += cnt;
    }

    Zeta_RadixDeque_Cursor pos_cursor = Access_(rd, 0);

    if (dst_cursor != NULL) {
        dst_cursor->rd = pos_cursor.rd;
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->seg = pos_cursor.seg;
        dst_cursor->seg_idx = pos_cursor.seg_idx;
        dst_cursor->ref = pos_cursor.ref;
    }

    return pos_cursor.ref;
}

void* Zeta_RadixDeque_PushR(void* rd_, void* dst_cursor_, size_t cnt) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    Zeta_RadixDeque_Cursor* dst_cursor = dst_cursor_;

    size_t origin_size = rd->size;

    if (0 < cnt) {
        size_t seg_capacity = rd->seg_capacity;

        size_t offset = rd->offset;
        size_t size = rd->size;
        size_t capacity =
            CalcCapacity_(seg_capacity, rd->branch_num, rd->order);

        ZETA_DebugAssert(cnt <= capacity - size);

        PushSeg_(rd, 1,
                 ZETA_CeilIntDiv(offset + size + cnt, seg_capacity) -
                     ZETA_CeilIntDiv(offset + size, seg_capacity));

        rd->size += cnt;
    }

    Zeta_RadixDeque_Cursor pos_cursor = Access_(rd, origin_size);

    if (dst_cursor != NULL) {
        dst_cursor->rd = pos_cursor.rd;
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->seg = pos_cursor.seg;
        dst_cursor->seg_idx = pos_cursor.seg_idx;
        dst_cursor->ref = pos_cursor.ref;
    }

    return pos_cursor.ref;
}

void Zeta_RadixDeque_PopL(void* rd_, size_t cnt) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    size_t offset = rd->offset;
    size_t size = rd->size;

    ZETA_DebugAssert(cnt <= size);

    if (cnt == 0) { return; }

    if (cnt == size) {
        Zeta_RadixDeque_EraseAll(rd);
        return;
    }

    size_t seg_capacity = rd->seg_capacity;

    PopSeg_(rd, 0, (offset + cnt) / seg_capacity);

    rd->offset = (offset + cnt) % seg_capacity;

    rd->size -= cnt;
}

void Zeta_RadixDeque_PopR(void* rd_, size_t cnt) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    size_t offset = rd->offset;
    size_t size = rd->size;

    ZETA_DebugAssert(cnt <= size);

    if (cnt == 0) { return; }

    if (cnt == size) {
        Zeta_RadixDeque_EraseAll(rd);
        return;
    }

    size_t seg_capacity = rd->seg_capacity;

    PopSeg_(rd, 1,
            ZETA_CeilIntDiv(offset + size, seg_capacity) -
                ZETA_CeilIntDiv(offset + size - cnt, seg_capacity));

    rd->size -= cnt;
}

void Zeta_RadixDeque_EraseAll(void* rd_) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    if (rd->size == 0) { return; }

    size_t branch_num = rd->branch_num;
    unsigned order = rd->order;

    for (unsigned order_i = 0; order_i <= order; ++order_i) {
        unsigned roots_cnt =
            ZETA_GetMinOf(branch_num, rd->roots_cnt_lw[order_i]);
        unsigned redundant_cnt = rd->roots_cnt_lw[order_i] - roots_cnt;

        for (unsigned root_i = 0; root_i < roots_cnt; ++root_i) {
            EraseTree_(rd, rd->roots_lw[order_i][root_i], order_i);
        }

        for (unsigned root_i = 0; root_i < redundant_cnt; ++root_i) {
            EraseTree_(rd, rd->redundant_roots_lw[order_i][root_i], order_i);
        }
    }

    for (unsigned order_i = 0; order_i <= order; ++order_i) {
        unsigned roots_cnt =
            ZETA_GetMinOf(branch_num, rd->roots_cnt_rw[order_i]);
        unsigned redundant_cnt = rd->roots_cnt_rw[order_i] - roots_cnt;

        for (unsigned root_i = 0; root_i < roots_cnt; ++root_i) {
            EraseTree_(rd, rd->roots_rw[order_i][root_i], order_i);
        }

        for (unsigned root_i = 0; root_i < redundant_cnt; ++root_i) {
            EraseTree_(rd, rd->redundant_roots_rw[order_i][root_i], order_i);
        }
    }

    rd->offset = 0;
    rd->size = 0;
}

void Zeta_RadixDeque_Check(void* rd_) {
    Zeta_RadixDeque* rd = rd_;
    ZETA_DebugAssert(rd != NULL);

    size_t width = rd->width;
    size_t stride = rd->stride;
    size_t seg_capacity = rd->seg_capacity;
    size_t branch_num = rd->branch_num;
    unsigned order = rd->order;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    ZETA_DebugAssert(0 < seg_capacity);
    ZETA_DebugAssert(seg_capacity <= ZETA_RadixDeque_max_seg_capacity);

    ZETA_DebugAssert(ZETA_RadixDeque_min_branch_num <= branch_num);
    ZETA_DebugAssert(branch_num <= ZETA_RadixDeque_max_branch_num);

    ZETA_DebugAssert(order <= ZETA_RadixDeque_max_order);

    size_t offset = rd->offset;

    ZETA_DebugAssert(offset < seg_capacity);

    size_t size = rd->size;

    size_t capacity =
        CalcCapacity_(rd->seg_capacity, rd->branch_num, rd->order);

    ZETA_DebugAssert(size <= capacity);

    ZETA_DebugAssert(rd->node_allocator != NULL);
    ZETA_DebugAssert(rd->node_allocator->Allocate != NULL);
    ZETA_DebugAssert(rd->node_allocator->Deallocate != NULL);

    ZETA_DebugAssert(rd->seg_allocator != NULL);
    ZETA_DebugAssert(rd->seg_allocator->Allocate != NULL);
    ZETA_DebugAssert(rd->seg_allocator->Deallocate != NULL);

    size_t tree_size = seg_capacity;

    size_t size_acc = 0;

    for (unsigned order_i = 0; order_i <= order;
         ++order_i, tree_size *= branch_num) {
        size_acc +=
            tree_size * (rd->roots_cnt_lw[order_i] + rd->roots_cnt_rw[order_i]);
    }

    size_t first_seg_l_vacant = offset;
    size_t last_seg_r_vacant = ZETA_ModAddInv(offset + size, seg_capacity);

    ZETA_DebugAssert(size_acc == size + first_seg_l_vacant + last_seg_r_vacant);
}

static void CheckTree_(Zeta_RadixDeque* rd, Zeta_MemRecorder* dst_node,
                       Zeta_MemRecorder* dst_seg, void* n, unsigned order_i) {
    ZETA_DebugAssert(n != NULL);

    if (order_i == 0) {
        if (dst_seg != NULL) {
            Zeta_MemRecorder_Record(dst_seg, n, rd->stride * rd->seg_capacity);
        }

        return;
    }

    if (dst_node != NULL) {
        Zeta_MemRecorder_Record(dst_node, n, sizeof(void*) * rd->branch_num);
    }

    for (unsigned child_i = 0; child_i < rd->branch_num; ++child_i) {
        CheckTree_(rd, dst_node, dst_seg, ((void**)n)[child_i], order_i - 1);
    }
}

void Zeta_RadixDeque_Sanitize(void* rd_, Zeta_MemRecorder* dst_node,
                              Zeta_MemRecorder* dst_seg) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    ZETA_DebugAssert(dst_node != NULL);
    ZETA_DebugAssert(dst_seg != NULL);

    size_t branch_num = rd->branch_num;
    unsigned order = rd->order;

    for (unsigned order_i = 0; order_i <= order; ++order_i) {
        ZETA_DebugAssert(rd->roots_lw[order_i] != NULL);
        ZETA_DebugAssert(rd->roots_rw[order_i] != NULL);
        ZETA_DebugAssert(rd->redundant_roots_lw[order_i] != NULL);
        ZETA_DebugAssert(rd->redundant_roots_rw[order_i] != NULL);

        if (dst_node != NULL) {
            Zeta_MemRecorder_Record(dst_node, rd->roots_lw[order_i],
                                    sizeof(void*) * branch_num);
            Zeta_MemRecorder_Record(dst_node, rd->roots_rw[order_i],
                                    sizeof(void*) * branch_num);
            Zeta_MemRecorder_Record(dst_node, rd->redundant_roots_lw[order_i],
                                    sizeof(void*) * branch_num);
            Zeta_MemRecorder_Record(dst_node, rd->redundant_roots_rw[order_i],
                                    sizeof(void*) * branch_num);
        }
    }

    for (unsigned order_i = 0; order_i <= order; ++order_i) {
        unsigned cnt_a = ZETA_GetMinOf(branch_num, rd->roots_cnt_lw[order_i]);
        unsigned cnt_b = rd->roots_cnt_lw[order_i] - cnt_a;

        for (unsigned root_i = 0; root_i < cnt_a; ++root_i) {
            CheckTree_(rd, dst_node, dst_seg, rd->roots_lw[order_i][root_i],
                       order_i);
        }

        for (unsigned root_i = 0; root_i < cnt_b; ++root_i) {
            CheckTree_(rd, dst_node, dst_seg,
                       rd->redundant_roots_lw[order_i][root_i], order_i);
        }
    }

    for (unsigned order_i = 0; order_i <= order; ++order_i) {
        unsigned cnt_a = ZETA_GetMinOf(branch_num, rd->roots_cnt_rw[order_i]);
        unsigned cnt_b = rd->roots_cnt_rw[order_i] - cnt_a;

        for (unsigned root_i = 0; root_i < cnt_a; ++root_i) {
            CheckTree_(rd, dst_node, dst_seg, rd->roots_rw[order_i][root_i],
                       order_i);
        }

        for (unsigned root_i = 0; root_i < cnt_b; ++root_i) {
            CheckTree_(rd, dst_node, dst_seg,
                       rd->redundant_roots_rw[order_i][root_i], order_i);
        }
    }
}

bool_t Zeta_RadixDeque_Cursor_IsEqual(void* rd_, void const* cursor_a_,
                                      void const* cursor_b_) {
    Zeta_RadixDeque* rd = rd_;

    Zeta_RadixDeque_Cursor const* cursor_a = cursor_a_;
    Zeta_RadixDeque_Cursor const* cursor_b = cursor_b_;

    CheckRDCursor_(rd, cursor_a);
    CheckRDCursor_(rd, cursor_b);

    return cursor_a->idx == cursor_b->idx;
}

int Zeta_RadixDeque_Cursor_Compare(void* rd_, void const* cursor_a_,
                                   void const* cursor_b_) {
    Zeta_RadixDeque* rd = rd_;

    Zeta_RadixDeque_Cursor const* cursor_a = cursor_a_;
    Zeta_RadixDeque_Cursor const* cursor_b = cursor_b_;

    CheckRDCursor_(rd, cursor_a);
    CheckRDCursor_(rd, cursor_b);

    size_t ka = cursor_a->idx + 1;
    size_t kb = cursor_b->idx + 1;

    if (ka < kb) { return -1; }
    if (kb < ka) { return 1; }
    return 0;
}

size_t Zeta_RadixDeque_Cursor_GetDist(void* rd_, void const* cursor_a_,
                                      void const* cursor_b_) {
    Zeta_RadixDeque* rd = rd_;

    Zeta_RadixDeque_Cursor const* cursor_a = cursor_a_;
    Zeta_RadixDeque_Cursor const* cursor_b = cursor_b_;

    CheckRDCursor_(rd, cursor_a);
    CheckRDCursor_(rd, cursor_b);

    return cursor_a->idx - cursor_b->idx;
}

size_t Zeta_RadixDeque_Cursor_GetIdx(void* rd_, void const* cursor_) {
    Zeta_RadixDeque* rd = rd_;
    Zeta_RadixDeque_Cursor const* cursor = cursor_;

    CheckRDCursor_(rd, cursor);

    return cursor->idx;
}

void Zeta_RadixDeque_Cursor_StepL(void* rd, void* cursor) {
    Zeta_RadixDeque_Cursor_AdvanceL(rd, cursor, 1);
}

void Zeta_RadixDeque_Cursor_StepR(void* rd, void* cursor) {
    Zeta_RadixDeque_Cursor_AdvanceR(rd, cursor, 1);
}

void Zeta_RadixDeque_Cursor_AdvanceL(void* rd_, void* cursor_, size_t step) {
    Zeta_RadixDeque* rd = rd_;
    Zeta_RadixDeque_Cursor* cursor = cursor_;

    CheckRDCursor_(rd, cursor);

    if (step == 0) { return; }

    ZETA_DebugAssert(step <= cursor->idx + 1);

    Zeta_RadixDeque_Access(rd, cursor, NULL, cursor->idx - step);
}

void Zeta_RadixDeque_Cursor_AdvanceR(void* rd_, void* cursor_, size_t step) {
    Zeta_RadixDeque* rd = rd_;
    Zeta_RadixDeque_Cursor* cursor = cursor_;

    CheckRDCursor_(rd, cursor);

    if (step == 0) { return; }

    ZETA_DebugAssert(step <= rd->size - cursor->idx);

    Zeta_RadixDeque_Access(rd, cursor, NULL, cursor->idx + step);
}

void Zeta_RadixDeque_Cursor_Check(void* rd_, void const* cursor_) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    Zeta_RadixDeque_Cursor const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    Zeta_RadixDeque_Cursor re_cursor;
    Zeta_RadixDeque_Access(rd, &re_cursor, NULL, cursor->idx);

    ZETA_DebugAssert(re_cursor.rd == cursor->rd);
    ZETA_DebugAssert(re_cursor.idx == cursor->idx);
    ZETA_DebugAssert(re_cursor.seg == cursor->seg);
    ZETA_DebugAssert(re_cursor.seg_idx == cursor->seg_idx);
    ZETA_DebugAssert(re_cursor.ref == cursor->ref);
}

void Zeta_RadixDeque_DeploySeqContainer(void* rd_,
                                        Zeta_SeqContainer* seq_cntr) {
    Zeta_RadixDeque* rd = rd_;
    CheckRD_(rd);

    ZETA_DebugAssert(seq_cntr != NULL);

    Zeta_SeqContainer_Init(seq_cntr);

    seq_cntr->context = rd;

    seq_cntr->cursor_size = sizeof(Zeta_RadixDeque_Cursor);

    seq_cntr->GetStride = Zeta_RadixDeque_GetStride;

    seq_cntr->GetWidth = Zeta_RadixDeque_GetWidth;

    seq_cntr->GetSize = Zeta_RadixDeque_GetSize;

    seq_cntr->GetLBCursor = Zeta_RadixDeque_GetLBCursor;

    seq_cntr->GetRBCursor = Zeta_RadixDeque_GetRBCursor;

    seq_cntr->PeekL = Zeta_RadixDeque_PeekL;

    seq_cntr->PeekR = Zeta_RadixDeque_PeekR;

    seq_cntr->Access = Zeta_RadixDeque_Access;

    seq_cntr->Refer = Zeta_RadixDeque_Refer;

    seq_cntr->Read = Zeta_RadixDeque_Read;

    seq_cntr->Write = Zeta_RadixDeque_Write;

    seq_cntr->PushL = Zeta_RadixDeque_PushL;

    seq_cntr->PushR = Zeta_RadixDeque_PushR;

    seq_cntr->PopL = Zeta_RadixDeque_PopL;

    seq_cntr->PopR = Zeta_RadixDeque_PopR;

    seq_cntr->EraseAll = Zeta_RadixDeque_EraseAll;

    seq_cntr->Cursor_IsEqual = Zeta_RadixDeque_Cursor_IsEqual;

    seq_cntr->Cursor_Compare = Zeta_RadixDeque_Cursor_Compare;

    seq_cntr->Cursor_GetDist = Zeta_RadixDeque_Cursor_GetDist;

    seq_cntr->Cursor_GetIdx = Zeta_RadixDeque_Cursor_GetIdx;

    seq_cntr->Cursor_StepL = Zeta_RadixDeque_Cursor_StepL;

    seq_cntr->Cursor_StepR = Zeta_RadixDeque_Cursor_StepR;

    seq_cntr->Cursor_AdvanceL = Zeta_RadixDeque_Cursor_AdvanceL;

    seq_cntr->Cursor_AdvanceR = Zeta_RadixDeque_Cursor_AdvanceR;
}
