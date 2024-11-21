#include "tree_allocator.h"

#include "debugger.h"
#include "rbtree.h"
#include "utils.h"

ZETA_DeclareStruct(Zeta_TreeAllocator_Head);

struct Zeta_TreeAllocator_Head {
    Zeta_OrdRBLinkedListNode hn;  // link to other heads
    Zeta_OrdRBTreeNode sn;        // link to other vacant
};

#define vacant_head_size sizeof(Zeta_TreeAllocator_Head)

#define occupied_head_size \
    (offsetof(Zeta_TreeAllocator_Head, hn) + sizeof(Zeta_OrdRBLinkedListNode))

#define vacant_color 0
#define occupied_color 1

void Zeta_TreeAllocator_Init(void* ta_) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    size_t head_align = alignof(Zeta_TreeAllocator_Head);

    size_t align = ta->align;
    ZETA_DebugAssert(0 < align);

    ta->align = align = Zeta_LCM(head_align, align);

    ZETA_DebugAssert(ta->mem != NULL);

    unsigned char* mem_beg = ta->mem;
    unsigned char* mem_end = mem_beg + ta->size;

    unsigned char* data_beg = (unsigned char*)__builtin_align_up(
                                  mem_beg + occupied_head_size, align) -
                              occupied_head_size;

    unsigned char* data_end = __builtin_align_down(mem_end, head_align);

    ZETA_DebugAssert(data_beg <= data_end);
    ZETA_DebugAssert(512 <= data_end - data_beg);

    ta->data_beg = data_beg;
    ta->data_end = data_end;

    size_t least_stride =
        ZETA_GetMaxOf(occupied_head_size + ta->least_size,
                      ZETA_GetMaxOf(vacant_head_size,
                                    occupied_head_size + sizeof(void*) * 8));

    ta->least_size = least_stride - occupied_head_size;

    Zeta_TreeAllocator_Head* beg_head = (void*)data_beg;
    Zeta_TreeAllocator_Head* end_head = (void*)(data_end - occupied_head_size);

    Zeta_OrdRBLinkedListNode_Init(&beg_head->hn);
    Zeta_OrdRBLinkedListNode_SetColor(&beg_head->hn, vacant_color);
    Zeta_OrdRBTreeNode_Init(NULL, &beg_head->sn);

    Zeta_OrdRBLinkedListNode_Init(&end_head->hn);
    Zeta_OrdRBLinkedListNode_SetColor(&end_head->hn, occupied_color);

    Zeta_OrdRBLinkedListNode_InsertR(&beg_head->hn, &end_head->hn);

    ta->sn_root = &beg_head->sn;
}

void Zeta_TreeAllocator_Deinit(void* ta_) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    unsigned char* data_beg = ta->data_beg;
    unsigned char* data_end = ta->data_end;

    Zeta_TreeAllocator_Head* beg_head = (void*)data_beg;
    Zeta_TreeAllocator_Head* end_head = (void*)(data_end - occupied_head_size);

    ZETA_DebugAssert(Zeta_OrdRBLinkedListNode_GetColor(&beg_head->hn) ==
                     vacant_color);

    ZETA_DebugAssert(Zeta_OrdRBLinkedListNode_GetR(&beg_head->hn) ==
                     &end_head->hn);
}

size_t Zeta_TreeAllocator_GetAlign(void* ta_) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    return ta->align;
}

#define IsVacant_(head) \
    (Zeta_OrdRBLinkedListNode_GetColor(&(head)->hn) == vacant_color)

#define HNToHead_(hn_) ZETA_MemberToStruct(Zeta_TreeAllocator_Head, hn, hn_)

#define SNToHead_(sn_) ZETA_MemberToStruct(Zeta_TreeAllocator_Head, sn, sn_)

static size_t GetWidth_(Zeta_TreeAllocator_Head* head) {
    return (unsigned char*)(Zeta_OrdRBLinkedListNode_GetR(&head->hn)) -
           (unsigned char*)(&head->hn);
}

