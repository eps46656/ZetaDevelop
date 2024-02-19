#include "OrdAllocator.h"

#include "Algorithm.h"
#include "DebugTreeMap.h"
#include "RBTree.h"
#include "RawVector.h"
#include "utils.h"

#define ZETA_OrdAllocator_state_vacant -1
#define ZETA_OrdAllocator_state_occupied -2

typedef struct Zeta_OrdAllocator_Head Zeta_OrdAllocator_Head;

struct Zeta_OrdAllocator_Head {
    int state;
    Zeta_RelLinkedListNode hn;  // link to other heads

    union {
        struct {
            Zeta_RelRBTreeNode sn;  // link to other vacant
        } vacant_info;

        struct {
            uintptr_t ptr;  // points to head
        } occupied_info;

        struct {
            Zeta_RelLinkedListNode slab_n;  // link to slab list
            uintptr_t unit_ptr;             // points to slab unit

            uintptr_t ptr;  // points to nxt unit or head
        } slab_info;
    };
};

/*

head {
    state 4
    pad 4

    hn {
        l 8
        r 8
    } 16

    union {
        vacant info {
            vacant_rbn {
                p 8
                l 8
                r 8
            } 8
        } 24

        occupied info {
            ptr 8
        } 8

        slab info {
            slab_n {
                l 8
                r 8
            } 16

            unit_ptr 8

            ptr 8
        } 32
    } 32
} 56

*/

static void* GetPtr_(uintptr_t* rel_ptr) {
    return ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(rel_ptr) + *rel_ptr);
}

static void SetPtr_(uintptr_t* rel_ptr, void* ptr) {
    *rel_ptr = ZETA_PTR_TO_UINT(ptr) - ZETA_PTR_TO_UINT(rel_ptr);
}

typedef struct WidthSizePair WidthSizePair;

struct WidthSizePair {
    size_t width;
    size_t size;
};

static int CompareWidthSizePair(void* context, const void* x_, const void* y_) {
    ZETA_Unused(context);

    const WidthSizePair* x = x_;
    const WidthSizePair* y = y_;

    if (x->width < y->width) { return -1; }
    if (y->width < x->width) { return 1; }
    return 0;
}

