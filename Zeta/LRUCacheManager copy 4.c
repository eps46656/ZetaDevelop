#include "LRUCacheManager.h"

#include "RelLinkedListNode.h"
#include "utils.h"

#define user_idx_level_branch_num (32)

#define blk_idx_level_branch_num (256)

#define ALPHA (2)

typedef int user_t;

/*

Each cache node corresponds to a cached section.

Each User has a reference list which contains many reference nodes.

Each refence node refers to an cache node means the User cached that section.


Reference List:
    Each user has his refence list which records the refenced sections and
    maintains the access temperature order.

Extented Refence List:
    Beside reference list, user's can has extended reference list to refer more
    sections if condition allow. This benifit is generated from different users
    refer to same section and save cache size.

Held:
    A section which is refered by any user in his reference list. This
    section will not be replaced. The section may be refered by other users in
    extended reference list.

Unheld:
    A section which is not refered by any user in his reference list. Is is
    only refered by users in extended reference list.

Unrefered:
    No user refers to the section. It will be immediately discarded and the
    cache frame will be deallocate to allocator.


| refernece list | extended refernece list |
|      held      |         unheld          |
|                 refered                  |


// The max of


Access process

User u request ith section

if ith section is already in user u's reference list:
    lift the reference node to the top of reference list
    return

if u's reference list reaches its extended max:
    derefer the last refence node of u's reference list

if ith section is already in user p' reference list:
    add a reference node at the top of u's reference list
    return

# ith section has not been cached

if the number of caches reaches its max:
    # decide to discard a cache

*/

typedef struct XNode XNode;
typedef struct UNode UNode;
typedef struct CNode CNode;

struct XNode {
    Zeta_RelLinkedListNode ul;
    Zeta_RelLinkedListNode cl;

    uintptr_t uh;
    uintptr_t ch;
};

struct UNode {
    user_t user_idx;

    Zeta_RelLinkedListNode cl_head;
    Zeta_RelLinkedListNode ext_cl_head;

    size_t cl_size;
    size_t ext_cl_size;

    size_t cache_num;
};

typedef struct CNode CNode;

struct CNode {
    size_t blk_idx;
    byte_t* data;

    XNode reder_head;

    size_t held_cnt;
};

static void* GetPtr_(uintptr_t* ptr) {
    return ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(ptr) + *ptr);
}

static void SetPtr_(uintptr_t* ptr, void* val) {
    *ptr = ZETA_PTR_TO_UINT(val) - ZETA_PTR_TO_UINT(ptr);
}

static XNode* GetXNodeFromUN_(Zeta_RelLinkedListNode* un) {
    return ZETA_GetStructFromMember(XNode, un, un);
}

static XNode* GetXNodeFromSN_(Zeta_RelLinkedListNode* sn) {
    return ZETA_GetStructFromMember(XNode, sn, sn);
}

static UNode* GetUNodeFromXNode_(XNode* x_node) {
    return ZETA_GetStructFromMember(UNode, x_node, x_node);
}

static SNode* GetSNodeFromXNode_(XNode* x_node) {
    return ZETA_GetStructFromMember(SNode, x_node, x_node);
}

void Zeta_LRUCacheManager_Init(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_BlockVector* blk_vec = lrucm->blk_vec;
    ZETA_DebugAssert(blk_vec != NULL);

    ZETA_DebugAssert(blk->GetBlockSize != NULL);

    size_t blk_num = blk->GetBlockNum(blk_vec->context);

    Zeta_MultiLevelVector* xn_map = &lrucm->xn_map;

    int level = 0;

    int user_idx_level = Zeta_GetLogCeil(ZETA_LRUCacheManager_user_num_max,
                                         user_idx_level_branch_num);

    int blk_idx_level = Zeta_GetLogCeil(blk_num, blk_idx_level_branch_num);

    ZETA_DebugAssert(user_idx_level + blk_idx_level <=
                     ZETA_MultiLevelVector_level_max);

    xn_map->level = user_idx_level + blk_idx_level;

    for (int i = 0; i < user_idx_level; ++i) {
        xn_map->branch_nums[i] = user_idx_level_branch_num;
    }

    for (int i = user_idx_level; i < user_idx_level + blk_idx_level; ++i) {
        xn_map->branch_nums[i] = blk_idx_level_branch_num;
    }

    Zeta_MultiLevelVector_Init(xn_map);
}

