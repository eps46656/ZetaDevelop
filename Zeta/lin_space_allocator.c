#include "lin_space_allocator.h"

#include "debugger.h"
#include "ptr_utils.h"
#include "rbtree.h"
#include "utils.h"

#if ZETA_EnableDebug

#define Check_(lsa) Zeta_LinSpaceAllocator_Check((lsa))

#else

#define Check_(lsa) ZETA_Unused((lsa))

#endif

#define VacantColor (0)
#define OccupiedColor (1)

static void* NodeGetGP_(void const* context, void* n) {
    ZETA_Unused(context);
    ZETA_DebugAssert(n != NULL);

    void* m = ZETA_RelColorPtr_GetPtr(&((Zeta_LinSpaceAllocator_Node*)n)->gpc,
                                      alignof(Zeta_LinSpaceAllocator_Node), n);

    return m == n ? NULL : m;
}

static void NodeSetGP_(void const* context, void* n, void* m) {
    ZETA_Unused(context);
    ZETA_DebugAssert(n != NULL);

    ZETA_RelColorPtr_SetPtr(&((Zeta_LinSpaceAllocator_Node*)n)->gpc,
                            alignof(Zeta_LinSpaceAllocator_Node), n,
                            m == NULL ? n : m);
}

static int NodeGetGPC_(void const* context, void* n) {
    ZETA_Unused(context);

    return n == NULL ? 0
                     : ZETA_RelColorPtr_GetColor(
                           &((Zeta_LinSpaceAllocator_Node*)n)->gpc,
                           alignof(Zeta_LinSpaceAllocator_Node), n);
}

static void NodeSetGPC_(void const* context, void* n, int color) {
    ZETA_Unused(context);
    ZETA_DebugAssert(n != NULL);

    ZETA_RelColorPtr_SetColor(&((Zeta_LinSpaceAllocator_Node*)n)->gpc,
                              alignof(Zeta_LinSpaceAllocator_Node), n, color);
}

static void* NodeGetGL_(void const* context, void* n) {
    ZETA_Unused(context);
    ZETA_DebugAssert(n != NULL);

    void* m = ZETA_RelColorPtr_GetPtr(&((Zeta_LinSpaceAllocator_Node*)n)->glc,
                                      alignof(Zeta_LinSpaceAllocator_Node), n);

    return m == n ? NULL : m;
}

static void NodeSetGL_(void const* context, void* n, void* m) {
    ZETA_Unused(context);
    ZETA_DebugAssert(n != NULL);

    ZETA_RelColorPtr_SetPtr(&((Zeta_LinSpaceAllocator_Node*)n)->glc,
                            alignof(Zeta_LinSpaceAllocator_Node), n,
                            m == NULL ? n : m);
}

static int NodeGetGLC_(void const* context, void* n) {
    ZETA_Unused(context);

    return n == NULL ? 0
                     : ZETA_RelColorPtr_GetColor(
                           &((Zeta_LinSpaceAllocator_Node*)n)->glc,
                           alignof(Zeta_LinSpaceAllocator_Node), n);
}

static void NodeSetGLC_(void const* context, void* n, int color) {
    ZETA_Unused(context);
    ZETA_DebugAssert(n != NULL);

    ZETA_RelColorPtr_SetColor(&((Zeta_LinSpaceAllocator_Node*)n)->glc,
                              alignof(Zeta_LinSpaceAllocator_Node), n, color);
}

static void* NodeGetGR_(void const* context, void* n) {
    ZETA_Unused(context);
    ZETA_DebugAssert(n != NULL);

    void* m = ZETA_RelPtr_GetPtr(&((Zeta_LinSpaceAllocator_Node*)n)->gr, n);

    return m == n ? NULL : m;
}

static void NodeSetGR_(void const* context, void* n, void* m) {
    ZETA_Unused(context);
    ZETA_DebugAssert(n != NULL);

    ZETA_RelPtr_SetPtr(&((Zeta_LinSpaceAllocator_Node*)n)->gr, n,
                       m == NULL ? n : m);
}

static void* NodeGetSP_(void const* context, void* n) {
    ZETA_Unused(context);
    ZETA_DebugAssert(n != NULL);

    void* m = ZETA_RelColorPtr_GetPtr(&((Zeta_LinSpaceAllocator_Node*)n)->spc,
                                      alignof(Zeta_LinSpaceAllocator_Node), n);

    return m == n ? NULL : m;
}

