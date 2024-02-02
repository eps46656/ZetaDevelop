#include "../Zeta/CntBinTree.h"
#include "../Zeta/OrdCntRBTreeNode.h"
#include "../Zeta/RBTree.h"
#include "../Zeta/utils.h"

typedef unsigned long long int val_t;

Zeta_BinTreeNodeAccessor bt_accessor = {
    .GetP = Zeta_OrdCntRBTreeNode_GetP,
    .GetL = Zeta_OrdCntRBTreeNode_GetL,
    .GetR = Zeta_OrdCntRBTreeNode_GetR,
};

Zeta_RBTreeNodeOpr rbtn_opr = {
    .context = NULL,

    .GetP = Zeta_OrdCntRBTreeNode_GetP,
    .GetL = Zeta_OrdCntRBTreeNode_GetL,
    .GetR = Zeta_OrdCntRBTreeNode_GetR,

    .GetColor = Zeta_OrdCntRBTreeNode_GetColor,
    .ReverseColor = Zeta_OrdCntRBTreeNode_ReverseColor,

    .AttachL = Zeta_OrdCntRBTreeNode_AttachL,
    .AttachR = Zeta_OrdCntRBTreeNode_AttachR,

    .Detach = Zeta_OrdCntRBTreeNode_Detach,

    .Swap = Zeta_OrdCntRBTreeNode_Swap,

    .RotateL = Zeta_OrdCntRBTreeNode_RotateL,
    .RotateR = Zeta_OrdCntRBTreeNode_RotateR,
};

