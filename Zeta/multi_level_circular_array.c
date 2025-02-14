#include "multi_level_circular_array.h"

#include "debugger.h"
#include "utils.h"

#if ZETA_EnableDebug

#define CheckCntr_(mlca) Zeta_MultiLevelCircularArray_Check((mlca))

#define CheckCursor_(mlca, cursor) \
    Zeta_MultiLevelCircularArray_Cursor_Check((mlca), (cursor))

#else

#define CheckCntr_(mlca)

#define CheckCursor_(mlca, cursor)

#endif

#define L (-1)
#define R (1)

#define branch_num ZETA_MultiLevelCircularArray_branch_num

static unsigned short const branch_nums[] = {
    [0 ... ZETA_MultiLevelTable_max_level - 1] = branch_num
};

#define GetLSeg_(seg)                                         \
    ZETA_MemberToStruct(Zeta_MultiLevelCircularArray_Seg, ln, \
                        Zeta_OrdLListNode_GetL(&(seg)->ln))

#define GetRSeg_(seg)                                         \
    ZETA_MemberToStruct(Zeta_MultiLevelCircularArray_Seg, ln, \
                        Zeta_OrdLListNode_GetR(&(seg)->ln))

#define AllocateMLTNode_(node_allocator)                      \
    ZETA_Allocator_SafeAllocate(                              \
        (node_allocator), alignof(Zeta_MultiLevelTable_Node), \
        offsetof(Zeta_MultiLevelTable_Node, ptrs[branch_num]));

void Zeta_MultiLevelCircularArray_Init(void* mlca_) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    ZETA_DebugAssert(mlca != NULL);

    size_t width = mlca->width;
    ZETA_DebugAssert(0 < width);

    size_t stride = mlca->stride;
    ZETA_DebugAssert(width <= stride);

    size_t seg_capacity = mlca->seg_capacity;
    ZETA_DebugAssert(0 < seg_capacity);
    ZETA_DebugAssert(seg_capacity <=
                     ZETA_MultiLevelCircularArray_max_seg_capacity);

    mlca->offset = 0;

    mlca->size = 0;

    mlca->level = 1;

    mlca->rotations[0] = 0;

    mlca->root = NULL;

    ZETA_Allocator_Check(mlca->node_allocator, alignof(void*));
    ZETA_Allocator_Check(mlca->seg_allocator,
                         alignof(Zeta_MultiLevelCircularArray_Seg));

    mlca->seg_head = ZETA_Allocator_SafeAllocate(
        mlca->seg_allocator, alignof(Zeta_MultiLevelCircularArray_Seg),
        offsetof(Zeta_MultiLevelCircularArray_Seg, data[0]));

    Zeta_OrdLListNode_Init(&mlca->seg_head->ln);
}

void Zeta_MultiLevelCircularArray_Deinit(void* mlca_) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    CheckCntr_(mlca);

    Zeta_MultiLevelCircularArray_EraseAll(mlca);

    ZETA_Allocator_Deallocate(mlca->seg_allocator, mlca->seg_head);
}

size_t Zeta_MultiLevelCircularArray_GetWidth(void const* mlca_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    CheckCntr_(mlca);

    return mlca->width;
}

size_t Zeta_MultiLevelCircularArray_GetStride(void const* mlca_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    CheckCntr_(mlca);

    return mlca->stride;
}

size_t Zeta_MultiLevelCircularArray_GetSize(void const* mlca_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    CheckCntr_(mlca);

    return mlca->size;
}

size_t Zeta_MultiLevelCircularArray_GetCapacity(void const* mlca_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    CheckCntr_(mlca);

    size_t seg_capacity = mlca->seg_capacity;

    int level = Zeta_FloorLog(ZETA_max_capacity / seg_capacity, branch_num);

    return level <= 2 ? seg_capacity * Zeta_Power(branch_num, level)
                      : seg_capacity * Zeta_Power(branch_num, level - 1) *
                            (branch_num - 2);
}

void Zeta_MultiLevelCircularArray_GetLBCursor(void const* mlca_,
                                              void* dst_cursor_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    CheckCntr_(mlca);

    Zeta_MultiLevelCircularArray_Cursor* dst_cursor = dst_cursor_;

    if (dst_cursor == NULL) { return; }

    dst_cursor->idx = -1;
    dst_cursor->seg = mlca->seg_head;
    dst_cursor->seg_slot_idx = 0;
    dst_cursor->elem = NULL;
}

void Zeta_MultiLevelCircularArray_GetRBCursor(void const* mlca_,
                                              void* dst_cursor_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    CheckCntr_(mlca);

    Zeta_MultiLevelCircularArray_Cursor* dst_cursor = dst_cursor_;

    if (dst_cursor == NULL) { return; }

    dst_cursor->idx = mlca->size;
    dst_cursor->seg = mlca->seg_head;
    dst_cursor->seg_slot_idx = 0;
    dst_cursor->elem = NULL;
}

void* Zeta_MultiLevelCircularArray_PeekL(void* mlca_, void* dst_cursor_,
                                         void* dst_elem) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    CheckCntr_(mlca);

    Zeta_MultiLevelCircularArray_Cursor* dst_cursor = dst_cursor_;

    size_t width = mlca->width;

    size_t stride = mlca->stride;

    size_t seg_capacity = mlca->seg_capacity;

    size_t offset = mlca->offset;

    Zeta_MultiLevelCircularArray_Seg* seg_head = mlca->seg_head;

    Zeta_MultiLevelCircularArray_Seg* seg = GetRSeg_(seg_head);

    size_t seg_slot_idx = offset % seg_capacity;

    void* elem = seg == seg_head ? NULL : seg->data + stride * seg_slot_idx;

    if (dst_cursor != NULL) {
        dst_cursor->mlca = mlca;
        dst_cursor->idx = 0;
        dst_cursor->seg = seg;
        dst_cursor->seg_slot_idx = seg_slot_idx;
        dst_cursor->elem = elem;
    }

    if (dst_elem != NULL && elem != NULL) {
        Zeta_MemCopy(dst_elem, elem, width);
    }

    return elem;
}