void Zeta_OrdAllocator_Init(void* ord_allocator_) {
    Zeta_OrdAllocator* ord_allocator = ord_allocator_;
    ZETA_DebugAssert(ord_allocator != NULL);

    size_t head_align = alignof(Zeta_OrdAllocator_Head);
    size_t head_size = sizeof(Zeta_OrdAllocator_Head);

    size_t align = ord_allocator->align;
    ZETA_DebugAssert(0 < align);

    align = Zeta_GetLCM(head_align, align);

    ord_allocator->align = align;

    ZETA_DebugAssert(ord_allocator->ptr != NULL);

    size_t beg = ZETA_PTR_TO_UINT(ord_allocator->ptr);
    size_t end = beg + ord_allocator->size;

    beg = Zeta_FindNextConMod(beg, align - head_size % align, align);
    end = end / align * align;

    ZETA_DebugAssert(beg <= end);

    ord_allocator->ptr = ZETA_UINT_TO_PTR(beg);
    ord_allocator->size = end - beg;

    int num_of_slab_level = ord_allocator->num_of_slab_level;

    if (num_of_slab_level < 0) {
        if (head_size * 8 <= align) {
            num_of_slab_level = 0;
        } else {
            size_t slab_width_unit =
                head_size < sizeof(void*) * 4 ? sizeof(void*) * 4 : head_size;

            slab_width_unit =
                Zeta_FindNextConMod(slab_width_unit + sizeof(void*), 0, align);

            size_t rates[] = { 1, 2, 3, 4, 5, 6, 7 };
            size_t sizes[] = { 12, 12, 12, 8, 6, 4, 4 };

            num_of_slab_level = sizeof(rates) / sizeof(rates[0]);
            ord_allocator->num_of_slab_level = num_of_slab_level;

            for (int level_i = 0; level_i < num_of_slab_level; ++level_i) {
                ord_allocator->slab_widths[level_i] =
                    slab_width_unit * rates[level_i] - sizeof(void*);

                ord_allocator->slab_sizes[level_i] = sizes[level_i];
            }
        }
    } else if (0 < num_of_slab_level) {
        ZETA_DebugAssert(num_of_slab_level <=
                         ZETA_OrdAllocator_max_num_of_slab_level);

        WidthSizePair width_size_pairs[ZETA_OrdAllocator_max_num_of_slab_level];

        for (int level_i = 0; level_i < num_of_slab_level; ++level_i) {
            size_t cur_width = ord_allocator->slab_widths[level_i];
            size_t cur_size = ord_allocator->slab_sizes[level_i];

            ZETA_DebugAssert(0 < cur_width);
            ZETA_DebugAssert(0 < cur_size);

            width_size_pairs[level_i].width = Zeta_FindNextConMod(
                cur_width, align - sizeof(void*) % align, align);
            width_size_pairs[level_i].size = cur_size;
        }

        Zeta_RawVector raw_vec = {
            .data = width_size_pairs,
            .width = sizeof(WidthSizePair),
            .stride = sizeof(WidthSizePair),
            .size = num_of_slab_level,
        };

        Zeta_Vector vec;
        Zeta_RawVector_ToVector(&raw_vec, &vec);

        Zeta_Sort(&vec, 0, num_of_slab_level, NULL, CompareWidthSizePair);

        ord_allocator->slab_widths[0] = width_size_pairs[0].width;
        ord_allocator->slab_sizes[0] = width_size_pairs[0].size;

        int dst_i = 1;

        for (int src_i = 1; src_i < num_of_slab_level; ++src_i) {
            size_t pre_width = width_size_pairs[src_i - 1].width;
            size_t cur_width = width_size_pairs[src_i].width;

            if (pre_width != cur_width) {
                ord_allocator->slab_widths[dst_i] = cur_width;
                ord_allocator->slab_sizes[dst_i] = width_size_pairs[src_i].size;
                ++dst_i;
            }
        }

        num_of_slab_level = dst_i;
    }

    for (int level_i = 0; level_i < ZETA_OrdAllocator_max_num_of_slab_level;
         ++level_i) {
        Zeta_RelLinkedListNode_Init(&ord_allocator->slab_list_heads[level_i]);
    }

    Zeta_OrdAllocator_Head* beg_head = ZETA_UINT_TO_PTR(beg);

    Zeta_OrdAllocator_Head* end_head =
        ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(end) - head_size);

    beg_head->state = ZETA_OrdAllocator_state_vacant;
    Zeta_RelLinkedListNode_Init(&beg_head->hn);
    Zeta_RelRBTreeNode_Init(NULL, &beg_head->vacant_info.sn);

    end_head->state = ZETA_OrdAllocator_state_occupied;
    Zeta_RelLinkedListNode_Init(&end_head->hn);
    SetPtr_(&end_head->occupied_info.ptr, end_head);

    Zeta_RelLinkedListNode_InsertR(&beg_head->hn, &end_head->hn);

    ord_allocator->sn_root = &beg_head->vacant_info.sn;
}

static Zeta_OrdAllocator_Head* GetHeadFromHN_(void* hn) {
    return ZETA_GetStructFromMember(Zeta_OrdAllocator_Head, hn, hn);
}

static Zeta_OrdAllocator_Head* GetHeadFromSN_(void* sn) {
    return ZETA_GetStructFromMember(Zeta_OrdAllocator_Head, vacant_info.sn, sn);
}

static Zeta_OrdAllocator_Head* GetHeadFromSlabN_(void* slab_n) {
    return ZETA_GetStructFromMember(Zeta_OrdAllocator_Head, slab_info.slab_n,
                                    slab_n);
}

