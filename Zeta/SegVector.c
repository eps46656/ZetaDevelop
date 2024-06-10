#include "SegVector.h"

#include "BinTree.h"
#include "CircularVector.h"
#include "RBTree.h"
#include "utils.h"

void Zeta_SegVector_Init(void* sv_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    ZETA_DebugAssert(0 < sv->width);
    ZETA_DebugAssert(sv->width <= sv->stride);
    ZETA_DebugAssert(0 < sv->seg_capacity);
    ZETA_DebugAssert(sv->seg_capacity <= ZETA_GetRangeMax(unsigned short));

    ZETA_DebugAssert(sv->node_allocator != NULL);
    ZETA_DebugAssert(sv->node_allocator->Allocate != NULL);
    ZETA_DebugAssert(sv->node_allocator->Deallocate != NULL);

    ZETA_DebugAssert(sv->seg_allocator != NULL);
    ZETA_DebugAssert(sv->seg_allocator->Allocate != NULL);
    ZETA_DebugAssert(sv->seg_allocator->Deallocate != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    void* lb_ = sv->node_allocator->Allocate(sv->node_allocator->context,
                                             sizeof(Zeta_OrdCntRBTreeNode));
    void* rb_ = sv->node_allocator->Allocate(sv->node_allocator->context,
                                             sizeof(Zeta_OrdCntRBTreeNode));

    ZETA_DebugAssert(lb_ != NULL);
    ZETA_DebugAssert(rb_ != NULL);

    ZETA_DebugAssert(
        ZETA_GetAddrFromPtr(lb_) % alignof(Zeta_OrdCntRBTreeNode) == 0);
    ZETA_DebugAssert(
        ZETA_GetAddrFromPtr(rb_) % alignof(Zeta_OrdCntRBTreeNode) == 0);

    sv->lb = lb_;
    sv->rb = rb_;

    Zeta_OrdCntRBTreeNode_Init(NULL, sv->lb);
    Zeta_OrdCntRBTreeNode_Init(NULL, sv->rb);

    Zeta_OrdCntRBTreeNode_SetAccSize(NULL, sv->lb, 1);
    Zeta_OrdCntRBTreeNode_SetAccSize(NULL, sv->rb, 1);

    void* root = NULL;

    root = Zeta_RBTree_InsertR(&btn_opr, root, sv->lb);
    root = Zeta_RBTree_InsertR(&btn_opr, root, sv->rb);

    sv->root = root;
}

void Zeta_SegVector_Deinit(void* sv) {
    Zeta_SegVector_EraseAll(sv, NULL, NULL);
}

size_t Zeta_SegVector_GetWidth(void* sv_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    return sv->width;
}

size_t Zeta_SegVector_GetStride(void* sv_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    return sv->stride;
}

size_t Zeta_SegVector_GetSize(void* sv_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    return Zeta_OrdCntRBTreeNode_GetAccSize(NULL, sv->root) - 2;
}

void Zeta_SegVector_GetLBCursor(void* sv_, void* dst_cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    if (dst_cursor == NULL) { return; }

    dst_cursor->sv = sv;
    dst_cursor->idx = -1;
    dst_cursor->n = sv->lb;
    dst_cursor->seg_idx = 0;
    dst_cursor->ele = NULL;
}

void Zeta_SegVector_GetRBCursor(void* sv_, void* dst_cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    if (dst_cursor == NULL) { return; }

    dst_cursor->sv = sv;
    dst_cursor->idx = Zeta_SegVector_GetSize(sv);
    dst_cursor->n = sv->rb;
    dst_cursor->seg_idx = 0;
    dst_cursor->ele = NULL;
}

void* Zeta_SegVector_PeekL(void* sv_, void* dst_cursor_, void* dst_ele) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = sv->width;
    size_t stride = sv->stride;
    size_t seg_capacity = sv->seg_capacity;

    void* n = Zeta_BinTree_StepR(&btn_opr, sv->lb);

    if (sv->rb == n) {
        if (dst_cursor != NULL) {
            dst_cursor->sv = sv;
            dst_cursor->idx = 0;
            dst_cursor->n = n;
            dst_cursor->seg_idx = 0;
            dst_cursor->ele = NULL;
        }

        return NULL;
    }

    Zeta_SegVector_Node* node =
        ZETA_GetStructFromMember(Zeta_SegVector_Node, n, n);

    Zeta_CircularVector cv;
    cv.data = node->seg;
    cv.width = width;
    cv.stride = stride;
    cv.offset = node->offset;
    cv.size = node->size;
    cv.capacity = seg_capacity;

    void* ele = Zeta_CircularVector_PeekL(&cv, NULL, NULL);

    if (dst_cursor != NULL) {
        dst_cursor->sv = sv;
        dst_cursor->idx = 0;
        dst_cursor->n = n;
        dst_cursor->seg_idx = 0;
        dst_cursor->ele = ele;
    }

    if (dst_ele != NULL) { Zeta_MemCopy(dst_ele, ele, width); }

    return ele;
}

void* Zeta_SegVector_PeekR(void* sv_, void* dst_cursor_, void* dst_ele) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = sv->width;
    size_t stride = sv->stride;
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_SegVector_GetSize(sv);

    void* n = Zeta_BinTree_StepL(&btn_opr, sv->rb);

    if (sv->lb == n) {
        if (dst_cursor != NULL) {
            dst_cursor->sv = sv;
            dst_cursor->idx = -1;
            dst_cursor->n = n;
            dst_cursor->seg_idx = 0;
            dst_cursor->ele = NULL;
        }

        return NULL;
    }

    Zeta_SegVector_Node* node =
        ZETA_GetStructFromMember(Zeta_SegVector_Node, n, n);

    Zeta_CircularVector cv;
    cv.width = width;
    cv.stride = stride;
    cv.offset = node->offset;
    cv.size = node->size;
    cv.capacity = seg_capacity;

    void* ele = Zeta_CircularVector_PeekR(&cv, NULL, NULL);

    if (dst_cursor != NULL) {
        dst_cursor->sv = sv;
        dst_cursor->idx = size - 1;
        dst_cursor->n = n;
        dst_cursor->seg_idx = cv.size - 1;
        dst_cursor->ele = ele;
    }

    if (dst_ele != NULL) { Zeta_MemCopy(dst_ele, ele, width); }

    return ele;
}

void* Zeta_SegVector_Access(void* sv_, void* dst_cursor_, void* dst_ele,
                            size_t idx) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = sv->width;
    size_t stride = sv->stride;
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_SegVector_GetSize(sv);

    ZETA_DebugAssert(idx + 1 < size + 2);

    void* n;
    Zeta_SegVector_Node* node;
    size_t seg_idx;

    Zeta_BinTree_AccessL(&n, &seg_idx, &btn_opr, sv->root, idx + 1);

    void* ele;

    if (idx < size) {
        node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, n);

        Zeta_CircularVector cv;
        cv.data = node->seg;
        cv.width = width;
        cv.stride = stride;
        cv.offset = node->offset;
        cv.size = node->size;
        cv.capacity = seg_capacity;

        ele = Zeta_CircularVector_Access(&cv, NULL, NULL, seg_idx);
    } else {
        ele = NULL;
    }

    if (dst_cursor != NULL) {
        dst_cursor->sv = sv;
        dst_cursor->idx = idx;
        dst_cursor->n = n;
        dst_cursor->seg_idx = seg_idx;
        dst_cursor->ele = ele;
    }

    if (dst_ele != NULL && ele != NULL) { Zeta_MemCopy(dst_ele, ele, width); }

    return ele;
}

