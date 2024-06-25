#pragma once

#include <fstream>

#include "../Zeta/BlockVector.h"

struct FileBlockVector {
    std::fstream fs;
    size_t blk_size;
    size_t blk_cnt;

    ~FileBlockVector() {
        if (this->fs.is_open()) { this->fs.close(); }
    }

    bool_t IsOpen() const { return this->fs.is_open(); }

    void Open(const std::string& filename, size_t blk_size) {
        this->fs.open(filename, std::fstream::in | std::fstream::out |
                                    std::fstream::binary);
        ZETA_DebugAssert(this->fs);

        this->blk_size = blk_size;

        this->fs.seekg(0, std::fstream::end);

        this->blk_cnt = fs.tellg() / blk_size;
    }

    void Close() { this->fs.close(); }
};

size_t FileBlockVector_GetBlockSize(void* file_blk_vec_) {
    FileBlockVector* file_blk_vec = (FileBlockVector*)file_blk_vec_;
    ZETA_DebugAssert(file_blk_vec != NULL);

    return file_blk_vec->blk_size;
}

size_t FileBlockVector_GetBlockCnt(void* file_blk_vec_) {
    FileBlockVector* file_blk_vec = (FileBlockVector*)file_blk_vec_;
    ZETA_DebugAssert(file_blk_vec != NULL);

    ZETA_PrintVar(file_blk_vec);
    ZETA_PrintVar(file_blk_vec->blk_cnt);

    return file_blk_vec->blk_cnt;
}

void FileBlockVector_ReadBlock(void* file_blk_vec_, size_t blk_num, void* dst) {
    FileBlockVector* file_blk_vec = (FileBlockVector*)file_blk_vec_;
    ZETA_DebugAssert(file_blk_vec != NULL);

    ZETA_DebugAssert(blk_num < file_blk_vec->blk_cnt);

    file_blk_vec->fs.seekg(file_blk_vec->blk_size * blk_num, std::fstream::beg);
    file_blk_vec->fs.read((char*)dst, file_blk_vec->blk_size);
}

void FileBlockVector_WriteBlock(void* file_blk_vec_, size_t blk_num,
                                void const* src) {
    FileBlockVector* file_blk_vec = (FileBlockVector*)file_blk_vec_;
    ZETA_DebugAssert(file_blk_vec != NULL);

    ZETA_DebugAssert(blk_num < file_blk_vec->blk_cnt);

    file_blk_vec->fs.seekp(file_blk_vec->blk_size * blk_num, std::fstream::beg);
    file_blk_vec->fs.write((char const*)src, file_blk_vec->blk_size);
}

void FileBlockVector_DeployBlockVector(void* file_blk_vec_,
                                       Zeta_BlockVector* blk_vec) {
    FileBlockVector* file_blk_vec = (FileBlockVector*)file_blk_vec_;
    ZETA_DebugAssert(file_blk_vec != NULL);

    ZETA_DebugAssert(blk_vec != NULL);

    blk_vec->context = file_blk_vec;

    blk_vec->GetBlockSize = FileBlockVector_GetBlockSize;

    blk_vec->ReadBlock = FileBlockVector_ReadBlock;
    blk_vec->WriteBlock = FileBlockVector_WriteBlock;
}
