#include "LRUCacheManager.h"

#include "RelLinkedListNode.h"
#include "RelRBLinkedListNode.h"
#include "utils.h"

#define user_level_branch_num (32)

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

static void* GetPtr_(uintptr_t* ptr) {
    return ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(ptr) + *ptr);
}

static void SetPtr_(uintptr_t* ptr, void* val) {
    *ptr = ZETA_PTR_TO_UINT(val) - ZETA_PTR_TO_UINT(ptr);
}

static Zeta_LRUCacheManager_XNode* GetXNodeFromUN_(Zeta_RelLinkedListNode* un) {
    return ZETA_GetStructFromMember(Zeta_LRUCacheManager_XNode, un, un);
}

static Zeta_LRUCacheManager_XNode* GetXNodeFromSN_(Zeta_RelLinkedListNode* sn) {
    return ZETA_GetStructFromMember(Zeta_LRUCacheManager_XNode, sn, sn);
}

static Zeta_LRUCacheManager_UNode* GetUNodeFromXNode_(
    Zeta_LRUCacheManager_XNode* x_node) {
    return ZETA_GetStructFromMember(Zeta_LRUCacheManager_UNode, x_node, x_node);
}

static SNode* GetSNodeFromXNode_(Zeta_LRUCacheManager_XNode* x_node) {
    return ZETA_GetStructFromMember(SNode, x_node, x_node);
}

void Zeta_LRUCacheManager_Init(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_BlockVector* blk_vec = lrucm->blk_vec;
    ZETA_DebugAssert(blk_vec != NULL);

    ZETA_DebugAssert(blk->GetBlockSize != NULL);

    size_t blk_num = blk->GetBlockNum(blk_vec->context);

    Zeta_MultiLevelVector* n_map = &lrucm->n_map;

    int level = 0;

    int user_level = Zeta_GetLogCeil(ZETA_LRUCacheManager_user_num_max,
                                     user_level_branch_num);

    int blk_idx_level = Zeta_GetLogCeil(blk_num, blk_idx_level_branch_num);

    ZETA_DebugAssert(user_level + blk_idx_level <=
                     ZETA_MultiLevelVector_level_max);

    n_map->level = user_level + blk_idx_level;

    for (int i = 0; i < user_level; ++i) {
        n_map->branch_nums[i] = user_level_branch_num;
    }

    for (int i = user_level; i < user_level + blk_idx_level; ++i) {
        n_map->branch_nums[i] = blk_idx_level_branch_num;
    }

    Zeta_MultiLevelVector_Init(n_map);
}

static void GetIdxes_(size_t* dst_idxes, Zeta_BlockVector* blk_vec, user_t user,
                      size_t blk_idx) {
    size_t blk_num = blk_vec->GetBlockNum(blk_vec->context);

    int user_level = Zeta_GetLogCeil(ZETA_LRUCacheManager_user_num_max,
                                     user_level_branch_num);

    int blk_idx_level = Zeta_GetLogCeil(blk_num, blk_idx_level_branch_num);

    for (int i = 0; i < user_level; ++i) {
        dst_idxes[user_level - 1 - i] = user % user_level_branch_num;
        user /= user_level_branch_num;
    }

    for (int i = 0; i < blk_idx_level; ++i) {
        dst_idxes[blk_idx_level + user_level - 1 - i] =
            blk_idx % blk_idx_level_branch_num;
        blk_idx /= blk_idx_level_branch_num;
    }
}

static Zeta_LRUCacheManager_XNode* GetXNode_(Zeta_MultiLevelVector* n_map,
                                             Zeta_BlockVector* blk_vec,
                                             user_t user, size_t blk_idx) {
    size_t idxes[ZETA_MultiLevelVector_level_max];
    GetIdxes_(idxes, blk_vec, user, blk_idx);

    void** n = Zeta_MultiLevleVector_Access(n_map, idxes);

    return n == 0 ? NULL : *n;
}

void Zeta_LRUCacheManager_Open(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);
}

void Zeta_LRUCacheManager_Close(void* lrucm_, void* ud_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    void* ud = ud_;
    ZETA_DebugAssert(ud != NULL);
}

