#include "../Zeta/CountingBinTree.h"
#include "../Zeta/OrdinaryCountingBinColoredTreeNode.h"
#include "../Zeta/RBTree.h"
#include "../Zeta/utils.h"
#include "SimpleVector.h"
#include "test_head.h"

static Zeta_RBTreeNodeOpr rbtn_opr = {
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

static Zeta_CountingBinTreeNodeOpr cbtn_opr = {
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

void GPList_Init(GPList* gplist) {
    Zeta_OrdinaryCountingBinColoredTreeNode_Init(&gplist->lb);
    Zeta_OrdinaryCountingBinColoredTreeNode_Init(&gplist->rb);
    cbtn_opr.SetSize(&gplist->lb, 1);
    cbtn_opr.SetSize(&gplist->rb, 1);

    void* root = NULL;
    root = Zeta_RBTree_InsertR(&rbtn_opr, root, &gplist->lb);
    root = Zeta_RBTree_InsertR(&rbtn_opr, root, &gplist->rb);
    gplist->root = root;
}

diff_t GPList_GetSize(GPList* gplist) {
    return cbtn_opr.GetAccSize(gplist->root) - 2;
}

void* GPList_GetNode(GPList* gplist, diff_t idx) {
    diff_t size = GPList_GetSize(gplist);
    ZETA_DEBUG_ASSERT(-1 <= idx && idx <= size);

    void* ret;

    Zeta_CountingBinTree_Access(&ret,          // dst_n
                                0,             // dst_idx
                                &cbtn_opr,     // cbtn_opr
                                gplist->root,  // n
                                idx + 1        // idx
    );

    diff_t dst_l_acc_size;
    diff_t dst_r_acc_size;

    Zeta_CountingBinTree_GetAccSize(&dst_l_acc_size,  // dst_l_acc_size
                                    &dst_r_acc_size,  // dst_r_acc_size
                                    &cbtn_opr,        // cbtn_opr
                                    ret               // n
    );

    ZETA_DEBUG_ASSERT(dst_l_acc_size == idx + 1);
    ZETA_DEBUG_ASSERT(dst_r_acc_size == size - idx);

    return ret;
}

val_t* GPList_Access(GPList* gplist, diff_t idx) {
    diff_t size = GPList_GetSize(gplist);
    ZETA_DEBUG_ASSERT(0 <= idx && idx < size);

    void* n = GPList_GetNode(gplist, idx);

    ZETA_DEBUG_ASSERT(
        gplist->root ==
        Zeta_GetMostLink(Zeta_OrdinaryCountingBinColoredTreeNode_GetP, n));
    ZETA_DEBUG_ASSERT(&gplist->lb != n);
    ZETA_DEBUG_ASSERT(&gplist->rb != n);

    return &ZETA_FROM_MEM(GPListNode, n, n)->val;
}

val_t* GPList_InsertL(GPList* gplist, void* n) {
    void* root =
        Zeta_GetMostLink(Zeta_OrdinaryCountingBinColoredTreeNode_GetP, n);

    ZETA_DEBUG_ASSERT(gplist->root == root);
    ZETA_DEBUG_ASSERT(&gplist->lb != n);

    GPListNode* gpn = malloc(sizeof(GPListNode));
    Zeta_OrdinaryCountingBinColoredTreeNode_Init(&gpn->n);
    Zeta_OrdinaryCountingBinColoredTreeNode_SetSize(&gpn->n, 1);
    gplist->root = Zeta_RBTree_InsertL(&rbtn_opr, n, &gpn->n);

    return &gpn->val;
}

val_t* GPList_InsertLWithIdx(GPList* gplist, diff_t idx) {
    return GPList_InsertL(gplist, GPList_GetNode(gplist, idx));
}

val_t* GPList_InsertR(GPList* gplist, void* n) {
    void* root =
        Zeta_GetMostLink(Zeta_OrdinaryCountingBinColoredTreeNode_GetP, n);

    ZETA_DEBUG_ASSERT(gplist->root == root);
    ZETA_DEBUG_ASSERT(&gplist->rb != n);

    GPListNode* gpn = malloc(sizeof(GPListNode));
    Zeta_OrdinaryCountingBinColoredTreeNode_Init(&gpn->n);
    Zeta_OrdinaryCountingBinColoredTreeNode_SetSize(&gpn->n, 1);
    gplist->root = Zeta_RBTree_InsertR(&rbtn_opr, n, &gpn->n);

    return &gpn->val;
}

val_t* GPList_InsertRWithIdx(GPList* gplist, diff_t idx) {
    return GPList_InsertR(gplist, GPList_GetNode(gplist, idx));
}

void GPList_Erase(GPList* gplist, void* n) {
    void* root =
        Zeta_GetMostLink(Zeta_OrdinaryCountingBinColoredTreeNode_GetP, n);

    ZETA_DEBUG_ASSERT(gplist->root == root);
    ZETA_DEBUG_ASSERT(&gplist->lb != n);
    ZETA_DEBUG_ASSERT(&gplist->rb != n);

    gplist->root = Zeta_RBTree_Extract(&rbtn_opr, n);
    free(ZETA_FROM_MEM(GPListNode, n, n));
}

void GPList_EraseWithIdx(GPList* gplist, diff_t idx) {
    return GPList_Erase(gplist, GPList_GetNode(gplist, idx));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static diff_t Check_(void* n) {
    if (n == NULL) { return 0; }

    ZETA_DEBUG_ASSERT(Zeta_OrdinaryCountingBinColoredTreeNode_GetSize(n) == 1);

    void* nl = Zeta_OrdinaryCountingBinColoredTreeNode_GetL(n);
    void* nr = Zeta_OrdinaryCountingBinColoredTreeNode_GetR(n);

    if (nl != NULL) {
        ZETA_DEBUG_ASSERT(Zeta_OrdinaryCountingBinColoredTreeNode_GetP(nl) ==
                          n);
    }

    if (nr != NULL) {
        ZETA_DEBUG_ASSERT(Zeta_OrdinaryCountingBinColoredTreeNode_GetP(nr) ==
                          n);
    }

    int nc = Zeta_OrdinaryCountingBinColoredTreeNode_GetColor(n);

    if (nc == 1) {
        ZETA_DEBUG_ASSERT(
            Zeta_OrdinaryCountingBinColoredTreeNode_GetColor(nl) == 0);
        ZETA_DEBUG_ASSERT(
            Zeta_OrdinaryCountingBinColoredTreeNode_GetColor(nr) == 0);
    }

    diff_t l_bh = Check_(nl);
    diff_t r_bh = Check_(nr);

    ZETA_DEBUG_ASSERT(l_bh == r_bh);

    return nc == 1 ? l_bh : 1 + l_bh;
}

void GPList_Check(GPList* gplist) { Check_(gplist->root); }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

int main() {
    /*
    s            : get size
    a <pos>      : access val at pos
    il <pos> <val>: insert lt val at pos
    ir <pos> <val>: insert rt val at pos
    e <pos>      : erase at pos
    z            : end
    */

    // FILE* f = fopen("out.txt", "w");

    GPList gplist;
    GPList_Init(&gplist);

    char command[8];
    diff_t pos;
    val_t val;

    for (;;) {
        GPList_Check(&gplist);

        fprintf(stderr, "waiting new command\n");
        scanf("%s", &command);

        if (strcmp(command, "s") == 0) {
            printf("%lld\n", GPList_GetSize(&gplist));
            continue;
        }

        if (strcmp(command, "a") == 0) {
            scanf("%lld", &pos);

            val_t* val = GPList_Access(&gplist, pos);

            fprintf(stderr, "%s %lld %d\n", command, pos, val);
            printf("%d\n", *val);
            continue;
        }

        if (strcmp(command, "il") == 0) {
            scanf("%lld", &pos);
            scanf("%d", &val);

            val_t* n_val = GPList_InsertLWithIdx(&gplist, pos);
            *n_val = val;

            fprintf(stderr, "%s %lld %d\n", command, pos, val);
            printf("0\n");
            continue;
        }

        if (strcmp(command, "ir") == 0) {
            scanf("%lld", &pos);
            scanf("%d", &val);

            val_t* n_val = GPList_InsertRWithIdx(&gplist, pos);
            *n_val = val;

            fprintf(stderr, "%s %lld %d\n", command, pos, val);
            printf("0\n");
            continue;
        }

        if (strcmp(command, "e") == 0) {
            scanf("%lld", &pos);

            GPList_EraseWithIdx(&gplist, pos);

            fprintf(stderr, "%s %lld\n", command, pos);
            printf("0\n");
            continue;
        }

        if (strcmp(command, "z") == 0) {
            fprintf(stderr, "%s\n", command);
            printf("0\n");
            break;
        }

        ZETA_DEBUG_ASSERT(0);
    }

    return 0;
}
