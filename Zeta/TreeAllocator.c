#include "TreeAllocator.h"

#include "DebugTreeMap.h"
#include "RBTree.h"
#include "utils.h"

typedef struct Zeta_TreeAllocator_Head Zeta_TreeAllocator_Head;

struct Zeta_TreeAllocator_Head {
    Zeta_OrdRBLinkedListNode hn;  // link to other heads
    Zeta_OrdRBTreeNode sn;        // link to other vacant
};

#define vacant_head_size sizeof(Zeta_TreeAllocator_Head)

#define occupied_head_size \
    (offsetof(Zeta_TreeAllocator_Head, hn) + sizeof(Zeta_OrdRBLinkedListNode))

#define GetNxtAlign_(base, align) ((base + align - 1) / align * align)

#define vacant_color 0
#define occupied_color 1

void Zeta_TreeAllocator_Init(void* ta_) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    size_t head_align = alignof(Zeta_TreeAllocator_Head);

    size_t align = ta->align;
    ZETA_DebugAssert(0 < align);

    ta->align = align = Zeta_GetLCM(head_align, align);

    ZETA_DebugAssert(ta->mem != NULL);

    uintptr_t mem_beg = ZETA_GetAddrFromPtr(ta->mem);
    uintptr_t mem_end = mem_beg + ta->size;

    uintptr_t data_beg =
        GetNxtAlign_(mem_beg + occupied_head_size, align) - occupied_head_size;
    uintptr_t data_end = mem_end / head_align * head_align;

    ZETA_DebugAssert(data_beg <= data_end);
    ZETA_DebugAssert(512 <= data_end - data_beg);

    ta->data_beg = ZETA_GetPtrFromAddr(data_beg);
    ta->data_end = ZETA_GetPtrFromAddr(data_end);

    {
        size_t least_stride = occupied_head_size + ta->least_size;
        size_t tmp;

        tmp = vacant_head_size;
        least_stride = least_stride < tmp ? tmp : least_stride;

        tmp = occupied_head_size + sizeof(void*) * 8;
        least_stride = least_stride < tmp ? tmp : least_stride;

        ta->least_size = least_stride - occupied_head_size;
    }

    Zeta_TreeAllocator_Head* beg_head = ZETA_GetPtrFromAddr(data_beg);
    Zeta_TreeAllocator_Head* end_head =
        ZETA_GetPtrFromAddr(data_end - occupied_head_size);

    Zeta_OrdRBLinkedListNode_Init(&beg_head->hn);
    Zeta_OrdRBLinkedListNode_SetColor(&beg_head->hn, vacant_color);
    Zeta_OrdRBTreeNode_Init(NULL, &beg_head->sn);

    Zeta_OrdRBLinkedListNode_Init(&end_head->hn);
    Zeta_OrdRBLinkedListNode_SetColor(&end_head->hn, occupied_color);

    Zeta_OrdRBLinkedListNode_InsertR(&beg_head->hn, &end_head->hn);

    ta->sn_root = &beg_head->sn;
}

size_t Zeta_TreeAllocator_GetAlign(void* ta_) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    return ta->align;
}

static bool_t IsVacant_(Zeta_TreeAllocator_Head* head) {
    return Zeta_OrdRBLinkedListNode_GetColor(&head->hn) == vacant_color;
}

static Zeta_TreeAllocator_Head* GetHeadFromHN_(Zeta_OrdRBLinkedListNode* hn) {
    return ZETA_GetStructFromMember(Zeta_TreeAllocator_Head, hn, hn);
}

static Zeta_TreeAllocator_Head* GetHeadFromSN_(Zeta_OrdRBTreeNode* sn) {
    return ZETA_GetStructFromMember(Zeta_TreeAllocator_Head, sn, sn);
}

static size_t GetStride_(Zeta_TreeAllocator_Head* head) {
    Zeta_TreeAllocator_Head* nxt_head =
        GetHeadFromHN_(Zeta_OrdRBLinkedListNode_GetR(&head->hn));

    return ZETA_GetAddrFromPtr(nxt_head) - ZETA_GetAddrFromPtr(head);
}

static Zeta_OrdRBTreeNode* FindBlock_(Zeta_OrdRBTreeNode* sn_root,
                                      size_t stride) {
    Zeta_OrdRBTreeNode* sn = sn_root;
    Zeta_OrdRBTreeNode* ret = NULL;

    while (sn != NULL) {
        size_t sn_stride = GetStride_(GetHeadFromSN_(sn));

        if (sn_stride < stride) {
            sn = Zeta_OrdRBTreeNode_GetR(NULL, sn);
        } else {
            ret = sn;
            sn = Zeta_OrdRBTreeNode_GetL(NULL, sn);
        }
    }

    return ret;
}