void* Zeta_SegVector_FindFirst(void* sv_, void* dst_cursor_, void* dst_ele,
                               void* context,
                               bool_t (*Compare)(void* context, void* ele)) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    ZETA_DebugAssert(Compare != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = sv->width;
    size_t stride = sv->stride;
    size_t seg_capacity = sv->seg_capacity;

    size_t ret_base_idx;
    void* ret_n;
    void* ret_ele;

    Zeta_CircularVector cv;
    cv.width = width;
    cv.stride = stride;
    cv.capacity = seg_capacity;

    size_t base = -1;

    void* n = sv->root;

    while (n != NULL) {
        void* nl = Zeta_OrdCntRBTreeNode_GetL(NULL, n);
        void* nr = Zeta_OrdCntRBTreeNode_GetR(NULL, n);

        if (sv->lb == n) {
            base += 1;
            n = nr;
            continue;
        }

        size_t nl_acc_size = Zeta_OrdCntRBTreeNode_GetAccSize(NULL, nl);

        if (sv->rb == n) {
            ret_base_idx = base + nl_acc_size;
            ret_n = n;
            ret_ele = NULL;
            n = nl;
            continue;
        }

        Zeta_SegVector_Node* node =
            ZETA_GetStructFromMember(Zeta_SegVector_Node, n, n);

        cv.data = node->seg;
        cv.offset = node->offset;
        cv.size = node->size;

        void* ele = Zeta_CircularVector_Access(&cv, NULL, NULL, cv.size - 1);

        if (Compare(context, ele)) {
            ret_base_idx = base + nl_acc_size;
            ret_n = n;
            ret_ele = ele;
            n = nl;
        } else {
            base += nl_acc_size + cv.size;
            n = nr;
        }
    }

    size_t ret_seg_idx;

    if (sv->rb == ret_n) {
        ret_seg_idx = 0;
    } else {
        Zeta_SegVector_Node* node =
            ZETA_GetStructFromMember(Zeta_SegVector_Node, n, ret_n);

        cv.data = node->seg;
        cv.offset = node->offset;
        cv.size = node->size;

        size_t lb = 0;
        size_t rb = cv.size - 1;

        while (lb < rb) {
            size_t mb = (lb + rb) / 2;

            void* ele = Zeta_CircularVector_Access(&cv, NULL, NULL, mb);

            if (Compare(context, ele)) {
                rb = mb;
                ret_ele = ele;
            } else {
                lb = mb + 1;
            }
        }

        ret_seg_idx = lb;
    }

    if (dst_cursor != NULL) {
        dst_cursor->sv = sv;
        dst_cursor->idx = ret_base_idx + ret_seg_idx;
        dst_cursor->n = ret_n;
        dst_cursor->seg_idx = ret_seg_idx;
        dst_cursor->ele = ret_ele;
    }

    if (dst_ele != NULL && ret_ele != NULL) {
        Zeta_MemCopy(dst_ele, ret_ele, width);
    }

    return ret_ele;
}

void* Zeta_SegVector_Refer(void* sv_, void* pos_cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* pos_cursor = pos_cursor_;
    Zeta_SegVector_Cursor_Check(sv, pos_cursor);

    return pos_cursor->ele;
}

void Zeta_SegVector_Read(void* sv_, void* pos_cursor_, size_t cnt, void* dst_,
                         void* dst_cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* pos_cursor = pos_cursor_;
    Zeta_SegVector_Cursor_Check(sv, pos_cursor);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    if (cnt == 0) {
        if (dst_cursor != NULL) {
            dst_cursor->sv = sv;
            dst_cursor->idx = pos_cursor->idx;
            dst_cursor->n = pos_cursor->n;
            dst_cursor->seg_idx = pos_cursor->seg_idx;
            dst_cursor->ele = pos_cursor->ele;
        }

        return;
    }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = sv->width;
    size_t stride = sv->stride;
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_SegVector_GetSize(sv);

    size_t end = pos_cursor->idx + cnt;
    ZETA_DebugAssert(end <= size);

    unsigned char* dst = dst_;
    ZETA_DebugAssert(dst != NULL);

    void* n = pos_cursor->n;
    Zeta_SegVector_Node* node;

    size_t seg_idx = pos_cursor->seg_idx;

    Zeta_CircularVector cv;
    cv.width = width;
    cv.stride = stride;
    cv.capacity = seg_capacity;

    while (0 < cnt) {
        node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, n);

        cv.data = node->seg;
        cv.offset = node->offset;
        cv.size = node->size;

        size_t cur_cnt = ZETA_GetMinOf(cnt, cv.size - seg_idx);

        Zeta_CircularVector cv_cursor;
        Zeta_CircularVector_Access(&cv, &cv_cursor, NULL, seg_idx);
        Zeta_CircularVector_Read(&cv, &cv_cursor, cur_cnt, dst, NULL);

        dst += stride * cur_cnt;

        seg_idx += cur_cnt;
        cnt -= cur_cnt;

        if (seg_idx == cv.size) {
            n = Zeta_BinTree_StepR(&btn_opr, n);
            seg_idx = 0;
        }
    }

    if (dst_cursor == NULL) { return; }

    dst_cursor->sv = sv;
    dst_cursor->idx = end;
    dst_cursor->n = n;
    dst_cursor->seg_idx = seg_idx;

    if (sv->rb == n) {
        dst_cursor->ele = NULL;
        return;
    }

    node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, n);

    cv.data = node->seg;
    cv.offset = node->offset;
    cv.size = node->size;

    dst_cursor->ele = Zeta_CircularVector_Access(&cv, NULL, NULL, seg_idx);
}

void Zeta_SegVector_Write(void* sv_, void* pos_cursor_, size_t cnt,
                          void const* src_, void* dst_cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* pos_cursor = pos_cursor_;
    Zeta_SegVector_Cursor_Check(sv, pos_cursor);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    if (cnt == 0) {
        if (dst_cursor != NULL) {
            dst_cursor->sv = sv;
            dst_cursor->idx = pos_cursor->idx;
            dst_cursor->n = pos_cursor->n;
            dst_cursor->seg_idx = pos_cursor->seg_idx;
            dst_cursor->ele = pos_cursor->ele;
        }

        return;
    }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = sv->width;
    size_t stride = sv->stride;
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_SegVector_GetSize(sv);

    size_t end = pos_cursor->idx + cnt;
    ZETA_DebugAssert(end <= size);

    unsigned char const* src = src_;
    ZETA_DebugAssert(src != NULL);

    void* n = pos_cursor->n;
    Zeta_SegVector_Node* node;

    size_t seg_idx = pos_cursor->seg_idx;

    Zeta_CircularVector cv;
    cv.width = width;
    cv.stride = stride;
    cv.capacity = seg_capacity;

    while (0 < cnt) {
        node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, n);

        cv.data = node->seg;
        cv.offset = node->offset;
        cv.size = node->size;

        size_t cur_cnt = ZETA_GetMinOf(cnt, cv.size - seg_idx);

        Zeta_CircularVector cv_cursor;
        Zeta_CircularVector_Access(&cv, &cv_cursor, NULL, seg_idx);
        Zeta_CircularVector_Write(&cv, &cv_cursor, cur_cnt, src, NULL);

        src += stride * cur_cnt;

        seg_idx += cur_cnt;
        cnt -= cur_cnt;

        if (seg_idx == cv.size) {
            n = Zeta_BinTree_StepR(&btn_opr, n);
            seg_idx = 0;
        }
    }

    if (dst_cursor == NULL) { return; }

    dst_cursor->sv = sv;
    dst_cursor->idx = end;
    dst_cursor->n = n;
    dst_cursor->seg_idx = seg_idx;

    if (sv->rb == n) {
        dst_cursor->ele = NULL;
        return;
    }

    node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, n);

    cv.data = node->seg;
    cv.offset = node->offset;
    cv.size = node->size;

    dst_cursor->ele = Zeta_CircularVector_Access(&cv, NULL, NULL, seg_idx);
}