void* Zeta_MultiLevelCircularArray_PeekR(void* mlca_, void* dst_cursor_,
                                         void* dst_elem) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    CheckCntr_(mlca);

    Zeta_MultiLevelCircularArray_Cursor* dst_cursor = dst_cursor_;

    size_t width = mlca->width;

    size_t stride = mlca->stride;

    size_t seg_capacity = mlca->seg_capacity;

    size_t offset = mlca->offset;

    size_t size = mlca->size;

    Zeta_MultiLevelCircularArray_Seg* seg_head = mlca->seg_head;

    Zeta_MultiLevelCircularArray_Seg* seg = GetLSeg_(seg_head);

    size_t seg_slot_idx = size == 0 ? 0 : (offset + size - 1) % seg_capacity;

    void* elem = seg == seg_head ? NULL : seg->data + stride * seg_slot_idx;

    if (dst_cursor != NULL) {
        dst_cursor->mlca = mlca;
        dst_cursor->idx = 0;
        dst_cursor->seg = seg;
        dst_cursor->seg_slot_idx = seg_slot_idx;
        dst_cursor->elem = elem;
    }

    if (dst_elem != NULL && elem != NULL) {
        Zeta_MemCopy(dst_elem, elem, width);
    }

    return elem;
}

void* Zeta_MultiLevelCircularArray_Access(void* mlca_, size_t idx,
                                          void* dst_cursor_, void* dst_elem) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    CheckCntr_(mlca);

    Zeta_MultiLevelCircularArray_Cursor* dst_cursor = dst_cursor_;

    size_t width = mlca->width;

    size_t stride = mlca->stride;

    size_t seg_capacity = mlca->seg_capacity;

    size_t offset = mlca->offset;

    size_t size = mlca->size;

    ZETA_DebugAssert(idx + 1 < size + 2);

    if (idx == (size_t)(-1) || idx == size) {
        if (dst_cursor != NULL) {
            dst_cursor->mlca = mlca;
            dst_cursor->idx = idx;
            dst_cursor->seg = mlca->seg_head;
            dst_cursor->seg_slot_idx = 0;
            dst_cursor->elem = NULL;
        }

        return NULL;
    }

    int level = mlca->level;

    size_t const* rotations = mlca->rotations;

    Zeta_MultiLevelTable mlt;
    mlt.level = level;
    mlt.branch_nums = branch_nums;
    mlt.size = ZETA_UnsafeCeilIntDiv(offset + size, seg_capacity);
    mlt.root = mlca->root;
    mlt.node_allocator = mlca->node_allocator;

    size_t slot_idx = offset + idx;

    size_t seg_idx = slot_idx / seg_capacity;
    size_t seg_slot_idx = slot_idx % seg_capacity;

    size_t idxes[ZETA_MultiLevelTable_max_level];

    {
        size_t tmp = seg_idx;

        for (int level_i = 0; level_i < level; ++level_i) {
            idxes[level_i] = (rotations[level_i] + tmp) % branch_num;
            tmp /= branch_num;
        }
    }

    Zeta_MultiLevelCircularArray_Seg* seg =
        *Zeta_MultiLevelTable_Access(&mlt, idxes);

    void* elem = seg->data + stride * seg_slot_idx;

    if (dst_cursor != NULL) {
        dst_cursor->mlca = mlca;
        dst_cursor->idx = idx;
        dst_cursor->seg = seg;
        dst_cursor->seg_slot_idx = seg_slot_idx;
        dst_cursor->elem = elem;
    }

    if (dst_elem != NULL) { Zeta_MemCopy(dst_elem, elem, width); }

    return elem;
}

void* Zeta_MultiLevelCircularArray_Refer(void* mlca_, void const* pos_cursor_) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;

    Zeta_MultiLevelCircularArray_Cursor const* pos_cursor = pos_cursor_;

    CheckCursor_(mlca, pos_cursor);

    return pos_cursor->elem;
}

ZETA_DeclareStruct(AssignRet);

struct AssignRet {
    Zeta_MultiLevelCircularArray_Seg* dst_seg;
    size_t dst_seg_slot_idx;
    Zeta_MultiLevelCircularArray_Seg* src_seg;
    size_t src_seg_slot_idx;
};

static AssignRet AssignL_(Zeta_MultiLevelCircularArray const* mlca,
                          Zeta_MultiLevelCircularArray_Seg* dst_seg,
                          size_t dst_seg_slot_idx,
                          Zeta_MultiLevelCircularArray_Seg* src_seg,
                          size_t src_seg_slot_idx, size_t cnt) {
    size_t width = mlca->width;

    size_t stride = mlca->stride;

    size_t seg_capacity = mlca->seg_capacity;

    size_t offset = mlca->offset;

    size_t size = mlca->size;

    Zeta_MultiLevelCircularArray_Seg* seg_head = mlca->seg_head;

    if (cnt == 0) {
        return (AssignRet){
            .dst_seg = dst_seg,
            .dst_seg_slot_idx = dst_seg_slot_idx,
            .src_seg = src_seg,
            .src_seg_slot_idx = src_seg_slot_idx,
        };
    }

    size_t last_seg_size =
        (offset + size + seg_capacity - 1) % seg_capacity + 1;

    if (dst_seg_slot_idx == 0) {
        dst_seg_slot_idx = dst_seg == seg_head ? last_seg_size : seg_capacity;
        dst_seg = GetLSeg_(dst_seg);
    }

    if (src_seg_slot_idx == 0) {
        src_seg_slot_idx = src_seg == seg_head ? last_seg_size : seg_capacity;
        src_seg = GetLSeg_(src_seg);
    }

    while (0 < cnt) {
        if (dst_seg_slot_idx == 0) {
            dst_seg = GetLSeg_(dst_seg);
            dst_seg_slot_idx = seg_capacity;
        }

        if (src_seg_slot_idx == 0) {
            src_seg = GetLSeg_(src_seg);
            src_seg_slot_idx = seg_capacity;
        }

        size_t cur_cnt = ZETA_GetMinOf(cnt, dst_seg_slot_idx, src_seg_slot_idx);
        cnt -= cur_cnt;

        dst_seg_slot_idx -= cur_cnt;
        src_seg_slot_idx -= cur_cnt;

        Zeta_ElemMove(dst_seg->data + stride * dst_seg_slot_idx,
                      src_seg->data + stride * src_seg_slot_idx, width, stride,
                      stride, cur_cnt);
    }

    return (AssignRet){
        .dst_seg = dst_seg,
        .dst_seg_slot_idx = dst_seg_slot_idx,
        .src_seg = src_seg,
        .src_seg_slot_idx = src_seg_slot_idx,
    };
}

