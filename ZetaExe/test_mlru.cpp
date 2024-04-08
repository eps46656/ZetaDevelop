#include "../Zeta/LRUCacheManager.h"
#include "FileBlockVector.h"
#include "StdAllocator.h"

FileBlockVector file_blk_vec;
Zeta_BlockVector blk_vec;

StdAllocator u_node_allocator_;
Zeta_Allocator u_node_allocator;

StdAllocator c_node_allocator_;
Zeta_Allocator c_node_allocator;

StdAllocator x_node_allocator_;
Zeta_Allocator x_node_allocator;

StdAllocator frame_allocator_;
Zeta_Allocator frame_allocator;

Zeta_LRUCacheManager lrucm;

#define BLOCK_SIZE (512)

void InitFileBlockVector() {
    file_blk_vec.Open("./fbv_1.fbv", BLOCK_SIZE);

    Zeta_BlockVector_Init(&blk_vec);

    FileBlockVector_DeployBlockVector(&file_blk_vec, &blk_vec);
}

void InitLRUCM() {
    // lrucm->
}

void main1() {
    InitFileBlockVector();

    InitLRUCM();
}

int main() {
    main1();
    std::cout << "ok\n";
    return 1;
}