static size_t GetStride_(Zeta_OrdAllocator_Head* head) {
    Zeta_OrdAllocator_Head* nxt_head =
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

static Zeta_OrdAllocator_Head* ChooseNiceBlock_(
    Zeta_OrdAllocator* ord_allocator, size_t size) {
    size_t align = ord_allocator->align;

    size_t head_size = sizeof(Zeta_OrdAllocator_Head);

    void* best_fit_sn = FindBlock_(ord_allocator->sn_root, head_size + size);

    if (best_fit_sn == NULL) { return NULL; }

    Zeta_OrdAllocator_Head* best_fit_head = GetHeadFromSN_(best_fit_sn);

    if (GetStride_(best_fit_head) == head_size + size) { return best_fit_head; }

    void* twice_fit_sn =
        FindBlock_(ord_allocator->sn_root,
                   Zeta_FindNextConMod(head_size + size, 0, align) * 2);

    if (twice_fit_sn != NULL) { return GetHeadFromSN_(twice_fit_sn); }

    return GetHeadFromSN_(Zeta_GetMostLink(NULL, Zeta_RelRBTreeNode_GetR,
                                           ord_allocator->sn_root));
}

static Zeta_OrdAllocator_Head* Allocate_(Zeta_OrdAllocator* ord_allocator,
                                         size_t size) {
    Zeta_OrdAllocator_Head* l_head = ChooseNiceBlock_(ord_allocator, size);
    if (l_head == NULL) { return NULL; }

    size_t align = ord_allocator->align;

    size_t head_size = sizeof(Zeta_OrdAllocator_Head);

    Zeta_OrdAllocator_Head* m_head =
        ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(l_head) +
                         Zeta_FindNextConMod(size + head_size, 0, align));

    Zeta_OrdAllocator_Head* r_head =
        GetHeadFromHN_(Zeta_RelLinkedListNode_GetR(&l_head->hn));

    const Zeta_RBTreeNodeOpr rbtn_opr = {
        .context = NULL,

        .GetP = Zeta_RelRBTreeNode_GetP,
        .GetL = Zeta_RelRBTreeNode_GetL,
        .GetR = Zeta_RelRBTreeNode_GetR,

        .GetColor = Zeta_RelRBTreeNode_GetColor,
        .ReverseColor = Zeta_RelRBTreeNode_ReverseColor,

        .AttachL = Zeta_RelRBTreeNode_AttachL,
        .AttachR = Zeta_RelRBTreeNode_AttachR,

        .Detach = Zeta_RelRBTreeNode_Detach,

        .Swap = Zeta_RelRBTreeNode_Swap,

        .RotateL = Zeta_RelRBTreeNode_RotateL,
        .RotateR = Zeta_RelRBTreeNode_RotateR,
    };

    ord_allocator->sn_root =
        Zeta_RBTree_Extract(&rbtn_opr, &l_head->vacant_info.sn);

    if (m_head == r_head) { return l_head; }

    m_head->state = ZETA_OrdAllocator_state_vacant;

    Zeta_RelLinkedListNode_Init(&m_head->hn);
    Zeta_RelLinkedListNode_InsertR(&l_head->hn, &m_head->hn);

    Zeta_RelRBTreeNode_Init(NULL, &m_head->vacant_info.sn);

    size_t m_stride = ZETA_PTR_TO_UINT(r_head) - ZETA_PTR_TO_UINT(m_head);

    void* ins_sn = FindBlock_(ord_allocator->sn_root, m_stride);

    if (ins_sn == NULL) {
        ord_allocator->sn_root =
            Zeta_RBTree_InsertR(&rbtn_opr,
                                Zeta_GetMostLink(NULL, Zeta_RelRBTreeNode_GetR,
                                                 ord_allocator->sn_root),
                                &m_head->vacant_info.sn);
    } else {
        ord_allocator->sn_root =
            Zeta_RBTree_InsertL(&rbtn_opr, ins_sn, &m_head->vacant_info.sn);
    }

    return l_head;
}

static Zeta_OrdAllocator_Head* Deallocate_(Zeta_OrdAllocator* ord_allocator,
                                           Zeta_OrdAllocator_Head* head) {
    Zeta_OrdAllocator_Head* m_head = head;

    Zeta_OrdAllocator_Head* l_head =
        GetHeadFromHN_(Zeta_RelLinkedListNode_GetL(&m_head->hn));

    Zeta_OrdAllocator_Head* r_head =
        GetHeadFromHN_(Zeta_RelLinkedListNode_GetR(&m_head->hn));

    const Zeta_RBTreeNodeOpr rbtn_opr = {
        .context = NULL,

        .GetP = Zeta_RelRBTreeNode_GetP,
        .GetL = Zeta_RelRBTreeNode_GetL,
        .GetR = Zeta_RelRBTreeNode_GetR,

        .GetColor = Zeta_RelRBTreeNode_GetColor,
        .ReverseColor = Zeta_RelRBTreeNode_ReverseColor,

        .AttachL = Zeta_RelRBTreeNode_AttachL,
        .AttachR = Zeta_RelRBTreeNode_AttachR,

        .Detach = Zeta_RelRBTreeNode_Detach,

        .Swap = Zeta_RelRBTreeNode_Swap,

        .RotateL = Zeta_RelRBTreeNode_RotateL,
        .RotateR = Zeta_RelRBTreeNode_RotateR,
    };

    bool_t merge_l = l_head->state == ZETA_OrdAllocator_state_vacant;
    bool_t merge_r = r_head->state == ZETA_OrdAllocator_state_vacant;

    if (merge_l) {
        Zeta_RelLinkedListNode_Extract(&m_head->hn);

        ord_allocator->sn_root =
            Zeta_RBTree_Extract(&rbtn_opr, &l_head->vacant_info.sn);
    } else {
        m_head->state = ZETA_OrdAllocator_state_vacant;

        Zeta_RelRBTreeNode_Init(NULL, &m_head->vacant_info.sn);

        l_head = m_head;
    }

    if (merge_r) {
        Zeta_OrdAllocator_Head* rr_head =
            GetHeadFromHN_(Zeta_RelLinkedListNode_GetR(&r_head->hn));

        Zeta_RelLinkedListNode_Extract(&r_head->hn);

        ord_allocator->sn_root =
            Zeta_RBTree_Extract(&rbtn_opr, &r_head->vacant_info.sn);

        r_head = rr_head;
    }

    size_t lr_stride = ZETA_PTR_TO_UINT(r_head) - ZETA_PTR_TO_UINT(l_head);

    void* ins_sn = FindBlock_(ord_allocator->sn_root, lr_stride);

    if (ins_sn == NULL) {
        ord_allocator->sn_root =
            Zeta_RBTree_InsertR(&rbtn_opr,
                                Zeta_GetMostLink(NULL, Zeta_RelRBTreeNode_GetR,
                                                 ord_allocator->sn_root),
                                &l_head->vacant_info.sn);
    } else {
        ord_allocator->sn_root =
            Zeta_RBTree_InsertL(&rbtn_opr, ins_sn, &l_head->vacant_info.sn);
    }

    return l_head;
}

