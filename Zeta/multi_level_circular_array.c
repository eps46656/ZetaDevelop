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

static unsigned short branch_nums[] = {
    [0 ... ZETA_MultiLevelTable_max_level - 1] =
        ZETA_MultiLevelCircularArray_branch_num
};

void Zeta_MultiLevelCircularArray_Init(void* mlca_) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    ZETA_DebugAssert(mlca != NULL);

    size_t width = mlca->width;
    ZETA_DebugAssert(0 < width);

    size_t seg_capacity = mlca->seg_capacity;
    ZETA_DebugAssert(0 < seg_capacity);
    ZETA_DebugAssert(seg_capacity <=
                     ZETA_MultiLevelCircularArray_max_seg_capacity);

    mlca->offset = 0;
    mlca->size = 0;

    mlca->level = 1;

    mlca->root = NULL;

    Zeta_Allocator* node_allocator = mlca->node_allocator;
    ZETA_DebugAssert(node_allocator != NULL);
    ZETA_DebugAssert(node_allocator->Allocate != NULL);
    ZETA_DebugAssert(node_allocator->Deallocate != NULL);

    Zeta_Allocator* seg_allocator = mlca->seg_allocator;
    ZETA_DebugAssert(seg_allocator != NULL);
    ZETA_DebugAssert(seg_allocator->Allocate != NULL);
    ZETA_DebugAssert(seg_allocator->Deallocate != NULL);
}

void Zeta_MultiLevelCircularArray_Deinit(void* mlca);

size_t Zeta_MultiLevelCircularArray_GetWidth(void const* mlca_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    CheckCntr_(mlca);

    return mlca->width;
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

    return seg_capacity *
               Zeta_Power(
                   ZETA_MultiLevelCircularArray_branch_num,
                   Zeta_FloorLog(ZETA_SIZE_MAX / seg_capacity,
                                 ZETA_MultiLevelCircularArray_branch_num)) -
           seg_capacity;
}

void Zeta_MultiLevelCircularArray_GetLBCursor(void const* mlca_,
                                              void* dst_cursor_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    CheckCntr_(mlca);

    Zeta_MultiLevelCircularArray_Cursor* dst_cursor = dst_cursor_;
    ZETA_DebugAssert(dst_cursor != NULL);
}

void Zeta_MultiLevelCircularArray_GetRBCursor(void const* mlca_,
                                              void* dst_cursor_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    CheckCntr_(mlca);

    Zeta_MultiLevelCircularArray_Cursor* dst_cursor = dst_cursor_;
    ZETA_DebugAssert(dst_cursor != NULL);
}

void* Zeta_MultiLevelCircularArray_PeekL(void* mlca_, void* dst_cursor,
                                         void* dst_elem) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    CheckCntr_(mlca);

    return Zeta_MultiLevelCircularArray_Access(mlca, 0, dst_cursor, dst_elem);
}

void* Zeta_MultiLevelCircularArray_PeekR(void* mlca_, void* dst_cursor,
                                         void* dst_elem) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    CheckCntr_(mlca);

    return Zeta_MultiLevelCircularArray_Access(mlca, mlca->size - 1, dst_cursor,
                                               dst_elem);
}

void* Zeta_MultiLevelCircularArray_Access(void* mlca_, size_t idx,
                                          void* dst_cursor_, void* dst_elem) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    CheckCntr_(mlca);

    Zeta_MultiLevelCircularArray_Cursor* dst_cursor = dst_cursor_;

    size_t width = mlca->width;

    size_t seg_capacity = mlca->seg_capacity;

    size_t offset = mlca->offset;

    size_t size = mlca->size;

    ZETA_DebugAssert(idx < size + 2);

    if (idx == (size_t)(-1) || idx == size) {
        if (dst_cursor != NULL) {
            dst_cursor->mlca = mlca;
            dst_cursor->idx = idx;
            dst_cursor->elem = NULL;
        }

        return NULL;
    }

    int level = mlca->level;

    void* root = mlca->root;

    size_t slot_idx = offset + idx;

    size_t seg_idx = slot_idx / seg_capacity;
    size_t seg_slot_idx = slot_idx & seg_capacity;

    size_t idxes[ZETA_MultiLevelTable_max_level + 1];

    {
        size_t tmp = seg_idx;

        for (int level_i = 0; level_i < level; ++level_i) {
            idxes[level_i] = (mlca->rotations[level_i] + tmp) %
                             ZETA_MultiLevelCircularArray_branch_num;
            tmp /= ZETA_MultiLevelCircularArray_branch_num;
        }
    }

    Zeta_MultiLevelTable mlt;
    mlt.level = level;
    mlt.branch_nums = branch_nums;
    mlt.size = ZETA_CeilIntDiv(offset + size, seg_capacity);
    mlt.root = root;

    void* seg = *Zeta_MultiLevelTable_Access(&mlt, idxes);

    void* elem = seg + width * seg_slot_idx;

    if (dst_cursor != NULL) {
        dst_cursor->mlca = mlca;
        dst_cursor->idx = idx;
        dst_cursor->elem = elem;
    }

    if (dst_elem != NULL) { Zeta_MemCopy(dst_elem, elem, width); }

    return elem;
}