static Zeta_SegVector_Node* InsertSeg_(Zeta_SegVector* sv,
                                       Zeta_BinTreeNodeOperator* btn_opr,
                                       void* pos) {
    ZETA_DebugAssert(sv->node_allocator != NULL);
    ZETA_DebugAssert(sv->node_allocator->Allocate != NULL);

    ZETA_DebugAssert(sv->seg_allocator != NULL);
    ZETA_DebugAssert(sv->seg_allocator->Allocate != NULL);

    void* node_ = sv->node_allocator->Allocate(sv->node_allocator->context,
                                               sizeof(Zeta_SegVector_Node));
    ZETA_DebugAssert(node_ != NULL);
    ZETA_DebugAssert(
        ZETA_GetAddrFromPtr(node_) % alignof(Zeta_SegVector_Node) == 0);

    Zeta_SegVector_Node* node = node_;

    Zeta_OrdCntRBTreeNode_Init(NULL, &node->n);
    Zeta_OrdCntRBTreeNode_SetAccSize(NULL, &node->n, 0);

    node->seg = sv->seg_allocator->Allocate(sv->seg_allocator->context,
                                            sv->stride * sv->seg_capacity);
    ZETA_DebugAssert(node->seg != NULL);

    node->offset = 0;
    node->size = 0;

    sv->root = Zeta_RBTree_InsertL(btn_opr, pos, &node->n);

    return node;
}

static void* EraseSeg_(Zeta_SegVector* sv, Zeta_BinTreeNodeOperator* btn_opr,
                       Zeta_SegVector_Node* node) {
    ZETA_DebugAssert(sv->node_allocator != NULL);
    ZETA_DebugAssert(sv->seg_allocator != NULL);

    ZETA_DebugAssert(sv->node_allocator->Deallocate != NULL);
    ZETA_DebugAssert(sv->seg_allocator->Deallocate != NULL);

    void* n = &node->n;

    void* ret = Zeta_BinTree_StepR(btn_opr, n);

    sv->root = Zeta_RBTree_Extract(btn_opr, n);

    sv->seg_allocator->Deallocate(sv->seg_allocator->context, node->seg);
    sv->node_allocator->Deallocate(sv->node_allocator->context, node);

    return ret;
}

void* Zeta_SegVector_PushL(void* sv_, void* dst_cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_SegVector_Cursor pos_cursor;
    Zeta_SegVector_PeekL(sv, &pos_cursor, NULL);

    Zeta_SegVector_Insert(sv, &pos_cursor, 1);

    if (dst_cursor != NULL) {
        dst_cursor->sv = sv;
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->n = pos_cursor.n;
        dst_cursor->seg_idx = pos_cursor.seg_idx;
        dst_cursor->ele = pos_cursor.ele;
    }

    return pos_cursor.ele;
}

void* Zeta_SegVector_PushR(void* sv_, void* dst_cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_SegVector_Cursor pos_cursor;
    Zeta_SegVector_GetRBCursor(sv, &pos_cursor);

    Zeta_SegVector_Insert(sv, &pos_cursor, 1);

    if (dst_cursor != NULL) {
        dst_cursor->sv = sv;
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->n = pos_cursor.n;
        dst_cursor->seg_idx = pos_cursor.seg_idx;
        dst_cursor->ele = pos_cursor.ele;
    }

    return pos_cursor.ele;
}

static void ShoveL_(Zeta_CircularVector* l_cv, Zeta_CircularVector* r_cv,
                    size_t width, size_t rl_cnt, size_t ins_cnt,
                    size_t rot_cnt) {
    size_t num_a = ZETA_GetMinOf(rl_cnt, rot_cnt);
    size_t num_b = ZETA_GetMinOf(ins_cnt, rot_cnt - num_a);
    size_t num_c = rot_cnt - num_a - num_b;

    Zeta_CircularVector_Cursor l_cv_cursor;
    Zeta_CircularVector_Cursor r_cv_cursor;

    Zeta_CircularVector_GetRBCursor(l_cv, &l_cv_cursor);

    Zeta_CircularVector_Insert(l_cv, &l_cv_cursor, rot_cnt);

    Zeta_CircularVector_PeekL(r_cv, &r_cv_cursor, NULL);

    for (size_t i = num_a; 0 < i--;) {
        Zeta_MemCopy(Zeta_CircularVector_Refer(l_cv, &l_cv_cursor),
                     Zeta_CircularVector_Refer(r_cv, &r_cv_cursor), width);

        Zeta_CircularVector_Cursor_StepR(l_cv, &l_cv_cursor);
        Zeta_CircularVector_Cursor_StepR(r_cv, &r_cv_cursor);
    }

    Zeta_CircularVector_Cursor_AdvanceR(l_cv, &l_cv_cursor, num_b);

    for (size_t i = num_c; 0 < i--;) {
        Zeta_MemCopy(Zeta_CircularVector_Refer(l_cv, &l_cv_cursor),
                     Zeta_CircularVector_Refer(r_cv, &r_cv_cursor), width);

        Zeta_CircularVector_Cursor_StepR(l_cv, &l_cv_cursor);
        Zeta_CircularVector_Cursor_StepR(r_cv, &r_cv_cursor);
    }

    Zeta_CircularVector_PeekL(r_cv, &r_cv_cursor, NULL);

    Zeta_CircularVector_Erase(r_cv, &r_cv_cursor, num_a + num_c);

    Zeta_CircularVector_Cursor_AdvanceR(r_cv, &r_cv_cursor, rl_cnt - num_a);

    Zeta_CircularVector_Insert(r_cv, &r_cv_cursor, ins_cnt - num_b);
}