static int FindSlabLevel_(Zeta_OrdAllocator* ord_allocator, size_t size) {
    int lb = 0;
    int rb = ord_allocator->num_of_slab_level;

    while (lb < rb) {
        int mb = (lb + rb) / 2;

        if (size <= (size_t)ord_allocator->slab_widths[mb]) {
            rb = mb;
        } else {
            lb = mb + 1;
        }
    }

    return lb;
}

static void SetupSlab_(Zeta_OrdAllocator* ord_allocator,
                       Zeta_OrdAllocator_Head* head, int slab_level) {
    size_t slab_width = ord_allocator->slab_widths[slab_level];
    size_t slab_stride = sizeof(void*) + slab_width;
    size_t slab_size = ord_allocator->slab_sizes[slab_level];

    head->state = slab_level;

    Zeta_RelLinkedListNode_Init(&head->slab_info.slab_n);

    Zeta_RelLinkedListNode_InsertR(&ord_allocator->slab_list_heads[slab_level],
                                   &head->slab_info.slab_n);

    uintptr_t* ptr_i = &head->slab_info.ptr;

    SetPtr_(&head->slab_info.unit_ptr, ptr_i);

    for (size_t i = 0; i < slab_size; ++i) {
        uintptr_t* nxt_ptr_i =
            ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(ptr_i) + slab_stride);
        SetPtr_(ptr_i, nxt_ptr_i);
        ptr_i = nxt_ptr_i;
    }
}

size_t Zeta_OrdAllocator_Query(void* ord_allocator_, size_t size) {
    Zeta_OrdAllocator* ord_allocator = ord_allocator_;
    ZETA_DebugAssert(ord_allocator != NULL);

    if (size == 0) { return 0; }

    size_t align = ord_allocator->align;

    size_t head_size = sizeof(Zeta_OrdAllocator_Head);

    int num_of_slab_level = ord_allocator->num_of_slab_level;

    int best_slab_level = FindSlabLevel_(ord_allocator, size);

    if (best_slab_level < num_of_slab_level) {
        return ord_allocator->slab_widths[best_slab_level];
    }

    return Zeta_FindNextConMod(size + head_size, 0, align) - head_size;
}

