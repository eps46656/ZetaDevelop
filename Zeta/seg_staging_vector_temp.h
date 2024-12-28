// define STAGING (true or false)

#include "allocator.h"
#include "mem_check_utils.h"
#include "ord_cnt_3rb_tree_node.h"
#include "ord_cnt_rb_tree_node.h"
#include "seq_cntr.h"

ZETA_ExternC_Beg;

#pragma push_macro("Cntr")
#pragma push_macro("Cntr_")

#if STAGING

#define Cntr Zeta_StagingVector

#else

#define Cntr Zeta_SegVector

#endif

#define Cntr_(x) ZETA_Concat(Cntr, ZETA_Concat(_, x))

ZETA_DeclareStruct(Cntr);
ZETA_DeclareStruct(Cntr_(Seg));
ZETA_DeclareStruct(Cntr_(Cursor));
ZETA_DeclareStruct(Cntr_(Stats));

#if STAGING

#define ZETA_StagingVector_ref_color (1)
#define ZETA_StagingVector_dat_color (2)

#define ZETA_StagingVector_WriteBackStrategy_L (0b001)
#define ZETA_StagingVector_WriteBackStrategy_R (0b010)
#define ZETA_StagingVector_WriteBackStrategy_LR (0b011)
#define ZETA_StagingVector_WriteBackStrategy_Random (0b100)

#endif

struct Cntr {
#if !STAGING
    size_t width;
#endif

    /** The maximum number of elements in a segment. */
    size_t seg_capacity;

#if STAGING
    Zeta_SeqCntr* origin;

    Zeta_OrdCnt3RBTreeNode* root;

    Zeta_OrdCnt3RBTreeNode* lb;
    Zeta_OrdCnt3RBTreeNode* rb;
#else
    Zeta_OrdCntRBTreeNode* root;

    Zeta_OrdCntRBTreeNode* lb;
    Zeta_OrdCntRBTreeNode* rb;
#endif

    Zeta_Allocator* seg_allocator;
    Zeta_Allocator* data_allocator;
};

struct Cntr_(Seg) {
#if STAGING
    Zeta_OrdCnt3RBTreeNode n;
#else
    Zeta_OrdCntRBTreeNode n;
#endif

    union {
#if STAGING
        struct {
            size_t beg;
            size_t size;
        } ref;
#endif

        struct {
            void* data;
            unsigned short offset;
            unsigned short size;
        } dat;
    };
};

struct Cntr_(Cursor) {
    void const* cntr;
    size_t idx;
    void* n;
    size_t seg_idx;
    void* ref;
};

struct Cntr_(Stats) {
    size_t ref_seg_cnt;
    size_t dat_seg_cnt;
    size_t ref_size;
    size_t dat_size;
};

void Cntr_(Init)(void* cntr);

/**
 * @copydoc Zeta_SeqCntr::Deinit
 */
void Cntr_(Deinit)(void* cntr);

/**
 * @copydoc Zeta_SeqCntr::GetWidth
 */
size_t Cntr_(GetWidth)(void const* cntr);

/**
 * @copydoc Zeta_SeqCntr::GetSize
 */
size_t Cntr_(GetSize)(void const* cntr);

/**
 * @copydoc Zeta_SeqCntr::GetCapacity
 */
size_t Cntr_(GetCapacity)(void const* cntr);

/**
 * @copydoc Zeta_SeqCntr::GetLBCursor
 */