static void ShoveR_(Zeta_CircularVector* l_cv, Zeta_CircularVector* r_cv,
                    size_t width, size_t lr_cnt, size_t ins_cnt,
                    size_t rot_cnt) {
    size_t num_a = ZETA_GetMinOf(lr_cnt, rot_cnt);
    size_t num_b = ZETA_GetMinOf(ins_cnt, rot_cnt - num_a);
    size_t num_c = rot_cnt - num_a - num_b;

    Zeta_CircularVector_Cursor l_cv_cursor;
    Zeta_CircularVector_Cursor r_cv_cursor;

    Zeta_CircularVector_PeekL(r_cv, &r_cv_cursor, NULL);

    Zeta_CircularVector_Insert(r_cv, &r_cv_cursor, rot_cnt);

    Zeta_CircularVector_Access(r_cv, &r_cv_cursor, NULL, rot_cnt - 1);

    Zeta_CircularVector_PeekR(l_cv, &l_cv_cursor, NULL);

    for (size_t i = num_a; 0 < i--;) {
        Zeta_MemCopy(Zeta_CircularVector_Refer(r_cv, &r_cv_cursor),
                     Zeta_CircularVector_Refer(l_cv, &l_cv_cursor), width);

        Zeta_CircularVector_Cursor_StepL(l_cv, &l_cv_cursor);
        Zeta_CircularVector_Cursor_StepL(r_cv, &r_cv_cursor);
    }

    Zeta_CircularVector_Cursor_AdvanceL(r_cv, &r_cv_cursor, num_b);

    for (size_t i = num_c; 0 < i--;) {
        Zeta_MemCopy(Zeta_CircularVector_Refer(r_cv, &r_cv_cursor),
                     Zeta_CircularVector_Refer(l_cv, &l_cv_cursor), width);

        Zeta_CircularVector_Cursor_StepL(l_cv, &l_cv_cursor);
        Zeta_CircularVector_Cursor_StepL(r_cv, &r_cv_cursor);
    }

    Zeta_CircularVector_Access(l_cv, &l_cv_cursor, NULL,
                               l_cv->size - num_a - num_c);

    Zeta_CircularVector_Erase(l_cv, &l_cv_cursor, num_a + num_c);

    Zeta_CircularVector_Cursor_AdvanceL(l_cv, &l_cv_cursor, lr_cnt - num_a);

    Zeta_CircularVector_Insert(l_cv, &l_cv_cursor, ins_cnt - num_b);
}

void* Zeta_SegVector_Insert(void* sv_, void* pos_cursor_, size_t cnt) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* pos_cursor = pos_cursor_;
    Zeta_SegVector_Cursor_Check(sv, pos_cursor);

    if (cnt == 0) { return pos_cursor->ele; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = sv->width;
    size_t stride = sv->stride;
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_SegVector_GetSize(sv);

    void* m_n = pos_cursor->n;
    size_t seg_idx = pos_cursor->seg_idx;

    ZETA_DebugAssert(sv->lb != m_n);

    Zeta_SegVector_Node* m_node;

    if (size == 0) {
        while (0 < cnt) {
            size_t cur_cnt = cnt / ((cnt + seg_capacity - 1) / seg_capacity);

            m_node = InsertSeg_(sv, &btn_opr, m_n);
            m_n = &m_node->n;

            m_node->size = cur_cnt;
            Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->size);

            cnt -= cur_cnt;
        }

        Zeta_CircularVector m_cv;
        m_cv.data = m_node->seg;
        m_cv.width = width;
        m_cv.stride = stride;
        m_cv.offset = 0;
        m_cv.size = m_node->size;
        m_cv.capacity = seg_capacity;

        pos_cursor->n = m_n;
        pos_cursor->seg_idx = 0;
        pos_cursor->ele = Zeta_CircularVector_Access(&m_cv, NULL, NULL, 0);

        return pos_cursor->ele;
    }

    if (sv->rb == m_n) {
        m_n = Zeta_BinTree_StepL(&btn_opr, m_n);
        m_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, m_n);
        seg_idx = m_node->size;
    } else {
        m_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, m_n);
    }

    Zeta_CircularVector m_cv;
    m_cv.data = m_node->seg;
    m_cv.width = width;
    m_cv.stride = stride;
    m_cv.offset = m_node->offset;
    m_cv.size = m_node->size;
    m_cv.capacity = seg_capacity;

    size_t m_vacant = seg_capacity - m_cv.size;

    if (cnt <= m_vacant) {
        Zeta_CircularVector_Cursor cv_cursor;
        Zeta_CircularVector_Access(&m_cv, &cv_cursor, NULL, seg_idx);

        void* ele = Zeta_CircularVector_Insert(&m_cv, &cv_cursor, cnt);

        m_node->offset = m_cv.offset;
        m_node->size = m_cv.size;
        Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->size);

        pos_cursor->n = m_n;
        pos_cursor->seg_idx = seg_idx;
        pos_cursor->ele = ele;

        return ele;
    }

    void* l_n = Zeta_BinTree_StepL(&btn_opr, m_n);
    void* r_n = Zeta_BinTree_StepR(&btn_opr, m_n);

    Zeta_SegVector_Node* l_node;
    Zeta_SegVector_Node* r_node;

    size_t l_vacant;
    size_t r_vacant;

    if (sv->lb == l_n) {
        l_node = NULL;
        l_vacant = 0;
    } else {
        l_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, l_n);
        l_vacant = seg_capacity - l_node->size;
    }

    if (sv->rb == r_n) {
        r_node = NULL;
        r_vacant = 0;
    } else {
        r_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, r_n);
        r_vacant = seg_capacity - r_node->size;
    }

    size_t m_l_move_cnt = seg_idx;
    size_t m_r_move_cnt = m_cv.size - seg_idx;

    size_t rand = Zeta_SimpleHash(m_l_move_cnt + m_r_move_cnt) % 2;

    if (r_vacant < l_vacant) { goto TRY_L_SHOVE; }
    if (l_vacant < r_vacant) { goto TRY_R_SHOVE; }

    if (m_l_move_cnt < m_r_move_cnt) { goto TRY_L_SHOVE; }
    if (m_r_move_cnt < m_l_move_cnt) { goto TRY_R_SHOVE; }

    if (Zeta_SimpleRandomRotate(&rand) % 2 == 0) {
        goto TRY_L_SHOVE;
    } else {
        goto TRY_R_SHOVE;
    }

TRY_L_SHOVE:;

    if (l_vacant + m_vacant < cnt) { goto TRY_SPLIT; }

    Zeta_CircularVector l_cv;
    l_cv.data = l_node->seg;
    l_cv.width = width;
    l_cv.stride = stride;
    l_cv.offset = l_node->offset;
    l_cv.size = seg_capacity - l_vacant;
    l_cv.capacity = seg_capacity;

    // l_size + m_l_move_cnt + cnt + m_r_move_cnt

    seg_idx = l_cv.size + m_l_move_cnt;

    size_t old_l_size = l_cv.size;
    size_t new_lm_size = old_l_size + m_cv.size + cnt;
    size_t new_l_size = (new_lm_size + (m_cv.size < old_l_size)) / 2;

    ShoveL_(&l_cv, &m_cv, width, m_l_move_cnt, cnt, new_l_size - old_l_size);

    l_node->offset = l_cv.offset;
    l_node->size = l_cv.size;
    Zeta_BinTree_SetSize(&btn_opr, l_n, l_node->size);

    m_node->offset = m_cv.offset;
    m_node->size = m_cv.size;
    Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->size);

    if (seg_idx < l_cv.size) {
        pos_cursor->n = l_n;
        pos_cursor->seg_idx = seg_idx;
        pos_cursor->ele =
            Zeta_CircularVector_Access(&l_cv, NULL, NULL, seg_idx);
    } else {
        seg_idx -= l_cv.size;

        pos_cursor->n = m_n;
        pos_cursor->seg_idx = seg_idx;
        pos_cursor->ele =
            Zeta_CircularVector_Access(&m_cv, NULL, NULL, seg_idx);
    }

    return pos_cursor->ele;