void* Zeta_OrdAllocator_Allocate(void* ord_allocator_, size_t size) {
    Zeta_OrdAllocator* ord_allocator = ord_allocator_;
    ZETA_DebugAssert(ord_allocator != NULL);

    if (size == 0) { return NULL; }

    size_t head_size = sizeof(Zeta_OrdAllocator_Head);

    int num_of_slab_level = ord_allocator->num_of_slab_level;

    int best_slab_level = FindSlabLevel_(ord_allocator, size);

    for (int slab_level_i = best_slab_level; slab_level_i < num_of_slab_level;
         ++slab_level_i) {
        Zeta_RelLinkedListNode* slab_list_head =
            &ord_allocator->slab_list_heads[slab_level_i];

        Zeta_RelLinkedListNode* slab_n =
            Zeta_RelLinkedListNode_GetR(slab_list_head);

        size_t slab_width = ord_allocator->slab_widths[slab_level_i];
        size_t slab_stride = sizeof(void*) + slab_width;
        size_t slab_size = ord_allocator->slab_sizes[slab_level_i];

        Zeta_OrdAllocator_Head* head = GetHeadFromSlabN_(slab_n);

        uintptr_t* unit_ptr;
        uintptr_t* ptr;

        if (slab_n == slab_list_head) { goto ALLOCATE_SLAB; }

        unit_ptr = &head->slab_info.unit_ptr;
        ptr = GetPtr_(unit_ptr);

        if (ptr == ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(head->slab_info.ptr) +
                                    slab_stride * slab_size)) {
            goto ALLOCATE_SLAB;
        } else {
            goto ALLOCATE_SLAB_UNIT;
        }

    ALLOCATE_SLAB:;

        {
            if (slab_level_i != best_slab_level) { continue; }

            size_t slab_needed_size =
                (ord_allocator->slab_widths[slab_level_i] + sizeof(void*)) *
                ord_allocator->slab_sizes[slab_level_i];

            head = Allocate_(ord_allocator, slab_needed_size);
            if (head == NULL) { continue; }

            SetupSlab_(ord_allocator, head, slab_level_i);

            unit_ptr = &head->slab_info.unit_ptr;
            ptr = GetPtr_(unit_ptr);
        }

    ALLOCATE_SLAB_UNIT:;

        uintptr_t* nxt_ptr = GetPtr_(ptr);
        SetPtr_(unit_ptr, nxt_ptr);
        SetPtr_(ptr, head);

        if (nxt_ptr == NULL) {
            Zeta_RelLinkedListNode_Extract(&head->slab_info.slab_n);

            Zeta_RelLinkedListNode_InsertL(
                &ord_allocator->slab_list_heads[slab_level_i],
                &head->slab_info.slab_n);
        }

        return ptr + 1;
    }

    Zeta_OrdAllocator_Head* head = Allocate_(ord_allocator, size);
    if (head == NULL) { return NULL; }

    head->state = ZETA_OrdAllocator_state_occupied;

    void* data = ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(head) + head_size);

    SetPtr_((uintptr_t*)data - 1, head);

    return data;
}

void Zeta_OrdAllocator_Deallocate(void* ord_allocator_, void* ptr_) {
    Zeta_OrdAllocator* ord_allocator = ord_allocator_;
    ZETA_DebugAssert(ord_allocator != NULL);

    void* data = ptr_;
    uintptr_t* ptr = (uintptr_t*)data - 1;

    Zeta_OrdAllocator_Head* head = GetPtr_(ptr);

    if (head->state == ZETA_OrdAllocator_state_occupied) {
        Deallocate_(ord_allocator, head);
        return;
    }

    uintptr_t* unit_ptr = &head->slab_info.unit_ptr;
    uintptr_t* nxt_ptr = GetPtr_(unit_ptr);
    SetPtr_(unit_ptr, ptr);
    SetPtr_(ptr, nxt_ptr);

    int slab_level = head->state;

    size_t slab_width = ord_allocator->slab_widths[slab_level];
    size_t slab_stride = sizeof(void*) + slab_width;
    size_t slab_size = ord_allocator->slab_sizes[slab_level];

    uintptr_t* ptr_i = &head->slab_info.ptr;

    for (size_t i = 0; i < slab_size; ++i) {
        if (GetPtr_(ptr_i) == head) { return; }
        ptr_i = ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(ptr_i) + slab_stride);
    }

    Zeta_RelLinkedListNode_Extract(&head->slab_info.slab_n);
    Deallocate_(ord_allocator, head);
}

void GetVacantHead_(void* dst_head_ht, Zeta_OrdAllocator_Head* head, size_t lb,
                    size_t ub) {
    ZETA_DebugAssert(head->state == ZETA_OrdAllocator_state_vacant);

    size_t stride = GetStride_(head);
    ZETA_DebugAssert(lb <= stride && stride <= ub);

    Zeta_RelRBTreeNode* sn = &head->vacant_info.sn;

    Zeta_RelRBTreeNode* sn_l = Zeta_RelRBTreeNode_GetL(NULL, sn);
    Zeta_RelRBTreeNode* sn_r = Zeta_RelRBTreeNode_GetR(NULL, sn);

    Zeta_DebugTreeMap_Insert(dst_head_ht, ZETA_PTR_TO_UINT(head));

    if (sn_l != NULL) {
        GetVacantHead_(dst_head_ht, GetHeadFromSN_(sn_l), lb, stride);
    }

    if (sn_r != NULL) {
        GetVacantHead_(dst_head_ht, GetHeadFromSN_(sn_r), stride, ub);
    }
}