static void GetIdxes_(size_t* dst_idxes, Zeta_BlockVector* blk_vec,
                      user_t user_idx, size_t blk_idx) {
    size_t blk_num = blk_vec->GetBlockNum(blk_vec->context);

    int user_idx_level = Zeta_GetLogCeil(ZETA_LRUCacheManager_user_num_max,
                                         user_idx_level_branch_num);

    int blk_idx_level = Zeta_GetLogCeil(blk_num, blk_idx_level_branch_num);

    for (int i = 0; i < user_idx_level; ++i) {
        dst_idxes[user_idx_level - 1 - i] =
            user_idx % user_idx_level_branch_num;
        user_idx /= user_idx_level_branch_num;
    }

    for (int i = 0; i < blk_idx_level; ++i) {
        dst_idxes[blk_idx_level + user_idx_level - 1 - i] =
            blk_idx % blk_idx_level_branch_num;
        blk_idx /= blk_idx_level_branch_num;
    }
}

static XNode* GetXNode_(Zeta_MultiLevelVector* xn_map,
                        Zeta_BlockVector* blk_vec, user_t user_idx,
                        size_t blk_idx) {
    size_t idxes[ZETA_MultiLevelVector_level_max];
    GetIdxes_(idxes, blk_vec, user_idx, blk_idx);

    void** n = Zeta_MultiLevleVector_Access(xn_map, idxes);

    return n == 0 ? NULL : *n;
}

void Zeta_LRUCacheManager_Open(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);
}

void Zeta_LRUCacheManager_Close(void* lrucm_, void* ud_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    UserDesc* ud = ud_;
    ZETA_DebugAssert(ud != NULL);
}

static void Deref_(Zeta_LRUCacheManager* lrucm, XNode* xn) {
    ZETA_DebugAssert(lrucm != NULL);

    ZETA_DebugAssert(xn != NULL);
    ZETA_DebugAssert(xn->ul != NULL);
    ZETA_DebugAssert(xn != &xn->un->xn);
    ZETA_DebugAssert(xn->cl != NULL);
    ZETA_DebugAssert(xn != &xn->cn->xn);

    user_t user_idx = xn->un->user_idx;
    size_t blk_idx = xn->cn->blk_idx;

    size_t idxes[ZETA_MultiLevelVector_level_max];

    GetIdxes_(idxes, lrucm->blk_vec, user_idx, blk_idx);
    Zeta_MultiLevelVector_Erase(&lrucm->xn_map, idxes);

    Zeta_RelLinkedListNode_Extract(&xn->cl);

    void* xn_ul_prv = Zeta_RelLinkedListNode_GetL(&xn->ul);
    void* xn_ul_nxt = Zeta_RelLinkedListNode_GetR(&xn->ul);

    Zeta_RelLinkedListNode_Extract(&xn->ul);

    Zeta_SlabAllocator_Deallocate(&lrucm->xn_allocator, xn);

    if (xn_ul_prv != xn_ul_nxt) { return; }

    CNode* cn = ZETA_GetStructFromMember(CNode, xn->ul, xn_ul_prv);

    Zeta_RelLinkedListNode_Extract(&cn->xn->cl);

    Zeta_Allocator* allocator = lrucm->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void (*Deallocate)(void* context, void* ptr) = allocator->Deallocate;
    ZETA_DebugAssert(Deallocate != NULL);

    Deallocate(allocator->context, cn->data);

    Zeta_SlabAllocator_Deallocate(&lrucm->cn_allocator, cn);
}