static AssignRet AssignR_(Zeta_MultiLevelCircularArray const* mlca,
                          Zeta_MultiLevelCircularArray_Seg* dst_seg,
                          size_t dst_seg_slot_idx,
                          Zeta_MultiLevelCircularArray_Seg* src_seg,
                          size_t src_seg_slot_idx, size_t cnt) {
    size_t width = mlca->width;

    size_t stride = mlca->stride;

    size_t seg_capacity = mlca->seg_capacity;

    while (0 < cnt) {
        size_t cur_cnt = ZETA_GetMinOf(cnt, seg_capacity - dst_seg_slot_idx,
                                       seg_capacity - src_seg_slot_idx);
        cnt -= cur_cnt;

        Zeta_ElemMove(dst_seg->data + stride * dst_seg_slot_idx,
                      src_seg->data + stride * src_seg_slot_idx, width, stride,
                      stride, cur_cnt);

        dst_seg_slot_idx += cur_cnt;
        src_seg_slot_idx += cur_cnt;

        if (dst_seg_slot_idx == seg_capacity) {
            dst_seg = GetRSeg_(dst_seg);
            dst_seg_slot_idx = 0;
        }

        if (src_seg_slot_idx == seg_capacity) {
            src_seg = GetRSeg_(src_seg);
            src_seg_slot_idx = 0;
        }
    }

    return (AssignRet){
        .dst_seg = dst_seg,
        .dst_seg_slot_idx = dst_seg_slot_idx,
        .src_seg = src_seg,
        .src_seg_slot_idx = src_seg_slot_idx,
    };
}

void Zeta_MultiLevelCircularArray_Read(void const* mlca_,
                                       void const* pos_cursor_, size_t cnt,
                                       void* dst, size_t dst_stride,
                                       void* dst_cursor_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;

    Zeta_MultiLevelCircularArray_Cursor const* pos_cursor = pos_cursor_;

    CheckCursor_(mlca, pos_cursor);

    Zeta_MultiLevelCircularArray_Cursor* dst_cursor = dst_cursor_;

    size_t width = mlca->width;

    size_t stride = mlca->stride;

    size_t seg_capacity = mlca->seg_capacity;

    size_t size = mlca->size;

    Zeta_MultiLevelCircularArray_Seg* seg_head = mlca->seg_head;

    size_t idx = pos_cursor->idx;

    ZETA_DebugAssert(idx <= size);
    ZETA_DebugAssert(cnt <= size - idx);

    size_t end = idx + cnt;

    Zeta_MultiLevelCircularArray_Seg* seg = pos_cursor->seg;
    size_t seg_slot_idx = pos_cursor->seg_slot_idx;

    while (0 < cnt) {
        size_t cur_cnt = ZETA_GetMinOf(cnt, seg_capacity - seg_slot_idx);
        cnt -= cur_cnt;

        Zeta_ElemCopy(dst, seg->data + stride * seg_slot_idx, width, dst_stride,
                      stride, cur_cnt);

        dst += dst_stride * cur_cnt;

        seg_slot_idx += cur_cnt;

        if (seg_slot_idx == seg_capacity) {
            seg = GetRSeg_(seg);
            seg_slot_idx = 0;
        }
    }

    if (end == size) {
        seg = seg_head;
        seg_slot_idx = 0;
    }

    if (dst_cursor != NULL) {
        dst_cursor->mlca = mlca;
        dst_cursor->idx = end;
        dst_cursor->seg = seg;
        dst_cursor->seg_slot_idx = seg_slot_idx;
        dst_cursor->elem =
            seg == seg_head ? NULL : seg->data + stride * seg_slot_idx;
    }
}

void Zeta_MultiLevelCircularArray_Write(void* mlca_, void* pos_cursor_,
                                        size_t cnt, void const* src,
                                        size_t src_stride, void* dst_cursor_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;

    Zeta_MultiLevelCircularArray_Cursor const* pos_cursor = pos_cursor_;

    CheckCursor_(mlca, pos_cursor);

    Zeta_MultiLevelCircularArray_Cursor* dst_cursor = dst_cursor_;

    size_t width = mlca->width;

    size_t stride = mlca->stride;

    size_t seg_capacity = mlca->seg_capacity;

    size_t size = mlca->size;

    Zeta_MultiLevelCircularArray_Seg* seg_head = mlca->seg_head;

    size_t idx = pos_cursor->idx;

    ZETA_DebugAssert(idx <= size);
    ZETA_DebugAssert(cnt <= size - idx);

    size_t end = idx + cnt;

    Zeta_MultiLevelCircularArray_Seg* seg = pos_cursor->seg;
    size_t seg_slot_idx = pos_cursor->seg_slot_idx;

    while (0 < cnt) {
        size_t cur_cnt = ZETA_GetMinOf(cnt, seg_capacity - seg_slot_idx);
        cnt -= cur_cnt;

        Zeta_ElemCopy(seg->data + stride * seg_slot_idx, src, width, stride,
                      src_stride, cur_cnt);

        src += src_stride * cur_cnt;

        seg_slot_idx += cur_cnt;

        if (seg_slot_idx == seg_capacity) {
            seg = GetRSeg_(seg);
            seg_slot_idx = 0;
        }
    }

    if (end == size) {
        seg = seg_head;
        seg_slot_idx = 0;
    }

    if (dst_cursor != NULL) {
        dst_cursor->mlca = mlca;
        dst_cursor->idx = end;
        dst_cursor->seg = seg;
        dst_cursor->seg_slot_idx = seg_slot_idx;
        dst_cursor->elem =
            seg == seg_head ? NULL : seg->data + stride * seg_slot_idx;
    }
}

static size_t PushSegs_(Zeta_MultiLevelCircularArray* mlca, int level_i,
                        Zeta_MultiLevelTable_Node* mlt_node, size_t idx,
                        size_t cnt, Zeta_MultiLevelCircularArray_Seg* tail) {
    size_t stride = mlca->stride;

    size_t seg_capacity = mlca->seg_capacity;

    size_t rotation = mlca->rotations[level_i];

    if (level_i == 0) {
        Zeta_Allocator seg_allocator = mlca->seg_allocator;

        size_t acc_cnt = 0;

        while (acc_cnt < cnt && idx < branch_num) {
            size_t i = (rotation + idx) % branch_num;

            mlt_node->active_map += 1ULL << i;

            Zeta_MultiLevelCircularArray_Seg* ins_seg =
                ZETA_Allocator_SafeAllocate(
                    seg_allocator, alignof(Zeta_MultiLevelCircularArray_Seg),
                    offsetof(Zeta_MultiLevelCircularArray_Seg,
                             data[stride * seg_capacity]));

            mlt_node->ptrs[i] = ins_seg;

            Zeta_OrdLListNode_Init(&ins_seg->ln);
            Zeta_LList_OrdLListNode_InsertL(&tail->ln, &ins_seg->ln);

            ++idx;
            ++acc_cnt;
        }

        return acc_cnt;
    }

    Zeta_Allocator node_allocator = mlca->node_allocator;

    size_t sub_tree_seg_capacity = Zeta_Power(branch_num, level_i);

    size_t sub_tree_idx = idx / sub_tree_seg_capacity;
    size_t sub_idx = idx % sub_tree_seg_capacity;

    size_t acc_cnt = 0;

    while (acc_cnt < cnt && sub_tree_idx < branch_num) {
        size_t i = (rotation + sub_tree_idx) % branch_num;

        void** ptr = mlt_node->ptrs + i;

        if ((mlt_node->active_map >> i) % 2 == 0) {
            mlt_node->active_map += 1ULL << i;

            Zeta_MultiLevelTable_Node* ins_mlt_node =
                AllocateMLTNode_(node_allocator);

            ins_mlt_node->active_map = 0;

            *ptr = ins_mlt_node;
        }

        size_t cur_cnt =
            PushSegs_(mlca, level_i - 1, *ptr, sub_idx, cnt - acc_cnt, tail);

        acc_cnt += cur_cnt;

        ++sub_tree_idx;
        sub_idx = 0;
    }

    return acc_cnt;
}

