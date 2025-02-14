// define STAGING (true or false)

#include "allocator.h"
#include "bin_tree_node.h"
#include "mem_check_utils.h"
#include "seq_cntr.h"

ZETA_ExternC_Beg;

#pragma push_macro("Cntr")
#pragma push_macro("Zeta_Cntr")
#pragma push_macro("Zeta_Cntr_")
#pragma push_macro("TreeNode")
#pragma push_macro("Zeta_TreeNode")
#pragma push_macro("Zeta_TreeNode_")

#if STAGING

#define Cntr StagingVector

#define TreeNode OrdCnt3RBTreeNode

#else

#define Cntr SegVector

#define TreeNode OrdCntRBTreeNode

#endif

#define Zeta_Cntr ZETA_Concat(Zeta_, Cntr)
#define Zeta_Cntr_(x) ZETA_Concat(Zeta_Cntr, _, x)

#define Zeta_TreeNode ZETA_Concat(Zeta_, TreeNode)
#define Zeta_TreeNode_(x) ZETA_Concat(Zeta_TreeNode, _, TreeNode)

ZETA_DeclareStruct(Zeta_Cntr);
ZETA_DeclareStruct(Zeta_Cntr_(Seg));
ZETA_DeclareStruct(Zeta_Cntr_(Cursor));
ZETA_DeclareStruct(Zeta_Cntr_(Stats));

#if STAGING

#define ZETA_StagingVector_ref_color (1)
#define ZETA_StagingVector_dat_color (2)

#define ZETA_StagingVector_WriteBackStrategy_L (0b001)
#define ZETA_StagingVector_WriteBackStrategy_R (0b010)
#define ZETA_StagingVector_WriteBackStrategy_LR (0b011)
#define ZETA_StagingVector_WriteBackStrategy_Random (0b100)

#endif

struct Zeta_Cntr {
#if !STAGING
    size_t width;
    // In staing mode, width is determined by origin.
#endif

    size_t stride;

    /** The maximum number of elements in a segment. */
    size_t seg_capacity;

#if STAGING
    Zeta_SeqCntr origin;
#endif

    Zeta_TreeNode* root;

    Zeta_TreeNode* lb;
    Zeta_TreeNode* rb;

    Zeta_Allocator seg_allocator;
    Zeta_Allocator data_allocator;
};

struct Zeta_Cntr_(Seg) {
    ZETA_DebugStructPadding;

    Zeta_TreeNode n;

