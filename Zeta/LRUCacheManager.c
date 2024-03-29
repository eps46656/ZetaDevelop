#include "LRUCacheManager.h"

#include "RBTree.h"
#include "RelLinkedListNode.h"
#include "RelRBLinkedListNode.h"
#include "utils.h"

#define user_level_branch_num (32)

#define blk_idx_level_branch_num (256)

#define ALPHA (2)

#define AL_COLOR (0)

#define EXT_AL_COLOR (1)

#define Clear (0)
#define Dirty (0)

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

void Zeta_LRUCacheManager_Init(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    ZETA_DebugAssert(lrucm->blk_vec != NULL);

    ZETA_DebugAssert(lrucm->un_allocator != NULL);
    ZETA_DebugAssert(lrucm->cn_allocator != NULL);
    ZETA_DebugAssert(lrucm->xn_allocator != NULL);
    ZETA_DebugAssert(lrucm->cache_allocator != NULL);

    lrucm->un_root = NULL;

    Zeta_RelRBLinkedListNode_Init(&lrucm->held_cl_head);
    Zeta_RelRBLinkedListNode_Init(&lrucm->unheld_cl_head);

    ZETA_DebugAssert(lrucm->blk_vec->GetBlockNum != NULL);
    size_t blk_num = lrucm->blk_vec->GetBlockNum(lrucm->blk_vec->context);

    lrucm->held_cl_cnt = 0;
    lrucm->unheld_cl_cnt = 0;
}

static void GetIdxes_(size_t* dst_idxes, Zeta_BlockVector* blk_vec,
                      size_t blk_idx) {
    size_t blk_num = blk_vec->GetBlockNum(blk_vec->context);

    int blk_idx_level = Zeta_GetLogCeil(blk_num, blk_idx_level_branch_num);

    for (int i = 0; i < blk_idx_level; ++i) {
        dst_idxes[blk_idx_level - 1 - i] = blk_idx % blk_idx_level_branch_num;
        blk_idx /= blk_idx_level_branch_num;
    }
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

static Zeta_LRUCacheManager_UNode* AllocateUNode_(Zeta_LRUCacheManager* lrucm) {
    ZETA_DebugAssert(lrucm->un_allocator != NULL);
    ZETA_DebugAssert(lrucm->un_allocator->Allocate != NULL);

    void* ret = lrucm->un_allocator->Allocate(
        allocator->context, sizeof(Zeta_LRUCacheManager_UNode));
    ZETA_DebugAssert(ret != NULL);

    return ret;
}

static void RotateFromHeldToUnheld_(Zeta_LRUCacheManager* lrucm,
                                    Zeta_LRUCacheManager_UNode* un) {
    Zeta_RelLinkedListNode* target_al =
        Zeta_RelLinkedListNode_GetL(&un->al_head);

    Zeta_RelLinkedListNode_Extract(target_al);
    --un->held_cnt;

    Zeta_RelLinkedListNode_InsertR(&un->ext_al_head, target_al);
    ++un->unheld_cnt;

    Zeta_LRUCacheManager_XNode* target_xn =
        ZETA_GetStructFromMember(Zeta_LRUCacheManager_XNode, al, target_al);

    SetPtr_(&target_xn->un,
            ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(&un) + EXT_AL_COLOR));

    Zeta_LRUCacheManager_CNode* target_cn = GetPtr_(&target_xn->cn);

    if (--target_cn->held_cnt == 0) {
        Zeta_RelRBLinkedListNode_Extract(&target_cn->cl);
        Zeta_RelRBLinkedListNode_InsertR(&lrucm->unheld_cl_head,
                                         &target_cn->cl);
        --lrucm->held_cl_cnt;
        ++lrucm->unheld_cl_cnt;
    }
}