static void RotateFromHeldToUnheld_(Zeta_LRUCacheManager* lrucm,
                                    Zeta_LRUCacheManager_UNode* un) {
    Zeta_RelRBLinkedListNode* target_al =
        Zeta_RelRBLinkedListNode_GetL(&un->al_head);

    Zeta_RelRBLinkedListNode_Extract(target_al);
    --un->held_cnt;

    Zeta_RelRBLinkedListNode_InsertR(&un->ext_al_head, target_al);
    ++un->unheld_cnt;

    Zeta_LRUCacheManager_XNode* target_xn =
        ZETA_GetStructFromMember(Zeta_LRUCacheManager_XNode, al, target_al);

    SetPtr_(&target_xn->ah, &un->ext_al_head);

    Zeta_LRUCacheManager_CNode* target_cn = GetPtr_(&target_xn->cn);

    if (--target_cn->held_cnt == 0) {
        Zeta_RelRBLinkedListNode_Extract(&target_cn->cl);
        Zeta_RelRBLinkedListNode_InsertR(&lrucm->unheld_cl_head,
                                         &target_cn->cl);
        ++lrucm->unheld_cl_cnt;
    }
}

static void RotateFromUnheldToHeld_(Zeta_LRUCacheManager* lrucm,
                                    Zeta_LRUCacheManager_UNode* un) {
    Zeta_RelRBLinkedListNode* target_al =
        Zeta_RelRBLinkedListNode_GetR(&un->ext_al_head);

    Zeta_RelRBLinkedListNode_Extract(target_al);
    --un->unheld_cnt;

    Zeta_RelRBLinkedListNode_InsertL(&un->al_head, target_al);
    ++un->held_cnt;

    Zeta_LRUCacheManager_XNode* target_xn =
        ZETA_GetStructFromMember(Zeta_LRUCacheManager_XNode, al, target_al);

    SetPtr_(&target_xn->ah, &un->al_head);

    Zeta_LRUCacheManager_CNode* target_cn = GetPtr_(&target_xn->cn);

    if (++target_cn->held_cnt == 1) {
        Zeta_RelRBLinkedListNode_Extract(&target_cn->cl);
        Zeta_RelRBLinkedListNode_InsertR(&lrucm->held_cl_head, &target_cn->cl);
        ++lrucm->held_cl_cnt;
    }
}

static user_t FindUnusedUser_(Zeta_LRUCacheManager* lrucm) {
    int user_level = Zeta_GetLogCeil(ZETA_LRUCacheManager_user_num_max,
                                     user_level_branch_num);

    lrucm->n_map.level = user_level;

    size_t idxes[ZETA_MultiLevelVector_level_max];
    for (int i = 0; i < user_level; ++i) { idxes[i] = 0; }

    Zeta_MultiLevelVector_FindNextNotNull(&lrucm->n_map, idxes, TRUE);

    user_t user = 0;

    for (int i = 0; i < user_level; ++i) {
        user = user * user_level_branch_num + idxes[i];
    }

    return user;
}

static void Lift_(Zeta_LRUCacheManager* lrucm, Zeta_LRUCacheManager_XNode* xn) {
    ZETA_DebugAssert(lrucm != NULL);
    ZETA_DebugAssert(xn != NULL);

    Zeta_RelRBLinkedListNode* ah = GetPtr_(&xn->ah);

    bool_t is_ext = Zeta_RelRBLinkedListNode_GetLColor(ah) == 1;

    Zeta_LRUCacheManager_UNode* un =
        is_ext
            ? ZETA_GetStructFromMember(Zeta_LRUCacheManager_UNode, ext_al_head,
                                       ah)
            : ZETA_GetStructFromMember(Zeta_LRUCacheManager_UNode, al_head, ah);

    Zeta_LRUCacheManager_CNode* cn = GetPtr_(&xn->cn);

    user_t user = un->user;
    size_t blk_idx = cn->blk_idx;

    Zeta_RelRBLinkedListNode_Extract(&xn->al);

    Zeta_RelRBLinkedListNode_InsertR(&un->al_head, &xn->al);

    if (!is_ext) { return; }

    ++un->held_cnt;
    --un->unheld_cnt;

    SetPtr_(&xn->ah, &un->al_head);

    Zeta_LRUCacheManager_CNode* cn = GetPtr_(&xn->cn);

    if (++cn->held_cnt == 1) {
        Zeta_RelRBLinkedListNode_Extract(&cn->cl);
        Zeta_RelRBLinkedListNode_InsertR(&lrucm->held_cl_head, &cn->cl);
    }

    RotateFromHeldToUnheld_(un);
}