static void* Push_(void* mlca_, size_t cnt, void* dst_cursor_, int dir) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    CheckCntr_(mlca);

    Zeta_MultiLevelCircularArray_Cursor* dst_cursor = dst_cursor_;

    size_t seg_capacity = mlca->seg_capacity;

    size_t offset = mlca->offset;

    size_t size = mlca->size;

    if (cnt == 0) {
        if (dir == L) {
            return Zeta_MultiLevelCircularArray_PeekL(mlca, dst_cursor, NULL);
        }

        Zeta_MultiLevelCircularArray_GetRBCursor(mlca, dst_cursor);
        return NULL;
    }

    size_t capacity = Zeta_MultiLevelCircularArray_GetCapacity(mlca);
    ZETA_DebugAssert(size + cnt <= capacity);

    size_t* rotations = mlca->rotations;

    Zeta_Allocator node_allocator = mlca->node_allocator;

    size_t cur_seg_slot_offset = offset % seg_capacity;
    size_t nxt_seg_slot_offset =
        dir == L ? (cur_seg_slot_offset + seg_capacity - cnt % seg_capacity) %
                       seg_capacity
                 : cur_seg_slot_offset;

    size_t cur_size = size;
    size_t nxt_size = cur_size + cnt;

    size_t cur_tree_segs_cnt =
        ZETA_UnsafeCeilIntDiv(cur_seg_slot_offset + cur_size, seg_capacity);
    size_t nxt_tree_segs_cnt =
        ZETA_UnsafeCeilIntDiv(nxt_seg_slot_offset + nxt_size, seg_capacity);
    size_t del_segs_cnt = nxt_tree_segs_cnt - cur_tree_segs_cnt;

    Zeta_MultiLevelTable mlt;
    mlt.level = mlca->level;
    mlt.branch_nums = branch_nums;
    mlt.size = cur_tree_segs_cnt;
    mlt.root = mlca->root;
    mlt.node_allocator = node_allocator;

    Zeta_MultiLevelCircularArray_Seg* seg_head = mlca->seg_head;

    size_t sub_tree_seg_capacity = Zeta_Power(branch_num, mlt.level - 1);

    size_t tree_seg_offset = offset / seg_capacity;

    if (dir == L) {
        size_t cur_sub_trees_cnt = ZETA_UnsafeCeilIntDiv(
            tree_seg_offset + cur_tree_segs_cnt, sub_tree_seg_capacity);

        size_t rot = ZETA_GetMinOf(
            branch_num - cur_sub_trees_cnt,
            ZETA_UnsafeCeilIntDiv(
                ZETA_GetMaxOf(del_segs_cnt, tree_seg_offset) - tree_seg_offset,
                sub_tree_seg_capacity));

        rotations[mlt.level - 1] =
            (rotations[mlt.level - 1] + branch_num - rot) % branch_num;

        tree_seg_offset += sub_tree_seg_capacity * rot;
    }

    while (dir == L ? tree_seg_offset < del_segs_cnt
                    : sub_tree_seg_capacity * branch_num <
                          tree_seg_offset + cur_tree_segs_cnt + del_segs_cnt) {
        if (mlt.root != NULL) {
            Zeta_MultiLevelTable_Node* new_root =
                AllocateMLTNode_(node_allocator);

            if (dir == L) {
                new_root->active_map = 1ULL << (branch_num - 1);
                new_root->ptrs[branch_num - 1] = mlt.root;
            } else {
                new_root->active_map = 1ULL;
                new_root->ptrs[0] = mlt.root;
            }

            mlt.root = new_root;
        }

        ++mlt.level;

        rotations[mlt.level - 1] = 0;

        sub_tree_seg_capacity *= branch_num;

        if (dir == L) {
            tree_seg_offset += sub_tree_seg_capacity * (branch_num - 1);
        }
    }

    if (mlt.root == NULL) {
        mlt.root = AllocateMLTNode_(node_allocator);
        mlt.root->active_map = 0;
    }

    if (dir == L) {
        tree_seg_offset -= del_segs_cnt;

        PushSegs_(mlca, mlt.level - 1, mlt.root, tree_seg_offset, del_segs_cnt,
                  GetRSeg_(seg_head));
    } else {
        PushSegs_(mlca, mlt.level - 1, mlt.root,
                  tree_seg_offset + cur_tree_segs_cnt, del_segs_cnt, seg_head);
    }

    if (dir == L) {
        rotations[mlt.level - 1] = (rotations[mlt.level - 1] +
                                    tree_seg_offset / sub_tree_seg_capacity) %
                                   branch_num;

        tree_seg_offset %= sub_tree_seg_capacity;
    }

    mlca->offset = seg_capacity * tree_seg_offset + nxt_seg_slot_offset;
    mlca->size = nxt_size;

    mlca->level = mlt.level;
    mlca->root = mlt.root;

    return dir == L ? Zeta_MultiLevelCircularArray_PeekL(mlca, dst_cursor, NULL)
                    : Zeta_MultiLevelCircularArray_Access(mlca, cur_size,
                                                          dst_cursor, NULL);
}

