#include "LRUCacheManager.h"

ZETA_DeclareStruct(Zeta_LRUCacheManager_Node);

struct Zeta_LRUCacheManager_Node {
    Zeta_OrdLinkedListNode n;
    size_t key;
    void* frame;
};

static void GetKeyIdxes_(Zeta_LRUCacheManager* lrucm, size_t* idxes,
                         size_t key) {
    int level = lrucm->mlv.level;

    for (int level_i = level - 1; 0 <= level_i; --level_i) {
        size_t branch_num = lrucm->mlv.branch_nums[level_i];
        idxes[level_i] = key % branch_num;
        key /= branch_num;
    }
}

void* Zeta_LRUCacheManager_Find(void* lrucm_, size_t key) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    size_t key_idxes[ZETA_MultiLevelVector_max_level];
    GetKeyIdxes_(lrucm, key_idxes, key);

    void** node_p = Zeta_MultiLevelVector_Access(&lrucm->mlv, key_idxes);

    if (node_p == NULL) { return NULL; }

    Zeta_LRUCacheManager_Node* node = *node_p;

    Zeta_OrdLinkedListNode_Extract(&node->n);
    Zeta_OrdLinkedListNode_InsertR(&lrucm->n_list_head, &node->n);

    return node->frame;
}

void* Zeta_LRUCacheManager_Push(void* lrucm_, size_t key, void* frame) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_Allocator* allocator = lrucm->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void* allocator_context = allocator->context;

    void* (*Allocate)(void* context, size_t size) = allocator->Allocate;

    size_t key_idxes[ZETA_MultiLevelVector_max_level];
    GetKeyIdxes_(lrucm, key_idxes, key);

    void** node_p = Zeta_MultiLevelVector_Insert(&lrucm->mlv, key_idxes);
    Zeta_LRUCacheManager_Node* node = *node_p;

    void* old_frame;

    if (node == NULL) {
        old_frame = NULL;

        node = Allocate(allocator_context, sizeof(Zeta_LRUCacheManager_Node));
        *node_p = node;

        Zeta_OrdLinkedListNode_Init(&node->n);
        node->key = key;
        node->frame = frame;
    } else {
        old_frame = node->frame;
        node->frame = frame;

        Zeta_OrdLinkedListNode_Extract(&node->n);
    }

    Zeta_OrdLinkedListNode_InsertR(&lrucm->n_list_head, &node->n);

    return old_frame;
}

void* Zeta_LRUCacheManager_Pop(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_Allocator* allocator = lrucm->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void* allocator_context = allocator->context;

    void (*Deallocate)(void* context, void* ptr) = allocator->Deallocate;

    Zeta_OrdLinkedListNode* target_n =
        Zeta_OrdLinkedListNode_GetL(&lrucm->n_list_head);

    if (&lrucm->n_list_head == target_n) { return NULL; }

    Zeta_LRUCacheManager_Node* target_node =
        ZETA_GetStructFromMember(Zeta_LRUCacheManager_Node, n, target_n);

    Zeta_OrdLinkedListNode_Extract(target_n);

    size_t key_idxes[ZETA_MultiLevelVector_max_level];
    GetKeyIdxes_(lrucm, key_idxes, target_node->key);

    Zeta_MultiLevelVector_Erase(&lrucm->mlv, key_idxes);

    void* frame = target_node->frame;

    Deallocate(allocator_context, target_node);

    return frame;
}

void Zeta_LRUCacheManager_PrintState(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_OrdLinkedListNode* n = &lrucm->n_list_head;

    printf("{ ");

    for (;;) {
        n = Zeta_OrdLinkedListNode_GetR(n);
        if (n == &lrucm->n_list_head) { continue; }

        Zeta_LRUCacheManager_Node* node =
            ZETA_GetStructFromMember(Zeta_LRUCacheManager_Node, n, n);

        printf("(%llu, %p), ", node->key, node->frame);
    }

    printf(" }\n");
}
