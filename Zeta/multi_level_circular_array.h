#include "allocator.h"
#include "multi_level_table.h"
#include "ord_linked_list_node.h"
#include "seq_cntr.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_MultiLevelCircularArray);
ZETA_DeclareStruct(Zeta_MultiLevelCircularArray_Seg);
ZETA_DeclareStruct(Zeta_MultiLevelCircularArray_Cursor);

#define ZETA_MultiLevelCircularArray_max_seg_capacity (8)

#define ZETA_MultiLevelCircularArray_branch_num \
    ZETA_MultiLevelTable_max_branch_num

struct Zeta_MultiLevelCircularArray {
    size_t width;

    size_t seg_capacity;

    size_t offset;

    size_t size;

    int level;

    size_t rotations[ZETA_MultiLevelTable_max_level];

    void* root;

    Zeta_MultiLevelCircularArray_Seg* seg_head;

    Zeta_Allocator* node_allocator;
    Zeta_Allocator* seg_allocator;
};

struct Zeta_MultiLevelCircularArray_Seg {
    Zeta_OrdLinkedListNode ln;

    unsigned char data[] __attribute__((aligned(alignof(max_align_t))));
};

struct Zeta_MultiLevelCircularArray_Cursor {
    Zeta_MultiLevelCircularArray const* mlca;

    size_t idx;

    Zeta_MultiLevelCircularArray_Seg* seg;
    size_t seg_slot_idx;

    void* elem;
};

void Zeta_MultiLevelCircularArray_Init(void* mlca);

/**
 * @copydoc Zeta_SeqCntr::Deinit
 */
void Zeta_MultiLevelCircularArray_Deinit(void* mlca);

/**
 * @copydoc Zeta_SeqCntr::GetWidth
 */
size_t Zeta_MultiLevelCircularArray_GetWidth(void const* mlca);

/**
 * @copydoc Zeta_SeqCntr::GetSize
 */
size_t Zeta_MultiLevelCircularArray_GetSize(void const* mlca);

/**
 * @copydoc Zeta_SeqCntr::GetCapacity
 */
size_t Zeta_MultiLevelCircularArray_GetCapacity(void const* mlca);

/**
 * @copydoc Zeta_SeqCntr::GetLBCursor
 */
void Zeta_MultiLevelCircularArray_GetLBCursor(void const* mlca,
                                              void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::GetRBCursor
 */
void Zeta_MultiLevelCircularArray_GetRBCursor(void const* mlca,
                                              void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::PeekL
 */
void* Zeta_MultiLevelCircularArray_PeekL(void* mlca, void* dst_cursor,
                                         void* dst_elem);

/**
 * @copydoc Zeta_SeqCntr::PeekR
 */
void* Zeta_MultiLevelCircularArray_PeekR(void* mlca, void* dst_cursor,
                                         void* dst_elem);

/**
 * @copydoc Zeta_SeqCntr::Access
 */
void* Zeta_MultiLevelCircularArray_Access(void* mlca, size_t idx,
                                          void* dst_cursor, void* dst_elem);

/**
 * @copydoc Zeta_SeqCntr::Refer
 */
void* Zeta_MultiLevelCircularArray_Refer(void* mlca, void const* pos_cursor);

/**
 * @copydoc Zeta_SeqCntr::Read
 *
 */
void Zeta_MultiLevelCircularArray_Read(void const* mlca, void const* pos_cursor,
                                       size_t cnt, void* dst, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::Write
 */
void Zeta_MultiLevelCircularArray_Write(void* mlca, void* pos_cursor,
                                        size_t cnt, void const* src,
                                        void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::Write
 */
void Zeta_MultiLevelCircularArray_RangeAssign(void* mlca, void* dst_cursor,
                                              void* src_cursor, size_t cnt);

/**
 * @copydoc Zeta_SeqCntr::PushL
 */
void* Zeta_MultiLevelCircularArray_PushL(void* mlca, size_t cnt,
                                         void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::PushR
 */
void* Zeta_MultiLevelCircularArray_PushR(void* mlca, size_t cnt,
                                         void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::Insert
 */
void* Zeta_MultiLevelCircularArray_Insert(void* mlca, void* pos_cursor,
                                          size_t cnt);

/**
 * @copydoc Zeta_SeqCntr::PopL
 */
void Zeta_MultiLevelCircularArray_PopL(void* mlca, size_t cnt);

/**
 * @copydoc Zeta_SeqCntr::PopR
 */
void Zeta_MultiLevelCircularArray_PopR(void* mlca, size_t cnt);

/**
 * @copydoc Zeta_SeqCntr::Erase
 */
void Zeta_MultiLevelCircularArray_Erase(void* mlca, void* pos_cursor,
                                        size_t cnt);

/**
 * @copydoc Zeta_SeqCntr::EraseAll
 */
void Zeta_MultiLevelCircularArray_EraseAll(void* mlca);

void Zeta_MultiLevelCircularArray_Check(void const* mlca);

void Zeta_MultiLevelCircularArray_Sanitize(void const* mlca,
                                           Zeta_MemRecorder* dst_node_allocator,
                                           Zeta_MemRecorder* dst_seg_allocator);

/**
 * @copydoc Zeta_SeqCntr::Cursor_AreEqual
 */
bool_t Zeta_MultiLevelCircularArray_Cursor_AreEqual(void const* mlca,
                                                    void const* cursor_a,
                                                    void const* cursor_b);

/**
 * @copydoc Zeta_SeqCntr::Cursor_Compare
 */
int Zeta_MultiLevelCircularArray_Cursor_Compare(void const* mlca,
                                                void const* cursor_a,
                                                void const* cursor_b);
/**
 * @copydoc Zeta_SeqCntr::Cursor_GetDist
 */
size_t Zeta_MultiLevelCircularArray_Cursor_GetDist(void const* mlca,
                                                   void const* cursor_a,
                                                   void const* cursor_b);
/**
 * @copydoc Zeta_SeqCntr::Cursor_GetIdx
 */
size_t Zeta_MultiLevelCircularArray_Cursor_GetIdx(void const* mlca,
                                                  void const* cursor);
/**
 * @copydoc Zeta_SeqCntr::Cursor_StepL
 */
void Zeta_MultiLevelCircularArray_Cursor_StepL(void const* mlca, void* cursor);

/**
 * @copydoc Zeta_SeqCntr::Cursor_StepR
 */
void Zeta_MultiLevelCircularArray_Cursor_StepR(void const* mlca, void* cursor);

/**
 * @copydoc Zeta_SeqCntr::Cursor_AdvanceL
 */
void Zeta_MultiLevelCircularArray_Cursor_AdvanceL(void const* mlca,
                                                  void* cursor, size_t step);
/**
 * @copydoc Zeta_SeqCntr::Cursor_AdvanceR
 */
void Zeta_MultiLevelCircularArray_Cursor_AdvanceR(void const* mlca,
                                                  void* cursor, size_t step);

void Zeta_MultiLevelCircularArray_Cursor_Check(void const* mlca,
                                               void const* cursor);

void Zeta_MultiLevelCircularArray_DeploySeqCntr(void* mlca,
                                                Zeta_SeqCntr* seq_cntr);

ZETA_ExternC_End;
