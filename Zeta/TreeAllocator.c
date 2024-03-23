#include "TreeAllocator.h"

#include "DebugTreeMap.h"
#include "RBTree.h"
#include "RelLinkedListNode.h"
#include "utils.h"

typedef struct Zeta_TreeAllocator_Head Zeta_TreeAllocator_Head;

struct Zeta_TreeAllocator_Head {
    Zeta_RelLinkedListNode hn;  // link to other heads
    Zeta_RelRBTreeNode sn;      // link to other vacant
};

void Zeta_TreeAllocator_Init(void* ta_) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    size_t head_align = alignof(Zeta_TreeAllocator_Head);
    size_t head_size = sizeof(Zeta_TreeAllocator_Head);

    size_t align = ta->align;
    ZETA_DebugAssert(0 < align);

    align = Zeta_GetLCM(head_align, align);

    ta->align = align;

    ZETA_DebugAssert(ta->mem != NULL);

    uintptr_t mem_beg = ZETA_PTR_TO_UINT(ta->mem);
    uintptr_t mem_end = mem_beg + ta->size;

    uintptr_t data_beg =
        Zeta_FindNextConMod(mem_beg + head_size, 0, align) - head_size;

    uintptr_t data_end = mem_end / align * align;

    ZETA_DebugAssert(data_beg <= data_end);

    ta->data_beg = ZETA_UINT_TO_PTR(data_beg);
    ta->data_end = ZETA_UINT_TO_PTR(data_end);

    Zeta_TreeAllocator_Head* beg_head = ZETA_UINT_TO_PTR(data_beg);
    Zeta_TreeAllocator_Head* end_head = ZETA_UINT_TO_PTR(data_end - head_size);

    Zeta_RelLinkedListNode_Init(&beg_head->hn);
    Zeta_RelRBTreeNode_Init(NULL, &beg_head->sn);

    Zeta_RelLinkedListNode_Init(&end_head->hn);
    Zeta_RelRBTreeNode_Init(NULL, &end_head->sn);

    Zeta_RelLinkedListNode_InsertR(&beg_head->hn, &end_head->hn);

    ta->sn_root = &beg_head->sn;
}

size_t Zeta_TreeAllocator_GetAlign(void* ta_) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    return ta->align;
}

static bool_t IsVacant_(Zeta_TreeAllocator* ta, Zeta_TreeAllocator_Head* head) {
    return ta->sn_root == &head->sn ||
           Zeta_RelRBTreeNode_GetP(NULL, &head->sn) != NULL ||
           Zeta_RelRBTreeNode_GetL(NULL, &head->sn) != NULL ||
           Zeta_RelRBTreeNode_GetR(NULL, &head->sn) != NULL;
}

static Zeta_TreeAllocator_Head* GetHeadFromHN_(void* hn) {
    return ZETA_GetStructFromMember(Zeta_TreeAllocator_Head, hn, hn);
}

static Zeta_TreeAllocator_Head* GetHeadFromSN_(void* sn) {
    return ZETA_GetStructFromMember(Zeta_TreeAllocator_Head, sn, sn);
}

static size_t GetStride_(Zeta_TreeAllocator_Head* head) {
    Zeta_TreeAllocator_Head* nxt_head =
        GetHeadFromHN_(Zeta_RelLinkedListNode_GetR(&head->hn));

    return ZETA_PTR_TO_UINT(nxt_head) - ZETA_PTR_TO_UINT(head);
}

static void* FindBlock_(void* sn_root, size_t stride) {
    void* sn = sn_root;
    void* ret = NULL;

    while (sn != NULL) {
        size_t sn_stride = GetStride_(GetHeadFromSN_(sn));

        if (sn_stride < stride) {
            sn = Zeta_RelRBTreeNode_GetR(NULL, sn);
        } else {
            ret = sn;
            sn = Zeta_RelRBTreeNode_GetL(NULL, sn);
        }
    }

    return ret;
}

static Zeta_TreeAllocator_Head* ChooseNiceBlock_(Zeta_TreeAllocator* ta,
                                                 size_t size) {
    size_t align = ta->align;

    size_t head_size = sizeof(Zeta_TreeAllocator_Head);

    void* best_fit_sn = FindBlock_(ta->sn_root, head_size + size);

    if (best_fit_sn == NULL) { return NULL; }

    Zeta_TreeAllocator_Head* best_fit_head = GetHeadFromSN_(best_fit_sn);

    if (GetStride_(best_fit_head) == head_size + size) { return best_fit_head; }

    void* twice_fit_sn = FindBlock_(
        ta->sn_root, Zeta_FindNextConMod(head_size + size, 0, align) * 2);

    if (twice_fit_sn != NULL) { return GetHeadFromSN_(twice_fit_sn); }

    return GetHeadFromSN_(
        Zeta_GetMostLink(NULL, Zeta_RelRBTreeNode_GetR, ta->sn_root));
}