static void RotateFromUnheldToHeld_(Zeta_LRUCacheManager* lrucm,
                                    Zeta_LRUCacheManager_UNode* un) {
    Zeta_RelLinkedListNode* target_al =
        Zeta_RelLinkedListNode_GetR(&un->ext_al_head);

    Zeta_RelLinkedListNode_Extract(target_al);
    --un->unheld_cnt;

    Zeta_RelLinkedListNode_InsertL(&un->al_head, target_al);
    ++un->held_cnt;

    Zeta_LRUCacheManager_XNode* target_xn =
        ZETA_GetStructFromMember(Zeta_LRUCacheManager_XNode, al, target_al);

    SetPtr_(&target_xn->un, ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(&un) + AL_COLOR));

    Zeta_LRUCacheManager_CNode* target_cn = GetPtr_(&target_xn->cn);

    if (++target_cn->held_cnt == 1) {
        Zeta_RelRBLinkedListNode_Extract(&target_cn->cl);
        Zeta_RelRBLinkedListNode_InsertR(&lrucm->held_cl_head, &target_cn->cl);
        ++lrucm->held_cl_cnt;
        --lrucm->unheld_cl_cnt;
    }
}

static void Lift_(Zeta_LRUCacheManager* lrucm, Zeta_LRUCacheManager_XNode* xn) {
    ZETA_DebugAssert(lrucm != NULL);
    ZETA_DebugAssert(xn != NULL);

    uintptr_t un_ = ZETA_PTR_TO_UINT(GetPtr_(&xn->un));
    bool_t is_ext = un_ % 2 == EXT_AL_COLOR;
    Zeta_LRUCacheManager_UNode* un = un_ / 2 * 2;

    Zeta_RelLinkedListNode_Extract(&xn->al);

    Zeta_RelLinkedListNode_InsertR(&un->al_head, &xn->al);

    if (!is_ext) { return; }

    Zeta_LRUCacheManager_CNode* cn = GetPtr_(&xn->cn);

    SetPtr_(&xn->un, ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(&un) + AL_COLOR));

    Zeta_LRUCacheManager_CNode* cn = GetPtr_(&xn->cn);

    if (++cn->held_cnt == 1) {
        Zeta_RelRBLinkedListNode_Extract(&cn->cl);
        Zeta_RelRBLinkedListNode_InsertR(&lrucm->held_cl_head, &cn->cl);
    }

    RotateFromHeldToUnheld_(un);
}

static Zeta_RelRBTreeNode* FindAT_(Zeta_RelRBTreeNode* at_root,
                                   size_t blk_idx) {
    Zeta_RelRBTreeNode* ret = NULL;

    for (Zeta_RelRBTreeNode* n = at_root; n != NULL;) {
        Zeta_LRUCacheManager_XNode* xn =
            ZETA_GetStructFromMember(Zeta_LRUCacheManager_XNode, at, n);

        Zeta_LRUCacheManager_CNode* cn = GetPtr_(&xn->cn);

        if (cn->blk_idx < blk_idx) {
            n = Zeta_RelRBTreeNode_GetR(NULL, n);
        } else {
            ret = n;
            n = Zeta_RelRBTreeNode_GetL(NULL, n);
        }
    }

    return ret;
}

static Zeta_LRUCacheManager_XNode* Ref_(Zeta_LRUCacheManager* lrucm,
                                        Zeta_LRUCacheManager_UNode* un,
                                        Zeta_LRUCacheManager_CNode* cn) {
    ZETA_DebugAssert(lrucm->xn_allocator != NULL);
    ZETA_DebugAssert(lrucm->xn_allocator->Allocate != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_RelRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_LRUCacheManager_UNode* xn = lrucm->xn_allocator->Allocate(
        lrucm->xn_allocator->context, sizeof(Zeta_LRUCacheManager_XNode));
    ZETA_DebugAssert(xn != NULL);

    Zeta_RelRBTreeNode_Init(&xn->at);

    Zeta_RelRBTreeNode* ins_n = FindAT_(un->at_root, cn->blk_idx);

    if (ins_n == NULL) {
        un->at_root = Zeta_RBTree_InsertR(
            &btn_opr, Zeta_GetMostLink(un->at_root, Zeta_RelRBTreeNode_GetR),
            &xn->at);
    } else {
        un->at_root = Zeta_RBTree_InsertL(&btn_opr, ins_n, &xn->at);
    }

    Zeta_RelLinkedListNode_Init(&xn->al);
    Zeta_RelLinkedListNode_InsertR(&un->al_head, &xn->al);

    Zeta_RelLinkedListNode_Init(&xn->bl);
    Zeta_RelLinkedListNode_InsertR(&cn->bl_head, &xn->bl);

    SetPtr_(&xn->un, ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(&un) + AL_COLOR));
    SetPtr_(&xn->cn, cn);

    ++un->al_num;

    if (++cn->held_cnt == 1) {
        Zeta_RelRBLinkedListNode_Extract(&cn->cl);
        Zeta_RelRBLinkedListNode_InsertR(&lrucm->held_cl_head, &cn->cl);
        ++lrucm->held_cl_head;
        --lrucm->unheld_cl_head;
    }

    if (un->cache_num <= un->al_num - 1) { RotateFromHeldToUnheld_(lrucm, un); }

    return xn;
}

