#include "OrdAllocator.h"
#include "Algorithm.h"
#include "DebugTreeMap.h"
#include "RBTree.h"
#include "RawVector.h"
#include "utils.h"

typedef struct Zeta_OrdAllocator_Head Zeta_OrdAllocator_Head;

struct Zeta_OrdAllocator_Head {
    int state;
    Zeta_OrdDoublyLinkedNode hn;  // link to other heads

    union {
        struct {
            Zeta_OrdRBTreeNode sn;  // link to other vacuum
        } vacuum_info;

        struct {
            void* ptr;  // points to head
        } occupied_info;

        struct {
            Zeta_OrdDoublyLinkedNode slab_n;  // link to slab list
            void* unit_ptr;                   // points to slab unit

            void* ptr;  // points to nxt unit or head
        } slab_info;
    };
};

#define Zeta_OrdAllocator_state_vacuum -1
#define Zeta_OrdAllocator_state_occupied -2

/*

head {
    state 4
    pad 4

    hn {
        l 8
        r 8
    } 16

    union {
        vacuum info {
            vacuum_rbn {
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

typedef struct WidthSizePair WidthSizePair;

struct WidthSizePair {
    size_t width;
    size_t size;
};

static int CompareWidthSizePair(void* context, const void* x_, const void* y_) {
    ZETA_UNUSED(context);

    const WidthSizePair* x = x_;
    const WidthSizePair* y = y_;

    if (x->width < y->width) { return -1; }
    if (y->width < x->width) { return 1; }
    return 0;
}

void Zeta_OrdAllocator_Init(void* ord_allocator_) {
    Zeta_OrdAllocator* ord_allocator = ord_allocator_;
    ZETA_DEBUG_ASSERT(ord_allocator != NULL);

    size_t head_align = alignof(Zeta_OrdAllocator_Head);
    size_t head_size = sizeof(Zeta_OrdAllocator_Head);

    size_t align = ord_allocator->align;
    ZETA_DEBUG_ASSERT(0 < align);

    align = GetLCM(head_align, align);

    ord_allocator->align = align;

    ZETA_DEBUG_ASSERT(ord_allocator->ptr != 0);
    ZETA_DEBUG_ASSERT(0 < ord_allocator->size);

    size_t beg = (uintptr_t)ord_allocator->ptr;
    size_t end = beg + ord_allocator->size;

    beg = FindNextConMod(beg, align - head_size % align, align);
    end = end / align * align;

    ZETA_DEBUG_ASSERT(beg < end);

    ord_allocator->ptr = (void*)(uintptr_t)beg;
    ord_allocator->size = end - beg;

    int num_of_slab_level = ord_allocator->num_of_slab_level;

    if (num_of_slab_level < 0) {
        if (head_size * 8 <= align) {
            num_of_slab_level = 0;
        } else {
            size_t slab_width_unit =
                head_size < sizeof(void*) * 4 ? sizeof(void*) * 4 : head_size;

            slab_width_unit =
                FindNextConMod(slab_width_unit + sizeof(void*), 0, align);

            size_t rates[] = {1, 2, 3, 4, 5, 6, 7};
            size_t sizes[] = {12, 12, 12, 8, 6, 4, 4};

            num_of_slab_level = sizeof(rates) / sizeof(rates[0]);
            ord_allocator->num_of_slab_level = num_of_slab_level;

            for (int level_i = 0; level_i < num_of_slab_level; ++level_i) {
                ord_allocator->slab_widths[level_i] =
                    slab_width_unit * rates[level_i] - sizeof(void*);

                ord_allocator->slab_sizes[level_i] = sizes[level_i];
            }
        }
    } else if (0 < num_of_slab_level) {
        ZETA_DEBUG_ASSERT(num_of_slab_level <=
                          Zeta_OrdAllocator_max_num_of_slab_level);

        WidthSizePair width_size_pairs[Zeta_OrdAllocator_max_num_of_slab_level];

        for (int level_i = 0; level_i < num_of_slab_level; ++level_i) {
            size_t cur_width = ord_allocator->slab_widths[level_i];
            size_t cur_size = ord_allocator->slab_sizes[level_i];

            ZETA_DEBUG_ASSERT(0 < cur_width);
            ZETA_DEBUG_ASSERT(0 < cur_size);

            width_size_pairs[level_i].width =
                FindNextConMod(cur_width, align - sizeof(void*) % align, align);
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

    for (int level_i = 0; level_i < Zeta_OrdAllocator_max_num_of_slab_level;
         ++level_i) {
        Zeta_OrdDoublyLinkedNode_Init(&ord_allocator->slab_list_heads[level_i]);
    }

    Zeta_OrdAllocator_Head* beg_head = (void*)(uintptr_t)beg;

    Zeta_OrdAllocator_Head* end_head =
        (void*)(uintptr_t)((uintptr_t)end - head_size);

    beg_head->state = Zeta_OrdAllocator_state_vacuum;
    Zeta_OrdDoublyLinkedNode_Init(&beg_head->hn);
    Zeta_OrdRBTreeNode_Init(NULL, &beg_head->vacuum_info.sn);

    end_head->state = Zeta_OrdAllocator_state_occupied;
    Zeta_OrdDoublyLinkedNode_Init(&end_head->hn);
    end_head->occupied_info.ptr = end_head;

    Zeta_OrdDoublyLinkedNode_InsertR(&beg_head->hn, &end_head->hn);

    ord_allocator->sn_root = &beg_head->vacuum_info.sn;
}

static Zeta_OrdAllocator_Head* GetHeadFromHN_(void* hn) {
    return ZETA_FROM_MEM(Zeta_OrdAllocator_Head, hn, hn);
}

static Zeta_OrdAllocator_Head* GetHeadFromSN_(void* sn) {
    return ZETA_FROM_MEM(Zeta_OrdAllocator_Head, vacuum_info.sn, sn);
}

static Zeta_OrdAllocator_Head* GetHeadFromSlabN_(void* slab_n) {
    return ZETA_FROM_MEM(Zeta_OrdAllocator_Head, slab_info.slab_n, slab_n);
}

static size_t GetStride_(Zeta_OrdAllocator_Head* head) {
    Zeta_OrdAllocator_Head* nxt_head =
        GetHeadFromHN_(Zeta_OrdDoublyLinkedNode_GetR(&head->hn));

    return (uintptr_t)(void*)nxt_head - (uintptr_t)(void*)head;
}

static void* FindBlock_(void* sn_root, size_t stride) {
    void* sn = sn_root;
    void* ret = NULL;

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

static Zeta_OrdAllocator_Head* ChooseNiceBlock_(
    Zeta_OrdAllocator* ord_allocator, size_t size) {
    size_t align = ord_allocator->align;

    size_t head_size = sizeof(Zeta_OrdAllocator_Head);

    void* best_fit_sn = FindBlock_(ord_allocator->sn_root, head_size + size);

    if (best_fit_sn == NULL) { return NULL; }

    Zeta_OrdAllocator_Head* best_fit_head = GetHeadFromSN_(best_fit_sn);

    if (GetStride_(best_fit_head) == head_size + size) { return best_fit_head; }

    void* twice_fit_sn = FindBlock_(
        ord_allocator->sn_root, FindNextConMod(head_size + size, 0, align) * 2);

    if (twice_fit_sn != NULL) { return GetHeadFromSN_(twice_fit_sn); }

    return GetHeadFromSN_(Zeta_GetMostLink(NULL, Zeta_OrdRBTreeNode_GetR,
                                           ord_allocator->sn_root));
}

static Zeta_OrdAllocator_Head* Allocate_(Zeta_OrdAllocator* ord_allocator,
                                         size_t size) {
    Zeta_OrdAllocator_Head* l_head = ChooseNiceBlock_(ord_allocator, size);
    if (l_head == NULL) { return NULL; }

    size_t align = ord_allocator->align;

    size_t head_size = sizeof(Zeta_OrdAllocator_Head);

    Zeta_OrdAllocator_Head* m_head =
        (void*)(uintptr_t)(FindNextConMod(
                               (uintptr_t)l_head + head_size + size + head_size,
                               0, align) -
                           head_size);

    ZETA_DEBUG_ASSERT(((uintptr_t)(void*)m_head + head_size) % align == 0);

    Zeta_OrdAllocator_Head* r_head =
        GetHeadFromHN_(Zeta_OrdDoublyLinkedNode_GetR(&l_head->hn));

    const Zeta_RBTreeNodeOpr rbtn_opr = {
        .context = NULL,

        .GetP = Zeta_OrdRBTreeNode_GetP,
        .GetL = Zeta_OrdRBTreeNode_GetL,
        .GetR = Zeta_OrdRBTreeNode_GetR,

        .GetColor = Zeta_OrdRBTreeNode_GetColor,
        .ReverseColor = Zeta_OrdRBTreeNode_ReverseColor,

        .AttachL = Zeta_OrdRBTreeNode_AttachL,
        .AttachR = Zeta_OrdRBTreeNode_AttachR,

        .Detach = Zeta_OrdRBTreeNode_Detach,

        .Swap = Zeta_OrdRBTreeNode_Swap,

        .RotateL = Zeta_OrdRBTreeNode_RotateL,
        .RotateR = Zeta_OrdRBTreeNode_RotateR,
    };

    ord_allocator->sn_root =
        Zeta_RBTree_Extract(&rbtn_opr, &l_head->vacuum_info.sn);

    if (m_head == r_head) { return l_head; }

    m_head->state = Zeta_OrdAllocator_state_vacuum;

    Zeta_OrdDoublyLinkedNode_Init(&m_head->hn);

    Zeta_OrdDoublyLinkedNode_InsertR(&l_head->hn, &m_head->hn);

    size_t m_stride = (uintptr_t)r_head - (uintptr_t)m_head;

    Zeta_OrdAllocator_Head* ins_head =
        FindBlock_(ord_allocator->sn_root, m_stride);

    if (ins_head == NULL) {
        ord_allocator->sn_root =
            Zeta_RBTree_InsertR(&rbtn_opr,
                                Zeta_GetMostLink(NULL, Zeta_OrdRBTreeNode_GetR,
                                                 ord_allocator->sn_root),
                                &m_head->vacuum_info.sn);
    } else {
        ord_allocator->sn_root = Zeta_RBTree_InsertL(
            &rbtn_opr, &ins_head->vacuum_info.sn, &m_head->vacuum_info.sn);
    }

    return l_head;
}

static Zeta_OrdAllocator_Head* Deallocate_(Zeta_OrdAllocator* ord_allocator,
                                           Zeta_OrdAllocator_Head* head) {
    Zeta_OrdAllocator_Head* m_head = head;

    Zeta_OrdAllocator_Head* l_head =
        GetHeadFromHN_(Zeta_OrdDoublyLinkedNode_GetL(&m_head->hn));

    Zeta_OrdAllocator_Head* r_head =
        GetHeadFromHN_(Zeta_OrdDoublyLinkedNode_GetR(&m_head->hn));

    const Zeta_RBTreeNodeOpr rbtn_opr = {
        .context = NULL,

        .GetP = Zeta_OrdRBTreeNode_GetP,
        .GetL = Zeta_OrdRBTreeNode_GetL,
        .GetR = Zeta_OrdRBTreeNode_GetR,

        .GetColor = Zeta_OrdRBTreeNode_GetColor,
        .ReverseColor = Zeta_OrdRBTreeNode_ReverseColor,

        .AttachL = Zeta_OrdRBTreeNode_AttachL,
        .AttachR = Zeta_OrdRBTreeNode_AttachR,

        .Detach = Zeta_OrdRBTreeNode_Detach,

        .Swap = Zeta_OrdRBTreeNode_Swap,

        .RotateL = Zeta_OrdRBTreeNode_RotateL,
        .RotateR = Zeta_OrdRBTreeNode_RotateR,
    };

    bool_t merge_l = l_head->state == Zeta_OrdAllocator_state_vacuum;
    bool_t merge_r = r_head->state == Zeta_OrdAllocator_state_vacuum;

    if (merge_l) {
        ord_allocator->sn_root =
            Zeta_RBTree_Extract(&rbtn_opr, &l_head->vacuum_info.sn);
    } else {
        l_head = m_head;

        m_head->state = Zeta_OrdAllocator_state_vacuum;
        Zeta_OrdRBTreeNode_Init(NULL, &m_head->vacuum_info.sn);
    }

    if (merge_r) {
        ord_allocator->sn_root =
            Zeta_RBTree_Extract(&rbtn_opr, &r_head->vacuum_info.sn);

        Zeta_OrdAllocator_Head* rr_head =
            GetHeadFromHN_(Zeta_OrdDoublyLinkedNode_GetR(&r_head->hn));

        Zeta_OrdDoublyLinkedNode_Extract(&r_head->hn);

        r_head = rr_head;
    }

    size_t lr_stride = (uintptr_t)r_head - (uintptr_t)l_head;

    Zeta_OrdAllocator_Head* ins_head =
        FindBlock_(ord_allocator->sn_root, lr_stride);

    if (ins_head == NULL) {
        ord_allocator->sn_root =
            Zeta_RBTree_InsertR(&rbtn_opr,
                                Zeta_GetMostLink(NULL, Zeta_OrdRBTreeNode_GetR,
                                                 ord_allocator->sn_root),
                                &l_head->vacuum_info.sn);
    } else {
        ord_allocator->sn_root = Zeta_RBTree_InsertL(
            &rbtn_opr, &ins_head->vacuum_info.sn, &l_head->vacuum_info.sn);
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

    Zeta_OrdDoublyLinkedNode_Init(&head->slab_info.slab_n);

    Zeta_OrdDoublyLinkedNode_InsertR(
        &ord_allocator->slab_list_heads[slab_level], &head->slab_info.slab_n);

    void* ptr_i = &head->slab_info.ptr;

    head->slab_info.unit_ptr = ptr_i;

    for (size_t i = 1; i < slab_size; ++i) {
        void* nxt_ptr_i = ZETA_ADDR_OFFSET(ptr_i, slab_stride);
        *(void**)ptr_i = nxt_ptr_i;
        ptr_i = nxt_ptr_i;
    }

    *(void**)ptr_i = NULL;
}

void* Zeta_OrdAllocator_Allocate(void* ord_allocator_, size_t size) {
    Zeta_OrdAllocator* ord_allocator = ord_allocator_;
    ZETA_DEBUG_ASSERT(ord_allocator != NULL);

    ZETA_DEBUG_ASSERT(0 <= size);

    if (size == 0) { return NULL; }

    size_t head_size = sizeof(Zeta_OrdAllocator_Head);

    int num_of_slab_level = ord_allocator->num_of_slab_level;

    int best_slab_level = FindSlabLevel_(ord_allocator, size);

    for (int slab_level_i = best_slab_level; slab_level_i < num_of_slab_level;
         ++slab_level_i) {
        Zeta_OrdDoublyLinkedNode* slab_list_head =
            &ord_allocator->slab_list_heads[slab_level_i];

        Zeta_OrdDoublyLinkedNode* slab_n =
            Zeta_OrdDoublyLinkedNode_GetR(slab_list_head);

        Zeta_OrdAllocator_Head* head = GetHeadFromSlabN_(slab_n);

        if (slab_n == slab_list_head || head->slab_info.unit_ptr == NULL) {
            if (slab_level_i != best_slab_level) { continue; }

            size_t slab_needed_size =
                (ord_allocator->slab_widths[slab_level_i] + sizeof(void*)) *
                ord_allocator->slab_sizes[slab_level_i];

            head = Allocate_(ord_allocator, slab_needed_size);
            if (head == NULL) { continue; }

            SetupSlab_(ord_allocator, head, slab_level_i);
        }

        void* ptr = head->slab_info.unit_ptr;
        void* nxt_ptr = *(void**)ptr;
        head->slab_info.unit_ptr = nxt_ptr;

        if (nxt_ptr == NULL) {
            Zeta_OrdDoublyLinkedNode_Extract(&head->slab_info.slab_n);

            Zeta_OrdDoublyLinkedNode_InsertL(
                &ord_allocator->slab_list_heads[slab_level_i],
                &head->slab_info.slab_n);
        }

        *(void**)ptr = head;

        return ZETA_ADDR_OFFSET(ptr, sizeof(void*));
    }

    Zeta_OrdAllocator_Head* head = Allocate_(ord_allocator, size);
    if (head == NULL) { return NULL; }

    head->state = Zeta_OrdAllocator_state_occupied;

    void* data = ZETA_ADDR_OFFSET(head, head_size);

    *(void**)ZETA_ADDR_OFFSET(data, -(intptr_t)sizeof(void*)) = head;

    return data;
}

void Zeta_OrdAllocator_Deallocate(void* ord_allocator_, void* ptr_) {
    Zeta_OrdAllocator* ord_allocator = ord_allocator_;
    ZETA_DEBUG_ASSERT(ord_allocator != NULL);

    void* data = ptr_;
    void* ptr = ZETA_ADDR_OFFSET(data, -(intptr_t)sizeof(void*));

    Zeta_OrdAllocator_Head* head = ptr;

    if (head->state == Zeta_OrdAllocator_state_occupied) {
        Deallocate_(ord_allocator, head);
    }

    void* nxt_ptr = head->slab_info.unit_ptr;
    *(void**)ptr = nxt_ptr;
    head->slab_info.unit_ptr = ptr;

    int slab_level = head->state;

    size_t stride = ord_allocator->slab_widths[slab_level] + sizeof(void*);
    size_t size = ord_allocator->slab_sizes[slab_level];

    bool_t any_unit_is_occupied = 0;

    void* ptr_i = &head->slab_info.ptr;

    for (size_t i = 0; i < size; ++i) {
        if (*(void**)ptr_i != head) {
            any_unit_is_occupied = 1;
            break;
        }

        ptr_i = ZETA_ADDR_OFFSET(ptr_i, stride);
    }

    if (!any_unit_is_occupied) { Deallocate_(ord_allocator, head); }
}

void CheckSlab_(Zeta_OrdAllocator* ord_allocator, bool_t print_state,
                Zeta_OrdAllocator_Head* head, void* dst_ptr_size_tm) {
    ZETA_UNUSED(dst_ptr_size_tm);

    void* vacuum_unit_ht = DebugTreeMap_Create();

    size_t align = ord_allocator->align;

    int num_of_slab_level = ord_allocator->num_of_slab_level;

    int slab_level = head->state;
    ZETA_DEBUG_ASSERT(0 <= slab_level && slab_level < num_of_slab_level);

    size_t slab_width = ord_allocator->slab_widths[slab_level];
    size_t slab_stride = slab_width + sizeof(void*);
    size_t slab_size = ord_allocator->slab_sizes[slab_level];

    if (print_state) {
        printf("slab %p\tlevel: %d\twidth: %d\tsize: %d\n", head, slab_level,
               slab_width, slab_size);
    }

    if (print_state) {
        printf("\toccpied units:\n");

        printf("\t%8s\t%16s\t%16s\n", "idx", "ptr", "data");
    }

    size_t occupied_unit_cnt = 0;

    for (size_t unit_i = 0; unit_i < slab_size; ++unit_i) {
        void* ptr =
            ZETA_ADDR_OFFSET(&head->slab_info.ptr, slab_stride * unit_i);
        void* data = ZETA_ADDR_OFFSET(ptr, sizeof(void*));

        ZETA_DEBUG_ASSERT((uintptr_t)data % align == 0);

        if (*(void**)ptr == head) {
            ++occupied_unit_cnt;

            if (print_state) { printf("\t%8d\t%p\t%p\n", unit_i, ptr, data); }

            KeyValPair kvp =
                DebugTreeMap_Insert(dst_ptr_size_tm, (uintptr_t)data);

            ZETA_DEBUG_ASSERT(kvp.b);

            *kvp.val = slab_width;
        } else {
            DebugTreeMap_Insert(vacuum_unit_ht, (uintptr_t)data);
        }
    }

    if (occupied_unit_cnt == 0) {
        if (print_state) { printf("\tnot occupied unit\n"); }
    }

    void* ptr = head->slab_info.unit_ptr;

    while (ptr != NULL) {
        intptr_t offset = (intptr_t)ptr - (intptr_t)(void*)&head->slab_info.ptr;
        intptr_t unit_i = offset / slab_stride;

        ZETA_DEBUG_ASSERT(offset % slab_stride == 0);
        ZETA_DEBUG_ASSERT(0 <= unit_i && unit_i < (intptr_t)slab_size);

        void* data = ZETA_ADDR_OFFSET(ptr, sizeof(void*));

        ZETA_DEBUG_ASSERT(DebugTreeMap_Erase(vacuum_unit_ht, (uintptr_t)data));

        void* nxt_ptr = *(void**)ptr;
        ptr = nxt_ptr;
    }
}

void Zeta_OrdAllocator_Check(void* ord_allocator_, bool_t print_state,
                             void* dst_ptr_size_tm) {
    ZETA_DEBUG_ASSERT(dst_ptr_size_tm != NULL);

    DebugTreeMap_EraseAll(dst_ptr_size_tm);

    if (print_state) { printf("\n\n--- ord allocator check begin ---\n\n"); }

    typedef unsigned long long ull;

    Zeta_OrdAllocator* ord_allocator = ord_allocator_;
    ZETA_DEBUG_ASSERT(ord_allocator != NULL);

    size_t beg = (uintptr_t)ord_allocator->ptr;
    size_t end = beg + ord_allocator->size;

    size_t head_align = alignof(Zeta_OrdAllocator_Head);
    size_t head_size = sizeof(Zeta_OrdAllocator_Head);

    if (print_state) {
        printf("basic config:\n");

        printf("head_align: %llu\n", head_align);
        printf("head_size: %llu\n", head_size);
        printf("\n");
    }

    size_t align = ord_allocator->align;

    if (print_state) {
        printf("align: %llu\n", align);
        printf("\n");
    }

    ZETA_DEBUG_ASSERT((beg + head_size) % align == 0)
    ZETA_DEBUG_ASSERT(end % align == 0)

    int num_of_slab_level = ord_allocator->num_of_slab_level;
    ZETA_DEBUG_ASSERT(0 <= num_of_slab_level &&
                      num_of_slab_level <
                          Zeta_OrdAllocator_max_num_of_slab_level);

    Zeta_OrdAllocator_Head* beg_head = (void*)(uintptr_t)beg;

    Zeta_OrdAllocator_Head* end_head =
        (void*)(uintptr_t)((uintptr_t)end - head_size);

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

        if (print_state) {
            printf("%8d\t%8llu\t%8llu\n", level_i, slab_width, slab_size);
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

    for (Zeta_OrdDoublyLinkedNode* hn_i = &beg_head->hn;
         hn_i != &end_head->hn;) {
        Zeta_OrdAllocator_Head* head_i = GetHeadFromHN_(hn_i);

        void* data_i = ZETA_ADDR_OFFSET(head_i, head_size);

        Zeta_OrdDoublyLinkedNode* nxt_hn_i =
            Zeta_OrdDoublyLinkedNode_GetR(hn_i);

        Zeta_OrdAllocator_Head* nxt_head_i = GetHeadFromHN_(nxt_hn_i);

        int state = head_i->state;

        if (print_state) {
            printf("\t%#16p\t%#16p\t%#16p\t", head_i, data_i, nxt_head_i);

            printf("%16llu\t", (ull)nxt_head_i - (ull)head_i);

            printf("%16llu\t", (ull)nxt_head_i - (ull)data_i);

            printf("  %+03d", state);
        }

        switch (state) {
            case Zeta_OrdAllocator_state_vacuum:
                if (print_state) { printf(" (  vacuum)    "); }
                break;
            case Zeta_OrdAllocator_state_occupied:
                if (print_state) { printf(" (occupied)    "); }
                break;
            default:
                ZETA_DEBUG_ASSERT(0 <= state && state < num_of_slab_level);
                if (print_state) { printf(" (slab %3d)\t", state); }
        }

        if (state == Zeta_OrdAllocator_state_occupied) {
            KeyValPair kvp =
                DebugTreeMap_Insert(dst_ptr_size_tm, (uintptr_t)data_i);

            ZETA_DEBUG_ASSERT(kvp.b);

            *kvp.val = (ull)nxt_head_i - (ull)data_i;
        }

        if (print_state) { printf("\n"); }

        hn_i = nxt_hn_i;
    }

    for (Zeta_OrdDoublyLinkedNode* hn_i = &beg_head->hn;
         hn_i != &end_head->hn;) {
        Zeta_OrdAllocator_Head* head_i = GetHeadFromHN_(hn_i);

        Zeta_OrdDoublyLinkedNode* nxt_hn_i =
            Zeta_OrdDoublyLinkedNode_GetR(hn_i);

        int state = head_i->state;

        if (state != Zeta_OrdAllocator_state_vacuum &&
            state != Zeta_OrdAllocator_state_occupied) {
            CheckSlab_(ord_allocator, print_state, head_i, dst_ptr_size_tm);
        }

        if (print_state) { printf("\n"); }

        hn_i = nxt_hn_i;
    }

    if (print_state) { printf("\n--- ord allocator check end ---\n\n"); }
}

void Zeta_OrdAllocator_ToAllocator(void* ord_allocator_, Zeta_Allocator* dst) {
    Zeta_OrdAllocator* ord_allocator = ord_allocator_;
    ZETA_DEBUG_ASSERT(ord_allocator != NULL);

    ZETA_DEBUG_ASSERT(dst != NULL);

    dst->context = ord_allocator;
    dst->Allocate = Zeta_OrdAllocator_Allocate;
    dst->Deallocate = Zeta_OrdAllocator_Deallocate;
}