void CheckSlab_(Zeta_OrdAllocator* ord_allocator, bool_t print_state,
                Zeta_OrdAllocator_Head* head, void* dst_ptr_size_tm) {
    ZETA_Unused(dst_ptr_size_tm);

    void* vacant_unit_ht = Zeta_DebugTreeMap_Create();

    size_t align = ord_allocator->align;

    int num_of_slab_level = ord_allocator->num_of_slab_level;

    int slab_level = head->state;
    ZETA_DebugAssert(0 <= slab_level && slab_level < num_of_slab_level);

    size_t slab_width = ord_allocator->slab_widths[slab_level];
    size_t slab_stride = slab_width + sizeof(void*);
    size_t slab_size = ord_allocator->slab_sizes[slab_level];

    if (print_state) {
        printf("slab %p\tlevel: %d\twidth: %zu\tsize: %zu\n", head, slab_level,
               slab_width, slab_size);
    }

    if (print_state) {
        printf("\toccpied units:\n");

        printf("\t%8s\t%16s\t%16s\n", "idx", "ptr", "data");
    }

    size_t occupied_unit_cnt = 0;

    for (size_t unit_i = 0; unit_i < slab_size; ++unit_i) {
        uintptr_t* ptr = ZETA_UINT_TO_PTR(
            ZETA_PTR_TO_UINT(&head->slab_info.ptr) + slab_stride * unit_i);
        void* data = ptr + 1;

        ZETA_DebugAssert(ZETA_PTR_TO_UINT(data) % align == 0);

        if (GetPtr_(ptr) == head) {
            ++occupied_unit_cnt;

            if (print_state) { printf("\t%8zu\t%p\t%p\n", unit_i, ptr, data); }

            Zeta_DebugTreeMap_KeyValPair kvp = Zeta_DebugTreeMap_Insert(
                dst_ptr_size_tm, ZETA_PTR_TO_UINT(data));

            ZETA_DebugAssert(kvp.b);

            *kvp.val = slab_width;
        } else {
            Zeta_DebugTreeMap_Insert(vacant_unit_ht, ZETA_PTR_TO_UINT(data));
        }
    }

    if (occupied_unit_cnt == 0) {
        if (print_state) { printf("\tnot occupied unit\n"); }
    }

    uintptr_t* ptr = GetPtr_(&head->slab_info.unit_ptr);
    uintptr_t* ptr_end = ZETA_UINT_TO_PTR(
        ZETA_PTR_TO_UINT(&head->slab_info.ptr) + slab_stride * slab_size);

    while (ptr != ptr_end) {
        ZETA_DebugAssert((void*)&head->slab_info.ptr <= (void*)ptr);

        uintptr_t offset =
            ZETA_PTR_TO_UINT(ptr) - ZETA_PTR_TO_UINT(&head->slab_info.ptr);
        uintptr_t unit_i = offset / slab_stride;

        ZETA_DebugAssert(offset % slab_stride == 0);
        ZETA_DebugAssert(unit_i < slab_size);

        void* data = ptr + 1;

        ZETA_DebugAssert(
            Zeta_DebugTreeMap_Erase(vacant_unit_ht, ZETA_PTR_TO_UINT(data)));

        uintptr_t* nxt_ptr = GetPtr_(ptr);
        ptr = nxt_ptr;
    }

    Zeta_DebugTreeMap_Destroy(vacant_unit_ht);
}

/*

range config check:
    beg is head_align aligned
    beg + head_size is align aligned
    end - head_size is head_align aligned
    end is align aligned

slab config check:
    num_of_slab_level is [0, max_num_of_slab_level)

    slab_width larger than 0
    slab_size larger than 0

    slab_width + sizeof(void*) is align aligned

sn tree check:
    all sn in sn tree, its block state should be vacant.
    stride is ordered

slab check:
    all ptr is void_p aligned
    all unit is align aligned

    check the size of this block has sufficient space to contain units

    check vacant units are in a list
    check occupied units point to head

block check:
    state of block is only vacant, occupied, slab ith
    head is head_align aligned
    begin of data is align aligned
    nxt head is after cur head
    stride from head to nxt head is sufficient for Head.

vacant block check:
    check its sn is in sn tree
    check sn_tree only contains vacant blocks
*/

