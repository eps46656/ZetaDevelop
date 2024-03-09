#include "LRUCacheManager.h"

#include "utils.h"

typedef struct Zeta_LRUCacheManager_Node Zeta_LRUCacheManager_Node;

struct Zeta_LRUCacheManager_Node {
    Zeta_OrdLinkedListNode ln;
    Zeta_CacheManager_Node cn;
};

void Zeta_LRUCacheManager_Init(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_OrdLinkedListNode_Init(&lrucm->ln_list_head);
}

Zeta_LRUCacheManager_Node* FindWithKey_(Zeta_LRUCacheManager* lrucm,
                                        size_t key) {
    Zeta_OrdLinkedListNode* ln = &lrucm->ln_list_head;

    for (;;) {
        ln = Zeta_OrdLinkedListNode_GetR(ln);
        if (ln == &lrucm->ln_list_head) { return NULL; }

        Zeta_LRUCacheManager_Node* node =
            ZETA_GetStructFromMember(Zeta_LRUCacheManager_Node, ln, ln);

        if (node->cn.key == key) { return node; }
    }
}

Zeta_LRUCacheManager_Node* FindWithFrame_(Zeta_LRUCacheManager* lrucm,
                                          void* frame) {
    Zeta_OrdLinkedListNode* ln = &lrucm->ln_list_head;

    for (;;) {
        ln = Zeta_OrdLinkedListNode_GetR(ln);
        if (ln == &lrucm->ln_list_head) { return NULL; }

        Zeta_LRUCacheManager_Node* node =
            ZETA_GetStructFromMember(Zeta_LRUCacheManager_Node, ln, ln);

        if (node->cn.frame == frame) { return node; }
    }
}

Zeta_CacheManager_Node* Zeta_LRUCacheManager_Find(void* lrucm_, size_t key) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_LRUCacheManager_Node* node = FindWithKey_(lrucm, key);

    if (node == NULL) { return NULL; }

    Zeta_OrdLinkedListNode_Extract(&node->ln);
    Zeta_OrdLinkedListNode_InsertR(&lrucm->ln_list_head, &node->ln);

    return &node->cn;
}

Zeta_CacheManager_Node* Zeta_LRUCacheManager_Push(void* lrucm_, size_t key) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_Allocator* allocator = lrucm->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void* allocator_context = allocator->context;

    void* (*Allocate)(void* context, size_t size) = allocator->Allocate;

    Zeta_LRUCacheManager_Node* node = FindWithKey_(lrucm, key);

    if (node == NULL) {
        node = Allocate(allocator_context, sizeof(Zeta_LRUCacheManager_Node));

        Zeta_OrdLinkedListNode_Init(&node->ln);
        node->cn.key = key;
        node->cn.frame = NULL;
        node->cn.flags = 0;
    } else {
        Zeta_OrdLinkedListNode_Extract(&node->ln);
    }

    Zeta_OrdLinkedListNode_InsertR(&lrucm->ln_list_head, &node->ln);

    return &node->cn;
}

Zeta_CacheManager_Node Zeta_LRUCacheManager_Pop(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_Allocator* allocator = lrucm->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void* allocator_context = allocator->context;

    void (*Deallocate)(void* context, void* ptr) = allocator->Deallocate;

    Zeta_OrdLinkedListNode* target_ln =
        Zeta_OrdLinkedListNode_GetL(&lrucm->ln_list_head);

    if (&lrucm->ln_list_head == target_ln) {
        return (Zeta_CacheManager_Node){ 0, NULL, 0 };
    }

    Zeta_LRUCacheManager_Node* target_node =
        ZETA_GetStructFromMember(Zeta_LRUCacheManager_Node, ln, target_ln);

    Zeta_OrdLinkedListNode_Extract(target_ln);

    Zeta_CacheManager_Node ret;
    Zeta_MemCopy((byte_t*)(void*)&ret, (byte_t*)(void*)&target_node->cn,
                 sizeof(Zeta_CacheManager_Node));

    Deallocate(allocator_context, target_node);

    return ret;
}

Zeta_CacheManager_Node Zeta_LRUCacheManager_PopWithKey(void* lrucm_,
                                                       size_t key) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_Allocator* allocator = lrucm->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void* allocator_context = allocator->context;

    void (*Deallocate)(void* context, void* ptr) = allocator->Deallocate;

    Zeta_LRUCacheManager_Node* target_node = FindWithKey_(lrucm, key);

    if (target_node == NULL) { return (Zeta_CacheManager_Node){ 0, NULL, 0 }; }

    Zeta_OrdLinkedListNode_Extract(&target_node->ln);

    Zeta_CacheManager_Node ret;
    Zeta_MemCopy(&ret, &target_node->cn, sizeof(Zeta_CacheManager_Node));

    Deallocate(allocator_context, target_node);

    return ret;
}

Zeta_CacheManager_Node Zeta_LRUCacheManager_PopWithFrame(void* lrucm_,
                                                         void* frame) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_Allocator* allocator = lrucm->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void* allocator_context = allocator->context;

    void (*Deallocate)(void* context, void* ptr) = allocator->Deallocate;

    Zeta_LRUCacheManager_Node* target_node = FindWithFrame_(lrucm, frame);

    if (target_node == NULL) { return (Zeta_CacheManager_Node){ 0, NULL, 0 }; }

    Zeta_OrdLinkedListNode_Extract(&target_node->ln);

    Zeta_CacheManager_Node ret;
    Zeta_MemCopy(&ret, &target_node->cn, sizeof(Zeta_CacheManager_Node));

    Deallocate(allocator_context, target_node);

    return ret;
}

void Zeta_LRUCacheManager_ToCacheManager(void* lrucm_, Zeta_CacheManager* dst) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    dst->context = lrucm;
    dst->Find = Zeta_LRUCacheManager_Find;
    dst->Push = Zeta_LRUCacheManager_Push;
    dst->Pop = Zeta_LRUCacheManager_Pop;
    dst->PopWithKey = Zeta_LRUCacheManager_PopWithKey;
    dst->PopWithFrame = Zeta_LRUCacheManager_PopWithFrame;
}

void Zeta_LRUCacheManager_PrintState(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_OrdLinkedListNode* ln = &lrucm->ln_list_head;

    printf("{ ");

    for (;;) {
        ln = Zeta_OrdLinkedListNode_GetR(ln);
        if (ln == &lrucm->ln_list_head) { break; }

        Zeta_LRUCacheManager_Node* node =
            ZETA_GetStructFromMember(Zeta_LRUCacheManager_Node, ln, ln);

        printf("(%zu, %p), ", node->cn.key, node->cn.frame);
    }

    printf(" }\n");
}