static size_t PopSegs_(Zeta_MultiLevelCircularArray* mlca, int level_i,
                       Zeta_MultiLevelTable_Node* mlt_node, size_t idx,
                       size_t cnt) {
    size_t rotation = mlca->rotations[level_i];

    if (level_i == 0) {
        Zeta_Allocator seg_allocator = mlca->seg_allocator;

        size_t acc_cnt = 0;

        while (acc_cnt < cnt && idx < branch_num) {
            size_t i = (rotation + idx) % branch_num;

            mlt_node->active_map -= 1ULL << i;

            Zeta_MultiLevelCircularArray_Seg* era_seg = mlt_node->ptrs[i];

            Zeta_LList_OrdLListNode_Extract(&era_seg->ln);

            ZETA_Allocator_Deallocate(seg_allocator, era_seg);

            ++idx;
            ++acc_cnt;
        }

        return acc_cnt;
    }

    Zeta_Allocator node_allocator = mlca->node_allocator;

    size_t sub_tree_seg_capacity = Zeta_Power(branch_num, level_i);

    size_t sub_tree_idx = idx / sub_tree_seg_capacity;
    size_t sub_idx = idx % sub_tree_seg_capacity;

    size_t acc_cnt = 0;

    while (acc_cnt < cnt && sub_tree_idx < branch_num) {
        size_t i = (rotation + sub_tree_idx) % branch_num;

        Zeta_MultiLevelTable_Node* sub_mlt_node = mlt_node->ptrs[i];

        size_t cur_cnt =
            PopSegs_(mlca, level_i - 1, sub_mlt_node, sub_idx, cnt - acc_cnt);

        if (sub_mlt_node->active_map == 0) {
            mlt_node->active_map -= 1ULL << i;
            ZETA_Allocator_Deallocate(node_allocator, sub_mlt_node);
        }

        acc_cnt += cur_cnt;

        ++sub_tree_idx;
        sub_idx = 0;
    }

    return acc_cnt;
}

static void Pop_(void* mlca_, size_t cnt, int dir) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    CheckCntr_(mlca);

    if (cnt == 0) { return; }

    size_t seg_capacity = mlca->seg_capacity;

    size_t offset = mlca->offset;
    size_t size = mlca->size;

    ZETA_DebugAssert(cnt <= size);

    if (cnt == size) {
        Zeta_MultiLevelCircularArray_EraseAll(mlca);
        return;
    }

    size_t* rotations = mlca->rotations;

    Zeta_Allocator node_allocator = mlca->node_allocator;

    size_t cur_seg_slot_offset = offset % seg_capacity;
    size_t nxt_seg_slot_offset =
        dir == L ? (cur_seg_slot_offset + cnt) % seg_capacity
                 : cur_seg_slot_offset;

    size_t cur_size = size;
    size_t nxt_size = cur_size - cnt;

    size_t cur_tree_segs_cnt =
        ZETA_UnsafeCeilIntDiv(cur_seg_slot_offset + cur_size, seg_capacity);
    size_t nxt_tree_segs_cnt =
        ZETA_UnsafeCeilIntDiv(nxt_seg_slot_offset + nxt_size, seg_capacity);
    size_t del_tree_segs_cnt = cur_tree_segs_cnt - nxt_tree_segs_cnt;

    Zeta_MultiLevelTable mlt;
    mlt.level = mlca->level;
    mlt.branch_nums = branch_nums;
    mlt.size = cur_tree_segs_cnt;
    mlt.root = mlca->root;
    mlt.node_allocator = node_allocator;

    size_t tree_seg_offset = offset / seg_capacity;

    if (dir == L) {
        PopSegs_(mlca, mlt.level - 1, mlt.root, tree_seg_offset,
                 del_tree_segs_cnt);

        tree_seg_offset += del_tree_segs_cnt;
    } else {
        PopSegs_(mlca, mlt.level - 1, mlt.root,
                 tree_seg_offset + nxt_tree_segs_cnt, del_tree_segs_cnt);
    }

    cur_tree_segs_cnt -= del_tree_segs_cnt;

    size_t sub_tree_seg_capacity = Zeta_Power(branch_num, mlt.level - 1);

    rotations[mlt.level - 1] =
        (rotations[mlt.level - 1] + tree_seg_offset / sub_tree_seg_capacity) %
        branch_num;

    tree_seg_offset %= sub_tree_seg_capacity;

    while (2 <= mlt.level && cur_tree_segs_cnt * 2 <= sub_tree_seg_capacity) {
        size_t top_rotation = rotations[mlt.level - 1];

        Zeta_MultiLevelTable_Node* l_node = mlt.root->ptrs[top_rotation];

        size_t r_node_idx = (top_rotation + 1) % branch_num;

        if ((mlt.root->active_map >> r_node_idx) % 2 == 0) {
            ZETA_Allocator_Deallocate(node_allocator, mlt.root);

            --mlt.level;
            mlt.root = l_node;

            sub_tree_seg_capacity /= branch_num;

            rotations[mlt.level - 1] =
                (rotations[mlt.level - 1] +
                 tree_seg_offset / sub_tree_seg_capacity) %
                branch_num;

            tree_seg_offset %= sub_tree_seg_capacity;

            continue;
        }

        size_t sub_rotation = rotations[mlt.level - 2];

        Zeta_MultiLevelTable_Node* r_node = mlt.root->ptrs[r_node_idx];

        size_t l_node_size = __builtin_popcountll(l_node->active_map);
        size_t r_node_size = __builtin_popcountll(r_node->active_map);

        for (size_t i = 0; i < l_node_size; ++i) {
            mlt.root->ptrs[i] =
                l_node->ptrs[(sub_rotation + branch_num - l_node_size + i) %
                             branch_num];
        }

        for (size_t i = 0; i < r_node_size; ++i) {
            mlt.root->ptrs[l_node_size + i] =
                r_node->ptrs[(sub_rotation + i) % branch_num];
        }

        mlt.root->active_map = (1ULL << (l_node_size + r_node_size)) - 1;

        ZETA_Allocator_Deallocate(node_allocator, l_node);
        ZETA_Allocator_Deallocate(node_allocator, r_node);

        --mlt.level;

        sub_tree_seg_capacity /= branch_num;

        rotations[mlt.level - 1] = 0;

        tree_seg_offset %= sub_tree_seg_capacity;
    }

    mlca->offset = seg_capacity * tree_seg_offset + nxt_seg_slot_offset;
    mlca->size = nxt_size;

    mlca->level = mlt.level;
    mlca->root = mlt.root;
}

void* Zeta_MultiLevelCircularArray_PushL(void* mlca, size_t cnt,
                                         void* dst_cursor) {
    return Push_(mlca, cnt, dst_cursor, L);
}

void* Zeta_MultiLevelCircularArray_PushR(void* mlca, size_t cnt,
                                         void* dst_cursor) {
    return Push_(mlca, cnt, dst_cursor, R);
}