void Cntr_(GetLBCursor)(void const* cntr, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::GetRBCursor
 */
void Cntr_(GetRBCursor)(void const* cntr, void* dst_cursor);

void* ZETA_Concat(Cntr,
                  _FindFirst)(void* cntr, void* dst_cursor, void* dst_elem,
                              void* compare_context,
                              bool_t (*Compare)(void* context, void* ele));

/**
 * @copydoc Zeta_SeqCntr::PeekL
 */
void* Cntr_(PeekL)(void* cntr, void* dst_cursor, void* dst_elem);

/**
 * @copydoc Zeta_SeqCntr::PeekR
 */
void* Cntr_(PeekR)(void* cntr, void* dst_cursor, void* dst_elem);

/**
 * @copydoc Zeta_SeqCntr::Access
 */
void* Cntr_(Access)(void* cntr, size_t idx, void* dst_cursor, void* dst_elem);

/**
 * @copydoc Zeta_SeqCntr::Refer
 */
void* Cntr_(Refer)(void* cntr, void const* pos_cursor);

void Cntr_(BinSearch)(void* cntr, void* elem_key_cmp_context,
                      int(ElemKeyCompare)(void* key_elem_cmp_context,
                                          void const* elem, void const* key));

/**
 * @copydoc Zeta_SeqCntr::Read
 */
void Cntr_(Read)(void const* cntr, void const* pos_cursor, size_t cnt,
                 void* dst, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::Write
 */
void Cntr_(Write)(void* cntr, void* pos_cursor, size_t cnt, void const* src,
                  void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::PushL
 */
void* Cntr_(PushL)(void* cntr, size_t cnt, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::PushR
 */
void* Cntr_(PushR)(void* cntr, size_t cnt, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::Insert
 */
void* Cntr_(Insert)(void* cntr, void* pos_cursor, size_t cnt);

/**
 * @copydoc Zeta_SeqCntr::PopL
 */
void Cntr_(PopL)(void* cntr, size_t cnt);

/**
 * @copydoc Zeta_SeqCntr::PopR
 */
void Cntr_(PopR)(void* cntr, size_t cnt);

/**
 * @copydoc Zeta_SeqCntr::Erase
 */
void Cntr_(Erase)(void* cntr, void* pos_cursor, size_t cnt);

/**
 * @copydoc Zeta_SeqCntr::EraseAll
 */
void Cntr_(EraseAll)(void* cntr);

#if STAGING
void Cntr_(Reset)(void* cntr);
#endif

void Cntr_(Copy)(void* cntr, void* src_cntr);

void Cntr_(Collapse)(void* cntr);

void Cntr_(WriteBack)(void* cntr, int write_back_strategy,
                      unsigned long long cost_coeff_read,
                      unsigned long long cost_coeff_write,
                      unsigned long long cost_coeff_insert,
                      unsigned long long cost_coeff_erase);

void Cntr_(Check)(void const* cntr);

void Cntr_(Sanitize)(void const* cntr, Zeta_MemRecorder* dst_seg,
                     Zeta_MemRecorder* dst_data);

void Cntr_(PrintState)(void* cntr);

Cntr_(Stats) ZETA_Concat(Cntr, GetStats)(void* cntr);

/**
 * @copydoc Zeta_SeqCntr::Cursor_AreEqual
 */
bool_t Cntr_(Cursor_AreEqual)(void const* cntr, void const* cursor_a,
                              void const* cursor_b);

/**
 * @copydoc Zeta_SeqCntr::Cursor_Compare
 */
int Cntr_(Cursor_Compare)(void const* cntr, void const* cursor_a,
                          void const* cursor_b);

/**
 * @copydoc Zeta_SeqCntr::Cursor_GetDist
 */
size_t Cntr_(Cursor_GetDist)(void const* cntr, void const* cursor_a,
                             void const* cursor_b);

/**
 * @copydoc Zeta_SeqCntr::Cursor_GetIdx
 */
size_t Cntr_(Cursor_GetIdx)(void const* cntr, void const* cursor);

/**
 * @copydoc Zeta_SeqCntr::Cursor_StepL
 */
void Cntr_(Cursor_StepL)(void const* cntr, void* cursor);

/**
 * @copydoc Zeta_SeqCntr::Cursor_StepR
 */
void Cntr_(Cursor_StepR)(void const* cntr, void* cursor);

/**
 * @copydoc Zeta_SeqCntr::Cursor_AdvanceL
 */
void Cntr_(Cursor_AdvanceL)(void const* cntr, void* cursor, size_t step);

/**
 * @copydoc Zeta_SeqCntr::Cursor_AdvanceR
 */
void Cntr_(Cursor_AdvanceR)(void const* cntr, void* cursor, size_t step);

void Cntr_(Cursor_Check)(void const* cntr, void const* cursor);

void Cntr_(DeploySeqCntr)(void* cntr, Zeta_SeqCntr* seq_cntr);

#pragma pop_macro("Cntr")
#pragma pop_macro("Cntr_")

ZETA_ExternC_End;