TRY_R_SHOVE:;

    if (m_vacant + r_vacant < cnt) { goto TRY_SPLIT; }

    Zeta_CircularVector r_cv;
    r_cv.data = r_node->seg;
    r_cv.width = width;
    r_cv.stride = stride;
    r_cv.offset = r_node->offset;
    r_cv.size = seg_capacity - r_vacant;
    r_cv.capacity = seg_capacity;

    // m_l_move_cnt + cnt + m_r_move_cnt + r_size

    seg_idx = m_l_move_cnt;

    size_t old_r_size = r_cv.size;
    size_t new_mr_size = old_r_size + m_cv.size + cnt;
    size_t new_r_size = (new_mr_size + (m_cv.size < old_r_size)) / 2;

    ShoveR_(&m_cv, &r_cv, width, m_r_move_cnt, cnt, new_r_size - old_r_size);

    m_node->offset = m_cv.offset;
    m_node->size = m_cv.size;
    Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->size);

    r_node->offset = r_cv.offset;
    r_node->size = r_cv.size;
    Zeta_BinTree_SetSize(&btn_opr, r_n, r_node->size);

    if (seg_idx < m_cv.size) {
        pos_cursor->n = m_n;
        pos_cursor->seg_idx = seg_idx;
        pos_cursor->ele =
            Zeta_CircularVector_Access(&m_cv, NULL, NULL, seg_idx);
    } else {
        seg_idx -= m_cv.size;

        pos_cursor->n = r_n;
        pos_cursor->seg_idx = seg_idx;
        pos_cursor->ele =
            Zeta_CircularVector_Access(&r_cv, NULL, NULL, seg_idx);
    }

    return pos_cursor->ele;

TRY_SPLIT:;

    size_t res_cnt = cnt % seg_capacity;
    size_t new_seg_cnt = cnt / seg_capacity;

    void* ml_n;
    void* mr_n;

    Zeta_SegVector_Node* ml_node;
    Zeta_SegVector_Node* mr_node;

    bool_t l_split =
        m_l_move_cnt < m_r_move_cnt ||
        ((m_l_move_cnt == m_r_move_cnt) && Zeta_SimpleRandomRotate(&rand) == 1);

    if (m_cv.size == 0) { l_split = true; }

    if (l_split) {
        if (m_l_move_cnt == 0 && res_cnt == 0) {
            ml_node = l_node;
            ml_n = l_n;

            seg_idx += sv->lb == l_n ? 0 : l_node->size;
        } else {
            ml_node = InsertSeg_(sv, &btn_opr, m_n);
            ml_n = &ml_node->n;
        }

        mr_node = m_node;
        mr_n = m_n;
    } else {
        ml_node = m_node;
        ml_n = m_n;

        if (m_r_move_cnt == 0 && res_cnt == 0) {
            mr_node = r_node;
            mr_n = r_n;
        } else {
            mr_node = InsertSeg_(sv, &btn_opr, r_n);
            mr_n = &mr_node->n;
        }
    }

    Zeta_CircularVector ml_cv;
    ml_cv.width = width;
    ml_cv.stride = stride;

    if (sv->lb == ml_n) {
        ml_cv.data = NULL;
        ml_cv.offset = 0;
        ml_cv.size = 0;
        ml_cv.capacity = 0;
    } else {
        ml_cv.data = ml_node->seg;
        ml_cv.offset = ml_node->offset;
        ml_cv.size = ml_node->size;
        ml_cv.capacity = seg_capacity;
    }

    Zeta_CircularVector mr_cv;
    mr_cv.width = width;
    mr_cv.stride = stride;

    if (sv->rb == mr_n) {
        mr_cv.data = NULL;
        mr_cv.offset = 0;
        mr_cv.size = 0;
        mr_cv.capacity = 0;
    } else {
        mr_cv.data = mr_node->seg;
        mr_cv.offset = mr_node->offset;
        mr_cv.size = mr_node->size;
        mr_cv.capacity = seg_capacity;
    }

    // m_l_move_cnt + res_cnt + m_r_move_cnt

    if (l_split) {
        if (0 < m_l_move_cnt || 0 < res_cnt) {
            size_t new_mlr_size = m_cv.size + res_cnt;
            size_t new_ml_size = new_mlr_size / 2;
            new_ml_size = ZETA_GetMaxOf(new_ml_size, m_l_move_cnt);
            new_ml_size = ZETA_GetMinOf(new_ml_size, m_l_move_cnt + res_cnt);

            ZETA_DebugAssert(0 < new_ml_size);

            ShoveL_(&ml_cv, &mr_cv, width, m_l_move_cnt, res_cnt, new_ml_size);
        }
    } else {
        if (0 < m_r_move_cnt || 0 < res_cnt) {
            size_t new_mlr_size = m_cv.size + res_cnt;
            size_t new_mr_size = new_mlr_size / 2;
            new_mr_size = ZETA_GetMaxOf(new_mr_size, m_r_move_cnt);
            new_mr_size = ZETA_GetMinOf(new_mr_size, m_r_move_cnt + res_cnt);

            ZETA_DebugAssert(0 < new_mr_size);

            ShoveR_(&ml_cv, &mr_cv, width, m_r_move_cnt, res_cnt, new_mr_size);
        }
    }

    if (sv->lb != ml_n) {
        ml_node->offset = ml_cv.offset;
        ml_node->size = ml_cv.size;
        Zeta_BinTree_SetSize(&btn_opr, ml_n, ml_node->size);
    }

    if (sv->rb != mr_n) {
        mr_node->offset = mr_cv.offset;
        mr_node->size = mr_cv.size;
        Zeta_BinTree_SetSize(&btn_opr, mr_n, mr_node->size);
    }

    while (0 < new_seg_cnt--) {
        mr_node = InsertSeg_(sv, &btn_opr, mr_n);
        mr_n = &mr_node->n;

        mr_node->size = seg_capacity;
        Zeta_BinTree_SetSize(&btn_opr, mr_n, mr_node->size);
    }

    if (seg_idx < ml_cv.size) {
        pos_cursor->n = ml_n;
        pos_cursor->seg_idx = seg_idx;
        pos_cursor->ele =
            Zeta_CircularVector_Access(&ml_cv, NULL, NULL, seg_idx);
    } else {
        seg_idx -= ml_cv.size;

        pos_cursor->n = mr_n;
        pos_cursor->seg_idx = seg_idx;

        ZETA_DebugAssert(sv->rb != mr_n);

        if (sv->rb == mr_n) {
            pos_cursor->ele = NULL;
        } else {
            mr_cv.data = mr_node->seg;
            mr_cv.offset = mr_node->offset;
            mr_cv.size = mr_node->size;
            mr_cv.capacity = seg_capacity;

            pos_cursor->ele =
                Zeta_CircularVector_Access(&mr_cv, NULL, NULL, seg_idx);
        }
    }

    return pos_cursor->ele;
}

void Zeta_SegVector_PopL(void* sv) {
    Zeta_SegVector_Cursor pos_cursor;
    Zeta_SegVector_PeekL(sv, &pos_cursor, NULL);

    Zeta_SegVector_Erase(sv, &pos_cursor, 1);
}

void Zeta_SegVector_PopR(void* sv) {
    Zeta_SegVector_Cursor pos_cursor;
    Zeta_SegVector_PeekR(sv, &pos_cursor, NULL);

    Zeta_SegVector_Erase(sv, &pos_cursor, 1);
}

