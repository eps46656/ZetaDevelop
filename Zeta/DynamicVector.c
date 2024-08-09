#include "DynamicVector.h"

#include "CircularArray.h"
#include "Debugger.h"
#include "MemCheck.h"
#include "utils.h"

#if ZETA_IsDebug

#define CheckDV_(dv) Zeta_DynamicVector_Check((dv))

#define CheckDVCursor_(dv, cursor) \
    Zeta_DynamicVector_Cursor_Check((dv), (cursor))

#else

#define CheckDV_(dv)

#define CheckDVCursor_(dv, cursor)

#endif

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
                        Zeta_CircularArray* nxt_ca, size_t op_cnt) {
    size_t move_cnt = ZETA_GetMinOf(ZETA_GetMaxOf(8, op_cnt * 2), dv->size_b);

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

    TryMigrate_(dv, &cur_ca, &nxt_ca, 1);

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

static void* Push_(void* dv_, void* dst_cursor_, size_t cnt, int dir) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    Zeta_DynamicVector_Cursor* dst_cursor = dst_cursor_;

    if (cnt == 0) {
        return dir == 0 ? Zeta_DynamicVector_PeekL(dv, dst_cursor, NULL)
                        : Zeta_DynamicVector_PeekR(dv, dst_cursor, NULL);
    }

    size_t offset = dv->offset;
    size_t seg_capacity = dv->seg_capacity;
    size_t size = dv->size;

    size_t seg_cnt =
        dir == 0
            ? ZETA_CeilIntDiv(cnt + seg_capacity - offset, seg_capacity) - 1
            : ZETA_CeilIntDiv(offset + size + cnt, seg_capacity) -
                  ZETA_CeilIntDiv(offset + size, seg_capacity);

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

    if (seg_cnt == 0) { goto RET; }

    size_t need_capacity = nxt_ca.size + seg_cnt;
    size_t new_capacity = GetNxtCapacity_(need_capacity);

    if (nxt_ca.data == NULL) {
        if (cur_ca.data == NULL) {
            cur_ca.data = AllocateData_(dv, new_capacity);
            cur_ca.capacity = new_capacity;
        }

        if (need_capacity <= cur_ca.capacity) {
            if (dir == 0) {
                Zeta_CircularArray_PushL(&cur_ca, NULL, seg_cnt);

                for (size_t i = 0; i < seg_cnt; ++i) {
                    *(void**)Zeta_CircularArray_Access(&cur_ca, NULL, NULL, i) =
                        AllocateSeg_(dv);
                }
            } else {
                Zeta_CircularArray_PushR(&cur_ca, NULL, seg_cnt);

                for (size_t i = 0; i < seg_cnt; ++i) {
                    *(void**)Zeta_CircularArray_Access(&cur_ca, NULL, NULL,
                                                       cur_ca.size - 1 - i) =
                        AllocateSeg_(dv);
                }
            }

            dv->cur_data = cur_ca.data;
            dv->cur_offset = cur_ca.offset;
            dv->cur_capacity = cur_ca.capacity;

            dv->size_b = cur_ca.size;

            goto PUSH_SEG_END;
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
        Zeta_CircularArray_PushL(&nxt_ca, NULL, seg_cnt);

        for (size_t i = 0; i < seg_cnt; ++i) {
            *(void**)Zeta_CircularArray_Access(&nxt_ca, NULL, NULL, i) =
                AllocateSeg_(dv);
        }

        dv->size_a += seg_cnt;
    } else {
        Zeta_CircularArray_PushR(&nxt_ca, NULL, seg_cnt);

        for (size_t i = 0; i < seg_cnt; ++i) {
            *(void**)Zeta_CircularArray_Access(
                &nxt_ca, NULL, NULL, nxt_ca.size - 1 - i) = AllocateSeg_(dv);
        }

        dv->size_c += seg_cnt;
    }

PUSH_SEG_END:;

    TryMigrate_(dv, &cur_ca, &nxt_ca, seg_cnt);

    UpdateCA_(dv, &cur_ca, &nxt_ca);

RET:;

    if (dir == 0) {
        dv->offset = offset =
            (offset + seg_capacity - cnt % seg_capacity) % seg_capacity;
    }

    dv->size = size += cnt;

    size_t ret_offset = dir == 0 ? offset : offset + size - cnt;

    size_t ret_seg_idx = ret_offset / seg_capacity;
    size_t ret_ele_idx = ret_offset % seg_capacity;

    void* ref =
        (unsigned char*)(*(void**)((
            dv->size_a <= ret_seg_idx && ret_seg_idx < dv->size_a + dv->size_b
                ? Zeta_CircularArray_Access(&cur_ca, NULL, NULL,
                                            ret_seg_idx - dv->size_a)
                : Zeta_CircularArray_Access(&nxt_ca, NULL, NULL,
                                            ret_seg_idx)))) +
        dv->stride * ret_ele_idx;

    if (dst_cursor != NULL) {
        dst_cursor->dv = dv;
        dst_cursor->idx = ret_offset - offset;
        dst_cursor->ref = ref;
    }

    return ref;
}

static void Pop_(void* dv_, int dir, size_t cnt) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    size_t offset = dv->offset;
    size_t seg_capacity = dv->seg_capacity;
    size_t size = dv->size;

    ZETA_DebugAssert(cnt <= size);

    size_t seg_cnt =
        dir == 0 ? (offset + cnt) / seg_capacity
                 : ZETA_CeilIntDiv(offset + size, seg_capacity) -
                       ZETA_CeilIntDiv(offset + size - cnt, seg_capacity);

    if (seg_cnt == 0) { goto POP_SEG_END; }

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

    size_t origin_seg_cnt = seg_cnt;

    if (nxt_ca.data != NULL && 0 < seg_cnt) {
        if (dir == 0) {
            size_t cur_cnt = ZETA_GetMinOf(seg_cnt, dv->size_a);
            seg_cnt -= cur_cnt;

            dv->size_a -= cur_cnt;

            for (size_t i = 0; i < cur_cnt; ++i) {
                dv->seg_allocator->Deallocate(
                    dv->seg_allocator->context,
                    *(void**)Zeta_CircularArray_Access(&nxt_ca, NULL, NULL, i));
            }

            Zeta_CircularArray_PopL(&nxt_ca, cur_cnt);
        } else {
            size_t cur_seg_cnt = ZETA_GetMinOf(seg_cnt, dv->size_c);
            seg_cnt -= cur_seg_cnt;

            dv->size_c -= cur_seg_cnt;

            for (size_t i = 0; i < cur_seg_cnt; ++i) {
                dv->seg_allocator->Deallocate(
                    dv->seg_allocator->context,
                    *(void**)Zeta_CircularArray_Access(&nxt_ca, NULL, NULL,
                                                       nxt_ca.size - 1 - i));
            }

            Zeta_CircularArray_PopR(&nxt_ca, cur_seg_cnt);
        }
    }

    if (0 < seg_cnt) {
        size_t cur_seg_cnt = ZETA_GetMinOf(seg_cnt, dv->size_b);
        seg_cnt -= cur_seg_cnt;

        dv->size_b -= cur_seg_cnt;

        if (dir == 0) {
            for (size_t i = 0; i < cur_seg_cnt; ++i) {
                dv->seg_allocator->Deallocate(
                    dv->seg_allocator->context,
                    *(void**)Zeta_CircularArray_Access(&cur_ca, NULL, NULL, i));
            }

            Zeta_CircularArray_PopL(&cur_ca, cur_seg_cnt);

            if (nxt_ca.data != NULL) {
                Zeta_CircularArray_PopL(&nxt_ca, cur_seg_cnt);
            }
        } else {
            for (size_t i = 0; i < cur_seg_cnt; ++i) {
                dv->seg_allocator->Deallocate(
                    dv->seg_allocator->context,
                    *(void**)Zeta_CircularArray_Access(&cur_ca, NULL, NULL,
                                                       cur_ca.size - 1 - i));
            }

            Zeta_CircularArray_PopR(&cur_ca, cur_seg_cnt);

            if (nxt_ca.data != NULL) {
                Zeta_CircularArray_PopR(&nxt_ca, cur_seg_cnt);
            }
        }
    }

    if (nxt_ca.data != NULL && 0 < seg_cnt) {
        if (dir == 0) {
            dv->size_a -= seg_cnt;

            for (size_t i = 0; i < seg_cnt; ++i) {
                dv->seg_allocator->Deallocate(
                    dv->seg_allocator->context,
                    *(void**)Zeta_CircularArray_Access(&nxt_ca, NULL, NULL, i));
            }

            Zeta_CircularArray_PopL(&nxt_ca, seg_cnt);
        } else {
            dv->size_c -= seg_cnt;

            for (size_t i = 0; i < seg_cnt; ++i) {
                dv->seg_allocator->Deallocate(
                    dv->seg_allocator->context,
                    *(void**)Zeta_CircularArray_Access(&nxt_ca, NULL, NULL,
                                                       nxt_ca.size - 1 - i));
            }

            Zeta_CircularArray_PopR(&nxt_ca, seg_cnt);
        }
    }

    if (nxt_ca.data == NULL) {
    } else if (dir == 0) {
        size_t move_cnt = ZETA_GetMinOf(2 * origin_seg_cnt, dv->size_b);

        Zeta_CircularArray_Assign(&nxt_ca, &cur_ca, dv->size_a, 0, move_cnt);
        Zeta_CircularArray_PopL(&cur_ca, move_cnt);

        dv->size_a += move_cnt;
        dv->size_b -= move_cnt;
    } else {
        size_t move_cnt = ZETA_GetMinOf(2 * origin_seg_cnt, dv->size_b);

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

POP_SEG_END:;

    if (dir == 1) { dv->offset = (offset + cnt) % seg_capacity; }

    dv->size -= cnt;
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
            dv->seg_allocator->Deallocate(
                dv->seg_allocator->context,
                *(void**)Zeta_CircularArray_Access(&nxt_ca, NULL, NULL, i));
        }
    }

    for (size_t i = 0; i < dv->size_b; ++i) {
        dv->seg_allocator->Deallocate(
            dv->seg_allocator->context,
            *(void**)Zeta_CircularArray_Access(&cur_ca, NULL, NULL, i));
    }

    if (nxt_ca.data != NULL) {
        for (size_t i = 0; i < dv->size_c; ++i) {
            dv->seg_allocator->Deallocate(
                dv->seg_allocator->context,
                *(void**)Zeta_CircularArray_Access(
                    &cur_ca, NULL, NULL, dv->size_a + dv->size_b + i));
        }
    }

    if (dv->cur_data != NULL) {
        dv->data_allocator->Deallocate(dv->data_allocator->context,
                                       dv->cur_data);
    }

    if (dv->nxt_data != NULL) {
        dv->data_allocator->Deallocate(dv->data_allocator->context,
                                       dv->nxt_data);
    }
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

    return pos_cursor->ref;
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