#define CHOOSE_RET_FAILED 0
#define CHOOSE_RET_FIT 1
#define CHOOSE_RET_CUT 2

static int ChooseNiceBlock_(Zeta_TreeAllocator* ta, size_t size,
                            Zeta_TreeAllocator_Head** dst) {
    size_t align = ta->align;

    Zeta_OrdRBTreeNode* best_fit_sn =
        FindBlock_(ta->sn_root, occupied_head_size + size);

    if (best_fit_sn == NULL) { return CHOOSE_RET_FAILED; }

    size_t least_stride = occupied_head_size + ta->least_size;

    uintptr_t cut_stride = GetNxtAlign_(occupied_head_size + size, align);

    Zeta_TreeAllocator_Head* best_fit_head = GetHeadFromSN_(best_fit_sn);

    if (GetStride_(best_fit_head) <= cut_stride + least_stride) {
        *dst = best_fit_head;
        return CHOOSE_RET_FIT;
    }

    Zeta_OrdRBTreeNode* twice_fit_sn =
        FindBlock_(ta->sn_root, cut_stride + least_stride);

    *dst = GetHeadFromSN_(
        twice_fit_sn == NULL
            ? twice_fit_sn
            : Zeta_GetMostLink(NULL, Zeta_OrdRBTreeNode_GetR, ta->sn_root));

    return CHOOSE_RET_CUT;
}

size_t Zeta_TreeAllocator_Query(void* ta_, size_t size) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    if (size == 0) { return 0; }

    size_t align = ta->align;

    if (size < ta->least_size) { size = ta->least_size; }

    return GetNxtAlign_(occupied_head_size + size, align) - occupied_head_size;
}

void* Zeta_TreeAllocator_Allocate(void* ta_, size_t size) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    if (size == 0) { return NULL; }

    if (size < ta->least_size) { size = ta->least_size; }

    Zeta_TreeAllocator_Head* l_head;
    int choose_ret = ChooseNiceBlock_(ta, size, &l_head);

    if (choose_ret == CHOOSE_RET_FAILED) { return NULL; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_OrdRBLinkedListNode_SetColor(&l_head->hn, occupied_color);

    ta->sn_root = Zeta_RBTree_Extract(&btn_opr, &l_head->sn);

    if (choose_ret == CHOOSE_RET_FIT) {
        return (unsigned char*)(l_head) + occupied_head_size;
    }

    size_t align = ta->align;

    Zeta_TreeAllocator_Head* m_head =
        (void*)((unsigned char*)(l_head) +
                GetNxtAlign_(occupied_head_size + size, align));

    Zeta_TreeAllocator_Head* r_head =
        GetHeadFromHN_(Zeta_OrdRBLinkedListNode_GetR(&l_head->hn));

    Zeta_OrdRBLinkedListNode_Init(&m_head->hn);
    Zeta_OrdRBLinkedListNode_SetColor(&m_head->hn, vacant_color);
    Zeta_OrdRBLinkedListNode_InsertR(&l_head->hn, &m_head->hn);

    Zeta_OrdRBTreeNode_Init(NULL, &m_head->sn);

    ta->sn_root = Zeta_RBTree_GeneralInsertL(
        &btn_opr, ta->sn_root,
        FindBlock_(ta->sn_root,
                   ZETA_GetAddrFromPtr(r_head) - ZETA_GetAddrFromPtr(m_head)),
        &m_head->sn);

    return (unsigned char*)(l_head) + occupied_head_size;
}

void Zeta_TreeAllocator_Deallocate(void* ta_, void* ptr) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    if (ptr == NULL) { return; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_TreeAllocator_Head* m_head =
        (void*)((unsigned char*)ptr - occupied_head_size);

    Zeta_TreeAllocator_Head* l_head =
        GetHeadFromHN_(Zeta_OrdRBLinkedListNode_GetL(&m_head->hn));

    Zeta_TreeAllocator_Head* r_head =
        GetHeadFromHN_(Zeta_OrdRBLinkedListNode_GetR(&m_head->hn));

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
            GetHeadFromHN_(Zeta_OrdRBLinkedListNode_GetR(&r_head->hn));

        Zeta_OrdRBLinkedListNode_Extract(&r_head->hn);

        ta->sn_root = Zeta_RBTree_Extract(&btn_opr, &r_head->sn);

        r_head = rr_head;
    }

    ta->sn_root = Zeta_RBTree_GeneralInsertL(
        &btn_opr, ta->sn_root,
        FindBlock_(ta->sn_root,
                   ZETA_GetAddrFromPtr(r_head) - ZETA_GetAddrFromPtr(l_head)),
        &l_head->sn);
}

