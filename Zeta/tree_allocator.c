#include "tree_allocator.h"

#include "debugger.h"
#include "utils.h"

ZETA_DeclareStruct(Head);

struct Head {
    Zeta_OrdRBLListNode hn;  // link to other heads
    Zeta_OrdRBTreeNode sn;   // link to other vacant
};

#define vacant_head_size sizeof(Head)

#define occupied_head_size (offsetof(Head, hn) + sizeof(Zeta_OrdRBLListNode))

#define vacant_color 0
#define occupied_color 1

void Zeta_TreeAllocator_Init(void* ta_) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    size_t head_align = alignof(Head);

    size_t align = ta->align;
    ZETA_DebugAssert(0 < align);

    ta->align = align = Zeta_LCM(head_align, align);

    ZETA_DebugAssert(ta->mem != NULL);

    void* mem_beg = ta->mem;
    void* mem_end = mem_beg + ta->size;

    void* data_beg = __builtin_align_up(mem_beg + occupied_head_size, align) -
                     occupied_head_size;

    void* data_end = __builtin_align_down(mem_end, head_align);

    ZETA_DebugAssert(data_beg <= data_end);
    ZETA_DebugAssert(512 <= data_end - data_beg);

    ta->data_beg = data_beg;
    ta->data_end = data_end;

    size_t least_stride =
        ZETA_GetMaxOf(occupied_head_size + ta->least_size,
                      ZETA_GetMaxOf(vacant_head_size,
                                    occupied_head_size + sizeof(void*) * 8));

    ta->least_size = least_stride - occupied_head_size;

    Head* beg_head = (void*)data_beg;
    Head* end_head = (void*)(data_end - occupied_head_size);

    Zeta_OrdRBLListNode_Init(&beg_head->hn);
    Zeta_OrdRBLListNode_SetColor(&beg_head->hn, vacant_color);
    Zeta_OrdRBTreeNode_Init(NULL, &beg_head->sn);

    Zeta_OrdRBLListNode_Init(&end_head->hn);
    Zeta_OrdRBLListNode_SetColor(&end_head->hn, occupied_color);

    Zeta_OrdRBLListNode_InsertR(&beg_head->hn, &end_head->hn);

    ta->sn_root = &beg_head->sn;
}

void Zeta_TreeAllocator_Deinit(void* ta_) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    void* data_beg = ta->data_beg;
    void* data_end = ta->data_end;

    Head* beg_head = (void*)data_beg;
    Head* end_head = (void*)(data_end - occupied_head_size);

    ZETA_DebugAssert(Zeta_OrdRBLListNode_GetColor(&beg_head->hn) ==
                     vacant_color);

    ZETA_DebugAssert(Zeta_OrdRBLListNode_GetR(&beg_head->hn) == &end_head->hn);
}

size_t Zeta_TreeAllocator_GetAlign(void* ta_) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    return ta->align;
}

#define IsVacant_(head) \
    (Zeta_OrdRBLListNode_GetColor(&(head)->hn) == vacant_color)

#define HNToHead_(hn_) ZETA_MemberToStruct(Head, hn, hn_)

#define SNToHead_(sn_) ZETA_MemberToStruct(Head, sn, sn_)

static size_t GetWidth_(Head* head) {
    return (Zeta_OrdRBLListNode_GetR(&head->hn)) - (void*)(&head->hn);
}

static Zeta_OrdRBTreeNode* FindBlock_(Zeta_OrdRBTreeNode* sn_root,
                                      size_t stride) {
    Zeta_OrdRBTreeNode* sn = sn_root;
    Zeta_OrdRBTreeNode* ret = NULL;

    while (sn != NULL) {
        size_t sn_stride = GetWidth_(SNToHead_(sn));

        if (sn_stride < stride) {
            sn = Zeta_OrdRBTreeNode_GetR(sn);
        } else {
            ret = sn;
            sn = Zeta_OrdRBTreeNode_GetL(sn);
        }
    }

    return ret;
}

#define choose_ret_failed 0
#define choose_ret_fit 1
#define choose_ret_cut 2