void* Zeta_DynamicVector_PeekL(void* dv_, void* dst_cursor, void* dst_elem) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    return Zeta_DynamicVector_Access(dv, dst_cursor, dst_elem, 0);
}

void* Zeta_DynamicVector_PeekR(void* dv_, void* dst_cursor, void* dst_elem) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    return Zeta_DynamicVector_Access(dv, dst_cursor, dst_elem, dv->size - 1);
}

void* Zeta_DynamicVector_Access(void* dv_, void* dst_cursor_, void* dst_elem,
                                size_t idx) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    ZETA_DebugAssert(idx + 1 < dv->size + 2);

    Zeta_DynamicVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_DynamicVector_Cursor pos_cursor =
        Access_((Zeta_DynamicVector*)dv, idx);

    if (dst_cursor != NULL) {
        dst_cursor->dv = pos_cursor.dv;
        dst_cursor->idx = pos_cursor.idx;
        dst_cursor->ref = pos_cursor.ref;
    }

    if (pos_cursor.ref != NULL && dst_elem != NULL) {
        Zeta_MemCopy(dst_elem, pos_cursor.ref, dv->width);
    }

    return pos_cursor.ref;
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
            dst_cursor->ref = pos_cursor->ref;
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

    TryMigrate_(dv, &cur_ca, &nxt_ca, cnt);

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
            dst_cursor->ref = NULL;
        } else {
            size_t seg_idx = offset / dv->seg_capacity;
            size_t ele_idx = offset % dv->seg_capacity;

            dst_cursor->ref =
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
            dst_cursor->ref = pos_cursor->ref;
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

    TryMigrate_(dv, &cur_ca, &nxt_ca, cnt);

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
            dst_cursor->ref = NULL;
        } else {
            size_t seg_idx = offset / dv->seg_capacity;
            size_t ele_idx = offset % dv->seg_capacity;

            dst_cursor->ref =
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

void* Zeta_DynamicVector_PushL(void* dv, void* dst_cursor, size_t cnt) {
    return Push_(dv, dst_cursor, cnt, 0);
}

void* Zeta_DynamicVector_PushR(void* dv, void* dst_cursor, size_t cnt) {
    return Push_(dv, dst_cursor, cnt, 1);
}

void Zeta_DynamicVector_PopL(void* dv, size_t cnt) { Pop_(dv, 0, cnt); }

void Zeta_DynamicVector_PopR(void* dv, size_t cnt) { Pop_(dv, 1, cnt); }

void Zeta_DynamicVector_EraseAll(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);
}

