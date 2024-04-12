#include <random>
#include <unordered_map>
#include <unordered_set>

#include "../Zeta/DummyBlockVector.h"
#include "../Zeta/DummyCacheManager.h"
#include "../Zeta/LRUCacheManager.h"
#include "../Zeta/RBTree.h"
#include "StdAllocator.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

std::mt19937_64 en;
std::uniform_int_distribution<size_t> size_generator{ 16, 2 * 1024 };
std::uniform_int_distribution<size_t> idx_generator{ 0,
                                                     ZETA_GetRangeMax(size_t) };

Zeta_DummyBlockVector dummy_blk_vec_a;
Zeta_DummyBlockVector dummy_blk_vec_b;

Zeta_BlockVector blk_vec_a;
Zeta_BlockVector blk_vec_b;

StdAllocator u_node_allocator_;
Zeta_Allocator u_node_allocator;

StdAllocator c_node_allocator_;
Zeta_Allocator c_node_allocator;

StdAllocator x_node_allocator_;
Zeta_Allocator x_node_allocator;

StdAllocator frame_allocator_;
Zeta_Allocator frame_allocator;

Zeta_LRUCacheManager lrucm;
Zeta_DummyCacheManager dcm;

std::unordered_set<void*> recorded_u_nodes;

#define BLOCK_SIZE (512)
#define BLOCK_NUM (64)

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

bool IsEqual(byte_t const* data_a, byte_t const* data_b, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (data_a[i] != data_b[i]) { return false; }
    }

    return true;
}

void ZeroFill(byte_t* data, size_t size) {
    for (size_t i = 0; i < size; ++i) { data[i] = 0; }
}

