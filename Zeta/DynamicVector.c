#include "DynamicVector.h"

#include "CircularArray.h"
#include "Debugger.h"
#include "utils.h"

#define CheckDV_(dv) CheckDVCore_((dv))

#define CheckDVCursor_(dv, cursor) CheckDVCursorCore_((dv), (cursor))

static void CheckDVCore_(Zeta_DynamicVector* dv) {
    size_t width = dv->width;
    size_t stride = dv->stride;
    size_t seg_capacity = dv->seg_capacity;

    size_t offset = dv->offset;
    size_t size = dv->size;

    size_t size_a = dv->size_a;
    size_t size_b = dv->size_b;
    size_t size_c = dv->size_c;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);
    ZETA_DebugAssert(0 < seg_capacity);
    ZETA_DebugAssert(offset < seg_capacity);

    ZETA_DebugAssert(ZETA_CeilIntDiv(offset + size, seg_capacity) ==
                     size_a + size_b + size_c);

    Zeta_CircularArray cur_ca;
    cur_ca.data = dv->cur_data;
    cur_ca.width = sizeof(void*);
    cur_ca.stride = sizeof(void*);
    cur_ca.offset = dv->cur_offset;
    cur_ca.size = dv->size_b;
    cur_ca.capacity = dv->cur_capacity;

    Zeta_CircularArray nxt_ca;
    nxt_ca.data = dv->nxt_data;
    nxt_ca.width = sizeof(void*);
    nxt_ca.stride = sizeof(void*);
    nxt_ca.offset = dv->nxt_offset;
    nxt_ca.size = dv->size_a + dv->size_b + dv->size_c;
    nxt_ca.capacity = dv->nxt_capacity;

    if (cur_ca.data != NULL) { Zeta_CircularArray_Check(&cur_ca); }
    if (nxt_ca.data != NULL) { Zeta_CircularArray_Check(&nxt_ca); }

    if (nxt_ca.data == NULL) {
        ZETA_DebugAssert(size_a == 0);
        ZETA_DebugAssert(size_c == 0);
    } else {
        size_t nxt_size = size_a + size_b + size_c;
        ZETA_DebugAssert(nxt_size <= nxt_ca.capacity);

        ZETA_DebugAssert(size_b <= (nxt_ca.capacity - nxt_size) * 2);
    }

    ZETA_DebugAssert(dv->data_allocator != NULL);
    ZETA_DebugAssert(dv->data_allocator->GetAlign != NULL);
    ZETA_DebugAssert(dv->data_allocator->Allocate != NULL);
    ZETA_DebugAssert(dv->data_allocator->Deallocate != NULL);

    ZETA_DebugAssert(dv->seg_allocator != NULL);
    ZETA_DebugAssert(dv->seg_allocator->GetAlign != NULL);
    ZETA_DebugAssert(dv->seg_allocator->Allocate != NULL);
    ZETA_DebugAssert(dv->seg_allocator->Deallocate != NULL);
}

static void CheckDVCursorCore_(Zeta_DynamicVector* dv,
                               Zeta_DynamicVector_Cursor const* cursor) {
    CheckDV_(dv);

    ZETA_DebugAssert(cursor != NULL);

    Zeta_DynamicVector_Cursor re_cursor;
    Zeta_DynamicVector_Access(dv, &re_cursor, NULL, cursor->idx);

    ZETA_DebugAssert(re_cursor.dv == cursor->dv);
    ZETA_DebugAssert(re_cursor.idx == cursor->idx);
    ZETA_DebugAssert(re_cursor.ele == cursor->ele);
}

static void* AllocateData_(Zeta_DynamicVector* dv, size_t capacity) {
    ZETA_DebugAssert(dv->data_allocator != NULL);
    ZETA_DebugAssert(dv->data_allocator->Allocate != NULL);

    void* data = dv->data_allocator->Allocate(dv->data_allocator->context,
                                              sizeof(void*) * capacity);

    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(__builtin_is_aligned(data, alignof(void*)));

    return data;
}

static void* AllocateSeg_(Zeta_DynamicVector* dv) {
    ZETA_DebugAssert(dv->data_allocator != NULL);
    ZETA_DebugAssert(dv->data_allocator->Allocate != NULL);

    void* seg = dv->seg_allocator->Allocate(dv->seg_allocator->context,
                                            dv->stride * dv->seg_capacity);

    ZETA_DebugAssert(seg != NULL);

    return seg;
}

static size_t GetNxtCapacity_(size_t cur_capacity) {
    return ZETA_CeilIntDiv(ZETA_GetMaxOf(8, cur_capacity) * 3, 2);
}