static int ChooseNiceBlock_(Zeta_TreeAllocator* ta, size_t size, Head** dst) {
    size_t align = ta->align;

    Zeta_OrdRBTreeNode* best_fit_sn =
        FindBlock_(ta->sn_root, occupied_head_size + size);

    if (best_fit_sn == NULL) { return choose_ret_failed; }

    size_t least_stride = occupied_head_size + ta->least_size;

    size_t cut_stride = ZETA_IntRoundUp(occupied_head_size + size, align);

    Head* best_fit_head = SNToHead_(best_fit_sn);

    if (GetWidth_(best_fit_head) <= cut_stride + least_stride) {
        *dst = best_fit_head;
        return choose_ret_fit;
    }

    Zeta_OrdRBTreeNode* twice_fit_sn = FindBlock_(ta->sn_root, cut_stride * 2);

    *dst = SNToHead_(
        twice_fit_sn == NULL
            ? Zeta_GetMostLink(NULL, Zeta_OrdRBTreeNode_GetR, ta->sn_root)
            : twice_fit_sn);

    return choose_ret_cut;
}

size_t Zeta_TreeAllocator_Query(void const* ta_, size_t size) {
    Zeta_TreeAllocator const* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    if (size == 0) { return 0; }

    size_t align = ta->align;

    if (size < ta->least_size) { size = ta->least_size; }

    return ZETA_IntRoundUp(occupied_head_size + size, align) -
           occupied_head_size;
}