Zeta_CntBinTreeNodeOpr cbtn_opr = {
    .context = NULL,

    .GetP = Zeta_OrdCntRBTreeNode_GetP,
    .GetL = Zeta_OrdCntRBTreeNode_GetL,
    .GetR = Zeta_OrdCntRBTreeNode_GetR,

    .GetAccSize = Zeta_OrdCntRBTreeNode_GetAccSize,
    .GetSize = Zeta_OrdCntRBTreeNode_GetSize,
    .SetSize = Zeta_OrdCntRBTreeNode_SetSize,
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

ZETA_DECL_STRUCT(GPListNode) {
    Zeta_OrdCntRBTreeNode n;
    val_t val;
};

ZETA_DECL_STRUCT(GPList) {
    void* root;
    Zeta_OrdCntRBTreeNode* lb;
    Zeta_OrdCntRBTreeNode* rb;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void GPList_Init(void* gpl_) {
    GPList* gpl = gpl_;
    ZETA_DEBUG_ASSERT(gpl != NULL);

    gpl->lb = malloc(sizeof(GPListNode));
    gpl->rb = malloc(sizeof(GPListNode));

    Zeta_OrdCntRBTreeNode_Init(NULL, gpl->lb);
    Zeta_OrdCntRBTreeNode_Init(NULL, gpl->rb);
    cbtn_opr.SetSize(NULL, gpl->lb, 1);
    cbtn_opr.SetSize(NULL, gpl->rb, 1);

    void* root = NULL;
    root = Zeta_RBTree_InsertR(&rbtn_opr, root, gpl->lb);
    root = Zeta_RBTree_InsertR(&rbtn_opr, root, gpl->rb);
    gpl->root = root;
}

void* GPList_Create() {
    GPList* gpl = malloc(sizeof(GPList));
    GPList_Init(gpl);
    return gpl;
}

size_t GPList_GetSize(void* gpl_) {
    GPList* gpl = gpl_;
    ZETA_DEBUG_ASSERT(gpl != NULL);

    return cbtn_opr.GetAccSize(NULL, gpl->root) - 2;
}

void* GPList_GetNode(void* gpl_, size_t idx) {
    GPList* gpl = gpl_;
    ZETA_DEBUG_ASSERT(gpl != NULL);

    size_t size = GPList_GetSize(gpl);
    ZETA_DEBUG_ASSERT(0 <= idx);
    ZETA_DEBUG_ASSERT(idx <= size);

    void* ret;

    Zeta_CntBinTree_Access(&ret,       // dst_n
                           0,          // dst_idx
                           &cbtn_opr,  // cbtn_opr
                           gpl->root,  // n
                           idx + 1     // idx
    );

    return ret;
}

val_t GPList_GetVal(void* gpl_, void* n) {
    GPList* gpl = gpl_;
    ZETA_DEBUG_ASSERT(gpl != NULL);

    ZETA_DEBUG_ASSERT(gpl->lb != n);
    ZETA_DEBUG_ASSERT(gpl->rb != n);

    return ZETA_FROM_MEM(GPListNode, n, n)->val;
}

void GPList_SetVal(void* gpl_, void* n, val_t val) {
    GPList* gpl = gpl_;
    ZETA_DEBUG_ASSERT(gpl != NULL);

    ZETA_DEBUG_ASSERT(gpl->lb != n);
    ZETA_DEBUG_ASSERT(gpl->rb != n);

    ZETA_FROM_MEM(GPListNode, n, n)->val = val;
}

void* GPList_GetL(void* gpl_, void* n) {
    GPList* gpl = gpl_;
    ZETA_DEBUG_ASSERT(gpl != NULL);

    ZETA_DEBUG_ASSERT(gpl->lb != n);

    return Zeta_BinTree_StepL(&bt_accessor, n);
}

void* GPList_GetR(void* gpl_, void* n) {
    GPList* gpl = gpl_;
    ZETA_DEBUG_ASSERT(gpl != NULL);

    ZETA_DEBUG_ASSERT(gpl->rb != n);

    return Zeta_BinTree_StepR(&bt_accessor, n);
}

void* GPList_InsertL(void* gpl_, void* n) {
    GPList* gpl = gpl_;
    ZETA_DEBUG_ASSERT(gpl != NULL);

    void* root = Zeta_GetMostLink(NULL, Zeta_OrdCntRBTreeNode_GetP, n);

    ZETA_DEBUG_ASSERT(gpl->root == root);
    ZETA_DEBUG_ASSERT(gpl->lb != n);

    GPListNode* gpn = malloc(sizeof(GPListNode));
    Zeta_OrdCntRBTreeNode_Init(NULL, &gpn->n);
    Zeta_OrdCntRBTreeNode_SetSize(NULL, &gpn->n, 1);
    gpl->root = Zeta_RBTree_InsertL(&rbtn_opr, n, &gpn->n);

    return &gpn->n;
}

void* GPList_InsertR(void* gpl_, void* n) {
    GPList* gpl = gpl_;
    ZETA_DEBUG_ASSERT(gpl != NULL);

    void* root = Zeta_GetMostLink(NULL, Zeta_OrdCntRBTreeNode_GetP, n);

    ZETA_DEBUG_ASSERT(gpl->root == root);
    ZETA_DEBUG_ASSERT(gpl->rb != n);

    GPListNode* gpn = malloc(sizeof(GPListNode));
    Zeta_OrdCntRBTreeNode_Init(NULL, &gpn->n);
    Zeta_OrdCntRBTreeNode_SetSize(NULL, &gpn->n, 1);
    gpl->root = Zeta_RBTree_InsertR(&rbtn_opr, n, &gpn->n);

    return &gpn->n;
}

void GPList_Erase(void* gpl_, void* n) {
    GPList* gpl = gpl_;
    ZETA_DEBUG_ASSERT(gpl != NULL);

    void* root = Zeta_GetMostLink(NULL, Zeta_OrdCntRBTreeNode_GetP, n);

    ZETA_DEBUG_ASSERT(gpl->root == root);
    ZETA_DEBUG_ASSERT(gpl->lb != n);
    ZETA_DEBUG_ASSERT(gpl->rb != n);

    gpl->root = Zeta_RBTree_Extract(&rbtn_opr, n);
    free(ZETA_FROM_MEM(GPListNode, n, n));
}

void GPList_Check(void* gpl_) {
    GPList* gpl = gpl_;
    ZETA_DEBUG_ASSERT(gpl != NULL);

    Zeta_RBTree_Check(&rbtn_opr, gpl->root);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static size_t Check_(void* n) {
    if (n == NULL) { return 0; }

    ZETA_DEBUG_ASSERT(Zeta_OrdCntRBTreeNode_GetSize(NULL, n) == 1);

    void* nl = Zeta_OrdCntRBTreeNode_GetL(NULL, n);
    void* nr = Zeta_OrdCntRBTreeNode_GetR(NULL, n);

    if (nl != NULL) {
        ZETA_DEBUG_ASSERT(Zeta_OrdCntRBTreeNode_GetP(NULL, nl) == n);
    }

    if (nr != NULL) {
        ZETA_DEBUG_ASSERT(Zeta_OrdCntRBTreeNode_GetP(NULL, nr) == n);
    }

    int nc = Zeta_OrdCntRBTreeNode_GetColor(NULL, n);

    if (nc == 1) {
        ZETA_DEBUG_ASSERT(Zeta_OrdCntRBTreeNode_GetColor(NULL, nl) == 0);
        ZETA_DEBUG_ASSERT(Zeta_OrdCntRBTreeNode_GetColor(NULL, nr) == 0);
    }

    size_t l_bh = Check_(nl);
    size_t r_bh = Check_(nr);

    ZETA_DEBUG_ASSERT(l_bh == r_bh);

    return nc == 1 ? l_bh : 1 + l_bh;
}

// void GPList_Check(GPList* gpl) {}