size_t Zeta_TreeAllocator_Query(void* ta_, size_t size) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    if (size == 0) { return 0; }

    size_t align = ta->align;

    size_t head_size = sizeof(Zeta_TreeAllocator_Head);

    return Zeta_FindNextConMod(size + head_size, 0, align) - head_size;
}

void* Zeta_TreeAllocator_Allocate(void* ta_, size_t size) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    if (size == 0) { return NULL; }

    Zeta_TreeAllocator_Head* l_head = ChooseNiceBlock_(ta, size);
    if (l_head == NULL) { return NULL; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_RelRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t align = ta->align;

    size_t head_size = sizeof(Zeta_TreeAllocator_Head);

    Zeta_TreeAllocator_Head* m_head =
        ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(l_head) +
                         Zeta_FindNextConMod(size + head_size, 0, align));

    Zeta_TreeAllocator_Head* r_head =
        GetHeadFromHN_(Zeta_RelLinkedListNode_GetR(&l_head->hn));

    ta->sn_root = Zeta_RBTree_Extract(&btn_opr, &l_head->sn);

    if (m_head == r_head) { return (Zeta_TreeAllocator_Head*)l_head + 1; }

    Zeta_RelLinkedListNode_Init(&m_head->hn);
    Zeta_RelLinkedListNode_InsertR(&l_head->hn, &m_head->hn);

    Zeta_RelRBTreeNode_Init(NULL, &m_head->sn);

    size_t m_stride = ZETA_PTR_TO_UINT(r_head) - ZETA_PTR_TO_UINT(m_head);

    void* ins_sn = FindBlock_(ta->sn_root, m_stride);

    if (ins_sn == NULL) {
        ta->sn_root = Zeta_RBTree_InsertR(
            &btn_opr,
            Zeta_GetMostLink(NULL, Zeta_RelRBTreeNode_GetR, ta->sn_root),
            &m_head->sn);
    } else {
        ta->sn_root = Zeta_RBTree_InsertL(&btn_opr, ins_sn, &m_head->sn);
    }

    return (Zeta_TreeAllocator_Head*)l_head + 1;
}

void Zeta_TreeAllocator_Deallocate(void* ta_, void* ptr) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    if (ptr == NULL) { return; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_RelRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_TreeAllocator_Head* m_head = (Zeta_TreeAllocator_Head*)ptr - 1;

    Zeta_TreeAllocator_Head* l_head =
        GetHeadFromHN_(Zeta_RelLinkedListNode_GetL(&m_head->hn));

    Zeta_TreeAllocator_Head* r_head =
        GetHeadFromHN_(Zeta_RelLinkedListNode_GetR(&m_head->hn));

    bool_t merge_l = IsVacant_(ta, l_head);
    bool_t merge_r = IsVacant_(ta, r_head);

    if (merge_l) {
        Zeta_RelLinkedListNode_Extract(&m_head->hn);
        ta->sn_root = Zeta_RBTree_Extract(&btn_opr, &l_head->sn);
    } else {
        Zeta_RelRBTreeNode_Init(NULL, &m_head->sn);
        l_head = m_head;
    }

    if (merge_r) {
        Zeta_TreeAllocator_Head* rr_head =
            GetHeadFromHN_(Zeta_RelLinkedListNode_GetR(&r_head->hn));

        Zeta_RelLinkedListNode_Extract(&r_head->hn);

        ta->sn_root = Zeta_RBTree_Extract(&btn_opr, &r_head->sn);

        r_head = rr_head;
    }

    size_t lr_stride = ZETA_PTR_TO_UINT(r_head) - ZETA_PTR_TO_UINT(l_head);

    void* ins_sn = FindBlock_(ta->sn_root, lr_stride);

    if (ins_sn == NULL) {
        ta->sn_root = Zeta_RBTree_InsertR(
            &btn_opr,
            Zeta_GetMostLink(NULL, Zeta_RelRBTreeNode_GetR, ta->sn_root),
            &l_head->sn);
    } else {
        ta->sn_root = Zeta_RBTree_InsertL(&btn_opr, ins_sn, &l_head->sn);
    }
}