void Zeta_SegVector_Erase(void* sv_, void* pos_cursor_, size_t cnt) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* pos_cursor = pos_cursor_;
    Zeta_SegVector_Cursor_Check(sv, pos_cursor);

    if (cnt == 0) { return; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t width = sv->width;
    size_t stride = sv->stride;
    size_t seg_capacity = sv->seg_capacity;
    size_t size = Zeta_SegVector_GetSize(sv);

    ZETA_DebugAssert(pos_cursor->idx <= size);
    ZETA_DebugAssert(pos_cursor->idx + cnt <= size);

    if (cnt == 0) { return; }

    void* m_n = pos_cursor->n;

    Zeta_SegVector_Node* m_node =
        ZETA_GetStructFromMember(Zeta_SegVector_Node, n, m_n);
    size_t seg_idx = pos_cursor->seg_idx;

    void* ret_n;
    size_t ret_seg_idx;

    Zeta_CircularVector m_cv;
    m_cv.data = m_node->seg;
    m_cv.width = width;
    m_cv.stride = stride;
    m_cv.offset = m_node->offset;
    m_cv.size = m_node->size;
    m_cv.capacity = seg_capacity;

    {
        size_t k = ZETA_GetMinOf(cnt, m_cv.size - seg_idx);

        Zeta_CircularVector_Cursor cv_cursor;
        Zeta_CircularVector_Access(&m_cv, &cv_cursor, NULL, seg_idx);
        Zeta_CircularVector_Erase(&m_cv, &cv_cursor, k);

        cnt -= k;
    }

    void* r_n = Zeta_BinTree_StepR(&btn_opr, m_n);
    Zeta_SegVector_Node* r_node;

    Zeta_CircularVector r_cv;
    r_cv.width = width;
    r_cv.stride = stride;

    for (;;) {
        if (sv->rb == r_n) {
            r_node = NULL;

            r_cv.data = NULL;
            r_cv.offset = 0;
            r_cv.size = 0;
            r_cv.capacity = 0;
        } else {
            r_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, r_n);

            r_cv.data = r_node->seg;
            r_cv.offset = r_node->offset;
            r_cv.size = r_node->size;
            r_cv.capacity = seg_capacity;
        }

        if (cnt == 0) { break; }

        if (cnt < r_cv.size) {
            Zeta_CircularVector_Cursor r_cv_cursor;
            Zeta_CircularVector_PeekL(&r_cv, &r_cv_cursor, NULL);
            Zeta_CircularVector_Erase(&r_cv, &r_cv_cursor, cnt);

            break;
        }

        cnt -= r_cv.size;

        r_n = EraseSeg_(sv, &btn_opr, r_node);
    }

    if (m_cv.size == 0) {
        EraseSeg_(sv, &btn_opr, m_node);

        if (sv->rb == r_n) {
            ret_n = r_n;
            ret_seg_idx = 0;
            goto RET;
        }

        m_n = r_n;
        m_node = r_node;

        m_cv.data = r_cv.data;
        m_cv.offset = r_cv.offset;
        m_cv.size = r_cv.size;

        r_n = Zeta_BinTree_StepR(&btn_opr, r_n);

        if (sv->rb == r_n) {
            r_node = NULL;

            r_cv.data = NULL;
            r_cv.offset = 0;
            r_cv.size = 0;
            r_cv.capacity = 0;
        } else {
            r_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, r_n);

            r_cv.data = r_node->seg;
            r_cv.offset = r_node->offset;
            r_cv.size = r_node->size;
        }

        seg_idx = 0;
    }

    if (seg_capacity <= m_cv.size * 2) {
        m_node->offset = m_cv.offset;
        m_node->size = m_cv.size;
        Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->size);

        if (sv->rb != r_n) {
            r_node->offset = r_cv.offset;
            r_node->size = r_cv.size;
            Zeta_BinTree_SetSize(&btn_opr, r_n, r_node->size);
        }

        if (seg_idx < m_cv.size) {
            ret_n = m_n;
            ret_seg_idx = seg_idx;
        } else {
            ret_n = r_n;
            ret_seg_idx = 0;
        }

        goto RET;
    }

    void* l_n = Zeta_BinTree_StepL(&btn_opr, m_n);
    Zeta_SegVector_Node* l_node;

    Zeta_CircularVector l_cv;
    l_cv.width = width;
    l_cv.stride = stride;

    if (sv->lb == l_n) {
        l_node = NULL;

        l_cv.data = NULL;
        l_cv.offset = 0;
        l_cv.size = 0;
        l_cv.capacity = 0;
    } else {
        l_node = ZETA_GetStructFromMember(Zeta_SegVector_Node, n, l_n);

        l_cv.data = l_node->seg;
        l_cv.offset = l_node->offset;
        l_cv.size = l_node->size;
        l_cv.capacity = seg_capacity;
    }

    size_t l_vacant = l_cv.capacity - l_cv.size;
    size_t m_vacant = seg_capacity - m_cv.size;
    size_t r_vacant = r_cv.capacity - r_cv.size;

    size_t lm_vacant = l_vacant + m_vacant;
    size_t mr_vacant = m_vacant + r_vacant;

    void* a_n;

    Zeta_CircularVector* a_cv;
    Zeta_CircularVector* b_cv;

    size_t a_vacant;
    size_t b_vacant;

    size_t rand = ZETA_GetAddrFromPtr(l_n) + ZETA_GetAddrFromPtr(m_n) +
                  ZETA_GetAddrFromPtr(r_n);

    if (lm_vacant < mr_vacant ||
        (lm_vacant == mr_vacant && Zeta_SimpleRandomRotate(&rand) % 2 == 0)) {
        a_n = m_n;
        a_cv = &m_cv;
        a_vacant = m_vacant;

        b_cv = &r_cv;
        b_vacant = r_vacant;
    } else {
        a_n = l_n;
        a_cv = &l_cv;
        a_vacant = l_vacant;

        b_cv = &m_cv;
        b_vacant = m_vacant;
    }

    size_t ab_vacant = a_vacant + b_vacant;

    // if (ab_vacant < seg_capacity * 1.5) { return; }
    if (ab_vacant * 4 < seg_capacity * 5) {
        m_node->offset = m_cv.offset;
        m_node->size = m_cv.size;
        Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->size);

        if (sv->rb != r_n) {
            r_node->offset = r_cv.offset;
            r_node->size = r_cv.size;
            Zeta_BinTree_SetSize(&btn_opr, r_n, r_node->size);
        }

        if (seg_idx < m_cv.size) {
            ret_n = m_n;
            ret_seg_idx = seg_idx;
        } else {
            ret_n = r_n;
            ret_seg_idx = 0;
        }

        goto RET;
    }

    if (a_vacant < b_vacant ||
        (a_vacant == b_vacant &&
         Zeta_SimpleRandomRotate(&rand) % 2 == 0)) {  // b to a
        if (a_n == m_n) {                             // r(b) to m(a)
            ret_n = m_n;
            ret_seg_idx = seg_idx;
        } else if (seg_idx < m_cv.size) {  // m(b) to l(a)
            ret_n = l_n;
            ret_seg_idx = l_cv.size + seg_idx;
        } else {
            ret_n = r_n;
            ret_seg_idx = 0;
        }

        ShoveL_(a_cv, b_cv, width, 0, 0, b_cv->size);
    } else {               // a to b
        if (a_n == m_n) {  // m(a) to r(b)
            ret_n = r_n;
            ret_seg_idx = seg_idx;
        } else if (seg_idx < m_cv.size) {  // l(a) to m(b)
            ret_n = m_n;
            ret_seg_idx = l_cv.size + seg_idx;
        } else {
            ret_n = r_n;
            ret_seg_idx = 0;
        }

        ShoveR_(a_cv, b_cv, width, 0, 0, a_cv->size);
    }

    if (sv->lb != l_n) {
        if (l_cv.size == 0) {
            EraseSeg_(sv, &btn_opr, l_node);
        } else {
            l_node->offset = l_cv.offset;
            l_node->size = l_cv.size;
            Zeta_BinTree_SetSize(&btn_opr, l_n, l_node->size);
        }
    }

    if (m_cv.size == 0) {
        EraseSeg_(sv, &btn_opr, m_node);
    } else {
        m_node->offset = m_cv.offset;
        m_node->size = m_cv.size;
        Zeta_BinTree_SetSize(&btn_opr, m_n, m_node->size);
    }

    if (sv->rb != r_n) {
        if (r_cv.size == 0) {
            EraseSeg_(sv, &btn_opr, r_node);
        } else {
            r_node->offset = r_cv.offset;
            r_node->size = r_cv.size;
            Zeta_BinTree_SetSize(&btn_opr, r_n, r_node->size);
        }
    }