static void TryMigrate_(Zeta_DynamicVector* dv, Zeta_CircularArray* cur_ca,
                        Zeta_CircularArray* nxt_ca, size_t max_cnt) {
    size_t move_cnt = ZETA_GetMinOf(max_cnt, dv->size_b);

    if (nxt_ca->data == NULL || move_cnt == 0) { return; }

    unsigned long long rand_seed = __builtin_readcyclecounter();

    if (Zeta_Choose2(dv->size_c <= dv->size_a, dv->size_a <= dv->size_c,
                     &rand_seed) == 0) {
        Zeta_CircularArray_Assign(nxt_ca, cur_ca, dv->size_a, 0, move_cnt);
        Zeta_CircularArray_PopL(cur_ca, move_cnt);

        dv->size_a += move_cnt;
        dv->size_b -= move_cnt;
    } else {
        Zeta_CircularArray_Assign(nxt_ca, cur_ca,
                                  dv->size_a + dv->size_b - move_cnt,
                                  dv->size_b - move_cnt, move_cnt);

        Zeta_CircularArray_PopR(cur_ca, move_cnt);

        dv->size_b -= move_cnt;
        dv->size_c += move_cnt;
    }

    if (dv->size_b == 0) {
        dv->size_a = 0;
        dv->size_b = nxt_ca->size;
        dv->size_c = 0;

        dv->data_allocator->Deallocate(dv->data_allocator->context,
                                       cur_ca->data);

        cur_ca->data = nxt_ca->data;
        cur_ca->offset = nxt_ca->offset;
        cur_ca->size = nxt_ca->size;
        cur_ca->capacity = nxt_ca->capacity;

        nxt_ca->data = NULL;
        nxt_ca->offset = 0;
        nxt_ca->size = 0;
        nxt_ca->capacity = 0;
    }
}

static void UpdateCA_(Zeta_DynamicVector* dv, Zeta_CircularArray* cur_ca,
                      Zeta_CircularArray* nxt_ca) {
    dv->cur_data = cur_ca->data;
    dv->cur_offset = cur_ca->offset;
    dv->cur_capacity = cur_ca->capacity;

    dv->nxt_data = nxt_ca->data;
    dv->nxt_offset = nxt_ca->offset;
    dv->nxt_capacity = nxt_ca->capacity;
}

static Zeta_DynamicVector_Cursor Access_(Zeta_DynamicVector* dv, size_t idx) {
    if (idx == (size_t)(-1) || idx == dv->size) {
        return (Zeta_DynamicVector_Cursor){ dv, idx, NULL };
    }

    Zeta_CircularArray cur_ca;
    cur_ca.data = dv->cur_data;
    cur_ca.width = sizeof(void*);
    cur_ca.stride = sizeof(void*);
    cur_ca.offset = dv->cur_offset;
    cur_ca.size = dv->size_b;
    cur_ca.capacity = dv->cur_capacity;

    Zeta_CircularArray nxt_ca;
    nxt_ca.data = dv->nxt_data;
    nxt_ca.width = sizeof(void*);
    nxt_ca.stride = sizeof(void*);
    nxt_ca.offset = dv->nxt_offset;
    nxt_ca.size = dv->size_a + dv->size_b + dv->size_c;
    nxt_ca.capacity = dv->nxt_capacity;

    TryMigrate_(dv, &cur_ca, &nxt_ca, 2);

    UpdateCA_(dv, &cur_ca, &nxt_ca);

    size_t offset = dv->offset + idx;

    size_t seg_idx = offset / dv->seg_capacity;
    size_t ele_idx = offset % dv->seg_capacity;

    return (Zeta_DynamicVector_Cursor){
        dv, idx,
        (unsigned char*)(*(void**)((
            dv->size_a <= seg_idx && seg_idx < dv->size_a + dv->size_b
                ? Zeta_CircularArray_Access(&cur_ca, NULL, NULL,
                                            seg_idx - dv->size_a)
                : Zeta_CircularArray_Access(&nxt_ca, NULL, NULL, seg_idx)))) +
            dv->stride * ele_idx
    };
}