static void GetVacantHead_(Zeta_DebugTreeMap* dst_head_mt,
                           Zeta_TreeAllocator_Head* head, size_t lb,
                           size_t ub) {
    size_t stride = GetStride_(head);
    ZETA_DebugAssert(lb <= stride && stride <= ub);

    Zeta_RelRBTreeNode* sn = &head->sn;

    Zeta_RelRBTreeNode* sn_l = Zeta_RelRBTreeNode_GetL(NULL, sn);
    Zeta_RelRBTreeNode* sn_r = Zeta_RelRBTreeNode_GetR(NULL, sn);

    Zeta_DebugTreeMap_Insert(dst_head_mt, ZETA_PTR_TO_UINT(head));

    if (sn_l != NULL) {
        GetVacantHead_(dst_head_mt, GetHeadFromSN_(sn_l), lb, stride);
    }

    if (sn_r != NULL) {
        GetVacantHead_(dst_head_mt, GetHeadFromSN_(sn_r), stride, ub);
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

void Zeta_TreeAllocator_Check(void* ta_, bool_t print_state,
                              void* dst_ptr_size_tm) {
    ZETA_Unused(print_state);
    ZETA_DebugAssert(dst_ptr_size_tm != NULL);

    Zeta_DebugTreeMap_EraseAll(dst_ptr_size_tm);

    typedef unsigned long long ull;

    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    uintptr_t data_beg = ZETA_PTR_TO_UINT(ta->data_beg);
    uintptr_t data_end = ZETA_PTR_TO_UINT(ta->data_end);

    size_t head_align = alignof(Zeta_TreeAllocator_Head);
    size_t head_size = sizeof(Zeta_TreeAllocator_Head);

    size_t align = ta->align;

    ZETA_DebugAssert(data_beg % head_align == 0);
    ZETA_DebugAssert((data_beg + head_size) % align == 0);
    ZETA_DebugAssert((data_end - head_size) % head_align == 0);
    ZETA_DebugAssert(data_end % align == 0);

    Zeta_TreeAllocator_Head* beg_head = ZETA_UINT_TO_PTR(data_beg);
    Zeta_TreeAllocator_Head* end_head = ZETA_UINT_TO_PTR(data_end - head_size);

    ZETA_DebugAssert(Zeta_RelRBTreeNode_GetP(NULL, ta->sn_root) == NULL);

    Zeta_DebugTreeMap vacant_head_mt;
    Zeta_DebugTreeMap_Create(&vacant_head_mt);

    GetVacantHead_(&vacant_head_mt, GetHeadFromSN_(ta->sn_root), 0,
                   ZETA_maxof(size_t));

    for (Zeta_RelLinkedListNode* hn_i = &beg_head->hn; hn_i != &end_head->hn;) {
        Zeta_TreeAllocator_Head* head_i = GetHeadFromHN_(hn_i);
        ZETA_DebugAssert(ZETA_PTR_TO_UINT(head_i) % head_align == 0);

        void* data_i = ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(head_i) + head_size);

        ZETA_DebugAssert(ZETA_PTR_TO_UINT(data_i) % align == 0);

        Zeta_RelLinkedListNode* nxt_hn_i = Zeta_RelLinkedListNode_GetR(hn_i);

        ZETA_DebugAssert(hn_i < nxt_hn_i);

        Zeta_TreeAllocator_Head* nxt_head_i = GetHeadFromHN_(nxt_hn_i);

        size_t stride = (ull)nxt_head_i - (ull)head_i;
        size_t size = (ull)nxt_head_i - (ull)data_i;

        ZETA_DebugAssert(head_size <= stride);
        ZETA_DebugAssert(size + head_size == stride);

        if (IsVacant_(ta, head_i)) {
            bool_t b = Zeta_DebugTreeMap_Erase(&vacant_head_mt,
                                               ZETA_PTR_TO_UINT(head_i));

            ZETA_DebugAssert(b);
        } else {
            Zeta_DebugTreeMap_KeyValPair kvp = Zeta_DebugTreeMap_Insert(
                dst_ptr_size_tm, ZETA_PTR_TO_UINT(data_i));

            ZETA_DebugAssert(kvp.b);

            *kvp.val = (ull)nxt_head_i - (ull)data_i;
        }

        hn_i = nxt_hn_i;
    }

    ZETA_DebugAssert(Zeta_DebugTreeMap_GetSize(&vacant_head_mt) == 0);
    Zeta_DebugTreeMap_Destroy(&vacant_head_mt);
}

void Zeta_TreeAllocator_ToAllocator(void* ta_, Zeta_Allocator* dst) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    ZETA_DebugAssert(dst != NULL);

    dst->context = ta;
    dst->GetAlign = Zeta_TreeAllocator_GetAlign;
    dst->Query = Zeta_TreeAllocator_Query;
    dst->Allocate = Zeta_TreeAllocator_Allocate;
    dst->Deallocate = Zeta_TreeAllocator_Deallocate;
}