static void DerefCache_(Zeta_LRUCacheManager* lrucm, CNode* cn) {
    Zeta_SlabAllocator* xn_allocator = &lrucm->xn_allocator;

    size_t idxes[ZETA_MultiLevelVector_level_max];

    for (;;) {
        XNode* xn = ZETA_GetStructFromMember(
            XNode, ul, Zeta_RelLinkedListNode_GetR(&cn->xn->ul));
        if (xn == &cn->xn) { break; }

        Zeta_RelLinkedListNode_Extract(&xn->ul);
        Zeta_RelLinkedListNode_Extract(&xn->cl);

        GetIdxes_(idxes, lrucm->blk_vec, xn->user_idx, ZETA_GetMaxOf(size_t));
        void** n = Zeta_MultiLevelVector_Access(&lrucm->xn_map, idxes);

        ZETA_DebugAssert(n != NULL);
        ZETA_DebugAssert(*n != NULL);

        --ZETA_GetStructFromMember(UNode, xn, *n)->cn_list_size;

        Zeta_SlabAllocator_Deallocate(xn_allocator->context, xn_allocator, xn);
    }
}

static void* Access_(Zeta_LRUCacheManager* lrucm, UNode* un, size_t blk_idx) {
    ZETA_DebugAssert(lrucm != NULL);
    ZETA_DebugAssert(un != NULL);

    Zeta_BlockVector* blk_vec = lrucm->blk_vec;
    ZETA_DebugAssert(blk_vec != NULL);

    ZETA_DebugAssert(blk_vec->GetBlockNum != NULL);

    size_t blk_num = blk_vec->GetBlockNum(blk_vec->context);

    ZETA_DebugAssert(blk_idx < blk_num);

    user_t user_idx = un->user_idx;

    size_t idxes[ZETA_MultiLevelVector_level_max];

    GetIdxes_(idxes, lrcum->blk_vec, ZETA_LRUCacheManager_user_num_max,
              blk_idx);
    void** n_all = Zeta_MultiLevelVector_Access(&lrucm->xn_map, idxes);

    if (n_all != NULL) {
        CNode* cn = *n_all;

        GetIdxes_(idxes, lrcum->blk_vec, user_idx, blk_idx);
        void** n = Zeta_MultiLevelVector_Access(&lrucm->xn_map, idxes);

        if (n != NULL) {
            XNode* xn = *n;

            ZETA_DebugAssert(xn != NULL);
            ZETA_DebugAssert(xn->user_idx == user_idx);
            ZETA_DebugAssert(xn->blk_idx == blk_idx);
            ZETA_DebugAssert(xn->data != NULL);

            Zeta_RelLinkedListNode_Extract(&xn->cl);
            Zeta_RelLinkedListNode_InsertR(&un->xn->cl, &xn->cl);

            return cn->data;
        }

        if (un->cache_num * ALPHA <= un->cl_list_size) {
            Deref_(lrucm, Zeta_RelLinkedListNode_GetL(&un->xn->cl));
        }

        XNode* xn =
            Zeta_SlabAllocator_Allocate(&lrucm->xn_allocator, sizeof(XNode));
        ZETA_DebugAssert(xn != NULL);

        Zeta_RelLinkedListNode_Init(&xn->cl);
        Zeta_RelLinkedListNode_Init(&xn->ul);

        Zeta_RelLinkedListNode_InsertR(&un->xn->cl, &xn->cl);
        Zeta_RelLinkedListNode_InsertR(&cn->xn->ul, &xn->ul);
    }

    n = Zeta_MultiLevelVector_Access(&lrucm->xn_map, idxes);

    if (n != NULL) {
        //
    }
}

static void ReduceNumOfCaches_(Zeta_LRUCacheManager* lrucm, UserDesc* ud,
                               size_t num_of_caches) {
    //
}

void Zeta_LRUCacheManager_SetNumOfCaches(void* lrucm_, void* ud_,
                                         size_t num_of_caches) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    UserDesc* ud = ud_;
    ZETA_DebugAssert(ud != NULL);

    if (ud->num_of_caches == num_of_caches) { return; }

    if (ud->num_of_caches < num_of_caches) {
        ud->num_of_caches = num_of_caches;
        return;
    }
}

void Zeta_LRUCacheManager_Read(void* lrucm_, void ud_, size_t block_idx) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    UserDesc* ud = ud_;
    ZETA_DebugAssert(ud != NULL);
}