static void GetVacantHead_(Zeta_DebugTreeMap* dst_head_mt,
                           Zeta_TreeAllocator_Head* head, size_t lb,
                           size_t ub) {
    size_t stride = GetStride_(head);
    ZETA_DebugAssert(lb <= stride && stride <= ub);

    Zeta_OrdRBTreeNode* sn = &head->sn;

    Zeta_OrdRBTreeNode* sn_l = Zeta_OrdRBTreeNode_GetL(NULL, sn);
    Zeta_OrdRBTreeNode* sn_r = Zeta_OrdRBTreeNode_GetR(NULL, sn);

    bool_t b =
        Zeta_DebugTreeMap_Insert(dst_head_mt, ZETA_GetAddrFromPtr(head)).b;

    ZETA_DebugAssert(b);

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

    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    uintptr_t data_beg = ZETA_GetAddrFromPtr(ta->data_beg);
    uintptr_t data_end = ZETA_GetAddrFromPtr(ta->data_end);

    size_t head_align = alignof(Zeta_TreeAllocator_Head);

    size_t align = ta->align;

    ZETA_DebugAssert(data_beg % head_align == 0);
    ZETA_DebugAssert((data_beg + occupied_head_size) % align == 0);
    ZETA_DebugAssert((data_end - occupied_head_size) % head_align == 0);
    ZETA_DebugAssert(data_end % align == 0);

    Zeta_TreeAllocator_Head* beg_head = ZETA_GetPtrFromAddr(data_beg);
    Zeta_TreeAllocator_Head* end_head =
        ZETA_GetPtrFromAddr(data_end - occupied_head_size);

    ZETA_DebugAssert(Zeta_OrdRBTreeNode_GetP(NULL, ta->sn_root) == NULL);

    Zeta_DebugTreeMap vacant_head_mt;
    Zeta_DebugTreeMap_Create(&vacant_head_mt);

    GetVacantHead_(&vacant_head_mt, GetHeadFromSN_(ta->sn_root), 0,
                   ZETA_GetRangeMax(size_t));

    for (Zeta_OrdRBLinkedListNode* hn_i = &beg_head->hn;
         hn_i != &end_head->hn;) {
        Zeta_TreeAllocator_Head* head_i = GetHeadFromHN_(hn_i);
        ZETA_DebugAssert(ZETA_GetAddrFromPtr(head_i) % head_align == 0);

        void* data_i = (unsigned char*)(head_i) + occupied_head_size;

        ZETA_DebugAssert(ZETA_GetAddrFromPtr(data_i) % align == 0);

        Zeta_OrdRBLinkedListNode* nxt_hn_i =
            Zeta_OrdRBLinkedListNode_GetR(hn_i);

        ZETA_DebugAssert(hn_i < nxt_hn_i);

        Zeta_TreeAllocator_Head* nxt_head_i = GetHeadFromHN_(nxt_hn_i);

        size_t stride =
            ZETA_GetAddrFromPtr(nxt_head_i) - ZETA_GetAddrFromPtr(head_i);
        size_t size =
            ZETA_GetAddrFromPtr(nxt_head_i) - ZETA_GetAddrFromPtr(data_i);

        ZETA_DebugAssert(occupied_head_size + ta->least_size <= stride);
        ZETA_DebugAssert(occupied_head_size + size == stride);

        if (IsVacant_(head_i)) {
            bool_t b = Zeta_DebugTreeMap_Erase(&vacant_head_mt,
                                               ZETA_GetAddrFromPtr(head_i));

            ZETA_DebugAssert(b);
        } else {
            Zeta_DebugTreeMap_KeyValPair kvp = Zeta_DebugTreeMap_Insert(
                dst_ptr_size_tm, ZETA_GetAddrFromPtr(data_i));

            ZETA_DebugAssert(kvp.b);

            *kvp.val =
                ZETA_GetAddrFromPtr(nxt_head_i) - ZETA_GetAddrFromPtr(data_i);
        }

        hn_i = nxt_hn_i;
    }

    ZETA_DebugAssert(Zeta_DebugTreeMap_GetSize(&vacant_head_mt) == 0);
    Zeta_DebugTreeMap_Destroy(&vacant_head_mt);
}

void Zeta_TreeAllocator_DeployAllocator(void* ta_, Zeta_Allocator* dst) {
    Zeta_TreeAllocator* ta = ta_;
    ZETA_DebugAssert(ta != NULL);

    ZETA_DebugAssert(dst != NULL);

    dst->context = ta;

    dst->GetAlign = Zeta_TreeAllocator_GetAlign;
    dst->Query = Zeta_TreeAllocator_Query;

    dst->Allocate = Zeta_TreeAllocator_Allocate;
    dst->Deallocate = Zeta_TreeAllocator_Deallocate;
}