static void PushSeg_(Zeta_DynamicVector* dv, int dir, size_t cnt) {
    if (cnt == 0) { return; }

    Zeta_CircularArray cur_ca;
    cur_ca.data = dv->cur_data;
    cur_ca.width = sizeof(void*);
    cur_ca.stride = sizeof(void*);
    cur_ca.offset = dv->cur_offset;
    cur_ca.size = dv->size_b;
    cur_ca.capacity = dv->cur_capacity;

    Zeta_CircularArray nxt_ca;
    nxt_ca.data = dv->nxt_data;
    nxt_ca.width = sizeof(void*);
    nxt_ca.stride = sizeof(void*);
    nxt_ca.offset = dv->nxt_offset;
    nxt_ca.size = dv->size_a + dv->size_b + dv->size_c;
    nxt_ca.capacity = dv->nxt_capacity;

    size_t need_capacity = nxt_ca.size + cnt;
    size_t new_capacity = GetNxtCapacity_(need_capacity);

    if (nxt_ca.data == NULL) {
        if (cur_ca.data == NULL) {
            cur_ca.data = AllocateData_(dv, new_capacity);
            cur_ca.capacity = new_capacity;
        }

        if (need_capacity <= cur_ca.capacity) {
            ZETA_PrintCurPos;

            if (dir == 0) {
                ZETA_PrintCurPos;

                Zeta_CircularArray_PushL(&cur_ca, NULL, cnt);

                for (size_t i = 0; i < cnt; ++i) {
                    *(void**)Zeta_CircularArray_Access(&cur_ca, NULL, NULL, i) =
                        AllocateSeg_(dv);
                }
            } else {
                Zeta_CircularArray_PushR(&cur_ca, NULL, cnt);

                for (size_t i = 0; i < cnt; ++i) {
                    *(void**)Zeta_CircularArray_Access(&cur_ca, NULL, NULL,
                                                       cur_ca.size - 1 - i) =
                        AllocateSeg_(dv);
                }
            }

            dv->cur_data = cur_ca.data;
            dv->cur_offset = cur_ca.offset;
            dv->cur_capacity = cur_ca.capacity;

            dv->size_b = cur_ca.size;

            return;
        }

        nxt_ca.data = AllocateData_(dv, new_capacity);
        nxt_ca.offset = 0;
        nxt_ca.size = dv->size_b;
        nxt_ca.capacity = new_capacity;
    }

    if (nxt_ca.capacity < need_capacity) {
        Zeta_CircularArray_Assign(&nxt_ca, &cur_ca, dv->size_a, 0, dv->size_b);

        dv->data_allocator->Deallocate(dv->data_allocator->context,
                                       cur_ca.data);

        cur_ca.data = nxt_ca.data;
        cur_ca.offset = nxt_ca.offset;
        cur_ca.size = nxt_ca.size;
        cur_ca.capacity = nxt_ca.capacity;

        nxt_ca.data = AllocateData_(dv, new_capacity);
        nxt_ca.offset = 0;
        nxt_ca.capacity = new_capacity;

        dv->size_a = 0;
        dv->size_b = cur_ca.size;
        dv->size_c = 0;
    }

    if (dir == 0) {
        Zeta_CircularArray_PushL(&nxt_ca, NULL, cnt);

        for (size_t i = 0; i < cnt; ++i) {
            *(void**)Zeta_CircularArray_Access(&nxt_ca, NULL, NULL, i) =
                AllocateSeg_(dv);
        }

        dv->size_a += cnt;
    } else {
        Zeta_CircularArray_PushR(&nxt_ca, NULL, cnt);

        for (size_t i = 0; i < cnt; ++i) {
            *(void**)Zeta_CircularArray_Access(
                &nxt_ca, NULL, NULL, nxt_ca.size - 1 - i) = AllocateSeg_(dv);
        }

        dv->size_c += cnt;
    }

    TryMigrate_(dv, &cur_ca, &nxt_ca, 2 * cnt);

    UpdateCA_(dv, &cur_ca, &nxt_ca);
}

