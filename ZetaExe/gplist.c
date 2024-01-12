#include "../Zeta/CountingBinTree.h"
#include "../Zeta/OrdinaryCountingBinColoredTreeNode.h"
#include "../Zeta/RBTree.h"
#include "../Zeta/utils.h"

typedef unsigned long long int val_t;

Zeta_BinTreeNodeAccessor bt_accessor = {
    .GetP = Zeta_OrdinaryCountingBinColoredTreeNode_GetP,
    .GetL = Zeta_OrdinaryCountingBinColoredTreeNode_GetL,
    .GetR = Zeta_OrdinaryCountingBinColoredTreeNode_GetR,
};

Zeta_RBTreeNodeOpr rbtn_opr = {
    .context = NULL,

    .GetP = Zeta_OrdinaryCountingBinColoredTreeNode_GetP,
    .GetL = Zeta_OrdinaryCountingBinColoredTreeNode_GetL,
    .GetR = Zeta_OrdinaryCountingBinColoredTreeNode_GetR,

    .GetColor = Zeta_OrdinaryCountingBinColoredTreeNode_GetColor,
    .ReverseColor = Zeta_OrdinaryCountingBinColoredTreeNode_ReverseColor,

    .AttachL = Zeta_OrdinaryCountingBinColoredTreeNode_AttachL,
    .AttachR = Zeta_OrdinaryCountingBinColoredTreeNode_AttachR,

    .Detach = Zeta_OrdinaryCountingBinColoredTreeNode_Detach,

    .Swap = Zeta_OrdinaryCountingBinColoredTreeNode_Swap,

    .RotateL = Zeta_OrdinaryCountingBinColoredTreeNode_RotateL,
    .RotateR = Zeta_OrdinaryCountingBinColoredTreeNode_RotateR,
};