RET:;

    void* ele;

    if (sv->rb == ret_n) {
        ele = NULL;
    } else {
        Zeta_SegVector_Node* ret_node =
            ZETA_GetStructFromMember(Zeta_SegVector_Node, n, ret_n);

        m_cv.data = ret_node->seg;
        m_cv.offset = ret_node->offset;
        m_cv.size = ret_node->size;

        ele = Zeta_CircularVector_Access(&m_cv, NULL, NULL, ret_seg_idx);
    }

    pos_cursor->n = ret_n;
    pos_cursor->seg_idx = ret_seg_idx;
    pos_cursor->ele = ele;
}

static void EraseAll_(Zeta_SegVector* sv, Zeta_BinTreeNodeOperator* btn_opr,
                      Zeta_CircularVector* cv, void* callback_context,
                      void (*Callback)(void* context, void* ele), void* n) {
    void* nl = Zeta_OrdCntRBTreeNode_GetL(NULL, n);
    void* nr = Zeta_OrdCntRBTreeNode_GetR(NULL, n);

    if (sv->lb != n && sv->rb != n) {
        Zeta_SegVector_Node* node =
            ZETA_GetStructFromMember(Zeta_SegVector_Node, n, n);

        cv->offset = node->offset;
        cv->size = node->size;
        cv->data = node->seg;

        Zeta_CircularVector_EraseAll(cv, callback_context, Callback);

        void (*DeallocateNode)(void* context, void* ptr) =
            sv->node_allocator->Deallocate;
        ZETA_DebugAssert(DeallocateNode != NULL);

        void (*DeallocateSeg)(void* context, void* ptr) =
            sv->seg_allocator->Deallocate;
        ZETA_DebugAssert(DeallocateSeg != NULL);

        DeallocateSeg(sv->seg_allocator->context, node->seg);
        DeallocateNode(sv->node_allocator->context, node);
    }

    if (nl != NULL) {
        EraseAll_(sv, btn_opr, cv, callback_context, Callback, nl);
    }

    if (nr != NULL) {
        EraseAll_(sv, btn_opr, cv, callback_context, Callback, nr);
    }
}

void Zeta_SegVector_EraseAll(void* sv_, void* callback_context,
                             void (*Callback)(void* context, void* ele)) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_CircularVector cv;
    cv.width = sv->width;
    cv.stride = sv->stride;
    cv.capacity = sv->seg_capacity;

    EraseAll_(sv, &btn_opr, &cv, callback_context, Callback, sv->root);

    Zeta_SegVector_Init(sv);
}

void Check_(Zeta_SegVector* sv, Zeta_DebugHashMap* dst_node_hm,
            Zeta_DebugHashMap* dst_seg_hm, Zeta_BinTreeNodeOperator* btn_opr,
            void* n) {
    if (sv->lb == n) {
        ZETA_DebugAssert(Zeta_BinTree_StepL(btn_opr, n) == NULL);

        ZETA_DebugAssert(Zeta_BinTree_GetSize(btn_opr, n) == 1);

        Zeta_DebugHashMap_KeyValPair p =
            Zeta_DebugHashMap_Insert(dst_node_hm, ZETA_GetAddrFromPtr(sv->lb));
        ZETA_DebugAssert(p.b);
        *p.val = sizeof(Zeta_OrdCntRBTreeNode);
    } else if (sv->rb == n) {
        ZETA_DebugAssert(Zeta_BinTree_StepR(btn_opr, n) == NULL);

        ZETA_DebugAssert(Zeta_BinTree_GetSize(btn_opr, n) == 1);

        Zeta_DebugHashMap_KeyValPair p =
            Zeta_DebugHashMap_Insert(dst_node_hm, ZETA_GetAddrFromPtr(sv->rb));
        ZETA_DebugAssert(p.b);
        *p.val = sizeof(Zeta_OrdCntRBTreeNode);
    } else {
        Zeta_SegVector_Node* node =
            ZETA_GetStructFromMember(Zeta_SegVector_Node, n, n);

        {
            Zeta_DebugHashMap_KeyValPair p = Zeta_DebugHashMap_Insert(
                dst_node_hm, ZETA_GetAddrFromPtr(node));
            ZETA_DebugAssert(p.b);
            *p.val = sizeof(Zeta_SegVector_Node);
        }

        size_t size = node->size;

        ZETA_DebugAssert(size == Zeta_BinTree_GetSize(btn_opr, n));

        ZETA_DebugAssert(0 < size);
        ZETA_DebugAssert(size <= sv->seg_capacity);
        ZETA_DebugAssert(node->offset < sv->seg_capacity);

        {
            Zeta_DebugHashMap_KeyValPair p = Zeta_DebugHashMap_Insert(
                dst_seg_hm, ZETA_GetAddrFromPtr(node->seg));
            ZETA_DebugAssert(p.b);
            *p.val = sv->stride * sv->seg_capacity;
        }
    }

    void* l_n = Zeta_OrdCntRBTreeNode_GetL(NULL, n);
    if (l_n != NULL) { Check_(sv, dst_node_hm, dst_seg_hm, btn_opr, l_n); }

    void* r_n = Zeta_OrdCntRBTreeNode_GetR(NULL, n);
    if (r_n != NULL) { Check_(sv, dst_node_hm, dst_seg_hm, btn_opr, r_n); }
}

void Zeta_SegVector_Check(void* sv_, Zeta_DebugHashMap* dst_node_hm,
                          Zeta_DebugHashMap* dst_seg_hm) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Check_(sv, dst_node_hm, dst_seg_hm, &btn_opr, sv->root);
}

bool_t Zeta_SegVector_Cursor_IsEqual(void* sv_, void const* cursor_a_,
                                     void const* cursor_b_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_SegVector_Cursor const* cursor_a = cursor_a_;
    Zeta_SegVector_Cursor const* cursor_b = cursor_b_;

    Zeta_SegVector_Cursor_Check(sv, cursor_a);
    Zeta_SegVector_Cursor_Check(sv, cursor_b);

    return cursor_a->idx == cursor_b->idx;
}

int Zeta_SegVector_Cursor_Compare(void* sv_, void const* cursor_a_,
                                  void const* cursor_b_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_SegVector_Cursor const* cursor_a = cursor_a_;
    Zeta_SegVector_Cursor const* cursor_b = cursor_b_;

    Zeta_SegVector_Cursor_Check(sv, cursor_a);
    Zeta_SegVector_Cursor_Check(sv, cursor_b);

    size_t ka = cursor_a->idx + 1;
    size_t kb = cursor_b->idx + 1;

    if (ka < kb) { return -1; }
    if (kb < ka) { return 1; }
    return 0;
}