static void PopSeg_(Zeta_DynamicVector* dv, int dir, size_t cnt) {
    Zeta_CircularArray cur_ca;
    cur_ca.data = dv->cur_data;
    cur_ca.width = sizeof(void*);
    cur_ca.stride = sizeof(void*);
    cur_ca.offset = dv->cur_offset;
    cur_ca.size = dv->size_b;
    cur_ca.capacity = dv->cur_capacity;

    Zeta_CircularArray nxt_ca;
    nxt_ca.data = dv->nxt_data;
    nxt_ca.width = sizeof(void*);
    nxt_ca.stride = sizeof(void*);
    nxt_ca.offset = dv->nxt_offset;
    nxt_ca.size = dv->size_a + dv->size_b + dv->size_c;
    nxt_ca.capacity = dv->nxt_capacity;

    ZETA_CheckAssert(cnt <= nxt_ca.size);

    size_t origin_cnt = cnt;

    if (nxt_ca.data != NULL && 0 < cnt) {
        if (dir == 0) {
            size_t cur_cnt = ZETA_GetMinOf(cnt, dv->size_a);
            cnt -= cur_cnt;

            dv->size_a -= cur_cnt;

            for (size_t i = 0; i < cur_cnt; ++i) {
                dv->seg_allocator->Deallocate(
                    dv->seg_allocator->context,
                    *(void**)Zeta_CircularArray_Access(&nxt_ca, NULL, NULL, i));
            }

            Zeta_CircularArray_PopL(&nxt_ca, cur_cnt);
        } else {
            size_t cur_cnt = ZETA_GetMinOf(cnt, dv->size_c);
            cnt -= cur_cnt;

            dv->size_c -= cur_cnt;

            for (size_t i = 0; i < cur_cnt; ++i) {
                dv->seg_allocator->Deallocate(
                    dv->seg_allocator->context,
                    *(void**)Zeta_CircularArray_Access(&nxt_ca, NULL, NULL,
                                                       nxt_ca.size - 1 - i));
            }

            Zeta_CircularArray_PopR(&nxt_ca, cur_cnt);
        }
    }

    if (0 < cnt) {
        size_t cur_cnt = ZETA_GetMinOf(cnt, dv->size_b);
        cnt -= cur_cnt;

        dv->size_b -= cur_cnt;

        if (dir == 0) {
            for (size_t i = 0; i < cur_cnt; ++i) {
                dv->seg_allocator->Deallocate(
                    dv->seg_allocator->context,
                    *(void**)Zeta_CircularArray_Access(&cur_ca, NULL, NULL, i));
            }

            Zeta_CircularArray_PopL(&cur_ca, cur_cnt);

            if (nxt_ca.data != NULL) {
                Zeta_CircularArray_PopL(&nxt_ca, cur_cnt);
            }
        } else {
            for (size_t i = 0; i < cur_cnt; ++i) {
                dv->seg_allocator->Deallocate(
                    dv->seg_allocator->context,
                    *(void**)Zeta_CircularArray_Access(&cur_ca, NULL, NULL,
                                                       cur_ca.size - 1 - i));
            }

            Zeta_CircularArray_PopR(&cur_ca, cur_cnt);

            if (nxt_ca.data != NULL) {
                Zeta_CircularArray_PopR(&nxt_ca, cur_cnt);
            }
        }
    }

    if (nxt_ca.data != NULL && 0 < cnt) {
        if (dir == 0) {
            dv->size_a -= cnt;

            for (size_t i = 0; i < cnt; ++i) {
                dv->seg_allocator->Deallocate(
                    dv->seg_allocator->context,
                    *(void**)Zeta_CircularArray_Access(&nxt_ca, NULL, NULL, i));
            }

            Zeta_CircularArray_PopL(&nxt_ca, cnt);
        } else {
            dv->size_c -= cnt;

            for (size_t i = 0; i < cnt; ++i) {
                dv->seg_allocator->Deallocate(
                    dv->seg_allocator->context,
                    *(void**)Zeta_CircularArray_Access(&nxt_ca, NULL, NULL,
                                                       nxt_ca.size - 1 - i));
            }

            Zeta_CircularArray_PopR(&nxt_ca, cnt);
        }
    }

    if (nxt_ca.data == NULL) {
    } else if (dir == 0) {
        size_t move_cnt = ZETA_GetMinOf(2 * origin_cnt, dv->size_b);

        Zeta_CircularArray_Assign(&nxt_ca, &cur_ca, dv->size_a, 0, move_cnt);
        Zeta_CircularArray_PopL(&cur_ca, move_cnt);

        dv->size_a += move_cnt;
        dv->size_b -= move_cnt;
    } else {
        size_t move_cnt = ZETA_GetMinOf(2 * origin_cnt, dv->size_b);

        Zeta_CircularArray_Assign(&nxt_ca, &cur_ca,
                                  dv->size_a + dv->size_b - move_cnt,
                                  dv->size_b - move_cnt, move_cnt);
        Zeta_CircularArray_PopR(&cur_ca, move_cnt);

        dv->size_b -= move_cnt;
        dv->size_c += move_cnt;
    }

    if (dv->size_b == 0) {
        dv->size_b = dv->size_a + dv->size_c;
        dv->size_a = 0;
        dv->size_c = 0;

        dv->data_allocator->Deallocate(dv->data_allocator->context,
                                       cur_ca.data);

        dv->cur_data = nxt_ca.data;
        dv->cur_offset = nxt_ca.offset;
        dv->cur_capacity = nxt_ca.capacity;

        dv->nxt_data = NULL;
        dv->nxt_offset = 0;
        dv->nxt_capacity = 0;
    } else {
        dv->cur_offset = cur_ca.offset;
        dv->nxt_offset = nxt_ca.offset;
    }
}

void Zeta_DynamicVector_Init(void* dv_) {
    Zeta_DynamicVector* dv = dv_;

    size_t width = dv->width;
    size_t stride = dv->stride;
    size_t seg_capacity = dv->seg_capacity;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);
    ZETA_DebugAssert(0 < seg_capacity);

    ZETA_DebugAssert(dv->data_allocator != NULL);
    ZETA_DebugAssert(dv->data_allocator->GetAlign != NULL);
    ZETA_DebugAssert(dv->data_allocator->Allocate != NULL);
    ZETA_DebugAssert(dv->data_allocator->Deallocate != NULL);
    ZETA_DebugAssert(dv->data_allocator->GetAlign(dv->data_allocator->context) %
                         alignof(void*) ==
                     0);

    ZETA_DebugAssert(dv->seg_allocator != NULL);
    ZETA_DebugAssert(dv->seg_allocator->GetAlign != NULL);
    ZETA_DebugAssert(dv->seg_allocator->Allocate != NULL);
    ZETA_DebugAssert(dv->seg_allocator->Deallocate != NULL);

    dv->size_a = 0;
    dv->size_b = 0;
    dv->size_c = 0;

    dv->cur_data = NULL;
    dv->cur_offset = 0;
    dv->cur_capacity = 0;

    dv->nxt_data = NULL;
    dv->nxt_offset = 0;
    dv->nxt_capacity = 0;

    dv->offset = 0;
    dv->size = 0;
}

void Zeta_DynamicVector_Deinit(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);
}

size_t Zeta_DynamicVector_GetWidth(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    return dv->width;
}

size_t Zeta_DynamicVector_GetStride(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    return dv->stride;
}

size_t Zeta_DynamicVector_GetSize(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    return dv->size;
}