static void Unref_(Zeta_LRUCacheManager* lrucm,
                   Zeta_LRUCacheManager_XNode* xn) {
    ZETA_DebugAssert(lrucm != NULL);
    ZETA_DebugAssert(xn != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_RelRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    uintptr_t un_ = ZETA_PTR_TO_UINT(GetPtr_(&xn->un));
    bool_t is_ext = un_ % 2 == EXT_AL_COLOR;
    Zeta_LRUCacheManager_UNode* un = un_ / 2 * 2;

    Zeta_LRUCacheManager_CNode* cn = GetPtr_(&xn->cn);

    un->at_root = Zeta_RBTree_Extract(&btn_opr, &xn->at);

    Zeta_RelLinkedListNode_Extract(&xn->al);

    --un->al_num;

    Zeta_RelLinkedListNode_Extract(&xn->bl);

    Zeta_SlabAllocator_Deallocate(&lrucm->xn_allocator, xn);

    if (!is_ext && --cn->held_cnt == 0) {
        Zeta_RelRBLinkedListNode_Extract(&cn->cl);
        Zeta_RelRBLinkedListNode_InsertR(&lrucm->unheld_cl_head, &cn->cl);
        --lrucm->held_cl_cnt;
        ++lrucm->unheld_cl_cnt;
    }

    if (!is_ext && un->cache_num <= un->al_num) {
        RotateFromUnheldToHeld_(lrucm, un);
    }

    if (&cn->bl_head != Zeta_RelLinkedList_GetL(&cn->bl_head)) { return; }

    Zeta_RBTree_Extract(&btn_opr, &cn->ct);

    Zeta_RelRBLinkedListNode_Extract(&cn->cl);
    --lrucm->unheld_cl_cnt;

    if (Zeta_RelRBLinkedListNode_GetLColor(&cn->cl) == Dirty) {
        Zeta_BlockVector* blk_vec = lrucm->blk_vec;
        ZETA_DebugAssert(blk_vec != NULL);

        void (*WriteBlock)(void* context, size_t blk_idx, byte_t const* src) =
            blk_vec->WriteBlock;
        ZETA_DebugAssert(WriteBlock != NULL);

        WriteBlock(blk_vec->context, cn->blk_idx, cn->data);
    }

    ZETA_DebugAssert(lrucm->cache_allocator != NULL);
    ZETA_DebugAssert(lrucm->cache_allocator->Deallocate != NULL);

    lrucm->cache_allocator->Deallocate(lrucm->cache_allocator->context,
                                       cn->data);

    ZETA_DebugAssert(lrucm->cn_allocator != NULL);
    ZETA_DebugAssert(lrucm->cn_allocator->Deallocate != NULL);

    lrucm->cn_allocator->Deallocate(lrucm->cn_allocator->context, cn);
}

static Zeta_RelRBTreeNode* FindCT_(Zeta_RelRBTreeNode* ct_root,
                                   size_t blk_idx) {
    Zeta_RelRBTreeNode* ret = NULL;

    for (Zeta_RelRBTreeNode* n = ct_root; n != NULL;) {
        Zeta_LRUCacheManager_XNode* cn =
            ZETA_GetStructFromMember(Zeta_LRUCacheManager_CNode, ct, n);

        if (cn->blk_idx < blk_idx) {
            n = Zeta_RelRBTreeNode_GetR(NULL, n);
        } else {
            ret = n;
            n = Zeta_RelRBTreeNode_GetL(NULL, n);
        }
    }

    return ret;
}

static Zeta_LRUCacheManager_CNode* Cache_(Zeta_LRUCacheManager* lrucm,
                                          size_t blk_idx) {
    ZETA_DebugAssert(lrucm->cn_allocator != NULL);
    ZETA_DebugAssert(lrucm->cn_allocator->Allocate != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_RelRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_LRUCacheManager_CNode* cn = lrucm->cn_allocator->Allocate(
        lrucm->cn_allocator->context, sizeof(Zeta_LRUCacheManager_CNode));
    ZETA_DebugAssert(cn != NULL);

    Zeta_RelRBTreeNode_Init(&cn->ct);

    Zeta_RelRBTreeNode* ins_ct = FindCT_(lrucm->ct_root, blk_idx);

    if (ins_ct == NULL) {
        lrucm->ct_root = Zeta_RBTree_InsertR(
            &btn_opr, Zeta_GetMostLink(lrucm->ct_root, Zeta_RelRBTreeNode_GetR),
            &xn->at);
    } else {
        lrucm->ct_root = Zeta_RBTree_InsertL(&btn_opr, ins_ct, &xn->at);
    }

    Zeta_RelRBLinkedListNode_Init(&cn->cl);
    Zeta_RelRBLinkedListNode_InsertL(&lrucm->unheld_cl_head, &cn->cl);

    Zeta_RelLinkedListNode_Init(&cn->bl_head);

    cn->blk_idx = blk_idx;

    ZETA_DebugAssert(lrucm->blk_vec != NULL);
    ZETA_DebugAssert(lrucm->blk_vec->GetBlockSize != NULL);

    size_t blk_size = lrucm->blk_vec->GetBlockSize(blk_vec->context);

    ZETA_DebugAssert(lrucm->cache_allocator != NULL);
    ZETA_DebugAssert(lrucm->cache_allocator->Allocate != NULL);

    cn->data = lrucm->cache_allocator->Allocate(allocator->context, blk_size);
    ZETA_DebugAssert(cn->data != NULL);

    cn->held_cnt = 0;

    return cn;
}

static void Uncache_(Zeta_LRUCacheManager* lrucm,
                     Zeta_LRUCacheManager_CNode* cn) {
    for (;;) {
        Zeta_RelLinkedListNode* last_bl =
            Zeta_RelLinkedListNode_GetL(&cn->bl_head);

        if (&cn->bl_head == last_bl) { break; }

        Unref_(lrucm, ZETA_GetStructFromMember(Zeta_LRUCacheManager_XNode, bl,
                                               last_bl));
    }
}

static Zeta_LRUCacheManager_XNode* UserCache_(Zeta_LRUCacheManager* lrucm,
                                              Zeta_LRUCacheManager_UNode* un,
                                              size_t blk_idx) {
    Zeta_BlockVector* blk_vec = lrucm->blk_vec;
    ZETA_DebugAssert(blk_vec != NULL);

    ZETA_DebugAssert(blk_vec->GetBlockNum != NULL);

    size_t blk_num = blk_vec->GetBlockNum(blk_vec->context);

    ZETA_DebugAssert(blk_idx < blk_num);

    Zeta_RelRBTreeNode* at = FindAT_(un->at_root, blk_idx);

    if (at != NULL) {
        Zeta_LRUCacheManager_XNode* xn =
            ZETA_GetStructFromMember(Zeta_LRUCacheManager_XNode, at, at);

        Lift_(lrucm, xn);

        return xn;
    }

    Zeta_RelRBTreeNode* ct = FindCT_(lrucm->ct_root, blk_idx);

    if (ct != NULL) {
        Zeta_LRUCacheManager_CNode* cn =
            ZETA_GetStructFromMember(Zeta_LRUCacheManager_CNode, ct, ct);

        if (un->cache_num * ALPHA <= un->al_num) {
            Unref_(lrucm, ZETA_GetStructFromMember(
                              Zeta_LRUCacheManager_XNode, al,
                              Zeta_RelLinkedListNode_GetL(&un->ext_al_head)));
        }

        Ref_(lrucm, un, cn);

        return xn;
    }

    if (lrucm->max_cache_sum <= lrucm->held_cl_cnt + lrucm->unheld_cl_cnt) {
        // discard a cache
    }

    Zeta_LRUCacheManager_CNode* cn = Cache_(lrucm, un, blk_idx);
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