static void NodeSetSP_(void const* context, void* n, void* m) {
    ZETA_Unused(context);
    ZETA_DebugAssert(n != NULL);

    ZETA_RelColorPtr_SetPtr(&((Zeta_LinSpaceAllocator_Node*)n)->spc,
                            alignof(Zeta_LinSpaceAllocator_Node), n,
                            m == NULL ? n : m);
}

static int NodeGetSPC_(void const* context, void* n) {
    ZETA_Unused(context);

    return n == NULL ? 0
                     : ZETA_RelColorPtr_GetColor(
                           &((Zeta_LinSpaceAllocator_Node*)n)->spc,
                           alignof(Zeta_LinSpaceAllocator_Node), n);
}

static void NodeSetSPC_(void const* context, void* n, int color) {
    ZETA_Unused(context);
    ZETA_DebugAssert(n != NULL);

    ZETA_RelColorPtr_SetColor(&((Zeta_LinSpaceAllocator_Node*)n)->spc,
                              alignof(Zeta_LinSpaceAllocator_Node), n, color);
}

static void* NodeGetSL_(void const* context, void* n) {
    ZETA_Unused(context);
    ZETA_DebugAssert(n != NULL);

    void* m = ZETA_RelPtr_GetPtr(&((Zeta_LinSpaceAllocator_Node*)n)->sl, n);

    return m == n ? NULL : m;
}

static void NodeSetSL_(void const* context, void* n, void* m) {
    ZETA_Unused(context);
    ZETA_DebugAssert(n != NULL);

    ZETA_RelPtr_SetPtr(&((Zeta_LinSpaceAllocator_Node*)n)->sl, n,
                       m == NULL ? n : m);
}

static void* NodeGetSR_(void const* context, void* n) {
    ZETA_Unused(context);
    ZETA_DebugAssert(n != NULL);

    void* m = ZETA_RelPtr_GetPtr(&((Zeta_LinSpaceAllocator_Node*)n)->sr, n);

    return m == n ? NULL : m;
}

static void NodeSetSR_(void const* context, void* n, void* m) {
    ZETA_Unused(context);
    ZETA_DebugAssert(n != NULL);

    ZETA_RelPtr_SetPtr(&((Zeta_LinSpaceAllocator_Node*)n)->sr, n,
                       m == NULL ? n : m);
}

static size_t NodeGetAccSize_(void const* context, void* n) {
    ZETA_Unused(context);

    return n == NULL ? 0 : ((Zeta_LinSpaceAllocator_Node*)n)->acc_size;
}

static void NodeSetAccSize_(void const* context, void* n, size_t acc_size) {
    ZETA_Unused(context);
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(acc_size <= ZETA_UINT_MAX);
    ((Zeta_LinSpaceAllocator_Node*)n)->acc_size = acc_size;
}

static Zeta_BinTreeNodeOperator const gt_opr = {
    .context = NULL,

    .GetP = NodeGetGP_,
    .GetL = NodeGetGL_,
    .GetR = NodeGetGR_,

    .SetP = NodeSetGP_,
    .SetL = NodeSetGL_,
    .SetR = NodeSetGR_,

    .GetColor = NodeGetGPC_,
    .SetColor = NodeSetGPC_,

    .GetAccSize = NodeGetAccSize_,
    .SetAccSize = NodeSetAccSize_,
};

static Zeta_BinTreeNodeOperator const st_opr = {
    .context = NULL,

    .GetP = NodeGetSP_,
    .GetL = NodeGetSL_,
    .GetR = NodeGetSR_,

    .SetP = NodeSetSP_,
    .SetL = NodeSetSL_,
    .SetR = NodeSetSR_,

    .GetColor = NodeGetSPC_,
    .SetColor = NodeSetSPC_,

    .GetAccSize = NULL,
    .SetAccSize = NULL,
};

static Zeta_LinSpaceAllocator_Node* FindNodeFromIdx_(
    Zeta_LinSpaceAllocator* lsa, size_t idx) {
    ZETA_DebugAssert(lsa->beg <= idx && idx <= lsa->end);

    void* n;
    size_t tail_idx;

    Zeta_BinTree_AccessL(&n, &tail_idx, &gt_opr, lsa->gt_root, idx - lsa->beg);

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(tail_idx == 0);

    return n;
}