static Zeta_OrdRBTreeNode* FindBlock_(Zeta_OrdRBTreeNode* sn_root,
                                      size_t stride) {
    Zeta_OrdRBTreeNode* sn = sn_root;
    Zeta_OrdRBTreeNode* ret = NULL;

    while (sn != NULL) {
        size_t sn_stride = GetWidth_(SNToHead_(sn));

        if (sn_stride < stride) {
            sn = Zeta_OrdRBTreeNode_GetR(NULL, sn);
        } else {
            ret = sn;
            sn = Zeta_OrdRBTreeNode_GetL(NULL, sn);
        }
    }

    return ret;
}

#define choose_ret_failed 0
#define choose_ret_fit 1
#define choose_ret_cut 2

static int ChooseNiceBlock_(Zeta_TreeAllocator* ta, size_t size,
                            Zeta_TreeAllocator_Head** dst) {
    size_t align = ta->align;

    Zeta_OrdRBTreeNode* best_fit_sn =
        FindBlock_(ta->sn_root, occupied_head_size + size);

    if (best_fit_sn == NULL) { return choose_ret_failed; }

    size_t least_stride = occupied_head_size + ta->least_size;

    uintptr_t cut_stride = ZETA_IntRoundUp(occupied_head_size + size, align);

    Zeta_TreeAllocator_Head* best_fit_head = SNToHead_(best_fit_sn);

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

size_t Zeta_TreeAllocator_Query(void* ta_, size_t size) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    if (size == 0) { return 0; }

    size_t align = ta->align;

    if (size < ta->least_size) { size = ta->least_size; }

    return ZETA_IntRoundUp(occupied_head_size + size, align) -
           occupied_head_size;
}

void* Zeta_TreeAllocator_Allocate(void* ta_, size_t size) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    if (size == 0) { return NULL; }

    if (size < ta->least_size) { size = ta->least_size; }

    Zeta_TreeAllocator_Head* l_head;

    int choose_ret = ChooseNiceBlock_(ta, size, &l_head);

    if (choose_ret == choose_ret_failed) { return NULL; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_OrdRBLinkedListNode_SetColor(&l_head->hn, occupied_color);

    ta->sn_root = Zeta_RBTree_Extract(&btn_opr, &l_head->sn);

    if (choose_ret == choose_ret_fit) {
        return (unsigned char*)(l_head) + occupied_head_size;
    }

    size_t align = ta->align;

    Zeta_TreeAllocator_Head* m_head =
        (void*)((unsigned char*)(l_head) +
                ZETA_IntRoundUp(occupied_head_size + size, align));

    Zeta_TreeAllocator_Head* r_head =
        HNToHead_(Zeta_OrdRBLinkedListNode_GetR(&l_head->hn));

    Zeta_OrdRBLinkedListNode_Init(&m_head->hn);
    Zeta_OrdRBLinkedListNode_SetColor(&m_head->hn, vacant_color);
    Zeta_OrdRBLinkedListNode_InsertR(&l_head->hn, &m_head->hn);

    Zeta_OrdRBTreeNode_Init(NULL, &m_head->sn);

    ta->sn_root = Zeta_RBTree_GeneralInsertL(
        &btn_opr, ta->sn_root,
        FindBlock_(ta->sn_root,
                   (unsigned char*)(r_head) - (unsigned char*)(m_head)),
        &m_head->sn);

    return (unsigned char*)(l_head) + occupied_head_size;
}