void* Zeta_MultiLevelCircularArray_Refer(void* mlca_, void const* pos_cursor_) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;

    Zeta_MultiLevelCircularArray_Cursor* pos_cursor = pos_cursor_;

    CheckCursor_(mlca, pos_cursor);

    return pos_cursor->elem;
}

void Zeta_MultiLevelCircularArray_Read(void const* mlca_,
                                       void const* pos_cursor_, size_t cnt,
                                       void* dst, void* dst_cursor) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;

    Zeta_MultiLevelCircularArray_Cursor const* pos_cursor = pos_cursor_;

    CheckCursor_(mlca, pos_cursor);
}

void Zeta_MultiLevelCircularArray_Write(void* mlca_, void* pos_cursor_,
                                        size_t cnt, void const* src,
                                        void* dst_cursor) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;

    Zeta_MultiLevelCircularArray_Cursor* pos_cursor = pos_cursor_;

    CheckCursor_(mlca, pos_cursor);
}

static void Push_(Zeta_MultiLevelCircularArray* mlca, size_t cnt, int dir) {
    if (cnt == 0) { return; }

    size_t width = mlca->width;

    size_t seg_capacity = mlca->seg_capacity;

    size_t offset = mlca->offset;
    size_t size = mlca->size;

    size_t capacity = Zeta_MultiLevelCircularArray_GetCapacity(mlca);
    ZETA_DebugAssert(size + cnt <= capacity);

    size_t* rotations = mlca->rotations;

    Zeta_Allocator* node_allocator = mlca->node_allocator;
    Zeta_Allocator* seg_allocator = mlca->seg_allocator;

    size_t cur_seg_slot_offset = offset % seg_capacity;

    size_t nxt_seg_slot_offset =
        dir == L ? (cur_seg_slot_offset + seg_capacity - cnt % seg_capacity) %
                       seg_capacity
                 : cur_seg_slot_offset;

    size_t cur_size = size;
    size_t nxt_size = cur_size + cnt;

    size_t cur_segs_cnt =
        ZETA_CeilIntDiv(cur_seg_slot_offset + cur_size, seg_capacity);
    size_t nxt_segs_cnt =
        ZETA_CeilIntDiv(nxt_seg_slot_offset + nxt_size, seg_capacity);

    Zeta_MultiLevelTable mlt;
    mlt.level = mlca->level;
    mlt.branch_nums = branch_nums;
    mlt.size = cur_segs_cnt;
    mlt.root = mlca->root;
    mlt.node_allocator = node_allocator;

    size_t segs_capacity = Zeta_MultiLevelTable_GetCapacity(&mlt);

    while (segs_capacity < nxt_segs_cnt) {
        Zeta_MultiLevelTable_Node* new_root = ZETA_Allocator_SafeAllocate(
            node_allocator, sizeof(Zeta_MultiLevelTable_Node),
            offsetof(Zeta_MultiLevelTable_Node,
                     ptrs[ZETA_MultiLevelCircularArray_branch_num]));

        new_root->hot = 1;
        new_root->ptrs[0] = mlt.root;

        ++mlt.level;
        mlt.root = new_root;

        rotations[mlt.level - 1] = ZETA_MultiLevelCircularArray_branch_num / 2;

        segs_capacity *= ZETA_MultiLevelCircularArray_branch_num;
    }

    size_t seg_offset = offset / seg_capacity;

    size_t sub_segs_capacity =
        segs_capacity / ZETA_MultiLevelCircularArray_branch_num;

    size_t idxes[ZETA_MultiLevelCircularArray_branch_num];

    {
        size_t rot = ZETA_CeilIntDiv(
            ZETA_GetMaxOf(nxt_segs_cnt - cur_segs_cnt, seg_offset) - seg_offset,
            sub_segs_capacity);

        rotations[mlt.level - 1] =
            (rotations[mlt.level - 1] +
             ZETA_MultiLevelCircularArray_branch_num - rot) %
            ZETA_MultiLevelCircularArray_branch_num;

        seg_offset += sub_segs_capacity * rot;
    }

    for (; cur_segs_cnt < nxt_segs_cnt; ++cur_segs_cnt) {
        size_t tmp = dir == L ? --seg_offset : seg_offset + cur_segs_cnt;

        for (int level_i = 0; level_i < mlt.level; ++level_i) {
            idxes[level_i] = (rotations[level_i] + tmp) %
                             ZETA_MultiLevelCircularArray_branch_num;
            tmp /= ZETA_MultiLevelCircularArray_branch_num;
        }

        *Zeta_MultiLevelTable_Insert(&mlt, idxes) =
            ZETA_Allocator_SafeAllocate(seg_allocator, 1, width * seg_capacity);
    }

    mlca->offset = seg_offset * seg_capacity + nxt_seg_slot_offset;
    mlca->size = nxt_size;

    mlca->level = mlt.level;
    mlca->root = mlt.root;
}