    ZETA_DebugStructPadding;

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

struct Zeta_Cntr_(Cursor) {
    void const* cntr;
    size_t idx;
    void* n;
    size_t seg_idx;
    void* ref;
};

struct Zeta_Cntr_(Stats) {
    size_t ref_seg_cnt;
    size_t dat_seg_cnt;
    size_t ref_size;
    size_t dat_size;
};

void Zeta_Cntr_(Init)(void* cntr);

/**
 * @copydoc Zeta_SeqCntr::Deinit
 */
void Zeta_Cntr_(Deinit)(void* cntr);

/**
 * @copydoc Zeta_SeqCntr::GetWidth
 */
size_t Zeta_Cntr_(GetWidth)(void const* cntr);

/**
 * @copydoc Zeta_SeqCntr::GetSize
 */
size_t Zeta_Cntr_(GetSize)(void const* cntr);

/**
 * @copydoc Zeta_SeqCntr::GetCapacity
 */
size_t Zeta_Cntr_(GetCapacity)(void const* cntr);

/**
 * @copydoc Zeta_SeqCntr::GetLBCursor
 */
void Zeta_Cntr_(GetLBCursor)(void const* cntr, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::GetRBCursor
 */
void Zeta_Cntr_(GetRBCursor)(void const* cntr, void* dst_cursor);

void* ZETA_Concat(Cntr,
                  _FindFirst)(void* cntr, void* dst_cursor, void* dst_elem,
                              void* compare_context,
                              bool_t (*Compare)(void* context, void* ele));

/**
 * @copydoc Zeta_SeqCntr::PeekL
 */
void* Zeta_Cntr_(PeekL)(void* cntr, void* dst_cursor, void* dst_elem);

/**
 * @copydoc Zeta_SeqCntr::PeekR
 */
void* Zeta_Cntr_(PeekR)(void* cntr, void* dst_cursor, void* dst_elem);

/**
 * @copydoc Zeta_SeqCntr::Access
 */
void* Zeta_Cntr_(Access)(void* cntr, size_t idx, void* dst_cursor,
                         void* dst_elem);

/**
 * @copydoc Zeta_SeqCntr::Refer
 */
void* Zeta_Cntr_(Refer)(void* cntr, void const* pos_cursor);

void Zeta_Cntr_(BinSearch)(void* cntr, void* elem_key_cmp_context,
                           int(ElemKeyCompare)(void* key_elem_cmp_context,
                                               void const* elem,
                                               void const* key));

/**
 * @copydoc Zeta_SeqCntr::Read
 */
void Zeta_Cntr_(Read)(void const* cntr, void const* pos_cursor, size_t cnt,
                      void* dst, size_t dst_stride, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::Write
 */
void Zeta_Cntr_(Write)(void* cntr, void* pos_cursor, size_t cnt,
                       void const* src, size_t src_stride, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::PushL
 */
void* Zeta_Cntr_(PushL)(void* cntr, size_t cnt, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::PushR
 */
void* Zeta_Cntr_(PushR)(void* cntr, size_t cnt, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::Insert
 */
void* Zeta_Cntr_(Insert)(void* cntr, void* pos_cursor, size_t cnt);

/**
 * @copydoc Zeta_SeqCntr::PopL
 */
void Zeta_Cntr_(PopL)(void* cntr, size_t cnt);

/**
 * @copydoc Zeta_SeqCntr::PopR
 */
void Zeta_Cntr_(PopR)(void* cntr, size_t cnt);

/**
 * @copydoc Zeta_SeqCntr::Erase
 */
void Zeta_Cntr_(Erase)(void* cntr, void* pos_cursor, size_t cnt);

/**
 * @copydoc Zeta_SeqCntr::EraseAll
 */
void Zeta_Cntr_(EraseAll)(void* cntr);

#if STAGING
void Zeta_Cntr_(Reset)(void* cntr);
#endif

void Zeta_Cntr_(Copy)(void* cntr, void* src_cntr);

void Zeta_Cntr_(Collapse)(void* cntr);

void Zeta_Cntr_(WriteBack)(void* cntr, int write_back_strategy,
                           unsigned long long cost_coeff_read,
                           unsigned long long cost_coeff_write,
                           unsigned long long cost_coeff_insert,
                           unsigned long long cost_coeff_erase);

void Zeta_Cntr_(Check)(void const* cntr);

void Zeta_Cntr_(Sanitize)(void const* cntr, Zeta_MemRecorder* dst_seg,
                          Zeta_MemRecorder* dst_data);

void Zeta_Cntr_(PrintState)(void* cntr);

Zeta_Cntr_(Stats) ZETA_Concat(Cntr, GetStats)(void* cntr);

/**
 * @copydoc Zeta_SeqCntr::Cursor_AreEqual
 */
bool_t Zeta_Cntr_(Cursor_AreEqual)(void const* cntr, void const* cursor_a,
                                   void const* cursor_b);

/**
 * @copydoc Zeta_SeqCntr::Cursor_Compare
 */
int Zeta_Cntr_(Cursor_Compare)(void const* cntr, void const* cursor_a,
                               void const* cursor_b);

/**
 * @copydoc Zeta_SeqCntr::Cursor_GetDist
 */
size_t Zeta_Cntr_(Cursor_GetDist)(void const* cntr, void const* cursor_a,
                                  void const* cursor_b);

/**
 * @copydoc Zeta_SeqCntr::Cursor_GetIdx
 */
size_t Zeta_Cntr_(Cursor_GetIdx)(void const* cntr, void const* cursor);

/**
 * @copydoc Zeta_SeqCntr::Cursor_StepL
 */
void Zeta_Cntr_(Cursor_StepL)(void const* cntr, void* cursor);

/**
 * @copydoc Zeta_SeqCntr::Cursor_StepR
 */
void Zeta_Cntr_(Cursor_StepR)(void const* cntr, void* cursor);

/**
 * @copydoc Zeta_SeqCntr::Cursor_AdvanceL
 */
void Zeta_Cntr_(Cursor_AdvanceL)(void const* cntr, void* cursor, size_t step);

/**
 * @copydoc Zeta_SeqCntr::Cursor_AdvanceR
 */
void Zeta_Cntr_(Cursor_AdvanceR)(void const* cntr, void* cursor, size_t step);

void Zeta_Cntr_(Cursor_Check)(void const* cntr, void const* cursor);

#if STAGING

extern Zeta_SeqCntr_VTable const zeta_staging_vector_seq_cntr_vtable;

#else

extern Zeta_SeqCntr_VTable const zeta_seg_vector_seq_cntr_vtable;

#endif

#pragma pop_macro("Cntr")
#pragma pop_macro("Zeta_Cntr")
#pragma pop_macro("Zeta_Cntr_")
#pragma pop_macro("TreeNode")
#pragma pop_macro("Zeta_TreeNode")
#pragma pop_macro("Zeta_TreeNode_")

ZETA_ExternC_End;
