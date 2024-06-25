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
                                                     ZETA_RangeMaxOf(size_t) };

Zeta_DummyBlockVector dummy_blk_vec_a;
Zeta_DummyBlockVector dummy_blk_vec_b;

Zeta_BlockVector blk_vec_a;
Zeta_BlockVector blk_vec_b;

StdAllocator s_node_allocator_;
Zeta_Allocator s_node_allocator;

StdAllocator c_node_allocator_;
Zeta_Allocator c_node_allocator;

StdAllocator x_node_allocator_;
Zeta_Allocator x_node_allocator;

StdAllocator frame_allocator_;
Zeta_Allocator frame_allocator;

Zeta_LRUCacheManager lrucm;
Zeta_DummyCacheManager dcm;

std::unordered_set<void*> recorded_s_nodes;

#define BLOCK_SIZE (512)
#define BLOCK_NUM (64)

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

bool_t IsEqual(byte_t const* data_a, byte_t const* data_b, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (data_a[i] != data_b[i]) { return FALSE; }
    }

    return TRUE;
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
    Zeta_Allocator_Init(&s_node_allocator);
    Zeta_Allocator_Init(&c_node_allocator);
    Zeta_Allocator_Init(&x_node_allocator);
    Zeta_Allocator_Init(&frame_allocator);

    StdAllocator_DeployAllocator(&s_node_allocator_, &s_node_allocator);
    StdAllocator_DeployAllocator(&c_node_allocator_, &c_node_allocator);
    StdAllocator_DeployAllocator(&x_node_allocator_, &x_node_allocator);
    StdAllocator_DeployAllocator(&frame_allocator_, &frame_allocator);

    lrucm.blk_vec = &blk_vec_a;
    lrucm.s_node_allocator = &s_node_allocator;
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
    void* s_node = Zeta_LRUCacheManager_Open(&lrucm, max_caches_num);
    recorded_s_nodes.insert(s_node);
    return s_node;
}

void LRUCM_LogOut(void* s_node) {
    ZETA_DebugAssert(0 < recorded_s_nodes.erase(s_node));
    Zeta_LRUCacheManager_Close(&lrucm, s_node);
}

void* DummyCM_LogIn(size_t max_caches_num) {
    return Zeta_DummyCacheManager_Open(&dcm, max_caches_num);
}

