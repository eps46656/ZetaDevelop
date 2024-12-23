#include "multi_level_circular_array.h"

#include "debugger.h"

#if ZETA_EnableDebug

#define CheckCntr_(mlca) Zeta_MultiLevelCircularArray_Check((mlca))

#define CheckCursor_(mlca, cursor) \
    Zeta_MultiLevelCircularArray_CheckCursor((mlca), (cursor))

#else

#define CheckCntr_(mlca)

#define CheckCursor_(mlca, cursor)

#endif

static unsigned short branch_nums[] = {
    [0 ... ZETA_MultiLevelTable_max_level - 1] =
        ZETA_MultiLevelCircularArray_branch_num
};

void Zeta_MultiLevelCircularArray_Init(void* mlca_) {
    //
}

void Zeta_MultiLevelCircularArray_Deinit(void* mlca);

size_t Zeta_MultiLevelCircularArray_GetWidth(void* mlca);

size_t Zeta_MultiLevelCircularArray_GetSize(void* mlca);

size_t Zeta_MultiLevelCircularArray_GetCapacity(void* mlca);

void Zeta_MultiLevelCircularArray_GetLBCursor(void* mlca, void* dst_cursor);

void Zeta_MultiLevelCircularArray_GetRBCursor(void* mlca, void* dst_cursor);

void* Zeta_MultiLevelCircularArray_Access(void* mlca, size_t idx,
                                          void* dst_elem);

void Zeta_MultiLevelCircularArray_Check(void* mlca_) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    ZETA_DebugAssert(mlca != NULL);

    //
}

void Zeta_MultiLevelCircularArray_CheckCursor(void* mlca_, void* cursor_) {
    Zeta_MultiLevelCircularArray* mlca = mlca_;
    CheckCntr_(mlca);

    Zeta_MultiLevelCircularArray_Cursor* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);
}