void* Zeta_TreeAllocator_Allocate(void const* ta_, size_t size) {
    Zeta_TreeAllocator const* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    if (size == 0) { return NULL; }

    if (size < ta->least_size) { size = ta->least_size; }

    Head* l_head;

    int choose_ret = ChooseNiceBlock_(ta, size, &l_head);

    if (choose_ret == choose_ret_failed) { return NULL; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_OrdRBLListNode_SetColor(&l_head->hn, occupied_color);

    ta->sn_root = Zeta_RBTree_Extract(&btn_opr, &l_head->sn);

    if (choose_ret == choose_ret_fit) {
        return (void*)(l_head) + occupied_head_size;
    }

    size_t align = ta->align;

    Head* m_head =
        (void*)(l_head) + ZETA_IntRoundUp(occupied_head_size + size, align);

    Head* r_head = HNToHead_(Zeta_OrdRBLListNode_GetR(&l_head->hn));

    Zeta_OrdRBLListNode_Init(&m_head->hn);
    Zeta_OrdRBLListNode_SetColor(&m_head->hn, vacant_color);
    Zeta_OrdRBLListNode_InsertR(&l_head->hn, &m_head->hn);

    Zeta_OrdRBTreeNode_Init(NULL, &m_head->sn);

    ta->sn_root = Zeta_RBTree_GeneralInsertL(
        &btn_opr, ta->sn_root,
        FindBlock_(ta->sn_root, (void*)(r_head) - (void*)(m_head)),
        &m_head->sn);

    return (void*)(l_head) + occupied_head_size;
}

void Zeta_TreeAllocator_Deallocate(void* ta_, void* ptr) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    if (ptr == NULL) { return; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Head* m_head = ptr - occupied_head_size;

    Head* l_head = HNToHead_(Zeta_OrdRBLListNode_GetL(&m_head->hn));

    Head* r_head = HNToHead_(Zeta_OrdRBLListNode_GetR(&m_head->hn));

    bool_t merge_l = IsVacant_(l_head);
    bool_t merge_r = IsVacant_(r_head);

    if (merge_l) {
        Zeta_OrdRBLListNode_Extract(&m_head->hn);
        ta->sn_root = Zeta_RBTree_Extract(&btn_opr, &l_head->sn);
    } else {
        Zeta_OrdRBLListNode_SetColor(&m_head->hn, vacant_color);
        Zeta_OrdRBTreeNode_Init(NULL, &m_head->sn);
        l_head = m_head;
    }

    if (merge_r) {
        Head* rr_head = HNToHead_(Zeta_OrdRBLListNode_GetR(&r_head->hn));

        Zeta_OrdRBLListNode_Extract(&r_head->hn);
        ta->sn_root = Zeta_RBTree_Extract(&btn_opr, &r_head->sn);

        r_head = rr_head;
    }

    ta->sn_root = Zeta_RBTree_GeneralInsertL(
        &btn_opr, ta->sn_root,
        FindBlock_(ta->sn_root, (void*)(r_head) - (void*)(l_head)),
        &l_head->sn);
}

static void GetVacantHead_(Zeta_MemRecorder* dst_head_recorder, Head* head,
                           size_t lb, size_t ub) {
    size_t stride = GetWidth_(head);
    ZETA_DebugAssert(lb <= stride && stride <= ub);

    Zeta_OrdRBTreeNode* sn = &head->sn;

    Zeta_OrdRBTreeNode* sn_l = Zeta_OrdRBTreeNode_GetL(sn);
    Zeta_OrdRBTreeNode* sn_r = Zeta_OrdRBTreeNode_GetR(sn);

    Zeta_MemRecorder_Record(dst_head_recorder, head, 0);

    if (sn_l != NULL) {
        GetVacantHead_(dst_head_recorder, SNToHead_(sn_l), lb, stride);
    }

    if (sn_r != NULL) {
        GetVacantHead_(dst_head_recorder, SNToHead_(sn_r), stride, ub);
    }
}

/*

range config check:
    beg is head_align aligned
    beg + head_size is align aligned
    end - head_size is head_align aligned
    end is align aligned

sn tree check:
    all sn in sn tree, its block state should be vacant.
    stride is ordered

block check:
    head is head_align aligned
    begin of data is align aligned
    nxt head is after cur head
    stride from head to nxt head is sufficient for Head.

vacant block check:
    check its sn is in sn tree
    check sn_tree only contains vacant blocks
*/

void Zeta_TreeAllocator_Sanitize(void* ta_, Zeta_MemRecorder* dst_recorder) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    void* data_beg = ta->data_beg;
    void* data_end = ta->data_end;

    size_t head_align = alignof(Head);

    size_t align = ta->align;

    ZETA_DebugAssert(__builtin_is_aligned(data_beg, head_align));
    ZETA_DebugAssert(
        __builtin_is_aligned(data_beg + occupied_head_size, align));
    ZETA_DebugAssert(
        __builtin_is_aligned(data_end - occupied_head_size, head_align));
    ZETA_DebugAssert(__builtin_is_aligned(data_end, align));

    Head* beg_head = (void*)data_beg;
    Head* end_head = (void*)(data_end - occupied_head_size);

    ZETA_DebugAssert(Zeta_OrdRBTreeNode_GetP(NULL, ta->sn_root) == NULL);

    Zeta_MemRecorder* vacant_head_recorder = Zeta_MemRecorder_Create();

    GetVacantHead_(vacant_head_recorder, SNToHead_(ta->sn_root), 0,
                   ZETA_SIZE_MAX);

    for (Zeta_OrdRBLListNode* hn_i = &beg_head->hn; hn_i != &end_head->hn;) {
        Head* head_i = HNToHead_(hn_i);
        ZETA_DebugAssert(__builtin_is_aligned(head_i, head_align));

        void* data_i = (void*)(head_i) + occupied_head_size;
        ZETA_DebugAssert(__builtin_is_aligned(data_i, align));

        Zeta_OrdRBLListNode* nxt_hn_i = Zeta_OrdRBLListNode_GetR(hn_i);

        ZETA_DebugAssert(hn_i < nxt_hn_i);

        Head* nxt_head_i = HNToHead_(nxt_hn_i);

        size_t stride = (void*)nxt_head_i - (void*)head_i;
        size_t size = (void*)nxt_head_i - (void*)data_i;

        ZETA_DebugAssert(occupied_head_size + ta->least_size <= stride);
        ZETA_DebugAssert(occupied_head_size + size == stride);

        if (IsVacant_(head_i)) {
            bool_t b = Zeta_MemRecorder_Unrecord(vacant_head_recorder, head_i);
            ZETA_DebugAssert(b);
        } else {
            Zeta_MemRecorder_Record(dst_recorder, data_i,
                                    (void*)nxt_head_i - (void*)data_i);
        }

        hn_i = nxt_hn_i;
    }

    ZETA_DebugAssert(Zeta_MemRecorder_GetSize(vacant_head_recorder) == 0);

    Zeta_MemRecorder_Destroy(vacant_head_recorder);
}

void Zeta_TreeAllocator_DeployAllocator(void* ta_, Zeta_Allocator* dst) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    ZETA_DebugAssert(dst != NULL);

    Zeta_Allocator_Init(dst);

    dst->context = ta;

    dst->GetAlign = Zeta_TreeAllocator_GetAlign;

    dst->Query = Zeta_TreeAllocator_Query;

    dst->Allocate = Zeta_TreeAllocator_Allocate;

    dst->Deallocate = Zeta_TreeAllocator_Deallocate;
}