void Zeta_DynamicVector_Check(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DebugAssert(dv != NULL);

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

void Zeta_DynamicVector_Sanitize(void* dv_, Zeta_AssocContainer* dst_data_assoc,
                                 Zeta_AssocContainer* dst_seg_assoc) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    if (dv->cur_data != NULL) {
        Zeta_MemCheck_AddPtrSize(dst_data_assoc, dv->cur_data,
                                 sizeof(void*) * dv->cur_capacity);
    }

    if (dv->nxt_data != NULL) {
        Zeta_MemCheck_AddPtrSize(dst_data_assoc, dv->nxt_data,
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
            Zeta_MemCheck_AddPtrSize(
                dst_seg_assoc,
                *(void**)Zeta_CircularArray_Access(&nxt_ca, NULL, NULL, i),
                stride * seg_capacity);
        }
    }

    for (size_t i = 0; i < dv->size_b; ++i) {
        Zeta_MemCheck_AddPtrSize(
            dst_seg_assoc,
            *(void**)Zeta_CircularArray_Access(&cur_ca, NULL, NULL, i),
            stride * seg_capacity);
    }

    if (nxt_ca.data != NULL) {
        for (size_t i = 0; i < dv->size_c; ++i) {
            Zeta_MemCheck_AddPtrSize(
                dst_seg_assoc,
                *(void**)Zeta_CircularArray_Access(&nxt_ca, NULL, NULL,
                                                   dv->size_a + dv->size_b + i),
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

void Zeta_DynamicVector_Cursor_Check(void* dv_, void const* cursor_) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    Zeta_DynamicVector_Cursor const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    Zeta_DynamicVector_Cursor re_cursor;
    Zeta_DynamicVector_Access(dv, &re_cursor, NULL, cursor->idx);

    ZETA_DebugAssert(re_cursor.dv == cursor->dv);
    ZETA_DebugAssert(re_cursor.idx == cursor->idx);
    ZETA_DebugAssert(re_cursor.ref == cursor->ref);
}

void Zeta_DynamicVector_DeploySeqContainer(void* dv_,
                                           Zeta_SeqContainer* seq_cntr) {
    Zeta_DynamicVector* dv = dv_;
    CheckDV_(dv);

    ZETA_DebugAssert(seq_cntr != NULL);

    Zeta_SeqContainer_Init(seq_cntr);

    seq_cntr->context = dv;

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