void* Zeta_DynamicVector_Refer(void* dv_, void const* pos_cursor_) {
    Zeta_DynamicVector* dv = dv_;
    Zeta_DynamicVector_Cursor const* pos_cursor = pos_cursor_;

    CheckDVCursor_(dv, pos_cursor);

    return pos_cursor->ele;
}

void Zeta_DynamicVector_GetLBCursor(void* dv_, void* dst_cursor) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    Zeta_DynamicVector_Access(dv, dst_cursor, NULL, -1);
}

void Zeta_DynamicVector_GetRBCursor(void* dv_, void* dst_cursor) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    Zeta_DynamicVector_Access(dv, dst_cursor, NULL, dv->size);
}

void* Zeta_DynamicVector_PeekL(void* dv_, void* dst_cursor, void* dst_ele) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    return Zeta_DynamicVector_Access(dv, dst_cursor, dst_ele, 0);
}

void* Zeta_DynamicVector_PeekR(void* dv_, void* dst_cursor, void* dst_ele) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    return Zeta_DynamicVector_Access(dv, dst_cursor, dst_ele, dv->size - 1);
}

void* Zeta_DynamicVector_Access(void* dv_, void* dst_cursor_, void* dst_ele,
                                size_t idx) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    ZETA_DebugAssert(idx + 1 < dv->size + 2);

    Zeta_DynamicVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_DynamicVector_Cursor pos_cursor = Access_(dv, idx);

    if (dst_cursor != NULL) {
        dst_cursor->dv = pos_cursor.dv;
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->ele = pos_cursor.ele;
    }

    if (pos_cursor.ele != NULL && dst_ele != NULL) {
        Zeta_MemCopy(dst_ele, pos_cursor.ele, dv->width);
    }

    return pos_cursor.ele;
}

void Zeta_DynamicVector_Read(void* dv_, void const* pos_cursor_, size_t cnt,
                             void* dst_, void* dst_cursor_) {
    Zeta_DynamicVector* dv = dv_;
    Zeta_DynamicVector_Cursor const* pos_cursor = pos_cursor_;

    CheckDVCursor_(dv, pos_cursor);

    ZETA_DebugAssert(pos_cursor->idx <= dv->size);
    ZETA_DebugAssert(cnt <= dv->size - pos_cursor->idx);

    Zeta_DynamicVector_Cursor* dst_cursor = dst_cursor_;

    if (cnt == 0) {
        if (dst_cursor != NULL) {
            dst_cursor->dv = pos_cursor->dv;
            dst_cursor->idx = pos_cursor->idx;
            dst_cursor->ele = pos_cursor->ele;
        }

        return;
    }

    size_t stride = dv->stride;
    size_t seg_capacity = dv->seg_capacity;

    Zeta_CircularArray cur_ca;
    cur_ca.data = dv->cur_data;
    cur_ca.width = sizeof(void*);
    cur_ca.stride = sizeof(void*);
    cur_ca.offset = dv->cur_offset;
    cur_ca.size = dv->size_b;
    cur_ca.capacity = dv->cur_capacity;

    Zeta_CircularArray nxt_ca;
    nxt_ca.data = dv->nxt_data;
    nxt_ca.width = sizeof(void*);
    nxt_ca.stride = sizeof(void*);
    nxt_ca.offset = dv->nxt_offset;
    nxt_ca.size = dv->size_a + dv->size_b + dv->size_c;
    nxt_ca.capacity = dv->nxt_capacity;

    TryMigrate_(dv, &cur_ca, &nxt_ca, 2 * cnt);

    UpdateCA_(dv, &cur_ca, &nxt_ca);

    unsigned char* dst = dst_;
    ZETA_DebugAssert(dst != NULL);

    size_t offset = dv->offset + pos_cursor->idx;

    while (0 < cnt) {
        size_t seg_idx = offset / seg_capacity;
        size_t ele_idx = offset % seg_capacity;

        size_t cur_cnt = ZETA_GetMinOf(cnt, seg_capacity - ele_idx);
        cnt -= cur_cnt;

        void* seg =
            *(void**)(dv->size_a <= seg_idx && seg_idx < dv->size_a + dv->size_b
                          ? Zeta_CircularArray_Access(&cur_ca, NULL, NULL,
                                                      seg_idx - dv->size_a)
                          : Zeta_CircularArray_Access(&nxt_ca, NULL, NULL,
                                                      seg_idx));

        Zeta_MemCopy(dst, seg + stride * ele_idx, stride * cur_cnt);

        dst += stride * cur_cnt;
        offset += cur_cnt;
    }

    if (dst_cursor != NULL) {
        dst_cursor->dv = dv;
        dst_cursor->idx = offset - dv->offset;

        if (dst_cursor->idx == dv->size) {
            dst_cursor->ele = NULL;
        } else {
            size_t seg_idx = offset / dv->seg_capacity;
            size_t ele_idx = offset % dv->seg_capacity;

            dst_cursor->ele =
                (unsigned char*)(*(void**)((
                    dv->size_a <= seg_idx && seg_idx < dv->size_a + dv->size_b
                        ? Zeta_CircularArray_Access(&cur_ca, NULL, NULL,
                                                    seg_idx - dv->size_a)
                        : Zeta_CircularArray_Access(&nxt_ca, NULL, NULL,
                                                    seg_idx)))) +
                stride * ele_idx;
        }
    }
}