static void Ref_(Zeta_LRUCacheManager* lrucm, Zeta_LRUCacheManager_UNode* un,
                 Zeta_LRUCacheManager_CNode* cn) {
    Zeta_LRUCacheManager_XNode* xn = Zeta_SlabAllocator_Allocate(
        &lrucm->xn_allocator, sizeof(Zeta_LRUCacheManager_XNode));
    ZETA_DebugAssert(xn != NULL);

    size_t idxes[ZETA_MultiLevelVector_level_max];
    GetIdxes_(idxes, lrucm->blk_vec, un->user, cn->blk_idx);

    *Zeta_MultiLevelVector_Insert(&lrucm->n_map, idxes) = xn;

    Zeta_RelRBLinkedListNode_Init(&xn->al);
    Zeta_RelLinkedListNode_Init(&xn->bl);

    Zeta_RelRBLinkedListNode_InsertR(&xn->al_head, &xn->al);
    Zeta_RelLinkedListNode_InsertR(&cn->bl_head, &xn->bl);

    SetPtr_(&xn->ah, &un->al_head);
    SetPtr_(&xn->cn, cn);

    ++un->held_cnt;

    if (++cn->held_cnt == 1) {
        Zeta_RelRBLinkedListNode_Extract(&cn->cl);
        --lrucm->unheld_cl_head;

        Zeta_RelRBLinkedListNode_InsertR(&lrucm->held_cl_head, &cn->cl);
        ++lrucm->held_cl_head;
    }
}

static void Unref_(Zeta_LRUCacheManager* lrucm,
                   Zeta_LRUCacheManager_XNode* xn) {
    ZETA_DebugAssert(lrucm != NULL);
    ZETA_DebugAssert(xn != NULL);

    Zeta_RelRBLinkedListNode* ah = GetPtr_(&xn->ah);

    bool_t is_ext = Zeta_RelRBLinkedListNode_GetLColor(ah) == 1;

    Zeta_LRUCacheManager_UNode* un =
        is_ext
            ? ZETA_GetStructFromMember(Zeta_LRUCacheManager_UNode, ext_al_head,
                                       ah)
            : ZETA_GetStructFromMember(Zeta_LRUCacheManager_UNode, al_head, ah);

    Zeta_LRUCacheManager_CNode* cn = GetPtr_(&xn->cn);

    user_t user = un->user;
    size_t blk_idx = cn->blk_idx;

    size_t idxes[ZETA_MultiLevelVector_level_max];

    GetIdxes_(idxes, lrucm->blk_vec, user, blk_idx);
    Zeta_MultiLevelVector_Erase(&lrucm->n_map, idxes);

    Zeta_RelLinkedListNode_Extract(&xn->al);

    --un->al_num;

    if (!is_ext && un->cache_num <= un->al_num) {
        RotateFromUnheldToHeld_(lrucm, un);
    }

    void* xn_bl_prv = Zeta_RelLinkedListNode_GetL(&xn->bl);
    void* xn_bl_nxt = Zeta_RelLinkedListNode_GetR(&xn->bl);

    Zeta_RelLinkedListNode_Extract(&xn->bl);

    Zeta_SlabAllocator_Deallocate(&lrucm->xn_allocator, xn);

    if (xn_bl_prv == xn_bl_nxt) {
        Zeta_RelRBLinkedListNode_Extract(&cn->cl);

        if (cn->held_cnt == 0) {
            --lrucm->unheld_cl_cnt;
        } else {
            --lrucm->held_cl_cnt;
        }

        if (Zeta_RelRBLinkedListNode_GetLColor(&cn->cl) == 1) {
            Zeta_BlockVector* blk_vec = lrucm->blk_vec;
            ZETA_DebugAssert(blk_vec != NULL);

            void (*WriteBlock)(void* context, size_t blk_idx,
                               byte_t const* src) = blk_vec->WriteBlock;
            ZETA_DebugAssert(WriteBlock != NULL);

            WriteBlock(blk_vec->context, cn->blk_idx, cn->data);
        }

        Zeta_Allocator* allocator = lrucm->allocator;
        ZETA_DebugAssert(allocator != NULL);

        void (*Deallocate)(void* context, void* ptr) = allocator->Deallocate;
        ZETA_DebugAssert(Deallocate != NULL);

        allocator->Deallocate(allocator->context, cn->data);

        GetIdxes_(idxes, lrucm->blk_vec, ZETA_LRUCacheManager_user_num_max,
                  blk_idx);
        Zeta_MultiLevelVector_Erase(&lrucm->n_map, idxes);

        Zeta_SlabAllocator_Deallocate(&lrucm->cn_allocator, cn);

        return;
    }

    if (--cn->held_cnt == 0) {
        Zeta_RelRBLinkedListNode_Extract(&cn->cl);
        --lrucm->held_cl_cnt;

        Zeta_RelRBLinkedListNode_InsertR(&lrucm->unheld_cl_head, &cn->cl);
        ++lrucm->unheld_cl_cnt;
    }
}

