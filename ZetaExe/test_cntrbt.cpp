#include <random>
#include <vector>

#include "../Zeta/CntBinTree.h"
#include "../Zeta/RBTree.h"
#include "../Zeta/RelCntRBTreeNode.h"
#include "../Zeta/utils.h"

// -----------------------------------------------------------------------------

struct Node {
    Zeta_RelCntRBTreeNode n;
};

struct NodeCup {
    Node* linked_node;
    size_t size;
};

// -----------------------------------------------------------------------------

Zeta_BinTreeNodeAccessor btn_accessor = (Zeta_BinTreeNodeAccessor){
    .context = NULL,

    .GetP = Zeta_RelCntRBTreeNode_GetP,
    .GetL = Zeta_RelCntRBTreeNode_GetL,
    .GetR = Zeta_RelCntRBTreeNode_GetR,
};

Zeta_RBTreeNodeOpr rbtn_opr = (Zeta_RBTreeNodeOpr){
    .context = NULL,

    .GetP = Zeta_RelCntRBTreeNode_GetP,
    .GetL = Zeta_RelCntRBTreeNode_GetL,
    .GetR = Zeta_RelCntRBTreeNode_GetR,

    .GetColor = Zeta_RelCntRBTreeNode_GetColor,
    .ReverseColor = Zeta_RelCntRBTreeNode_ReverseColor,

    .AttachL = Zeta_RelCntRBTreeNode_AttachL,
    .AttachR = Zeta_RelCntRBTreeNode_AttachR,

    .Detach = Zeta_RelCntRBTreeNode_Detach,

    .Swap = Zeta_RelCntRBTreeNode_Swap,

    .RotateL = Zeta_RelCntRBTreeNode_RotateL,
    .RotateR = Zeta_RelCntRBTreeNode_RotateR,
};

Zeta_CntBinTreeNodeOpr cbtn_opr = (Zeta_CntBinTreeNodeOpr){
    .context = NULL,

    .GetP = Zeta_RelCntRBTreeNode_GetP,
    .GetL = Zeta_RelCntRBTreeNode_GetL,
    .GetR = Zeta_RelCntRBTreeNode_GetR,

    .GetAccSize = Zeta_RelCntRBTreeNode_GetAccSize,
    .GetSize = Zeta_RelCntRBTreeNode_GetSize,
    .SetSize = Zeta_RelCntRBTreeNode_SetSize,
};

// -----------------------------------------------------------------------------

std::vector<NodeCup> vec;

size_t size_sum;

void* root;
void* rb;

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

void CompareLR() {
    void* n = Zeta_GetMostLink(NULL, rbtn_opr.GetL, root);
    auto iter = vec.begin();
    auto end = vec.end();

    for (;;) {
        if (iter == end) {
            ZETA_DebugAssert(n == NULL);
            return;
        }

        ZETA_DebugAssert(n != NULL);

        Node* node = ZETA_GetStructFromMember(Node, n, n);

        ZETA_DebugAssert(iter->linked_node == node);
        ZETA_DebugAssert(iter->size ==
                         Zeta_RelCntRBTreeNode_GetSize(NULL, node));

        ++iter;
        n = Zeta_BinTree_StepR(&btn_accessor, n);
    }
}

void CompareRL() {
    void* n = Zeta_GetMostLink(NULL, rbtn_opr.GetR, root);
    auto iter = vec.rbegin();
    auto end = vec.rend();

    for (;;) {
        if (iter == end) {
            ZETA_DebugAssert(n == NULL);
            return;
        }

        ZETA_DebugAssert(n != NULL);

        Node* node = ZETA_GetStructFromMember(Node, n, n);

        ZETA_DebugAssert(iter->linked_node == node);
        ZETA_DebugAssert(iter->size ==
                         Zeta_RelCntRBTreeNode_GetSize(NULL, node));

        ++iter;
        n = Zeta_BinTree_StepL(&btn_accessor, n);
    }
}

void Check() {
    Zeta_RBTree_Check(&rbtn_opr, root);
    CompareLR();
    CompareRL();
}

// -----------------------------------------------------------------------------

size_t fallback_sign = 0x479237197577;

void Access(size_t idx) {
    void* target_n;
    size_t target_tail_idx;

    Zeta_CntBinTree_Access(&target_n, &target_tail_idx, &cbtn_opr, root, idx);

    auto target_iter = vec.end();

    for (auto iter = vec.begin(), end = vec.end(); iter != end; ++iter) {
        if (idx < iter->size) {
            target_iter = iter;
            break;
        }

        idx -= iter->size;
    }

    if (target_iter == vec.end()) {
        ZETA_DebugAssert(target_n == NULL);
    } else {
        ZETA_DebugAssert(&target_iter->linked_node->n == target_n);
    }

    ZETA_DebugAssert(target_tail_idx == idx);
}

void Insert(size_t idx, size_t size) {
    ZETA_DebugAssert(0 <= idx);
    ZETA_DebugAssert(idx <= vec.size());

    Node* new_node = new Node;
    Zeta_RelCntRBTreeNode_Init(NULL, &new_node->n);
    Zeta_RelCntRBTreeNode_SetSize(NULL, &new_node->n, size);

    size_sum += size;

    if (idx < vec.size()) {
        Node* ins_node = vec[idx].linked_node;
        root = Zeta_RBTree_InsertL(&rbtn_opr, &ins_node->n, &new_node->n);
    } else if (vec.size() == 0) {
        root = &new_node->n;
    } else {
        Node* ins_node = vec.back().linked_node;
        root = Zeta_RBTree_InsertR(&rbtn_opr, &ins_node->n, &new_node->n);
    }

    vec.insert(vec.begin() + idx, (NodeCup){
                                      .linked_node = new_node,
                                      .size = size,
                                  });
}

// -----------------------------------------------------------------------------

void Erase(size_t idx) {
    ZETA_DebugAssert(0 <= idx);
    ZETA_DebugAssert(idx < vec.size());

    size_sum -= vec[idx].size;

    Node* target_node = vec[idx].linked_node;
    root = Zeta_RBTree_Extract(&rbtn_opr, &target_node->n);
    delete target_node;

    vec.erase(vec.begin() + idx);
}

// -----------------------------------------------------------------------------

void main1() {
    unsigned int seed = time(NULL);

    ZETA_PrintVar("%d", seed);

    std::mt19937_64 en{ seed };
    std::uniform_int_distribution<size_t> idx_generator{ 0,
                                                         ZETA_maxof(size_t) };
    std::uniform_int_distribution<size_t> size_generator{ 0, 16 };

    size_sum = 0;
    root = NULL;

    for (int i = 0; i < 20; ++i) {
        Insert(idx_generator(en) % (vec.size() + 1), size_generator(en));
        Check();
    }

    ZETA_PrintPos;

    for (int _ = 0; _ < 5; ++_) {
        for (int i = 0, end = idx_generator(en) % 100 + 20; i < end; ++i) {
            Insert(idx_generator(en) % (vec.size() + 1), size_generator(en));
            Check();
        }

        ZETA_PrintPos;

        for (int i = 0,
                 end = std::min(vec.size(), idx_generator(en) % 100 + 20);
             i < end; ++i) {
            Erase(idx_generator(en) % vec.size());
            Check();
        }

        for (int i = 0, end = vec.size() * 2; i < end; ++i) {
            int idx = idx_generator(en) % (size_sum * 2);
            ZETA_PrintVar("%lld", idx);
            Access(idx);
        }
    }

    ZETA_PrintPos;

    printf("ok\n");
}

// -----------------------------------------------------------------------------

int main() {
    main1();
    return 0;
}