void* Zeta_MultiLevelCircularArray_Insert(void* mlca_, void* pos_cursor_,
                                          size_t cnt) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;

    Zeta_MultiLevelCircularArray_Cursor* pos_cursor = pos_cursor_;

    CheckCursor_(mlca, pos_cursor);

    if (cnt == 0) { return pos_cursor->elem; }

    size_t stride = mlca->stride;

    size_t size = mlca->size;

    size_t idx = pos_cursor->idx;

    ZETA_SeqCntr_CheckInsertable(
        idx, cnt, size, Zeta_MultiLevelCircularArray_GetCapacity(mlca));

    size_t l_size = idx;
    size_t r_size = size - l_size;

    Zeta_MultiLevelCircularArray_Cursor dst_cursor;
    Zeta_MultiLevelCircularArray_Cursor src_cursor;

    if (l_size < r_size) {
        Zeta_MultiLevelCircularArray_PushL(mlca, cnt, &dst_cursor);

        Zeta_MultiLevelCircularArray_Access(mlca, cnt, &src_cursor, NULL);

        AssignRet assign_ret =
            AssignR_(mlca, dst_cursor.seg, dst_cursor.seg_slot_idx,
                     src_cursor.seg, src_cursor.seg_slot_idx, l_size);

        pos_cursor->seg = assign_ret.dst_seg;
        pos_cursor->seg_slot_idx = assign_ret.dst_seg_slot_idx;
        pos_cursor->elem =
            assign_ret.dst_seg->data + stride * assign_ret.dst_seg_slot_idx;
    } else {
        Zeta_MultiLevelCircularArray_PushR(mlca, cnt, &src_cursor);

        Zeta_MultiLevelCircularArray_GetRBCursor(mlca, &dst_cursor);

        AssignRet assign_ret =
            AssignL_(mlca, dst_cursor.seg, dst_cursor.seg_slot_idx,
                     src_cursor.seg, src_cursor.seg_slot_idx, r_size);

        pos_cursor->seg = assign_ret.src_seg;
        pos_cursor->seg_slot_idx = assign_ret.src_seg_slot_idx;
        pos_cursor->elem =
            assign_ret.src_seg->data + stride * assign_ret.src_seg_slot_idx;
    }

    return pos_cursor->elem;
}

void Zeta_MultiLevelCircularArray_PopL(void* mlca, size_t cnt) {
    Pop_(mlca, cnt, L);
}

void Zeta_MultiLevelCircularArray_PopR(void* mlca, size_t cnt) {
    Pop_(mlca, cnt, R);
}

void Zeta_MultiLevelCircularArray_Erase(void* mlca_, void* pos_cursor_,
                                        size_t cnt) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;

    Zeta_MultiLevelCircularArray_Cursor* pos_cursor = pos_cursor_;

    CheckCursor_(mlca, pos_cursor);

    if (cnt == 0) { return; }

    size_t size = mlca->size;

    Zeta_MultiLevelCircularArray_Seg* seg_head = mlca->seg_head;

    size_t idx = pos_cursor->idx;

    ZETA_SeqCntr_CheckErasable(idx, cnt, size);

    size_t l_size = idx;
    size_t r_size = size - idx - cnt;

    Zeta_MultiLevelCircularArray_Cursor dst_cursor;
    Zeta_MultiLevelCircularArray_Cursor src_cursor;

    if (l_size < r_size) {
        dst_cursor = *pos_cursor;
        src_cursor = *pos_cursor;

        Zeta_MultiLevelCircularArray_Cursor_AdvanceR(mlca, &dst_cursor, cnt);

        pos_cursor->seg = dst_cursor.seg;
        pos_cursor->seg_slot_idx = dst_cursor.seg_slot_idx;
        pos_cursor->elem = dst_cursor.elem;

        AssignL_(mlca, dst_cursor.seg, dst_cursor.seg_slot_idx, src_cursor.seg,
                 src_cursor.seg_slot_idx, l_size);

        Zeta_MultiLevelCircularArray_PopL(mlca, cnt);
    } else {
        dst_cursor = *pos_cursor;
        src_cursor = *pos_cursor;

        Zeta_MultiLevelCircularArray_Cursor_AdvanceR(mlca, &src_cursor, cnt);

        AssignR_(mlca, dst_cursor.seg, dst_cursor.seg_slot_idx, src_cursor.seg,
                 src_cursor.seg_slot_idx, r_size);

        Zeta_MultiLevelCircularArray_PopR(mlca, cnt);
    }

    if (idx == mlca->size) {
        pos_cursor->seg = seg_head;
        pos_cursor->seg_slot_idx = 0;
        pos_cursor->elem = NULL;
    }
}

void Zeta_MultiLevelCircularArray_EraseAll(void* mlca_) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    CheckCntr_(mlca);

    size_t seg_capacity = mlca->seg_capacity;

    size_t offset = mlca->offset;
    size_t size = mlca->size;

    Zeta_MultiLevelCircularArray_Seg* seg_head = mlca->seg_head;

    Zeta_Allocator seg_allocator = mlca->seg_allocator;

    Zeta_MultiLevelTable mlt;
    mlt.level = mlca->level;
    mlt.branch_nums = branch_nums;
    mlt.size = ZETA_UnsafeCeilIntDiv(offset + size, seg_capacity);
    mlt.root = mlca->root;
    mlt.node_allocator = mlca->node_allocator;

    Zeta_MultiLevelTable_EraseAll(&mlt);

    for (;;) {
        Zeta_MultiLevelCircularArray_Seg* seg = GetRSeg_(seg_head);
        if (seg == seg_head) { break; }
        Zeta_LList_OrdLListNode_Extract(&seg->ln);
        ZETA_Allocator_Deallocate(seg_allocator, seg);
    }

    mlca->offset = 0;
    mlca->size = 0;

    mlca->level = 1;

    mlca->rotations[0] = 0;

    mlca->root = NULL;
}