void RandomFill(byte_t* data, size_t size) {
    for (size_t i = 0; i < size; ++i) { data[i] = size_generator(en) % 256; }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void DummyBlockVector_Init() {
    dummy_blk_vec_a.blk_size = BLOCK_SIZE;
    dummy_blk_vec_b.blk_size = BLOCK_SIZE;

    dummy_blk_vec_a.blk_num = BLOCK_NUM;
    dummy_blk_vec_b.blk_num = BLOCK_NUM;

    size_t data_size = BLOCK_SIZE * BLOCK_NUM;

    dummy_blk_vec_a.data = (byte_t*)std::malloc(sizeof(byte_t) * data_size);
    dummy_blk_vec_b.data = (byte_t*)std::malloc(sizeof(byte_t) * data_size);

    ZeroFill(dummy_blk_vec_a.data, data_size);
    ZeroFill(dummy_blk_vec_b.data, data_size);

    Zeta_DummyBlockVector_Init(&dummy_blk_vec_a);
    Zeta_DummyBlockVector_Init(&dummy_blk_vec_b);

    Zeta_BlockVector_Init(&blk_vec_a);
    Zeta_BlockVector_Init(&blk_vec_b);

    Zeta_DummyBlockVector_DeployBlockVector(&dummy_blk_vec_a, &blk_vec_a);
    Zeta_DummyBlockVector_DeployBlockVector(&dummy_blk_vec_b, &blk_vec_b);
}

void LRUCM_Init() {
    Zeta_Allocator_Init(&u_node_allocator);
    Zeta_Allocator_Init(&c_node_allocator);
    Zeta_Allocator_Init(&x_node_allocator);
    Zeta_Allocator_Init(&frame_allocator);

    StdAllocator_DeployAllocator(&u_node_allocator_, &u_node_allocator);
    StdAllocator_DeployAllocator(&c_node_allocator_, &c_node_allocator);
    StdAllocator_DeployAllocator(&x_node_allocator_, &x_node_allocator);
    StdAllocator_DeployAllocator(&frame_allocator_, &frame_allocator);

    lrucm.blk_vec = &blk_vec_a;
    lrucm.u_node_allocator = &u_node_allocator;
    lrucm.c_node_allocator = &c_node_allocator;
    lrucm.x_node_allocator = &x_node_allocator;
    lrucm.frame_allocator = &frame_allocator;

    Zeta_LRUCacheManager_Init(&lrucm);
}

void DummyCM_Init() {
    dcm.blk_vec = &blk_vec_b;

    dcm.buffer = (byte_t*)std::malloc(sizeof(byte_t) * BLOCK_SIZE);

    Zeta_DummyCacheManager_Init(&dcm);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void* LRUCM_LogIn(size_t max_caches_num) {
    void* u_node = Zeta_LRUCacheManager_LogIn(&lrucm, max_caches_num);
    recorded_u_nodes.insert(u_node);
    return u_node;
}

void LRUCM_LogOut(void* u_node) {
    ZETA_DebugAssert(0 < recorded_u_nodes.erase(u_node));
    Zeta_LRUCacheManager_LogOut(&lrucm, u_node);
}

void* DummyCM_LogIn(size_t max_caches_num) {
    return Zeta_DummyCacheManager_LogIn(&dcm, max_caches_num);
}

void DummyCM_LogOut(void* u_node) {
    Zeta_DummyCacheManager_LogOut(&dcm, u_node);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void LRUCM_Check() {
    Zeta_BinTreeNodeOperator rbtn_opr;
    Zeta_BinTree_InitOpr(&rbtn_opr);
    Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(NULL, &rbtn_opr);

    std::unordered_set<Zeta_LRUCacheManager_UNode*> u_nodes;
    std::unordered_set<Zeta_LRUCacheManager_CNode*> c_nodes;
    std::unordered_set<Zeta_LRUCacheManager_XNode*> x_nodes;
    std::unordered_set<size_t> blk_idxes;

    std::unordered_map<Zeta_LRUCacheManager_UNode*,
                       std::unordered_set<Zeta_LRUCacheManager_XNode*>>
        u_node_to_x_nodes;

    std::unordered_map<Zeta_LRUCacheManager_UNode*,
                       std::unordered_set<Zeta_LRUCacheManager_CNode*>>
        u_node_to_c_nodes;

    std::unordered_map<Zeta_LRUCacheManager_UNode*, std::unordered_set<size_t>>
        u_node_to_blk_idxes;

    std::unordered_set<Zeta_LRUCacheManager_CNode*> refered_c_nodes;
    std::unordered_set<Zeta_LRUCacheManager_CNode*> unrefered_c_nodes;

    // check al
    for (auto u_iter{ recorded_u_nodes.begin() },
         u_end{ recorded_u_nodes.end() };
         u_iter != u_end; ++u_iter) {
        Zeta_LRUCacheManager_UNode* u_node =
            (Zeta_LRUCacheManager_UNode*)(*u_iter);

        ZETA_DebugAssert(u_nodes.insert(u_node).second);

        for (void* al_node = &u_node->al_head;;) {
            al_node = Zeta_OrdLinkedListNode_GetR(al_node);
            if (&u_node->al_head == al_node) { break; }

            Zeta_LRUCacheManager_XNode* x_node = ZETA_GetStructFromMember(
                Zeta_LRUCacheManager_XNode, al_node, al_node);

            ZETA_DebugAssert(x_node->c_node->refered);

            refered_c_nodes.insert(x_node->c_node);

            ZETA_DebugAssert(x_nodes.insert(x_node).second);

            ZETA_DebugAssert(x_node->u_node == u_node);

            ZETA_DebugAssert(u_node_to_x_nodes[u_node].insert(x_node).second);

            ZETA_DebugAssert(
                u_node_to_c_nodes[u_node].insert(x_node->c_node).second);

            ZETA_DebugAssert(u_node_to_blk_idxes[u_node]
                                 .insert(x_node->c_node->blk_idx)
                                 .second);
        }
    }

    std::unordered_map<Zeta_LRUCacheManager_CNode*,
                       std::unordered_set<Zeta_LRUCacheManager_XNode*>>
        c_node_to_x_nodes;

    std::unordered_map<Zeta_LRUCacheManager_CNode*,
                       std::unordered_set<Zeta_LRUCacheManager_UNode*>>
        c_node_to_u_nodes;

    // check ct
    {
        Zeta_DebugPipe ns;
        Zeta_DebugPipe_Create(&ns);

        Zeta_RBTree_Check(&rbtn_opr, &ns, lrucm.ct_root);

        size_t blk_idx_lb{ 0 };

        while (0 < Zeta_DebugPipe_GetSize(&ns)) {
            Zeta_OrdRBTreeNode* ct_node =
                (Zeta_OrdRBTreeNode*)ZETA_GetPtrFromAddr(
                    Zeta_DebugPipe_Pop(&ns));

            Zeta_LRUCacheManager_CNode* c_node = ZETA_GetStructFromMember(
                Zeta_LRUCacheManager_CNode, ct_node, ct_node);

            ZETA_DebugAssert(c_nodes.insert(c_node).second);
            ZETA_DebugAssert(blk_idxes.insert(c_node->blk_idx).second);

            ZETA_DebugAssert(blk_idx_lb <= c_node->blk_idx);
            blk_idx_lb = c_node->blk_idx + 1;

            if (c_node->refered) {
                ZETA_DebugAssert(0 < refered_c_nodes.count(c_node));
            } else {
                ZETA_DebugAssert(refered_c_nodes.count(c_node) == 0);
                ZETA_DebugAssert(unrefered_c_nodes.insert(c_node).second);
            }

            if (!c_node->refered) { continue; }

            for (void* bl_node = &c_node->bl_head;;) {
                bl_node = Zeta_OrdLinkedListNode_GetR(bl_node);
                if (&c_node->bl_head == bl_node) { break; }

                Zeta_LRUCacheManager_XNode* x_node = ZETA_GetStructFromMember(
                    Zeta_LRUCacheManager_XNode, bl_node, bl_node);

                ZETA_DebugAssert(x_node->c_node == c_node);

                ZETA_DebugAssert(
                    c_node_to_x_nodes[c_node].insert(x_node).second);

                ZETA_DebugAssert(
                    c_node_to_u_nodes[c_node].insert(x_node->u_node).second);
            }
        }

        Zeta_DebugPipe_Destroy(&ns);
    }

    // check from u to c
    for (auto u_iter{ u_node_to_x_nodes.begin() },
         u_end{ u_node_to_x_nodes.end() };
         u_iter != u_end; ++u_iter) {
        Zeta_LRUCacheManager_UNode* u_node = u_iter->first;

        for (auto x_iter{ u_iter->second.begin() },
             x_end{ u_iter->second.end() };
             x_iter != x_end; ++x_iter) {
            Zeta_LRUCacheManager_XNode* x_node = *x_iter;

            Zeta_LRUCacheManager_CNode* c_node = x_node->c_node;

            ZETA_DebugAssert(0 < c_node_to_x_nodes[c_node].count(x_node));
            ZETA_DebugAssert(0 < c_node_to_u_nodes[c_node].count(u_node));
        }
    }

    // check from c to u
    for (auto c_iter{ c_node_to_x_nodes.begin() },
         c_end{ c_node_to_x_nodes.end() };
         c_iter != c_end; ++c_iter) {
        Zeta_LRUCacheManager_CNode* c_node = c_iter->first;

        for (auto x_iter{ c_iter->second.begin() },
             x_end{ c_iter->second.end() };
             x_iter != x_end; ++x_iter) {
            Zeta_LRUCacheManager_XNode* x_node = *x_iter;

            Zeta_LRUCacheManager_UNode* u_node = x_node->u_node;

            ZETA_DebugAssert(0 < u_node_to_x_nodes[u_node].count(x_node));

            ZETA_DebugAssert(0 < u_node_to_c_nodes[u_node].count(c_node));

            ZETA_DebugAssert(
                0 < u_node_to_blk_idxes[u_node].count(c_node->blk_idx));
        }
    }

    // check cl
    for (void* cl_node = &lrucm.cl_head;;) {
        cl_node = Zeta_OrdLinkedListNode_GetR(cl_node);
        if (&lrucm.cl_head == cl_node) { break; }

        Zeta_LRUCacheManager_CNode* c_node = ZETA_GetStructFromMember(
            Zeta_LRUCacheManager_CNode, cl_node, cl_node);

        ZETA_DebugAssert(!c_node->refered);
        ZETA_DebugAssert(0 < unrefered_c_nodes.count(c_node));
    }

    // check at
    for (auto u_iter{ recorded_u_nodes.begin() },
         u_end{ recorded_u_nodes.end() };
         u_iter != u_end; ++u_iter) {
        Zeta_LRUCacheManager_UNode* u_node =
            (Zeta_LRUCacheManager_UNode*)(*u_iter);

        Zeta_DebugPipe ns;
        Zeta_DebugPipe_Create(&ns);

        Zeta_RBTree_Check(&rbtn_opr, &ns, u_node->at_root);

        std::unordered_set<Zeta_LRUCacheManager_XNode*> cur_x_nodes;

        size_t blk_idx_lb{ 0 };

        while (0 < Zeta_DebugPipe_GetSize(&ns)) {
            Zeta_OrdRBTreeNode* at_node =
                (Zeta_OrdRBTreeNode*)ZETA_GetPtrFromAddr(
                    Zeta_DebugPipe_Pop(&ns));

            Zeta_LRUCacheManager_XNode* x_node = ZETA_GetStructFromMember(
                Zeta_LRUCacheManager_XNode, at_node, at_node);

            ZETA_DebugAssert(cur_x_nodes.insert(x_node).second);

            Zeta_LRUCacheManager_CNode* c_node = x_node->c_node;

            ZETA_DebugAssert(blk_idx_lb <= c_node->blk_idx);
            blk_idx_lb = c_node->blk_idx + 1;
        }

        ZETA_DebugAssert(u_node_to_x_nodes[u_node] == cur_x_nodes);

        Zeta_DebugPipe_Destroy(&ns);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void main1() {
    time_t seed = time(NULL);
    // time_t seed = 1712858944;

    ZETA_PrintVar(seed);

    en.seed(seed);

    DummyBlockVector_Init();

    LRUCM_Init();
    DummyCM_Init();

    LRUCM_Check();

    size_t blk_num = lrucm.blk_vec->GetBlockNum(lrucm.blk_vec->context);
    ZETA_PrintVar(blk_num);

    size_t blk_size = lrucm.blk_vec->GetBlockSize(lrucm.blk_vec->context);
    ZETA_PrintVar(blk_size);

    void* a_u_node = LRUCM_LogIn(16);
    void* b_u_node = DummyCM_LogIn(16);

    byte_t buffer[BLOCK_SIZE];

    for (int op_i{ 0 }; op_i < 100; ++op_i) {
        int type = size_generator(en) % 2;
        size_t blk_idx = size_generator(en) % BLOCK_NUM;

        if (type == 0) {
            std::cout << "read  " << blk_idx << "\n";

            byte_t const* ra =
                Zeta_LRUCacheManager_ReadBlock(&lrucm, a_u_node, blk_idx);

            byte_t const* rb =
                Zeta_DummyCacheManager_ReadBlock(&dcm, b_u_node, blk_idx);

            ZETA_DebugAssert(IsEqual(ra, rb, BLOCK_SIZE));
        } else {
            std::cout << "write " << blk_idx << "\n";

            RandomFill(buffer, BLOCK_SIZE);

            Zeta_LRUCacheManager_WriteBlock(&lrucm, a_u_node, blk_idx, buffer);
            Zeta_DummyCacheManager_WriteBlock(&dcm, b_u_node, blk_idx, buffer);
        }

        LRUCM_Check();
    }

    LRUCM_LogOut(a_u_node);
    DummyCM_LogOut(b_u_node);

    Zeta_LRUCacheManager_FlushAll(&lrucm);
    Zeta_DummyCacheManager_FlushAll(&dcm);

    ZETA_PrintVar(dummy_blk_vec_a.read_cnt);
    ZETA_PrintVar(dummy_blk_vec_a.write_cnt);

    ZETA_PrintVar(dummy_blk_vec_b.read_cnt);
    ZETA_PrintVar(dummy_blk_vec_b.write_cnt);

    for (size_t i{ 0 }, end{ BLOCK_SIZE * BLOCK_NUM }; i < end; ++i) {
        ZETA_DebugAssert(dummy_blk_vec_a.data[i] == dummy_blk_vec_b.data[i]);
    }
}

int main() {
    main1();
    std::cout << "ok\n";
    return 1;
}
