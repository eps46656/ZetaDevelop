#include "allocator.h"
#include "multi_level_table.h"
#include "seq_cntr.h"

ZETA_DeclareStruct(Zeta_MultiLevelCircularArray);
ZETA_DeclareStruct(Zeta_MultiLevelCircularArray_Cursor);

#define ZETA_MultiLevelCircularArray_branch_num \
    ZETA_MultiLevelTable_max_branch_num

struct Zeta_MultiLevelCircularArray {
    int level;

    size_t offsets[ZETA_MultiLevelTable_max_level];

    size_t elem_offset;
    size_t size;

    Zeta_Allocator* node_allocator;
    Zeta_Allocator* seg_allocator;
};

struct Zeta_MultiLevelCircularArray_Cursor {
    //
};

void Zeta_MultiLevelCircularArray_Init(void* mlca);

void Zeta_MultiLevelCircularArray_Deinit(void* mlca);

size_t Zeta_MultiLevelCircularArray_GetWidth(void* mlca);

size_t Zeta_MultiLevelCircularArray_GetSize(void* mlca);

size_t Zeta_MultiLevelCircularArray_GetCapacity(void* mlca);

void Zeta_MultiLevelCircularArray_GetLBCursor(void* mlca, void* dst_cursor);

void Zeta_MultiLevelCircularArray_GetRBCursor(void* mlca, void* dst_cursor);

void* Zeta_MultiLevelCircularArray_Access(void* mlca, size_t idx,
                                          void* dst_elem);

void Zeta_MultiLevelCircularArray_Check(void* mlca);
