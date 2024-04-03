#include "../Zeta/LRUCacheManager.h"
#include "FileBlockVector.h"

FileBlockVector file_blk_vec;
Zeta_BlockVector blk_vec;

Zeta_LRUCacheManager lrucm;

// Zeta_Std

#define BLOCK_SIZE (512)

void InitFileBlockVector() {
    file_blk_vec.Open("ajdiwaj", BLOCK_SIZE);

    blk_vec.context = &file_blk_vec;

    blk_vec.GetBlockSize = FileBlockVector_GetBlockSize;
    blk_vec.GetBlockNum = FileBlockVector_GetBlockNum;
    blk_vec.ReadBlock = FileBlockVector_ReadBlock;
    blk_vec.WriteBlock = FileBlockVector_WriteBlock;
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