static void Pop_(Zeta_MultiLevelCircularArray* mlca, size_t cnt, int dir) {
    if (cnt == 0) { return; }

    size_t seg_capacity = mlca->seg_capacity;

    size_t offset = mlca->offset;
    size_t size = mlca->size;

    ZETA_DebugAssert(cnt <= size);

    size_t* rotations = mlca->rotations;

    Zeta_Allocator* node_allocator = mlca->node_allocator;
    Zeta_Allocator* seg_allocator = mlca->seg_allocator;

    size_t cur_seg_slot_offset = offset % seg_capacity;

    size_t nxt_seg_slot_offset =
        dir == L ? (cur_seg_slot_offset + cnt) % seg_capacity
                 : cur_seg_slot_offset;

    size_t cur_size = size;
    size_t nxt_size = cur_size - cnt;

    size_t cur_segs_cnt =
        ZETA_CeilIntDiv(cur_seg_slot_offset + cur_size, seg_capacity);
    size_t nxt_segs_cnt =
        ZETA_CeilIntDiv(nxt_seg_slot_offset + nxt_size, seg_capacity);

    Zeta_MultiLevelTable mlt;
    mlt.level = mlca->level;
    mlt.branch_nums = branch_nums;
    mlt.size = cur_segs_cnt;
    mlt.root = mlca->root;
    mlt.node_allocator = node_allocator;

    size_t segs_capacity = Zeta_MultiLevelTable_GetCapacity(&mlt);

    size_t seg_offset = offset / seg_capacity;

    size_t sub_segs_capacity =
        segs_capacity / ZETA_MultiLevelCircularArray_branch_num;

    size_t idxes[ZETA_MultiLevelCircularArray_branch_num];

    for (; nxt_segs_cnt < cur_segs_cnt; --cur_segs_cnt) {
        size_t tmp = dir == L ? seg_offset++ : seg_offset + cur_segs_cnt - 1;

        for (int level_i = 0; level_i < mlt.level; ++level_i) {
            idxes[level_i] = (rotations[level_i] + tmp) %
                             ZETA_MultiLevelCircularArray_branch_num;
            tmp /= ZETA_MultiLevelCircularArray_branch_num;
        }

        ZETA_Allocator_Deallocate(seg_allocator,
                                  Zeta_MultiLevelTable_Erase(&mlt, idxes));
    }

    rotations[mlt.level - 1] =
        (rotations[mlt.level - 1] + seg_offset / sub_segs_capacity) %
        ZETA_MultiLevelCircularArray_branch_num;

    seg_offset %= sub_segs_capacity;

    for (;;) {
        if (mlt.level < 2 || sub_segs_capacity < cur_segs_cnt * 2) { break; }

        size_t top_rotation = rotations[mlt.level - 1];

        Zeta_MultiLevelTable_Node* l_node = mlt.root->ptrs[top_rotation];

        size_t r_node_idx =
            (top_rotation + 1) % ZETA_MultiLevelCircularArray_branch_num;

        if ((mlt.root->hot >> r_node_idx) % 2 == 0) {
            --mlt.level;
            mlt.root = l_node;

            segs_capacity /= ZETA_MultiLevelCircularArray_branch_num;
            sub_segs_capacity /= ZETA_MultiLevelCircularArray_branch_num;

            rotations[mlt.level - 1] =
                (rotations[mlt.level - 1] + seg_offset / sub_segs_capacity) %
                ZETA_MultiLevelCircularArray_branch_num;

            seg_offset %= sub_segs_capacity;

            continue;
        }

        size_t sub_rotation = rotations[mlt.level - 2];

        Zeta_MultiLevelTable_Node* r_node = mlt.root->ptrs[r_node_idx];

        size_t l_node_size = __builtin_popcountll(l_node->hot);
        size_t r_node_size = __builtin_popcountll(r_node->hot);

        for (size_t i = 0; i < l_node_size; ++i) {
            mlt.root->ptrs[i] =
                l_node->ptrs[(sub_rotation +
                              ZETA_MultiLevelCircularArray_branch_num -
                              l_node_size + i) %
                             ZETA_MultiLevelCircularArray_branch_num];
        }

        for (size_t i = 0; i < r_node_size; ++i) {
            mlt.root->ptrs[l_node_size + i] =
                r_node->ptrs[(sub_rotation + i) %
                             ZETA_MultiLevelCircularArray_branch_num];
        }

        mlt.root->hot = (1ULL << (l_node_size + r_node_size)) - 1;

        ZETA_Allocator_Deallocate(node_allocator, l_node);
        ZETA_Allocator_Deallocate(node_allocator, r_node);

        --mlt.level;

        segs_capacity /= ZETA_MultiLevelCircularArray_branch_num;
        sub_segs_capacity /= ZETA_MultiLevelCircularArray_branch_num;

        rotations[mlt.level - 1] = 0;

        seg_offset %= sub_segs_capacity;
    }

    mlca->offset = seg_offset * seg_capacity + nxt_seg_slot_offset;
    mlca->size = nxt_size;

    mlca->level = mlt.level;
    mlca->root = mlt.root;
}