Zeta_CountingBinTreeNodeOpr cbtn_opr = {
    .context = NULL,

    .GetP = Zeta_OrdinaryCountingBinColoredTreeNode_GetP,
    .GetL = Zeta_OrdinaryCountingBinColoredTreeNode_GetL,
    .GetR = Zeta_OrdinaryCountingBinColoredTreeNode_GetR,

    .GetAccSize = Zeta_OrdinaryCountingBinColoredTreeNode_GetAccSize,
    .GetSize = Zeta_OrdinaryCountingBinColoredTreeNode_GetSize,
    .SetSize = Zeta_OrdinaryCountingBinColoredTreeNode_SetSize,
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

ZETA_DECL_STRUCT(GPListNode) {
    Zeta_OrdinaryCountingBinColoredTreeNode n;
    val_t val;
};

ZETA_DECL_STRUCT(GPList) {
    void* root;
    Zeta_OrdinaryCountingBinColoredTreeNode lb;
    Zeta_OrdinaryCountingBinColoredTreeNode rb;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void GPList_Init(void* gpl_) {
    GPList* gpl = gpl_;

    Zeta_OrdinaryCountingBinColoredTreeNode_Init(NULL, &gpl->lb);
    Zeta_OrdinaryCountingBinColoredTreeNode_Init(NULL, &gpl->rb);
    cbtn_opr.SetSize(NULL, &gpl->lb, 1);
    cbtn_opr.SetSize(NULL, &gpl->rb, 1);

    void* root = NULL;
    root = Zeta_RBTree_InsertR(&rbtn_opr, root, &gpl->lb);
    root = Zeta_RBTree_InsertR(&rbtn_opr, root, &gpl->rb);
    gpl->root = root;
}

void* GPList_Create() {
    GPList* gpl = malloc(sizeof(GPList));
    GPList_Init(gpl);
    return gpl;
}

diff_t GPList_GetSize(void* gpl_) {
    GPList* gpl = gpl_;
    return cbtn_opr.GetAccSize(NULL, gpl->root) - 2;
}

void* GPList_GetNode(void* gpl_, diff_t idx) {
    GPList* gpl = gpl_;

    diff_t size = GPList_GetSize(gpl);
    ZETA_DEBUG_ASSERT(-1 <= idx && idx <= size);

    void* ret;

    Zeta_CountingBinTree_Access(&ret,       // dst_n
                                0,          // dst_idx
                                &cbtn_opr,  // cbtn_opr
                                gpl->root,  // n
                                idx + 1     // idx
    );

    return ret;
}

val_t GPList_GetVal(void* gpl_, void* n) {
    GPList* gpl = gpl_;
    ZETA_DEBUG_ASSERT(&gpl->lb != n);
    ZETA_DEBUG_ASSERT(&gpl->rb != n);

    return ZETA_FROM_MEM(GPListNode, n, n)->val;
}

void GPList_SetVal(void* gpl_, void* n, val_t val) {
    GPList* gpl = gpl_;
    ZETA_DEBUG_ASSERT(&gpl->lb != n);
    ZETA_DEBUG_ASSERT(&gpl->rb != n);

    ZETA_FROM_MEM(GPListNode, n, n)->val = val;
}

void* GPList_GetL(void* gpl_, void* n) {
    GPList* gpl = gpl_;
    ZETA_DEBUG_ASSERT(&gpl->lb != n);
    return Zeta_BinTree_StepL(&bt_accessor, n);
}

void* GPList_GetR(void* gpl_, void* n) {
    GPList* gpl = gpl_;
    ZETA_DEBUG_ASSERT(&gpl->rb != n);
    return Zeta_BinTree_StepR(&bt_accessor, n);
}

void* GPList_InsertL(void* gpl_, void* n) {
    GPList* gpl = gpl_;

    void* root =
        Zeta_GetMostLink(NULL, Zeta_OrdinaryCountingBinColoredTreeNode_GetP, n);

    ZETA_DEBUG_ASSERT(gpl->root == root);
    ZETA_DEBUG_ASSERT(&gpl->lb != n);

    GPListNode* gpn = malloc(sizeof(GPListNode));
    Zeta_OrdinaryCountingBinColoredTreeNode_Init(NULL, &gpn->n);
    Zeta_OrdinaryCountingBinColoredTreeNode_SetSize(NULL, &gpn->n, 1);
    gpl->root = Zeta_RBTree_InsertL(&rbtn_opr, n, &gpn->n);

    return &gpn->n;
}

void* GPList_InsertR(void* gpl_, void* n) {
    GPList* gpl = gpl_;

    void* root =
        Zeta_GetMostLink(NULL, Zeta_OrdinaryCountingBinColoredTreeNode_GetP, n);

    ZETA_DEBUG_ASSERT(gpl->root == root);
    ZETA_DEBUG_ASSERT(&gpl->rb != n);

    GPListNode* gpn = malloc(sizeof(GPListNode));
    Zeta_OrdinaryCountingBinColoredTreeNode_Init(NULL, &gpn->n);
    Zeta_OrdinaryCountingBinColoredTreeNode_SetSize(NULL, &gpn->n, 1);
    gpl->root = Zeta_RBTree_InsertR(&rbtn_opr, n, &gpn->n);

    return &gpn->n;
}

void GPList_Erase(void* gpl_, void* n) {
    GPList* gpl = gpl_;

    void* root =
        Zeta_GetMostLink(NULL, Zeta_OrdinaryCountingBinColoredTreeNode_GetP, n);

    ZETA_DEBUG_ASSERT(gpl->root == root);
    ZETA_DEBUG_ASSERT(&gpl->lb != n);
    ZETA_DEBUG_ASSERT(&gpl->rb != n);

    gpl->root = Zeta_RBTree_Extract(&rbtn_opr, n);
    free(ZETA_FROM_MEM(GPListNode, n, n));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static diff_t Check_(void* n) {
    if (n == NULL) { return 0; }

    ZETA_DEBUG_ASSERT(
        Zeta_OrdinaryCountingBinColoredTreeNode_GetSize(NULL, n) == 1);

    void* nl = Zeta_OrdinaryCountingBinColoredTreeNode_GetL(NULL, n);
    void* nr = Zeta_OrdinaryCountingBinColoredTreeNode_GetR(NULL, n);

    if (nl != NULL) {
        ZETA_DEBUG_ASSERT(
            Zeta_OrdinaryCountingBinColoredTreeNode_GetP(NULL, nl) == n);
    }

    if (nr != NULL) {
        ZETA_DEBUG_ASSERT(
            Zeta_OrdinaryCountingBinColoredTreeNode_GetP(NULL, nr) == n);
    }

    int nc = Zeta_OrdinaryCountingBinColoredTreeNode_GetColor(NULL, n);

    if (nc == 1) {
        ZETA_DEBUG_ASSERT(
            Zeta_OrdinaryCountingBinColoredTreeNode_GetColor(NULL, nl) == 0);
        ZETA_DEBUG_ASSERT(
            Zeta_OrdinaryCountingBinColoredTreeNode_GetColor(NULL, nr) == 0);
    }

    diff_t l_bh = Check_(nl);
    diff_t r_bh = Check_(nr);

    ZETA_DEBUG_ASSERT(l_bh == r_bh);

    return nc == 1 ? l_bh : 1 + l_bh;
}

void GPList_Check(GPList* gpl) { Check_(gpl->root); }