size_t Zeta_SegVector_Cursor_GetDist(void* sv_, void const* cursor_a_,
                                     void const* cursor_b_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_SegVector_Cursor const* cursor_a = cursor_a_;
    Zeta_SegVector_Cursor const* cursor_b = cursor_b_;

    Zeta_SegVector_Cursor_Check(sv, cursor_a);
    Zeta_SegVector_Cursor_Check(sv, cursor_b);

    return cursor_a->idx - cursor_b->idx;
}

size_t Zeta_SegVector_Cursor_GetIdx(void* sv_, void const* cursor_) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    Zeta_SegVector_Cursor const* cursor = cursor_;
    Zeta_SegVector_Cursor_Check(sv, cursor);

    return cursor->idx;
}

void Zeta_SegVector_Cursor_StepL(void* sv, void* cursor) {
    Zeta_SegVector_Cursor_AdvanceL(sv, cursor, 1);
}

void Zeta_SegVector_Cursor_StepR(void* sv, void* cursor) {
    Zeta_SegVector_Cursor_AdvanceR(sv, cursor, 1);
}

void Zeta_SegVector_Cursor_AdvanceL(void* sv_, void* cursor_, size_t step) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* cursor = cursor_;
    Zeta_SegVector_Cursor_Check(sv, cursor);

    if (step == 0) { return; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t size = Zeta_SegVector_GetSize(sv);

    ZETA_DebugAssert(cursor->idx - step + 1 < size + 2);

    void* dst_n;
    size_t dst_seg_idx;

    void* n = cursor->n;

    size_t n_size =
        (sv->lb == n || sv->rb == n)
            ? 1
            : ZETA_GetStructFromMember(Zeta_SegVector_Node, n, n)->size;

    Zeta_BinTree_AdvanceL(&dst_n, &dst_seg_idx, &btn_opr, n,
                          n_size - 1 - cursor->seg_idx + step);

    ZETA_DebugAssert(dst_n != NULL);

    cursor->idx -= step;
    cursor->n = dst_n;

    if (sv->lb == dst_n) {
        cursor->seg_idx = 0;
        cursor->ele = NULL;
        return;
    }

    Zeta_SegVector_Node* dst_node =
        ZETA_GetStructFromMember(Zeta_SegVector_Node, n, dst_n);

    dst_seg_idx = dst_node->size - 1 - dst_seg_idx;

    cursor->seg_idx = dst_seg_idx;

    Zeta_CircularVector cv;
    cv.data = dst_node->seg;
    cv.width = sv->width;
    cv.stride = sv->stride;
    cv.offset = dst_node->offset;
    cv.size = dst_node->size;
    cv.capacity = sv->seg_capacity;

    cursor->ele = Zeta_CircularVector_Access(&cv, NULL, NULL, dst_seg_idx);
}

void Zeta_SegVector_Cursor_AdvanceR(void* sv_, void* cursor_, size_t step) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor* cursor = cursor_;
    Zeta_SegVector_Cursor_Check(sv, cursor);

    if (step == 0) { return; }

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_BinTree_InitOpr(&btn_opr);
    Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    size_t size = Zeta_SegVector_GetSize(sv);

    ZETA_DebugAssert(cursor->idx + step + 1 < size + 2);

    void* dst_n;
    size_t dst_seg_idx;

    Zeta_BinTree_AdvanceR(&dst_n, &dst_seg_idx, &btn_opr, cursor->n,
                          cursor->seg_idx + step);

    ZETA_DebugAssert(dst_n != NULL);

    cursor->idx += step;
    cursor->n = dst_n;
    cursor->seg_idx = dst_seg_idx;

    if (sv->rb == dst_n) {
        cursor->ele = NULL;
        return;
    }

    Zeta_SegVector_Node* dst_node =
        ZETA_GetStructFromMember(Zeta_SegVector_Node, n, dst_n);

    Zeta_CircularVector cv;
    cv.data = dst_node->seg;
    cv.width = sv->width;
    cv.stride = sv->stride;
    cv.offset = dst_node->offset;
    cv.size = dst_node->size;
    cv.capacity = sv->seg_capacity;

    cursor->ele = Zeta_CircularVector_Access(&cv, NULL, NULL, dst_seg_idx);
}

void Zeta_SegVector_Cursor_Check(void* sv_, void const* cursor_) {
    if (!ZETA_IsDebug) { return; }

    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    Zeta_SegVector_Cursor const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    Zeta_SegVector_Cursor re_cursor;
    Zeta_SegVector_Access(sv, &re_cursor, NULL, cursor->idx);

    ZETA_DebugAssert(re_cursor.sv == cursor->sv);
    ZETA_DebugAssert(re_cursor.idx == cursor->idx);
    ZETA_DebugAssert(re_cursor.n == cursor->n);
    ZETA_DebugAssert(re_cursor.seg_idx == cursor->seg_idx);
    ZETA_DebugAssert(re_cursor.ele == cursor->ele);
}

void Zeta_SegVector_DeploySeqContainer(void* sv_, Zeta_SeqContainer* seq_cntr) {
    Zeta_SegVector* sv = sv_;
    ZETA_DebugAssert(sv != NULL);

    ZETA_DebugAssert(seq_cntr != NULL);

    seq_cntr->context = sv;

    seq_cntr->cursor_width = sizeof(Zeta_SegVector_Cursor);

    seq_cntr->GetWidth = Zeta_SegVector_GetWidth;

    seq_cntr->GetSize = Zeta_SegVector_GetSize;

    seq_cntr->GetLBCursor = Zeta_SegVector_GetLBCursor;

    seq_cntr->GetRBCursor = Zeta_SegVector_GetRBCursor;

    seq_cntr->PeekL = Zeta_SegVector_PeekL;

    seq_cntr->PeekR = Zeta_SegVector_PeekR;

    seq_cntr->Access = Zeta_SegVector_Access;

    seq_cntr->Refer = Zeta_SegVector_Refer;

    seq_cntr->Read = Zeta_SegVector_Read;

    seq_cntr->Write = Zeta_SegVector_Write;

    seq_cntr->PushL = Zeta_SegVector_PushL;

    seq_cntr->PushR = Zeta_SegVector_PushR;

    seq_cntr->Insert = Zeta_SegVector_Insert;

    seq_cntr->PopL = Zeta_SegVector_PopL;

    seq_cntr->PopR = Zeta_SegVector_PopR;

    seq_cntr->Erase = Zeta_SegVector_Erase;

    seq_cntr->EraseAll = Zeta_SegVector_EraseAll;

    seq_cntr->Cursor_IsEqual = Zeta_SegVector_Cursor_IsEqual;

    seq_cntr->Cursor_Compare = Zeta_SegVector_Cursor_Compare;

    seq_cntr->Cursor_GetDist = Zeta_SegVector_Cursor_GetDist;

    seq_cntr->Cursor_GetIdx = Zeta_SegVector_Cursor_GetIdx;

    seq_cntr->Cursor_StepL = Zeta_SegVector_Cursor_StepL;

    seq_cntr->Cursor_StepR = Zeta_SegVector_Cursor_StepR;

    seq_cntr->Cursor_AdvanceL = Zeta_SegVector_Cursor_AdvanceL;

    seq_cntr->Cursor_AdvanceR = Zeta_SegVector_Cursor_AdvanceR;
}