static Zeta_LinSpaceAllocator_Node* FindNode_(Zeta_LinSpaceAllocator* lsa,
                                              size_t size) {
    Zeta_LinSpaceAllocator_Node* ret = NULL;

    for (Zeta_LinSpaceAllocator_Node* n = lsa->st_root; n != NULL;) {
        if (Zeta_BinTree_GetSize(&gt_opr, n) < size) {
            n = NodeGetSR_(NULL, n);
        } else {
            ret = n;
            n = NodeGetSL_(NULL, n);
        }
    }

    return ret;
}

static Zeta_LinSpaceAllocator_Node* FindNiceNode_(Zeta_LinSpaceAllocator* lsa,
                                                  size_t size) {
    Zeta_LinSpaceAllocator_Node* best_fit_n = FindNode_(lsa, size);

    if (best_fit_n == NULL) { return NULL; }

    if (lsa->strategy == ZETA_LinSpaceAllocator_AllocationStrategy_BestFit ||
        Zeta_BinTree_GetSize(&gt_opr, best_fit_n) == size) {
        return best_fit_n;
    }

    Zeta_LinSpaceAllocator_Node* twice_best_fit_n = FindNode_(lsa, size * 2);

    return twice_best_fit_n == NULL
               ? Zeta_GetMostLink(NULL, NodeGetSR_, lsa->st_root)
               : twice_best_fit_n;
}

static Zeta_LinSpaceAllocator_Node* AllocateNode_(Zeta_LinSpaceAllocator* lsa) {
    Zeta_LinSpaceAllocator_Node* n = ZETA_Allocator_SafeAllocate(
        lsa->node_allocator, alignof(Zeta_LinSpaceAllocator_Node),
        sizeof(Zeta_LinSpaceAllocator_Node));

    ZETA_RelColorPtr_Set(&n->gpc, alignof(Zeta_LinSpaceAllocator_Node), n, n,
                         VacantColor);
    ZETA_RelColorPtr_Set(&n->glc, alignof(Zeta_LinSpaceAllocator_Node), n, n,
                         VacantColor);
    ZETA_RelPtr_SetPtr(&n->gr, n, n);

    ZETA_RelColorPtr_Set(&n->spc, alignof(Zeta_LinSpaceAllocator_Node), n, n,
                         0);
    ZETA_RelPtr_SetPtr(&n->sl, n, n);
    ZETA_RelPtr_SetPtr(&n->sr, n, n);

    return n;
}

static void DeallocateNode_(Zeta_LinSpaceAllocator* lsa,
                            Zeta_LinSpaceAllocator_Node* n) {
    ZETA_Allocator_Deallocate(lsa->node_allocator, n);
}

void Zeta_LinSpaceAllocator_Init(void* lsa_) {
    Zeta_LinSpaceAllocator* lsa = lsa_;
    ZETA_DebugAssert(lsa != NULL);

    size_t beg = lsa->beg;
    size_t end = lsa->end;

    ZETA_DebugAssert(beg < end);

    ZETA_DebugAssert(lsa->node_allocator != NULL);

    Zeta_LinSpaceAllocator_Node* n = AllocateNode_(lsa);
    n->acc_size = end - beg;

    lsa->gt_root = Zeta_RBTree_GeneralInsertL(&gt_opr, NULL, NULL, n);
    lsa->st_root = Zeta_RBTree_GeneralInsertL(&st_opr, NULL, NULL, n);
}

size_t Zeta_LinSpaceAllocator_Allocate(void* lsa_, size_t size) {
    Zeta_LinSpaceAllocator* lsa = lsa_;
    Check_(lsa);

    if (size == 0) { return ZETA_SIZE_MAX; }

    Zeta_LinSpaceAllocator_Node* nm = FindNiceNode_(lsa, size);

    if (nm == NULL) { return ZETA_SIZE_MAX; }

    size_t nm_size = Zeta_BinTree_GetSize(&gt_opr, nm);

    NodeSetGLC_(NULL, nm, OccupiedColor);
    lsa->st_root = Zeta_RBTree_Extract(&st_opr, nm);

    size_t ret = ({
        size_t l_acc_size;
        Zeta_BinTree_GetAccSize(&l_acc_size, NULL, &gt_opr, nm);
        lsa->beg + l_acc_size;
    });

    size_t res_size = nm_size - size;

    if (res_size == 0) { return ret; }

    Zeta_BinTree_SetSize(&gt_opr, nm, size);

    Zeta_LinSpaceAllocator_Node* new_nr = AllocateNode_(lsa);
    new_nr->acc_size = res_size;

    lsa->gt_root =
        Zeta_RBTree_GeneralInsertR(&gt_opr, lsa->gt_root, nm, new_nr);

    lsa->st_root = Zeta_RBTree_GeneralInsertL(
        &st_opr, lsa->st_root, FindNode_(lsa, res_size + 1), new_nr);

    return ret;
}