void Zeta_MultiLevelCircularArray_Check(void const* mlca_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    ZETA_DebugAssert(mlca != NULL);

    size_t width = mlca->width;
    ZETA_DebugAssert(0 < width);

    size_t stride = mlca->stride;
    ZETA_DebugAssert(width <= stride);

    size_t seg_capacity = mlca->seg_capacity;
    ZETA_DebugAssert(0 < seg_capacity);
    ZETA_DebugAssert(seg_capacity <=
                     ZETA_MultiLevelCircularArray_max_seg_capacity);

    size_t offset = mlca->offset;

    size_t size = mlca->size;

    int level = mlca->level;
    ZETA_DebugAssert(0 < level);
    ZETA_DebugAssert(level <= ZETA_MultiLevelTable_max_level);

    size_t const* rotations = mlca->rotations;

    for (int level_i = 0; level_i < level; ++level_i) {
        ZETA_DebugAssert(rotations[level_i] < branch_num);
    }

    size_t sub_tree_seg_capacity = Zeta_Power(branch_num, level - 1);
    size_t tree_seg_capacity = sub_tree_seg_capacity * branch_num;

    ZETA_DebugAssert(offset < seg_capacity * sub_tree_seg_capacity);

    size_t tree_segs_cnt =
        ZETA_UnsafeCeilIntDiv(offset % seg_capacity + size, seg_capacity);
    ZETA_DebugAssert(tree_segs_cnt <= tree_seg_capacity);

    Zeta_MultiLevelTable mlt;
    mlt.level = mlca->level;
    mlt.branch_nums = branch_nums;
    mlt.size = tree_segs_cnt;
    mlt.root = mlca->root;
    mlt.node_allocator = mlca->node_allocator;

    Zeta_MultiLevelTable_Check(&mlt);

    ZETA_Allocator_Check(mlca->node_allocator, alignof(void*));
    ZETA_Allocator_Check(mlca->seg_allocator,
                         alignof(Zeta_MultiLevelCircularArray_Seg));
}

void Zeta_MultiLevelCircularArray_Sanitize(
    void const* mlca_, Zeta_MemRecorder* dst_node_allocator,
    Zeta_MemRecorder* dst_seg_allocator) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    CheckCntr_(mlca);

    size_t stride = mlca->stride;

    size_t seg_capacity = mlca->seg_capacity;

    size_t offset = mlca->offset;

    size_t size = mlca->size;

    int level = mlca->level;

    size_t const* rotations = mlca->rotations;

    Zeta_MultiLevelCircularArray_Seg* seg_head = mlca->seg_head;

    size_t tree_segs_cnt =
        ZETA_UnsafeCeilIntDiv(offset % seg_capacity + size, seg_capacity);

    size_t sub_tree_segs_capacity = Zeta_Power(branch_num, level);

    if (size == 0) { ZETA_DebugAssert(offset == 0); }

    ZETA_DebugAssert(offset / seg_capacity < sub_tree_segs_capacity);

    Zeta_MultiLevelTable mlt;
    mlt.level = level;
    mlt.branch_nums = branch_nums;
    mlt.size = tree_segs_cnt;
    mlt.root = mlca->root;
    mlt.node_allocator = mlca->node_allocator;

    Zeta_MultiLevelTable_Sanitize(&mlt, dst_node_allocator);

    if (dst_seg_allocator != NULL) {
        Zeta_MemRecorder_Record(
            dst_seg_allocator, seg_head,
            offsetof(Zeta_MultiLevelCircularArray_Seg, data[0]));
    }

    {
        Zeta_MultiLevelCircularArray_Seg* seg = seg_head;

        size_t seg_i = 0;

        size_t idxes[ZETA_MultiLevelTable_max_level];

        for (;; ++seg_i) {
            Zeta_MultiLevelCircularArray_Seg* r_seg = GetRSeg_(seg);

            ZETA_DebugAssert(GetLSeg_(r_seg) == seg);

            seg = r_seg;

            if (seg_i == tree_segs_cnt) {
                ZETA_DebugAssert(seg == seg_head);
                break;
            }

            ZETA_DebugAssert(seg != seg_head);

            if (dst_seg_allocator != NULL) {
                Zeta_MemRecorder_Record(
                    dst_seg_allocator, seg,
                    offsetof(Zeta_MultiLevelCircularArray_Seg,
                             data[stride * seg_capacity]));
            }

            size_t tmp = offset / seg_capacity + seg_i;

            for (int level_i = 0; level_i < level; ++level_i) {
                idxes[level_i] = (rotations[level_i] + tmp) % branch_num;
                tmp /= branch_num;
            }

            void** n = Zeta_MultiLevelTable_Access(&mlt, idxes);

            ZETA_DebugAssert(n != NULL);
            ZETA_DebugAssert(*n == seg);
        }
    }
}

bool_t Zeta_MultiLevelCircularArray_Cursor_AreEqual(void const* mlca_,
                                                    void const* cursor_a_,
                                                    void const* cursor_b_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    Zeta_MultiLevelCircularArray_Cursor const* cursor_a = cursor_a_;
    Zeta_MultiLevelCircularArray_Cursor const* cursor_b = cursor_b_;

    CheckCursor_(mlca, cursor_a);
    CheckCursor_(mlca, cursor_b);

    return cursor_a->idx == cursor_b->idx;
}

int Zeta_MultiLevelCircularArray_Cursor_Compare(void const* mlca_,
                                                void const* cursor_a_,
                                                void const* cursor_b_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    Zeta_MultiLevelCircularArray_Cursor const* cursor_a = cursor_a_;
    Zeta_MultiLevelCircularArray_Cursor const* cursor_b = cursor_b_;

    CheckCursor_(mlca, cursor_a);
    CheckCursor_(mlca, cursor_b);

    return ZETA_ThreeWayCompare(cursor_a->idx, cursor_b->idx);
}

size_t Zeta_MultiLevelCircularArray_Cursor_GetDist(void const* mlca_,
                                                   void const* cursor_a_,
                                                   void const* cursor_b_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    Zeta_MultiLevelCircularArray_Cursor const* cursor_a = cursor_a_;
    Zeta_MultiLevelCircularArray_Cursor const* cursor_b = cursor_b_;

    CheckCursor_(mlca, cursor_a);
    CheckCursor_(mlca, cursor_b);

    return cursor_b->idx - cursor_a->idx;
}

size_t Zeta_MultiLevelCircularArray_Cursor_GetIdx(void const* mlca_,
                                                  void const* cursor_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    Zeta_MultiLevelCircularArray_Cursor const* cursor = cursor_;

    CheckCursor_(mlca, cursor);

    return cursor->idx;
}

void Zeta_MultiLevelCircularArray_Cursor_StepL(void const* mlca, void* cursor) {
    Zeta_MultiLevelCircularArray_Cursor_AdvanceL(mlca, cursor, 1);
}

void Zeta_MultiLevelCircularArray_Cursor_StepR(void const* mlca, void* cursor) {
    Zeta_MultiLevelCircularArray_Cursor_AdvanceR(mlca, cursor, 1);
}

