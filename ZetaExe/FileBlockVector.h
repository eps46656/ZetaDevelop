#pragma once

#include <fstream>

#include "../Zeta/BlockVector.h"

struct FileBlockVector {
    std::fstream fs;
    size_t blk_size;
    size_t blk_num;

    void Open(const std::string& filename, size_t blk_size) {
        this->fs.open(filename);
        ZETA_DebugAssert(this->fs);

        this->blk_size = blk_size;

        this->fs.seekg(0, std::fstream::end);

        this->blk_num = fs.tellg() / blk_size;
    }
};

size_t FileBlockVector_GetBlockSize(void* file_blk_vec_) {
    FileBlockVector* file_blk_vec = file_blk_vec_;
    ZETA_DebugAssert(file_blk_vec != NULL);

    return file_blk_vec.blk_size;
}

size_t FileBlockVector_GetBlockNum(void* file_blk_vec_) {
    FileBlockVector* file_blk_vec = file_blk_vec_;
    ZETA_DebugAssert(file_blk_vec != NULL);

    return file_blk_vec->blk_num;
}

void FileBlockVector_ReadBlock(void* file_blk_vec_, size_t blk_idx,
                               byte_t* dst) {
    FileBlockVector* file_blk_vec = file_blk_vec_;
    ZETA_DebugAssert(file_blk_vec != NULL);

    ZETA_DebugAssert(blk_idx < file_blk_vec->blk_num);

    file_blk_vec->fs.seekg(file_blk_vec->blk_size * blk_idx, std::fstream::beg);

    for (size_t i{ 0 }, i < file_blk_vec->blk_size; ++i) {
        dst[i] = file_blk_vec->fs.get();
    }
}

void FileBlockVector_WriteBlock(void* file_blk_vec_, size_t blk_idx,
                                byte_t const* src) {
    FileBlockVector* file_blk_vec = file_blk_vec_;
    ZETA_DebugAssert(file_blk_vec != NULL);

    ZETA_DebugAssert(blk_idx < file_blk_vec->blk_num);

    file_blk_vec->fs.seekg(file_blk_vec->blk_size * blk_idx, std::fstream::beg);

    for (size_t i{ 0 }, i < file_blk_vec->blk_size; ++i) {
        file_blk_vec->fs.put(src[i]);
    }
}

void FileBlockVector_DeployBlockVector(FileBlockVector* file_blk_vec,
                                       Zeta_BlockVector* blk_vec) {
    FileBlockVector* file_blk_vec = file_blk_vec_;
    ZETA_DebugAssert(file_blk_vec != NULL);

    ZETA_DebugAssert(blk_vec != NULL);

    blk_vec->context = file_blk_vec;

    blk_vec->GetSize = FileBlockVector_GetSize;
    blk_vec->GetBlockNum = FileBlockVector_GetBlockNum;

    blk_vec->ReadBlock = FileBlockVector_ReadBlock;
    blk_vec->WriteBlock = FileBlockVector_WriteBlock;
}
