#include <random>
#include <vector>

#include "../Zeta/Debugger.h"
#include "../Zeta/OrdCnt3RBTreeNode.h"
#include "../Zeta/OrdCntRBTreeNode.h"
#include "../Zeta/RBTree.h"
#include "../Zeta/utils.h"

// -----------------------------------------------------------------------------

struct Node {
    Zeta_OrdCntRBTreeNode n;
};

struct NodeCup {
    Node* linked_node;
    size_t size;
};

// -----------------------------------------------------------------------------

Zeta_BinTreeNodeOperator btn_opr;

// -----------------------------------------------------------------------------

std::vector<NodeCup> vec;

size_t size_sum;

void* root;
void* rb;

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

void CompareLR() {
    void* n = Zeta_GetMostLink(NULL, btn_opr.GetL, root);
    auto iter = vec.begin();
    auto end = vec.end();

    for (;;) {
        if (iter == end) {
            ZETA_DebugAssert(n == NULL);
            return;
        }

        ZETA_DebugAssert(n != NULL);

        Node* node = ZETA_MemberToStruct(Node, n, n);

        ZETA_DebugAssert(iter->linked_node == node);
        ZETA_DebugAssert(iter->size == Zeta_BinTree_GetSize(&btn_opr, n));

        ++iter;
        n = Zeta_BinTree_StepR(&btn_opr, n);
    }
}

void CompareRL() {
    void* n = Zeta_GetMostLink(NULL, btn_opr.GetR, root);
    auto iter = vec.rbegin();
    auto end = vec.rend();

    for (;;) {
        if (iter == end) {
            ZETA_DebugAssert(n == NULL);
            return;
        }

        ZETA_DebugAssert(n != NULL);

        Node* node = ZETA_MemberToStruct(Node, n, n);

        ZETA_DebugAssert(iter->linked_node == node);
        ZETA_DebugAssert(iter->size == Zeta_BinTree_GetSize(&btn_opr, n));

        ++iter;
        n = Zeta_BinTree_StepL(&btn_opr, n);
    }
}

void Check() {
    Zeta_RBTree_Check(&btn_opr, NULL, root);
    CompareLR();
    CompareRL();
}

// -----------------------------------------------------------------------------

size_t fallback_sign = 0x479237197577;

void Access(size_t idx) {
    void* target_n;
    size_t target_tail_idx;

    Zeta_BinTree_AccessL(&target_n, &target_tail_idx, &btn_opr, root, idx);

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
    Zeta_OrdCntRBTreeNode_Init(NULL, &new_node->n);
    Zeta_BinTree_SetSize(&btn_opr, &new_node->n, size);

    size_sum += size;

    if (idx < vec.size()) {
        Node* ins_node = vec[idx].linked_node;
        root = Zeta_RBTree_InsertL(&btn_opr, &ins_node->n, &new_node->n);
    } else if (vec.size() == 0) {
        root = &new_node->n;
    } else {
        Node* ins_node = vec.back().linked_node;
        root = Zeta_RBTree_InsertR(&btn_opr, &ins_node->n, &new_node->n);
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
    root = Zeta_RBTree_Extract(&btn_opr, &target_node->n);
    delete target_node;

    vec.erase(vec.begin() + idx);
}

// -----------------------------------------------------------------------------

void main1() {
    unsigned seed = time(NULL);

    ZETA_PrintVar(seed);

    std::mt19937_64 en{ seed };
    std::uniform_int_distribution<size_t> idx_generator{ 0, ZETA_RangeMaxOf(
                                                                size_t) };
    std::uniform_int_distribution<size_t> size_generator{ 0, 16 };

    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_sum = 0;
    root = NULL;

    for (int i = 0; i < 1024; ++i) {
        Insert(idx_generator(en) % (vec.size() + 1), size_generator(en));
        Check();
    }

    for (int _ = 0; _ < 16; ++_) {
        for (int i = 0, end = idx_generator(en) % 1024 + 4096; i < end; ++i) {
            Insert(idx_generator(en) % (vec.size() + 1), size_generator(en));
            Check();
        }

        for (int i = 0,
                 end = std::min(vec.size(), idx_generator(en) % 1024 + 4096);
             i < end; ++i) {
            Erase(idx_generator(en) % vec.size());
            Check();
        }

        for (int i = 0, end = vec.size() * 2; i < end; ++i) {
            int idx = idx_generator(en) % (size_sum * 2);
            Access(idx);
        }
    }
}

// -----------------------------------------------------------------------------

int main() {
    main1();
    printf("ok\n");
    return 0;
}