void Zeta_DynamicVector_Write(void* dv_, void* pos_cursor_, size_t cnt,
                              void const* src_, void* dst_cursor_) {
    Zeta_DynamicVector* dv = dv_;
    Zeta_DynamicVector_Cursor const* pos_cursor = pos_cursor_;

    CheckDVCursor_(dv, pos_cursor);

    ZETA_DebugAssert(pos_cursor->idx <= dv->size);
    ZETA_DebugAssert(cnt <= dv->size - pos_cursor->idx);

    Zeta_DynamicVector_Cursor* dst_cursor = dst_cursor_;

    if (cnt == 0) {
        if (dst_cursor != NULL) {
            dst_cursor->dv = pos_cursor->dv;
            dst_cursor->idx = pos_cursor->idx;
            dst_cursor->ele = pos_cursor->ele;
        }

        return;
    }

    size_t stride = dv->stride;
    size_t seg_capacity = dv->seg_capacity;

    Zeta_CircularArray cur_ca;
    cur_ca.data = dv->cur_data;
    cur_ca.width = sizeof(void*);
    cur_ca.stride = sizeof(void*);
    cur_ca.offset = dv->cur_offset;
    cur_ca.size = dv->size_b;
    cur_ca.capacity = dv->cur_capacity;

    Zeta_CircularArray nxt_ca;
    nxt_ca.data = dv->nxt_data;
    nxt_ca.width = sizeof(void*);
    nxt_ca.stride = sizeof(void*);
    nxt_ca.offset = dv->nxt_offset;
    nxt_ca.size = dv->size_a + dv->size_b + dv->size_c;
    nxt_ca.capacity = dv->nxt_capacity;

    TryMigrate_(dv, &cur_ca, &nxt_ca, 2 * cnt);

    UpdateCA_(dv, &cur_ca, &nxt_ca);

    unsigned char const* src = src_;
    ZETA_DebugAssert(src != NULL);

    size_t offset = dv->offset + pos_cursor->idx;

    while (0 < cnt) {
        size_t seg_idx = offset / dv->seg_capacity;
        size_t ele_idx = offset % dv->seg_capacity;

        size_t cur_cnt = ZETA_GetMinOf(cnt, seg_capacity - ele_idx);
        cnt -= cur_cnt;

        void* seg = *(void**)((
            dv->size_a <= seg_idx && seg_idx < dv->size_a + dv->size_b
                ? Zeta_CircularArray_Access(&cur_ca, NULL, NULL,
                                            seg_idx - dv->size_a)
                : Zeta_CircularArray_Access(&nxt_ca, NULL, NULL, seg_idx)));

        Zeta_MemCopy(seg + stride * ele_idx, src, stride * cur_cnt);

        src += stride * cur_cnt;
        offset += cur_cnt;
    }

    if (dst_cursor != NULL) {
        dst_cursor->dv = dv;
        dst_cursor->idx = offset - dv->offset;

        if (dst_cursor->idx == dv->size) {
            dst_cursor->ele = NULL;
        } else {
            size_t seg_idx = offset / dv->seg_capacity;
            size_t ele_idx = offset % dv->seg_capacity;

            dst_cursor->ele =
                (unsigned char*)(*(void**)((
                    dv->size_a <= seg_idx && seg_idx < dv->size_a + dv->size_b
                        ? Zeta_CircularArray_Access(&cur_ca, NULL, NULL,
                                                    seg_idx - dv->size_a)
                        : Zeta_CircularArray_Access(&nxt_ca, NULL, NULL,
                                                    seg_idx)))) +
                stride * ele_idx;
        }
    }
}

void* Zeta_DynamicVector_PushL(void* dv_, void* dst_cursor_, size_t cnt) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    Zeta_DynamicVector_Cursor* dst_cursor = dst_cursor_;

    if (cnt == 0) { goto RET; }

    size_t offset = dv->offset;
    size_t seg_capacity = dv->seg_capacity;

    ZETA_PrintCurPos;

    PushSeg_(dv, 0,
             ZETA_CeilIntDiv(cnt + seg_capacity - offset, seg_capacity) - 1);

    dv->offset = (offset + seg_capacity - cnt % seg_capacity) % seg_capacity;

    dv->size += cnt;

RET:;

    ZETA_PrintCurPos;

    Zeta_DynamicVector_Cursor pos_cursor = Access_(dv, 0);

    if (dst_cursor != NULL) {
        dst_cursor->dv = pos_cursor.dv;
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->ele = pos_cursor.ele;
    }

    ZETA_PrintCurPos;

    return pos_cursor.ele;
}

void* Zeta_DynamicVector_PushR(void* dv_, void* dst_cursor_, size_t cnt) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    Zeta_DynamicVector_Cursor* dst_cursor = dst_cursor_;

    size_t origin_size = dv->size;

    if (cnt == 0) { goto RET; }

    size_t offset = dv->offset;
    size_t seg_capacity = dv->seg_capacity;
    size_t size = dv->size;

    PushSeg_(dv, 1,
             ZETA_CeilIntDiv(offset + size + cnt, seg_capacity) -
                 ZETA_CeilIntDiv(offset + size, seg_capacity));

    dv->size += cnt;