void Zeta_LinSpaceAllocator_Deallocate(void* lsa_, size_t idx) {
    Zeta_LinSpaceAllocator* lsa = lsa_;
    Check_(lsa);

    if (idx == ZETA_SIZE_MAX) { return; }

    Zeta_LinSpaceAllocator_Node* nm = FindNodeFromIdx_(lsa, idx);

    Zeta_LinSpaceAllocator_Node* nl = Zeta_BinTree_StepL(&gt_opr, nm);
    Zeta_LinSpaceAllocator_Node* nr = Zeta_BinTree_StepR(&gt_opr, nm);

    int nm_color = NodeGetGLC_(NULL, nm);
    ZETA_DebugAssert(nm_color == OccupiedColor);

    int nl_color = nl == NULL ? OccupiedColor : NodeGetGLC_(NULL, nl);
    ZETA_DebugAssert(nl_color == VacantColor || nl_color == OccupiedColor);

    int nr_color = nr == NULL ? OccupiedColor : NodeGetGLC_(NULL, nr);
    ZETA_DebugAssert(nr_color == VacantColor || nr_color == OccupiedColor);

    bool_t l_merge = nl_color == VacantColor;
    bool_t r_merge = nr_color == VacantColor;

    size_t m_size = Zeta_BinTree_GetSize(&gt_opr, nm);

    Zeta_LinSpaceAllocator_Node* ins_n = nm;
    size_t ins_size = m_size;

    if (l_merge) {
        ins_n = nl;
        ins_size += Zeta_BinTree_GetSize(&gt_opr, nl);

        lsa->gt_root = Zeta_RBTree_Extract(&gt_opr, nm);
        DeallocateNode_(lsa, nm);

        lsa->st_root = Zeta_RBTree_Extract(&st_opr, nl);
    } else {
        NodeSetGLC_(NULL, nm, VacantColor);
    }

    if (r_merge) {
        ins_size += Zeta_BinTree_GetSize(&gt_opr, nr);

        lsa->gt_root = Zeta_RBTree_Extract(&gt_opr, nr);
        lsa->st_root = Zeta_RBTree_Extract(&st_opr, nr);

        DeallocateNode_(lsa, nr);
    }

    Zeta_BinTree_SetSize(&gt_opr, ins_n, ins_size);

    lsa->st_root = Zeta_RBTree_GeneralInsertL(
        &st_opr, lsa->st_root, FindNode_(lsa, ins_size + 1), ins_n);
}

size_t Zeta_LinSpaceAllocator_GetVacantSizeL(void* lsa_) {
    Zeta_LinSpaceAllocator* lsa = lsa_;
    Check_(lsa);

    Zeta_LinSpaceAllocator_Node* n =
        Zeta_GetMostLink(NULL, NodeGetGL_, lsa->gt_root);

    return NodeGetGLC_(NULL, n) == VacantColor
               ? Zeta_BinTree_GetSize(&gt_opr, n)
               : 0;
}

size_t Zeta_LinSpaceAllocator_GetVacantSizeR(void* lsa_) {
    Zeta_LinSpaceAllocator* lsa = lsa_;
    Check_(lsa);

    Zeta_LinSpaceAllocator_Node* n =
        Zeta_GetMostLink(NULL, NodeGetGR_, lsa->gt_root);

    return NodeGetGLC_(NULL, n) == VacantColor
               ? Zeta_BinTree_GetSize(&gt_opr, n)
               : 0;
}

void Zeta_LinSpaceAllocator_ExtendL(void* lsa_, size_t cnt) {
    Zeta_LinSpaceAllocator* lsa = lsa_;
    Check_(lsa);

    if (cnt == 0) { return; }

    ZETA_DebugAssert(cnt <= lsa->beg);

    lsa->beg -= cnt;

    Zeta_LinSpaceAllocator_Node* n =
        Zeta_GetMostLink(NULL, NodeGetGL_, lsa->gt_root);

    size_t n_size;

    if (NodeGetGLC_(NULL, n) == VacantColor) {
        lsa->st_root = Zeta_RBTree_Extract(&st_opr, n);

        n_size = Zeta_BinTree_GetSize(&gt_opr, n) + cnt;

        Zeta_BinTree_SetDiffSize(&gt_opr, n, cnt);
    } else {
        n = AllocateNode_(lsa);
        n->acc_size = cnt;

        n_size = cnt;

        lsa->gt_root =
            Zeta_RBTree_GeneralInsertR(&gt_opr, lsa->gt_root, NULL, n);
    }

    lsa->st_root = Zeta_RBTree_GeneralInsertR(&st_opr, lsa->st_root,
                                              FindNode_(lsa, n_size + 1), n);
}