void* Zeta_MultiLevelCircularArray_PushL(void* mlca_, size_t cnt,
                                         void* dst_cursor_) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    CheckCntr_(mlca);

    Zeta_MultiLevelCircularArray_Cursor* dst_cursor = dst_cursor_;

    Push_(mlca, cnt, L);

    return Zeta_MultiLevelCircularArray_Access(mlca, 0, dst_cursor, NULL);
}

void* Zeta_MultiLevelCircularArray_PushR(void* mlca_, size_t cnt,
                                         void* dst_cursor_) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    CheckCntr_(mlca);

    Zeta_MultiLevelCircularArray_Cursor* dst_cursor = dst_cursor_;

    size_t cur_size = mlca->size;

    Push_(mlca, cnt, R);

    return Zeta_MultiLevelCircularArray_Access(mlca, cur_size, dst_cursor,
                                               NULL);
}

void* Zeta_MultiLevelCircularArray_Insert(void* mlca_, void* pos_cursor_,
                                          size_t cnt) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;

    Zeta_MultiLevelCircularArray_Cursor* pos_cursor = pos_cursor_;

    CheckCursor_(mlca, pos_cursor);

    if (cnt == 0) { return pos_cursor->elem; }

    size_t capacity = Zeta_MultiLevelCircularArray_GetCapacity(mlca);
    ZETA_DebugAssert(mlca->size + cnt <= capacity);
}

void Zeta_MultiLevelCircularArray_PopL(void* mlca_, size_t cnt) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    CheckCntr_(mlca);

    Pop_(mlca, cnt, L);
}

void Zeta_MultiLevelCircularArray_PopR(void* mlca_, size_t cnt) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    CheckCntr_(mlca);

    Pop_(mlca, cnt, R);
}

void Zeta_MultiLevelCircularArray_Erase(void* mlca_, void* pos_cursor_,
                                        size_t cnt) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;

    Zeta_MultiLevelCircularArray_Cursor* pos_cursor = pos_cursor_;

    CheckCursor_(mlca, pos_cursor);

    if (cnt == 0) { return; }

    ZETA_DebugAssert(cnt <= mlca->size);
}