RET:;

    Zeta_DynamicVector_Cursor pos_cursor = Access_(dv, origin_size);

    if (dst_cursor != NULL) {
        dst_cursor->dv = pos_cursor.dv;
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->ele = pos_cursor.ele;
    }

    return pos_cursor.ele;
}

void Zeta_DynamicVector_PopL(void* dv_, size_t cnt) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    size_t offset = dv->offset;
    size_t seg_capacity = dv->seg_capacity;
    size_t size = dv->size;

    ZETA_DebugAssert(cnt <= size);

    PopSeg_(dv, 0, (offset + cnt) / seg_capacity);

    dv->offset = (offset + cnt) % seg_capacity;
    dv->size -= cnt;
}

void Zeta_DynamicVector_PopR(void* dv_, size_t cnt) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    size_t offset = dv->offset;
    size_t seg_capacity = dv->seg_capacity;
    size_t size = dv->size;

    ZETA_DebugAssert(cnt <= size);

    PopSeg_(dv, 1,
            ZETA_CeilIntDiv(offset + size, seg_capacity) -
                ZETA_CeilIntDiv(offset + size - cnt, seg_capacity));

    dv->size -= cnt;
}

void Zeta_DynamicVector_EraseAll(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);
}

static void AddPtrSize_(Zeta_DebugHashMap* dst_hm, void const* ptr,
                        size_t size) {
    Zeta_DebugHashMap_KeyValPair kvp =
        Zeta_DebugHashMap_Insert(dst_hm, ZETA_PtrToAddr(ptr));

    ZETA_DebugAssert(kvp.b);

    *kvp.val = size;
}

void Zeta_DynamicVector_Check(void* dv_, Zeta_DebugHashMap* dst_data_hm,
                              Zeta_DebugHashMap* dst_seg_hm) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    if (dv->cur_data != NULL) {
        AddPtrSize_(dst_data_hm, dv->cur_data,
                    sizeof(void*) * dv->cur_capacity);
    }

    if (dv->nxt_data != NULL) {
        AddPtrSize_(dst_data_hm, dv->nxt_data,
                    sizeof(void*) * dv->nxt_capacity);
    }

    size_t stride = dv->stride;
    size_t seg_capacity = dv->seg_capacity;

    Zeta_CircularArray cur_ca;
    cur_ca.data = dv->cur_data;
    cur_ca.width = sizeof(void*);
    cur_ca.stride = sizeof(void*);
    cur_ca.offset = dv->cur_offset;
    cur_ca.size = dv->size_b;
    cur_ca.capacity = dv->cur_capacity;

    Zeta_CircularArray nxt_ca;
    nxt_ca.data = dv->nxt_data;
    nxt_ca.width = sizeof(void*);
    nxt_ca.stride = sizeof(void*);
    nxt_ca.offset = dv->nxt_offset;
    nxt_ca.size = dv->size_a + dv->size_b + dv->size_c;
    nxt_ca.capacity = dv->nxt_capacity;

    if (nxt_ca.data != NULL) {
        for (size_t i = 0; i < dv->size_a; ++i) {
            AddPtrSize_(
                dst_seg_hm,
                *(void**)Zeta_CircularArray_Access(&nxt_ca, NULL, NULL, i),
                stride * seg_capacity);
        }
    }

    for (size_t i = 0; i < dv->size_b; ++i) {
        AddPtrSize_(dst_seg_hm,
                    *(void**)Zeta_CircularArray_Access(&cur_ca, NULL, NULL, i),
                    stride * seg_capacity);
    }

    if (nxt_ca.data != NULL) {
        for (size_t i = 0; i < dv->size_c; ++i) {
            AddPtrSize_(dst_seg_hm,
                        *(void**)Zeta_CircularArray_Access(
                            &nxt_ca, NULL, NULL, dv->size_a + dv->size_b + i),
                        stride * seg_capacity);
        }
    }
}

bool_t Zeta_DynamicVector_Cursor_IsEqual(void* dv_, void const* cursor_a_,
                                         void const* cursor_b_) {
    Zeta_DynamicVector* dv = dv_;

    Zeta_DynamicVector_Cursor const* cursor_a = cursor_a_;
    Zeta_DynamicVector_Cursor const* cursor_b = cursor_b_;

    CheckDVCursor_(dv, cursor_a);
    CheckDVCursor_(dv, cursor_b);

    return cursor_a->idx == cursor_b->idx;
}

int Zeta_DynamicVector_Cursor_Compare(void* dv_, void const* cursor_a_,
                                      void const* cursor_b_) {
    Zeta_DynamicVector* dv = dv_;

    Zeta_DynamicVector_Cursor const* cursor_a = cursor_a_;
    Zeta_DynamicVector_Cursor const* cursor_b = cursor_b_;

    CheckDVCursor_(dv, cursor_a);
    CheckDVCursor_(dv, cursor_b);

    size_t ka = cursor_a->idx + 1;
    size_t kb = cursor_b->idx + 1;

    if (ka < kb) { return -1; }
    if (kb < ka) { return 1; }
    return 0;
}