void Zeta_OrdAllocator_Check(void* ord_allocator_, bool_t print_state,
                             void* dst_ptr_size_tm) {
    ZETA_DebugAssert(dst_ptr_size_tm != NULL);

    Zeta_DebugTreeMap_EraseAll(dst_ptr_size_tm);

    if (print_state) { printf("\n\n--- ord allocator check begin ---\n\n"); }

    typedef unsigned long long ull;

    Zeta_OrdAllocator* ord_allocator = ord_allocator_;
    ZETA_DebugAssert(ord_allocator != NULL);

    size_t beg = ZETA_PTR_TO_UINT(ord_allocator->ptr);
    size_t end = beg + ord_allocator->size;

    size_t head_align = alignof(Zeta_OrdAllocator_Head);
    size_t head_size = sizeof(Zeta_OrdAllocator_Head);

    if (print_state) {
        printf("basic config:\n");

        printf("head_align: %zu\n", head_align);
        printf("head_size: %zu\n", head_size);
        printf("\n");
    }

    size_t align = ord_allocator->align;

    if (print_state) {
        printf("align: %zu\n", align);
        printf("\n");
    }

    ZETA_DebugAssert(beg % head_align == 0);
    ZETA_DebugAssert((beg + head_size) % align == 0);
    ZETA_DebugAssert((end - head_size) % head_align == 0);
    ZETA_DebugAssert(end % align == 0);

    int num_of_slab_level = ord_allocator->num_of_slab_level;
    ZETA_DebugAssert(0 <= num_of_slab_level &&
                     num_of_slab_level <
                         ZETA_OrdAllocator_max_num_of_slab_level);

    Zeta_OrdAllocator_Head* beg_head = ZETA_UINT_TO_PTR(beg);

    Zeta_OrdAllocator_Head* end_head =
        ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(end) - head_size);

    // slab config

    if (print_state) {
        printf("slab config:\n");

        printf("%8s\t%8s\t%8s\n", "level", "width", "size");
    }

    if (num_of_slab_level == 0) {
        if (print_state) { printf("not used\n"); }
    }

    for (int level_i = 0; level_i < num_of_slab_level; ++level_i) {
        size_t slab_width = ord_allocator->slab_widths[level_i];
        size_t slab_size = ord_allocator->slab_sizes[level_i];

        ZETA_DebugAssert(0 < slab_width);
        ZETA_DebugAssert(0 < slab_size);

        ZETA_DebugAssert((slab_width + sizeof(void*)) % align == 0);

        if (print_state) {
            printf("%8d\t%8zu\t%8zu\n", level_i, slab_width, slab_size);
        }
    }

    if (print_state) {
        printf("\n");

        printf("blocks:\n\n");

        printf("\tbeg_head: %p\n", beg_head);
        printf("\tend_head: %p\n", end_head);

        printf("\n");

        printf("\t%16s\t%16s\t%16s\t%16s\t%16s\t%16s\n", "head", "data begin",
               "data end", "stride", "size", "state");
    }

    void* slab_hts[ZETA_OrdAllocator_max_num_of_slab_level];

    for (int i = 0; i < ZETA_OrdAllocator_max_num_of_slab_level; ++i) {
        slab_hts[i] = Zeta_DebugTreeMap_Create();
    }

    ZETA_DebugAssert(Zeta_RelRBTreeNode_GetP(NULL, ord_allocator->sn_root) ==
                     NULL);

    void* vacant_head_ht = Zeta_DebugTreeMap_Create();

    GetVacantHead_(vacant_head_ht, GetHeadFromSN_(ord_allocator->sn_root), 0,
                   ZETA_maxof(size_t));

    for (Zeta_RelLinkedListNode* hn_i = &beg_head->hn; hn_i != &end_head->hn;) {
        Zeta_OrdAllocator_Head* head_i = GetHeadFromHN_(hn_i);
        ZETA_DebugAssert(ZETA_PTR_TO_UINT(head_i) % head_align == 0);

        void* data_i = ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(head_i) + head_size);

        ZETA_DebugAssert(ZETA_PTR_TO_UINT(data_i) % align == 0);

        Zeta_RelLinkedListNode* nxt_hn_i = Zeta_RelLinkedListNode_GetR(hn_i);

        ZETA_DebugAssert(hn_i < nxt_hn_i);
        // ZETA_DebugAssert(hn_i < nxt_hn_i);

        Zeta_OrdAllocator_Head* nxt_head_i = GetHeadFromHN_(nxt_hn_i);

        int state = head_i->state;

        size_t stride = (ull)nxt_head_i - (ull)head_i;
        size_t size = (ull)nxt_head_i - (ull)data_i;

        ZETA_DebugAssert(head_size <= stride);
        ZETA_DebugAssert(size + head_size == stride);

        if (print_state) {
            printf("\t%16p\t%16p\t%16p\t", head_i, data_i, nxt_head_i);

            printf("%16zu\t", stride);

            printf("%16zu\t", size);

            printf("  %+03d", state);
        }

        switch (state) {
            case ZETA_OrdAllocator_state_vacant: {
                if (print_state) { printf(" (  vacant)    "); }

                bool_t b = Zeta_DebugTreeMap_Erase(vacant_head_ht,
                                                   ZETA_PTR_TO_UINT(head_i));

                ZETA_DebugAssert(b);

                break;
            }
            case ZETA_OrdAllocator_state_occupied: {
                if (print_state) { printf(" (occupied)    "); }
                break;
            }
            default: {
                ZETA_DebugAssert(0 <= state && state < num_of_slab_level);
                if (print_state) { printf(" (slab %3d)\t", state); }

                bool_t b = Zeta_DebugTreeMap_Insert(slab_hts[state],
                                                    ZETA_PTR_TO_UINT(head_i))
                               .b;

                ZETA_DebugAssert(b);
            }
        }

        if (state == ZETA_OrdAllocator_state_occupied) {
            Zeta_DebugTreeMap_KeyValPair kvp = Zeta_DebugTreeMap_Insert(
                dst_ptr_size_tm, ZETA_PTR_TO_UINT(data_i));

            ZETA_DebugAssert(kvp.b);

            *kvp.val = (ull)nxt_head_i - (ull)data_i;
        }

        if (print_state) { printf("\n"); }

        hn_i = nxt_hn_i;
    }

    if (print_state) { printf("\n\n\n"); }

    ZETA_DebugAssert(Zeta_DebugTreeMap_GetSize(vacant_head_ht) == 0);
    Zeta_DebugTreeMap_Destroy(vacant_head_ht);

    for (Zeta_RelLinkedListNode* hn_i = &beg_head->hn; hn_i != &end_head->hn;) {
        Zeta_OrdAllocator_Head* head_i = GetHeadFromHN_(hn_i);

        Zeta_RelLinkedListNode* nxt_hn_i = Zeta_RelLinkedListNode_GetR(hn_i);

        int state = head_i->state;

        if (state != ZETA_OrdAllocator_state_vacant &&
            state != ZETA_OrdAllocator_state_occupied) {
            CheckSlab_(ord_allocator, print_state, head_i, dst_ptr_size_tm);
            if (print_state) { printf("\n"); }
        }

        hn_i = nxt_hn_i;
    }

    for (int level_i = 0; level_i < num_of_slab_level; ++level_i) {
        void* slab_ht = slab_hts[level_i];

        Zeta_RelLinkedListNode* slab_list_head =
            &ord_allocator->slab_list_heads[level_i];

        Zeta_RelLinkedListNode* slab_n = slab_list_head;

        for (;;) {
            slab_n = Zeta_RelLinkedListNode_GetR(slab_n);
            if (slab_n == slab_list_head) { break; }

            Zeta_OrdAllocator_Head* head_i = GetHeadFromSlabN_(slab_n);

            bool_t b =
                Zeta_DebugTreeMap_Erase(slab_ht, ZETA_PTR_TO_UINT(head_i));

            ZETA_DebugAssert(b);
        }

        ZETA_DebugAssert(Zeta_DebugTreeMap_GetSize(slab_ht) == 0);
    }

    for (int i = 0; i < ZETA_OrdAllocator_max_num_of_slab_level; ++i) {
        Zeta_DebugTreeMap_Destroy(slab_hts[i]);
    }

    if (print_state) { printf("\n--- ord allocator check end ---\n\n"); }
}

void Zeta_OrdAllocator_ToAllocator(void* ord_allocator_, Zeta_Allocator* dst) {
    Zeta_OrdAllocator* ord_allocator = ord_allocator_;
    ZETA_DebugAssert(ord_allocator != NULL);

    ZETA_DebugAssert(dst != NULL);

    dst->context = ord_allocator;
    dst->Allocate = Zeta_OrdAllocator_Allocate;
    dst->Deallocate = Zeta_OrdAllocator_Deallocate;
}