void DummyCM_LogOut(void* s_node) {
    Zeta_DummyCacheManager_Close(&dcm, s_node);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void LRUCM_Check() {
    Zeta_BinTreeNodeOperator rbtn_opr;
    Zeta_BinTree_InitOpr(&rbtn_opr);
    Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(NULL, &rbtn_opr);

    std::unordered_set<Zeta_LRUCacheManager_SNode*> s_nodes;
    std::unordered_set<Zeta_LRUCacheManager_CNode*> c_nodes;
    std::unordered_set<Zeta_LRUCacheManager_XNode*> x_nodes;
    std::unordered_set<size_t> blk_idxes;

    std::unordered_map<Zeta_LRUCacheManager_SNode*,
                       std::unordered_set<Zeta_LRUCacheManager_XNode*>>
        s_node_to_x_nodes;

    std::unordered_map<Zeta_LRUCacheManager_SNode*,
                       std::unordered_set<Zeta_LRUCacheManager_CNode*>>
        s_node_to_c_nodes;

    std::unordered_map<Zeta_LRUCacheManager_SNode*, std::unordered_set<size_t>>
        s_node_to_blk_idxes;

    std::unordered_set<Zeta_LRUCacheManager_CNode*> refered_c_nodes;
    std::unordered_set<Zeta_LRUCacheManager_CNode*> unrefered_c_nodes;

    // check al
    for (auto u_iter{ recorded_s_nodes.begin() },
         u_end{ recorded_s_nodes.end() };
         u_iter != u_end; ++u_iter) {
        Zeta_LRUCacheManager_SNode* s_node =
            (Zeta_LRUCacheManager_SNode*)(*u_iter);

        ZETA_DebugAssert(s_nodes.insert(s_node).second);

        for (void* al_node = &s_node->al_head;;) {
            al_node = Zeta_OrdLinkedListNode_GetR(al_node);
            if (&s_node->al_head == al_node) { break; }

            Zeta_LRUCacheManager_XNode* x_node = ZETA_MemberToStruct(
                Zeta_LRUCacheManager_XNode, al_node, al_node);

            ZETA_DebugAssert(x_node->c_node->refered);

            refered_c_nodes.insert(x_node->c_node);

            ZETA_DebugAssert(x_nodes.insert(x_node).second);

            ZETA_DebugAssert(x_node->s_node == s_node);

            ZETA_DebugAssert(s_node_to_x_nodes[s_node].insert(x_node).second);

            ZETA_DebugAssert(
                s_node_to_c_nodes[s_node].insert(x_node->c_node).second);

            ZETA_DebugAssert(s_node_to_blk_idxes[s_node]
                                 .insert(x_node->c_node->blk_idx)
                                 .second);
        }
    }

    std::unordered_map<Zeta_LRUCacheManager_CNode*,
                       std::unordered_set<Zeta_LRUCacheManager_XNode*>>
        c_node_to_x_nodes;

    std::unordered_map<Zeta_LRUCacheManager_CNode*,
                       std::unordered_set<Zeta_LRUCacheManager_SNode*>>
        c_node_to_s_nodes;

    // check ct
    {
        Zeta_DebugPipe ns;
        Zeta_DebugPipe_Create(&ns);

        Zeta_RBTree_Check(&rbtn_opr, &ns, lrucm.ct_root);

        size_t blk_idx_lb{ 0 };

        while (0 < Zeta_DebugPipe_GetSize(&ns)) {
            Zeta_OrdRBTreeNode* ct_node =
                (Zeta_OrdRBTreeNode*)ZETA_AddrToPtr(Zeta_DebugPipe_Pop(&ns));

            Zeta_LRUCacheManager_CNode* c_node = ZETA_MemberToStruct(
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

                Zeta_LRUCacheManager_XNode* x_node = ZETA_MemberToStruct(
                    Zeta_LRUCacheManager_XNode, bl_node, bl_node);

                ZETA_DebugAssert(x_node->c_node == c_node);

                ZETA_DebugAssert(
                    c_node_to_x_nodes[c_node].insert(x_node).second);

                ZETA_DebugAssert(
                    c_node_to_s_nodes[c_node].insert(x_node->s_node).second);
            }
        }

        Zeta_DebugPipe_Destroy(&ns);
    }

    // check from u to c
    for (auto u_iter{ s_node_to_x_nodes.begin() },
         u_end{ s_node_to_x_nodes.end() };
         u_iter != u_end; ++u_iter) {
        Zeta_LRUCacheManager_SNode* s_node = u_iter->first;

        for (auto x_iter{ u_iter->second.begin() },
             x_end{ u_iter->second.end() };
             x_iter != x_end; ++x_iter) {
            Zeta_LRUCacheManager_XNode* x_node = *x_iter;

            Zeta_LRUCacheManager_CNode* c_node = x_node->c_node;

            ZETA_DebugAssert(0 < c_node_to_x_nodes[c_node].count(x_node));
            ZETA_DebugAssert(0 < c_node_to_s_nodes[c_node].count(s_node));
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

            Zeta_LRUCacheManager_SNode* s_node = x_node->s_node;

            ZETA_DebugAssert(0 < s_node_to_x_nodes[s_node].count(x_node));

            ZETA_DebugAssert(0 < s_node_to_c_nodes[s_node].count(c_node));

            ZETA_DebugAssert(
                0 < s_node_to_blk_idxes[s_node].count(c_node->blk_idx));
        }
    }

    // check cl
    for (void* cl_node = &lrucm.cl_head;;) {
        cl_node = Zeta_OrdLinkedListNode_GetR(cl_node);
        if (&lrucm.cl_head == cl_node) { break; }

        Zeta_LRUCacheManager_CNode* c_node =
            ZETA_MemberToStruct(Zeta_LRUCacheManager_CNode, cl_node, cl_node);

        ZETA_DebugAssert(!c_node->refered);
        ZETA_DebugAssert(0 < unrefered_c_nodes.count(c_node));
    }

    // check at
    for (auto u_iter{ recorded_s_nodes.begin() },
         u_end{ recorded_s_nodes.end() };
         u_iter != u_end; ++u_iter) {
        Zeta_LRUCacheManager_SNode* s_node =
            (Zeta_LRUCacheManager_SNode*)(*u_iter);

        Zeta_DebugPipe ns;
        Zeta_DebugPipe_Create(&ns);

        Zeta_RBTree_Check(&rbtn_opr, &ns, s_node->at_root);

        std::unordered_set<Zeta_LRUCacheManager_XNode*> cur_x_nodes;

        size_t blk_idx_lb{ 0 };

        while (0 < Zeta_DebugPipe_GetSize(&ns)) {
            Zeta_OrdRBTreeNode* at_node =
                (Zeta_OrdRBTreeNode*)ZETA_AddrToPtr(Zeta_DebugPipe_Pop(&ns));

            Zeta_LRUCacheManager_XNode* x_node = ZETA_MemberToStruct(
                Zeta_LRUCacheManager_XNode, at_node, at_node);

            ZETA_DebugAssert(cur_x_nodes.insert(x_node).second);

            Zeta_LRUCacheManager_CNode* c_node = x_node->c_node;

            ZETA_DebugAssert(blk_idx_lb <= c_node->blk_idx);
            blk_idx_lb = c_node->blk_idx + 1;
        }

        ZETA_DebugAssert(s_node_to_x_nodes[s_node] == cur_x_nodes);

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

    void* a_s_node = LRUCM_LogIn(16);
    void* b_s_node = DummyCM_LogIn(16);

    byte_t buffer[BLOCK_SIZE];

    for (int op_i{ 0 }; op_i < 100; ++op_i) {
        int type = size_generator(en) % 2;
        size_t blk_idx = size_generator(en) % BLOCK_NUM;

        if (type == 0) {
            std::cout << "read  " << blk_idx << "\n";

            byte_t const* ra = (byte_t const*)Zeta_LRUCacheManager_ReadBlock(
                &lrucm, a_s_node, blk_idx);

            byte_t const* rb = (byte_t const*)Zeta_DummyCacheManager_ReadBlock(
                &dcm, b_s_node, blk_idx);

            ZETA_DebugAssert(IsEqual(ra, rb, BLOCK_SIZE));
        } else {
            std::cout << "write " << blk_idx << "\n";

            RandomFill(buffer, BLOCK_SIZE);

            Zeta_LRUCacheManager_WriteBlock(&lrucm, a_s_node, blk_idx, buffer);
            Zeta_DummyCacheManager_WriteBlock(&dcm, b_s_node, blk_idx, buffer);
        }

        LRUCM_Check();
    }

    LRUCM_LogOut(a_s_node);
    DummyCM_LogOut(b_s_node);

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