void Zeta_MultiLevelCircularArray_Check(void const* mlca_) {
    Zeta_MultiLevelCircularArray const* mlca = mlca_;
    ZETA_DebugAssert(mlca != NULL);

    size_t width = mlca->width;
    ZETA_DebugAssert(0 < width);

    size_t seg_capacity = mlca->seg_capacity;
    ZETA_DebugAssert(0 < seg_capacity);
    ZETA_DebugAssert(seg_capacity <=
                     ZETA_MultiLevelCircularArray_max_seg_capacity);

    size_t offset = mlca->offset;
    size_t size = mlca->size;

    int level = mlca->level;
    ZETA_DebugAssert(0 < level);
    ZETA_DebugAssert(level <= ZETA_MultiLevelTable_max_level);

    size_t segs_capacity =
        seg_capacity *
        Zeta_Power(ZETA_MultiLevelCircularArray_branch_num, level);

    ZETA_DebugAssert(offset < segs_capacity);

    Zeta_MultiLevelTable mlt;
    mlt.level = mlca->level;
    mlt.branch_nums = branch_nums;
    mlt.size = ZETA_CeilIntDiv(offset % segs_capacity + size, segs_capacity);
    mlt.root = mlca->root;
    mlt.node_allocator = mlca->node_allocator;

    Zeta_MultiLevelTable_Check(&mlt);

    Zeta_Allocator* seg_allocator = mlca->seg_allocator;
    ZETA_DebugAssert(seg_allocator != NULL);
    ZETA_DebugAssert(seg_allocator->Allocate != NULL);
    ZETA_DebugAssert(seg_allocator->Deallocate != NULL);
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

void Zeta_MultiLevelCircularArray_CheckCursor(void* mlca_,
                                              void const* cursor_) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    CheckCntr_(mlca);

    Zeta_MultiLevelCircularArray_Cursor const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);
}

void Zeta_MultiLevelCircularArray_DeploySeqCntr(void* mlca_,
                                                Zeta_SeqCntr* seq_cntr) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    CheckCntr_(mlca);

    Zeta_SeqCntr_Init(seq_cntr);

    seq_cntr->context = mlca;

    seq_cntr->cursor_size = sizeof(Zeta_MultiLevelCircularArray_Cursor);

    seq_cntr->GetWidth = Zeta_MultiLevelCircularArray_GetWidth;

    seq_cntr->GetSize = Zeta_MultiLevelCircularArray_GetSize;

    seq_cntr->GetCapacity = Zeta_MultiLevelCircularArray_GetCapacity;

    seq_cntr->GetLBCursor = Zeta_MultiLevelCircularArray_GetLBCursor;

    seq_cntr->GetRBCursor = Zeta_MultiLevelCircularArray_GetRBCursor;

    seq_cntr->PeekL = Zeta_MultiLevelCircularArray_PeekL;

    seq_cntr->PeekR = Zeta_MultiLevelCircularArray_PeekR;

    seq_cntr->Access = Zeta_MultiLevelCircularArray_Access;

    seq_cntr->Refer = Zeta_MultiLevelCircularArray_Refer;

    seq_cntr->Read = Zeta_MultiLevelCircularArray_Read;

    seq_cntr->Write = Zeta_MultiLevelCircularArray_Write;

    seq_cntr->PushL = Zeta_MultiLevelCircularArray_PushL;

    seq_cntr->PushR = Zeta_MultiLevelCircularArray_PushR;

    seq_cntr->Insert = Zeta_MultiLevelCircularArray_Insert;

    seq_cntr->PopL = Zeta_MultiLevelCircularArray_PopL;

    seq_cntr->PopR = Zeta_MultiLevelCircularArray_PopR;

    seq_cntr->Erase = Zeta_MultiLevelCircularArray_Erase;

    seq_cntr->EraseAll = Zeta_MultiLevelCircularArray_EraseAll;

    seq_cntr->Cursor_AreEqual = Zeta_MultiLevelCircularArray_Cursor_AreEqual;

    seq_cntr->Cursor_Compare = Zeta_MultiLevelCircularArray_Cursor_Compare;

    seq_cntr->Cursor_GetDist = Zeta_MultiLevelCircularArray_Cursor_GetDist;

    seq_cntr->Cursor_GetIdx = Zeta_MultiLevelCircularArray_Cursor_GetIdx;

    seq_cntr->Cursor_StepL = Zeta_MultiLevelCircularArray_Cursor_StepL;

    seq_cntr->Cursor_StepR = Zeta_MultiLevelCircularArray_Cursor_StepR;

    seq_cntr->Cursor_AdvanceL = Zeta_MultiLevelCircularArray_Cursor_AdvanceL;

    seq_cntr->Cursor_AdvanceR = Zeta_MultiLevelCircularArray_Cursor_AdvanceR;
}