void Zeta_MultiLevelCircularArray_Cursor_AdvanceL(void const* mlca_,
                                                  void* cursor_, size_t step) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    Zeta_MultiLevelCircularArray_Cursor* cursor = cursor_;

    CheckCursor_(mlca, cursor);

    size_t stride = mlca->stride;

    size_t seg_capacity = mlca->seg_capacity;

    size_t offset = mlca->offset;

    int level = mlca->level;

    Zeta_MultiLevelCircularArray_Seg* seg_head = mlca->seg_head;

    size_t idx = cursor->idx;
    ZETA_DebugAssert(step <= idx + 1);

    size_t dst_idx = idx - step;

    size_t k = offset % seg_capacity;

    size_t seg_idx = (seg_capacity + k + idx) / seg_capacity;
    size_t dst_seg_idx = (seg_capacity + k + dst_idx) / seg_capacity;

    if (level * 2ULL < seg_idx - dst_seg_idx) {
        Zeta_MultiLevelCircularArray_Access((void*)mlca, dst_idx, cursor, NULL);
        return;
    }

    Zeta_MultiLevelCircularArray_Seg* dst_seg = cursor->seg;

    for (; seg_idx < dst_seg_idx; ++seg_idx) { dst_seg = GetLSeg_(dst_seg); }

    size_t dst_seg_slot_idx = (cursor->seg_slot_idx + step) % seg_capacity;

    cursor->idx = dst_idx;
    cursor->seg = dst_seg;
    cursor->seg_slot_idx = dst_seg_slot_idx;
    cursor->elem =
        dst_seg == seg_head ? NULL : dst_seg->data + stride * dst_seg_slot_idx;
}

void Zeta_MultiLevelCircularArray_Cursor_AdvanceR(void const* mlca_,
                                                  void* cursor_, size_t step) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    Zeta_MultiLevelCircularArray_Cursor* cursor = cursor_;

    CheckCursor_(mlca, cursor);

    size_t stride = mlca->stride;

    size_t seg_capacity = mlca->seg_capacity;

    size_t offset = mlca->offset;

    size_t size = mlca->size;

    int level = mlca->level;

    Zeta_MultiLevelCircularArray_Seg* seg_head = mlca->seg_head;

    size_t idx = cursor->idx;
    ZETA_DebugAssert(step <= size - idx + 1);

    size_t dst_idx = idx + step;

    size_t k = offset % seg_capacity;

    size_t seg_idx = (seg_capacity + k + idx) / seg_capacity;
    size_t dst_seg_idx = (seg_capacity + k + dst_idx) / seg_capacity;

    if (level * 2ULL < dst_seg_idx - seg_idx) {
        Zeta_MultiLevelCircularArray_Access((void*)mlca, dst_idx, cursor, NULL);
        return;
    }

    Zeta_MultiLevelCircularArray_Seg* dst_seg = cursor->seg;

    for (; seg_idx < dst_seg_idx; ++seg_idx) { dst_seg = GetRSeg_(dst_seg); }

    size_t dst_seg_slot_idx = (cursor->seg_slot_idx + step) % seg_capacity;

    cursor->idx = dst_idx;
    cursor->seg = dst_seg;
    cursor->seg_slot_idx = dst_seg_slot_idx;
    cursor->elem =
        dst_seg == seg_head ? NULL : dst_seg->data + stride * dst_seg_slot_idx;
}

void Zeta_MultiLevelCircularArray_Cursor_Check(void const* mlca_,
                                               void const* cursor_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    CheckCntr_(mlca);

    Zeta_MultiLevelCircularArray_Cursor const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    Zeta_MultiLevelCircularArray_Cursor re_cursor;
    Zeta_MultiLevelCircularArray_Access((void*)mlca, cursor->idx, &re_cursor,
                                        NULL);

    ZETA_DebugAssert(re_cursor.mlca == cursor->mlca);
    ZETA_DebugAssert(re_cursor.idx == cursor->idx);
    ZETA_DebugAssert(re_cursor.seg == cursor->seg);
    ZETA_DebugAssert(re_cursor.seg_slot_idx == cursor->seg_slot_idx);
    ZETA_DebugAssert(re_cursor.elem == cursor->elem);
}

Zeta_SeqCntr_VTable const zeta_multi_level_circular_array_seq_cntr_vtable = {
    .cursor_size = sizeof(Zeta_MultiLevelCircularArray_Cursor),

    .Deinit = Zeta_MultiLevelCircularArray_Deinit,

    .GetWidth = Zeta_MultiLevelCircularArray_GetWidth,

    .GetSize = Zeta_MultiLevelCircularArray_GetSize,

    .GetCapacity = Zeta_MultiLevelCircularArray_GetCapacity,

    .GetLBCursor = Zeta_MultiLevelCircularArray_GetLBCursor,

    .GetRBCursor = Zeta_MultiLevelCircularArray_GetRBCursor,

    .PeekL = Zeta_MultiLevelCircularArray_PeekL,

    .PeekR = Zeta_MultiLevelCircularArray_PeekR,

    .Access = Zeta_MultiLevelCircularArray_Access,

    .Refer = Zeta_MultiLevelCircularArray_Refer,

    .Read = Zeta_MultiLevelCircularArray_Read,

    .Write = Zeta_MultiLevelCircularArray_Write,

    .PushL = Zeta_MultiLevelCircularArray_PushL,

    .PushR = Zeta_MultiLevelCircularArray_PushR,

    .Insert = Zeta_MultiLevelCircularArray_Insert,

    .PopL = Zeta_MultiLevelCircularArray_PopL,

    .PopR = Zeta_MultiLevelCircularArray_PopR,

    .Erase = Zeta_MultiLevelCircularArray_Erase,

    .EraseAll = Zeta_MultiLevelCircularArray_EraseAll,

    .Cursor_AreEqual = Zeta_MultiLevelCircularArray_Cursor_AreEqual,

    .Cursor_Compare = Zeta_MultiLevelCircularArray_Cursor_Compare,

    .Cursor_GetDist = Zeta_MultiLevelCircularArray_Cursor_GetDist,

    .Cursor_GetIdx = Zeta_MultiLevelCircularArray_Cursor_GetIdx,

    .Cursor_StepL = Zeta_MultiLevelCircularArray_Cursor_StepL,

    .Cursor_StepR = Zeta_MultiLevelCircularArray_Cursor_StepR,

    .Cursor_AdvanceL = Zeta_MultiLevelCircularArray_Cursor_AdvanceL,

    .Cursor_AdvanceR = Zeta_MultiLevelCircularArray_Cursor_AdvanceR,
};