static void Cache_(Zeta_LRUCacheManager* lrucm, Zeta_LRUCacheManager_UNode* un,
                   size_t blk_idx) {
    Zeta_LRUCacheManager_CNode* cn = Zeta_SlabAllocator_Allocate(
        &lrucm->cn_allocator, sizeof(Zeta_LRUCacheManager_CNode));
    ZETA_DebugAssert(cn != NULL);

    size_t idxes[ZETA_MultiLevelVector_level_max];
    GetIdxes_(idxes, lrucm->blk_vec, ZETA_LRUCacheManager_user_num_max,
              blk_idx);

    *Zeta_MultiLevelVector_Insert(&lrucm->n_map, idxes) = cn;

    Zeta_RelRBLinkedListNode_Init(&cn->cl);
    Zeta_RelLinkedListNode_Init(&cn->bl_head);

    cn->held_cnt = 0;

    cn->blk_idx = blk_idx;

    Zeta_BlockVector* blk_vec = lrucm->blk_vec;
    ZETA_DebugAssert(blk_vec != NULL);

    size_t (*GetBlockSize)(void* context) = blk_vec->GetBlockSize;
    ZETA_DebugAssert(GetBlockSize != NULL);

    size_t blk_size = GetBlockSize(blk_vec->context);

    Zeta_Allocator* allocator = lrucm->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void* (*Allocate)(void* context, size_t size) = allocator->Allocate;
    ZETA_DebugAssert(Allocate != NULL);

    cn->data = allocator->Allocate(allocator->context, blk_size);
    ZETA_DebugAssert(cn->data != NULL);

    Ref_(lrucm, un, cn);
}

static void Uncache_(Zeta_LRUCacheManager* lrucm,
                     Zeta_LRUCacheManager_CNode* cn) {
    for (Zeta_RelLinkedListNode* last_bl =
             Zeta_RelLinkedListNode_GetL(&cn->bl_head);
         ;) {
        Zeta_RelLinkedListNode* first_bl =
            Zeta_RelLinkedListNode_GetR(&cn->bl_head);

        Unref_(lrucm, ZETA_GetStructFromMember(Zeta_LRUCacheManager_XNode, bl,
                                               first_bl));

        if (first_bl == last_bl) { break; }
    }
}

static void* Access_(Zeta_LRUCacheManager* lrucm,
                     Zeta_LRUCacheManager_UNode* un, size_t blk_idx) {
    ZETA_DebugAssert(lrucm != NULL);
    ZETA_DebugAssert(un != NULL);

    Zeta_BlockVector* blk_vec = lrucm->blk_vec;
    ZETA_DebugAssert(blk_vec != NULL);

    ZETA_DebugAssert(blk_vec->GetBlockNum != NULL);

    size_t blk_num = blk_vec->GetBlockNum(blk_vec->context);

    ZETA_DebugAssert(blk_idx < blk_num);

    user_t user = un->user;

    size_t idxes[ZETA_MultiLevelVector_level_max];

    GetIdxes_(idxes, lrcum->blk_vec, ZETA_LRUCacheManager_user_num_max,
              blk_idx);
    void** cn_ = Zeta_MultiLevelVector_Access(&lrucm->n_map, idxes);

    if (cn_ != NULL) {
        Zeta_LRUCacheManager_CNode* cn = *cn_;

        GetIdxes_(idxes, lrcum->blk_vec, user, blk_idx);
        void** xn_ = Zeta_MultiLevelVector_Access(&lrucm->n_map, idxes);

        if (xn_ != NULL) {
            Zeta_LRUCacheManager_XNode* xn = *xn_;
            Lift_(lrucm, xn);
            return cn->data;
        }

        if (un->cache_num * ALPHA <= un->al_num) {
            Unref_(lrucm, ZETA_GetStructFromMember(
                              Zeta_LRUCacheManager_XNode, al,
                              Zeta_RelRBLinkedListNode_GetL(&un->ext_al_head)));
        }

        Ref_(lrucm, un, cn);
    }

    n = Zeta_MultiLevelVector_Access(&lrucm->n_map, idxes);

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