void Zeta_LinSpaceAllocator_ExtendR(void* lsa_, size_t cnt) {
    Zeta_LinSpaceAllocator* lsa = lsa_;
    Check_(lsa);

    if (cnt == 0) { return; }

    ZETA_DebugAssert(cnt <= ZETA_SIZE_MAX - lsa->end);

    lsa->end += cnt;

    Zeta_LinSpaceAllocator_Node* n =
        Zeta_GetMostLink(NULL, NodeGetGR_, lsa->gt_root);

    size_t n_size;

    if (NodeGetGLC_(NULL, n) == VacantColor) {
        lsa->st_root = Zeta_RBTree_Extract(&st_opr, n);

        n_size = Zeta_BinTree_GetSize(&gt_opr, n) + cnt;

        Zeta_BinTree_SetDiffSize(&gt_opr, n, cnt);
    } else {
        n = AllocateNode_(lsa);
        n->acc_size = cnt;

        n_size = cnt;

        lsa->gt_root =
            Zeta_RBTree_GeneralInsertL(&gt_opr, lsa->gt_root, NULL, n);
    }

    lsa->st_root = Zeta_RBTree_GeneralInsertL(&st_opr, lsa->st_root,
                                              FindNode_(lsa, n_size + 1), n);
}

bool_t Zeta_LinSpaceAllocator_ShrinkL(void* lsa_, size_t cnt) {
    Zeta_LinSpaceAllocator* lsa = lsa_;
    Check_(lsa);

    Zeta_LinSpaceAllocator_Node* n =
        Zeta_GetMostLink(NULL, NodeGetGL_, lsa->gt_root);

    if (NodeGetGLC_(NULL, n) == OccupiedColor) { return FALSE; }

    size_t n_size = Zeta_BinTree_GetSize(&gt_opr, n);

    if (n_size < cnt) { return FALSE; }

    lsa->beg += cnt;

    lsa->st_root = Zeta_RBTree_Extract(&st_opr, n);

    if (cnt == n_size) {
        lsa->gt_root = Zeta_RBTree_Extract(&gt_opr, n);
        DeallocateNode_(lsa, n);
        return TRUE;
    }

    n_size -= cnt;

    Zeta_BinTree_SetDiffSize(&gt_opr, n, -cnt);

    lsa->st_root = Zeta_RBTree_GeneralInsertL(&st_opr, lsa->st_root,
                                              FindNode_(lsa, n_size + 1), n);

    return TRUE;
}

bool_t Zeta_LinSpaceAllocator_ShrinkR(void* lsa_, size_t cnt) {
    Zeta_LinSpaceAllocator* lsa = lsa_;
    Check_(lsa);

    Zeta_LinSpaceAllocator_Node* n =
        Zeta_GetMostLink(NULL, NodeGetGR_, lsa->gt_root);

    if (NodeGetGLC_(NULL, n) == OccupiedColor) { return FALSE; }

    size_t n_size = Zeta_BinTree_GetSize(&gt_opr, n);

    if (n_size < cnt) { return FALSE; }

    lsa->end -= cnt;

    lsa->st_root = Zeta_RBTree_Extract(&st_opr, n);

    if (cnt == n_size) {
        lsa->gt_root = Zeta_RBTree_Extract(&gt_opr, n);
        DeallocateNode_(lsa, n);
        return TRUE;
    }

    n_size -= cnt;

    Zeta_BinTree_SetDiffSize(&gt_opr, n, -cnt);

    lsa->st_root = Zeta_RBTree_GeneralInsertL(&st_opr, lsa->st_root,
                                              FindNode_(lsa, n_size + 1), n);

    return TRUE;
}

void Zeta_LinSpaceAllocator_Check(void* lsa_) {
    Zeta_LinSpaceAllocator* lsa = lsa_;
    ZETA_DebugAssert(lsa != NULL);

    ZETA_DebugAssert(lsa->beg <= lsa->end);

    ZETA_DebugAssert(lsa->node_allocator != NULL);

    ZETA_DebugAssert(lsa->gt_root != NULL);
}