void Zeta_TreeAllocator_Deallocate(void* ta_, void* ptr) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    if (ptr == NULL) { return; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_TreeAllocator_Head* m_head =
        (void*)((unsigned char*)ptr - occupied_head_size);

    Zeta_TreeAllocator_Head* l_head =
        HNToHead_(Zeta_OrdRBLinkedListNode_GetL(&m_head->hn));

    Zeta_TreeAllocator_Head* r_head =
        HNToHead_(Zeta_OrdRBLinkedListNode_GetR(&m_head->hn));

    bool_t merge_l = IsVacant_(l_head);
    bool_t merge_r = IsVacant_(r_head);

    if (merge_l) {
        Zeta_OrdRBLinkedListNode_Extract(&m_head->hn);
        ta->sn_root = Zeta_RBTree_Extract(&btn_opr, &l_head->sn);
    } else {
        Zeta_OrdRBLinkedListNode_SetColor(&m_head->hn, vacant_color);
        Zeta_OrdRBTreeNode_Init(NULL, &m_head->sn);
        l_head = m_head;
    }

    if (merge_r) {
        Zeta_TreeAllocator_Head* rr_head =
            HNToHead_(Zeta_OrdRBLinkedListNode_GetR(&r_head->hn));

        Zeta_OrdRBLinkedListNode_Extract(&r_head->hn);
        ta->sn_root = Zeta_RBTree_Extract(&btn_opr, &r_head->sn);

        r_head = rr_head;
    }

    ZETA_CheckAssert(!IsVacant_(r_head));

    ta->sn_root = Zeta_RBTree_GeneralInsertL(
        &btn_opr, ta->sn_root,
        FindBlock_(ta->sn_root,
                   (unsigned char*)(r_head) - (unsigned char*)(l_head)),
        &l_head->sn);

    ZETA_CheckAssert(!IsVacant_(r_head));
}

static void GetVacantHead_(Zeta_MemRecorder* dst_head_recorder,
                           Zeta_TreeAllocator_Head* head, size_t lb,
                           size_t ub) {
    size_t stride = GetWidth_(head);
    ZETA_DebugAssert(lb <= stride && stride <= ub);

    Zeta_OrdRBTreeNode* sn = &head->sn;

    Zeta_OrdRBTreeNode* sn_l = Zeta_OrdRBTreeNode_GetL(NULL, sn);
    Zeta_OrdRBTreeNode* sn_r = Zeta_OrdRBTreeNode_GetR(NULL, sn);

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

    unsigned char* data_beg = ta->data_beg;
    unsigned char* data_end = ta->data_end;

    size_t head_align = alignof(Zeta_TreeAllocator_Head);

    size_t align = ta->align;

    ZETA_DebugAssert(__builtin_is_aligned(data_beg, head_align));
    ZETA_DebugAssert(
        __builtin_is_aligned(data_beg + occupied_head_size, align));
    ZETA_DebugAssert(
        __builtin_is_aligned(data_end - occupied_head_size, head_align));
    ZETA_DebugAssert(__builtin_is_aligned(data_end, align));

    Zeta_TreeAllocator_Head* beg_head = (void*)data_beg;
    Zeta_TreeAllocator_Head* end_head = (void*)(data_end - occupied_head_size);

    ZETA_DebugAssert(Zeta_OrdRBTreeNode_GetP(NULL, ta->sn_root) == NULL);

    Zeta_MemRecorder* vacant_head_recorder = Zeta_MemRecorder_Create();

    GetVacantHead_(vacant_head_recorder, SNToHead_(ta->sn_root), 0,
                   ZETA_RangeMaxOf(size_t));

    for (Zeta_OrdRBLinkedListNode* hn_i = &beg_head->hn;
         hn_i != &end_head->hn;) {
        Zeta_TreeAllocator_Head* head_i = HNToHead_(hn_i);
        ZETA_DebugAssert(__builtin_is_aligned(head_i, head_align));

        void* data_i = (unsigned char*)(head_i) + occupied_head_size;
        ZETA_DebugAssert(__builtin_is_aligned(data_i, align));

        Zeta_OrdRBLinkedListNode* nxt_hn_i =
            Zeta_OrdRBLinkedListNode_GetR(hn_i);

        ZETA_DebugAssert(hn_i < nxt_hn_i);

        Zeta_TreeAllocator_Head* nxt_head_i = HNToHead_(nxt_hn_i);

        size_t stride = (unsigned char*)nxt_head_i - (unsigned char*)head_i;
        size_t size = (unsigned char*)nxt_head_i - (unsigned char*)data_i;

        ZETA_DebugAssert(occupied_head_size + ta->least_size <= stride);
        ZETA_DebugAssert(occupied_head_size + size == stride);

        if (IsVacant_(head_i)) {
            bool_t b = Zeta_MemRecorder_Unrecord(vacant_head_recorder, head_i);
            ZETA_DebugAssert(b);
        } else {
            Zeta_MemRecorder_Record(
                dst_recorder, data_i,
                (unsigned char*)nxt_head_i - (unsigned char*)data_i);
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