size_t Zeta_DynamicVector_Cursor_GetDist(void* dv_, void const* cursor_a_,
                                         void const* cursor_b_) {
    Zeta_DynamicVector* dv = dv_;

    Zeta_DynamicVector_Cursor const* cursor_a = cursor_a_;
    Zeta_DynamicVector_Cursor const* cursor_b = cursor_b_;

    CheckDVCursor_(dv, cursor_a);
    CheckDVCursor_(dv, cursor_b);

    return cursor_a->idx - cursor_b->idx;
}

size_t Zeta_DynamicVector_Cursor_GetIdx(void* dv_, void const* cursor_) {
    Zeta_DynamicVector* dv = dv_;
    Zeta_DynamicVector_Cursor const* cursor = cursor_;
    CheckDVCursor_(dv, cursor);

    return cursor->idx;
}

void Zeta_DynamicVector_Cursor_StepL(void* dv, void* cursor) {
    Zeta_DynamicVector_Cursor_AdvanceL(dv, cursor, 1);
}

void Zeta_DynamicVector_Cursor_StepR(void* dv, void* cursor) {
    Zeta_DynamicVector_Cursor_AdvanceR(dv, cursor, 1);
}

void Zeta_DynamicVector_Cursor_AdvanceL(void* dv_, void* cursor_, size_t step) {
    Zeta_DynamicVector* dv = dv_;
    Zeta_DynamicVector_Cursor* cursor = cursor_;
    CheckDVCursor_(dv, cursor);

    if (step == 0) { return; }

    ZETA_DebugAssert(step <= cursor->idx + 1);

    Zeta_DynamicVector_Access(dv, cursor, NULL, cursor->idx - step);
}

void Zeta_DynamicVector_Cursor_AdvanceR(void* dv_, void* cursor_, size_t step) {
    Zeta_DynamicVector* dv = dv_;
    Zeta_DynamicVector_Cursor* cursor = cursor_;
    CheckDVCursor_(dv, cursor);

    if (step == 0) { return; }

    ZETA_DebugAssert(step <= dv->size - cursor->idx);

    Zeta_DynamicVector_Access(dv, cursor, NULL, cursor->idx + step);
}

void Zeta_DynamicVector_Cursor_Check(void* dv, void const* cursor) {
    CheckDVCursorCore_(dv, cursor);
}

void Zeta_DynamicVector_DeploySeqContainer(void* dv_,
                                           Zeta_SeqContainer* seq_cntr) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    ZETA_DebugAssert(seq_cntr != NULL);

    Zeta_SeqContainer_Init(seq_cntr);

    seq_cntr->context = dv;

    seq_cntr->cursor_align = alignof(Zeta_DynamicVector_Cursor);

    seq_cntr->cursor_size = sizeof(Zeta_DynamicVector_Cursor);

    seq_cntr->GetStride = Zeta_DynamicVector_GetStride;

    seq_cntr->GetWidth = Zeta_DynamicVector_GetWidth;

    seq_cntr->GetSize = Zeta_DynamicVector_GetSize;

    seq_cntr->GetLBCursor = Zeta_DynamicVector_GetLBCursor;

    seq_cntr->GetRBCursor = Zeta_DynamicVector_GetRBCursor;

    seq_cntr->PeekL = Zeta_DynamicVector_PeekL;

    seq_cntr->PeekR = Zeta_DynamicVector_PeekR;

    seq_cntr->Access = Zeta_DynamicVector_Access;

    seq_cntr->Refer = Zeta_DynamicVector_Refer;

    seq_cntr->Read = Zeta_DynamicVector_Read;

    seq_cntr->Write = Zeta_DynamicVector_Write;

    seq_cntr->PushL = Zeta_DynamicVector_PushL;

    seq_cntr->PushR = Zeta_DynamicVector_PushR;

    seq_cntr->PopL = Zeta_DynamicVector_PopL;

    seq_cntr->PopR = Zeta_DynamicVector_PopR;

    seq_cntr->EraseAll = Zeta_DynamicVector_EraseAll;

    seq_cntr->Cursor_IsEqual = Zeta_DynamicVector_Cursor_IsEqual;

    seq_cntr->Cursor_Compare = Zeta_DynamicVector_Cursor_Compare;

    seq_cntr->Cursor_GetDist = Zeta_DynamicVector_Cursor_GetDist;

    seq_cntr->Cursor_GetIdx = Zeta_DynamicVector_Cursor_GetIdx;

    seq_cntr->Cursor_StepL = Zeta_DynamicVector_Cursor_StepL;

    seq_cntr->Cursor_StepR = Zeta_DynamicVector_Cursor_StepR;

    seq_cntr->Cursor_AdvanceL = Zeta_DynamicVector_Cursor_AdvanceL;

    seq_cntr->Cursor_AdvanceR = Zeta_DynamicVector_Cursor_AdvanceR;
}