#if ZETA_EnableDebug

static void CheckGT_(Zeta_MemRecorder* dst_gt_table, Zeta_MemRecorder* st_table,
                     Zeta_LinSpaceAllocator_Node* n) {
    if (n == NULL) { return; }

    size_t n_size = Zeta_BinTree_GetSize(&gt_opr, n);

    ZETA_DebugAssert(0 < n_size);

    int n_color = NodeGetGLC_(NULL, n);

    ZETA_DebugAssert(n_color == VacantColor || n_color == OccupiedColor);

    Zeta_MemRecorder_Record(dst_gt_table, n,
                            sizeof(Zeta_LinSpaceAllocator_Node));

    if (n_color == VacantColor) {
        ZETA_DebugAssert(Zeta_MemRecorder_Unrecord(st_table, n));
    }

    CheckGT_(dst_gt_table, st_table, NodeGetGL_(NULL, n));
    CheckGT_(dst_gt_table, st_table, NodeGetGR_(NULL, n));

    Zeta_LinSpaceAllocator_Node* nl = Zeta_BinTree_StepL(&gt_opr, n);
    Zeta_LinSpaceAllocator_Node* nr = Zeta_BinTree_StepR(&gt_opr, n);

    ZETA_DebugAssert(nl == NULL || n_color == OccupiedColor ||
                     NodeGetGLC_(NULL, nl) == OccupiedColor);
    ZETA_DebugAssert(nr == NULL || n_color == OccupiedColor ||
                     NodeGetGLC_(NULL, nr) == OccupiedColor);
}

static void CheckST_(Zeta_MemRecorder* dst_st_table,
                     Zeta_LinSpaceAllocator_Node* n) {
    if (n == NULL) { return; }

    ZETA_DebugAssert(NodeGetGLC_(NULL, n) == VacantColor);

    Zeta_MemRecorder_Record(dst_st_table, n,
                            sizeof(Zeta_LinSpaceAllocator_Node));

    size_t n_size = Zeta_BinTree_GetSize(&gt_opr, n);
    ZETA_DebugAssert(0 < n_size);

    CheckST_(dst_st_table, NodeGetSL_(NULL, n));
    CheckST_(dst_st_table, NodeGetSR_(NULL, n));

    Zeta_LinSpaceAllocator_Node* nl = Zeta_BinTree_StepL(&st_opr, n);
    Zeta_LinSpaceAllocator_Node* nr = Zeta_BinTree_StepR(&st_opr, n);

    ZETA_DebugAssert(nl == NULL || Zeta_BinTree_GetSize(&gt_opr, nl) <= n_size);
    ZETA_DebugAssert(nr == NULL || n_size <= Zeta_BinTree_GetSize(&gt_opr, nr));
}

#endif

void Zeta_LinSpaceAllocator_Sanitize(void* lsa_, Zeta_MemRecorder* dst_ns) {
    Zeta_LinSpaceAllocator* lsa = lsa_;
    Check_(lsa);

#if !ZETA_EnableDebug
    ZETA_Unused(dst_ns);
#else
    ZETA_DebugAssert(NodeGetGP_(NULL, lsa->gt_root) == NULL);

    ZETA_DebugAssert(lsa->st_root == NULL ||
                     NodeGetSP_(NULL, lsa->st_root) == NULL);

    Zeta_MemRecorder* gt_table =
        dst_ns == NULL ? Zeta_MemRecorder_Create() : dst_ns;

    Zeta_MemRecorder_Clear(gt_table);

    Zeta_MemRecorder* st_table = Zeta_MemRecorder_Create();

    Zeta_RBTree_Check(&gt_opr, NULL, lsa->gt_root);
    Zeta_RBTree_Check(&st_opr, NULL, lsa->st_root);

    ZETA_DebugAssert(lsa->end - lsa->beg ==
                     NodeGetAccSize_(NULL, lsa->gt_root));

    CheckST_(st_table, lsa->st_root);
    CheckGT_(gt_table, st_table, lsa->gt_root);

    ZETA_DebugAssert(Zeta_MemRecorder_GetSize(st_table) == 0);

    if (dst_ns == NULL) { Zeta_MemRecorder_Destroy(gt_table); }

    Zeta_MemRecorder_Destroy(st_table);
#endif
}
