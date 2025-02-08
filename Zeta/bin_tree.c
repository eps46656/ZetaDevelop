#include "bin_tree.h"

#include "debugger.h"
#include "utils.h"

void Zeta_BinTree_InitOpr(Zeta_BinTreeNodeOperator* btn_opr) {
    btn_opr->context = NULL;

    btn_opr->GetP = NULL;
    btn_opr->GetL = NULL;
    btn_opr->GetR = NULL;

    btn_opr->SetP = NULL;
    btn_opr->SetL = NULL;
    btn_opr->SetR = NULL;

    btn_opr->GetColor = NULL;
    btn_opr->SetColor = NULL;

    btn_opr->GetAccSize = NULL;
    btn_opr->SetAccSize = NULL;
}

static size_t Count_(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    size_t ret = 1;

    void* nl = ZETA_BinTreeNodeOperator_GetL(btn_opr, n);
    void* nr = ZETA_BinTreeNodeOperator_GetR(btn_opr, n);

    if (nl != NULL) { ret += Count_(btn_opr, nl); }
    if (nr != NULL) { ret += Count_(btn_opr, nr); }

    return ret;
}

size_t Zeta_BinTree_Count(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    if (n == NULL) { return 0; }

    return Count_(btn_opr, n);
}

static void AddDiffSize_(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                         size_t diff_size) {
    if (diff_size == 0) { return; }

    for (; n != NULL; n = ZETA_BinTreeNodeOperator_GetP(btn_opr, n)) {
        ZETA_BinTreeNodeOperator_SetAccSize(
            btn_opr, n,
            ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, n) + diff_size);
    }
}

size_t Zeta_BinTree_GetSize(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    ZETA_DebugAssert(btn_opr != NULL);

    size_t n_acc_size = ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, n);

    if (n == NULL) { return n_acc_size; }

    return n_acc_size -
           ZETA_BinTreeNodeOperator_GetAccSize(
               btn_opr, ZETA_BinTreeNodeOperator_GetL(btn_opr, n)) -
           ZETA_BinTreeNodeOperator_GetAccSize(
               btn_opr, ZETA_BinTreeNodeOperator_GetR(btn_opr, n));
}

void Zeta_BinTree_SetSize(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                          size_t size) {
    ZETA_DebugAssert(btn_opr != NULL);
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(btn_opr->GetAccSize != NULL);

    AddDiffSize_(btn_opr, n, size - Zeta_BinTree_GetSize(btn_opr, n));
}

void Zeta_BinTree_SetDiffSize(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                              size_t diff_size) {
    ZETA_DebugAssert(btn_opr != NULL);
    ZETA_DebugAssert(n != NULL);

    AddDiffSize_(btn_opr, n, diff_size);
}

#define Attatch_(D)                                                           \
    ZETA_DebugAssert(btn_opr != NULL);                                        \
                                                                              \
    ZETA_DebugAssert(n != NULL);                                              \
                                                                              \
    ZETA_DebugAssert(ZETA_BinTreeNodeOperator_Get##D(btn_opr, n) == NULL);    \
    ZETA_DebugAssert(m == NULL ||                                             \
                     ZETA_BinTreeNodeOperator_GetP(btn_opr, m) == NULL);      \
                                                                              \
    if (m == NULL) { return; }                                                \
                                                                              \
    ZETA_BinTreeNodeOperator_Set##D(btn_opr, n, m);                           \
    ZETA_BinTreeNodeOperator_SetP(btn_opr, m, n);                             \
                                                                              \
    if (btn_opr->GetAccSize != NULL) {                                        \
        AddDiffSize_(btn_opr, n,                                              \
                     ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, m) -        \
                         ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, NULL)); \
    }

void Zeta_BinTree_AttatchL(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                           void* m) {
    Attatch_(L);
}

void Zeta_BinTree_AttatchR(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                           void* m) {
    Attatch_(R);
}

void Zeta_BinTree_Detach(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    ZETA_DebugAssert(btn_opr != NULL);

    ZETA_DebugAssert(n != NULL);

    void* np = ZETA_BinTreeNodeOperator_GetP(btn_opr, n);

    if (np == NULL) { return; }

    if (ZETA_BinTreeNodeOperator_GetL(btn_opr, np) == n) {
        ZETA_BinTreeNodeOperator_SetL(btn_opr, np, NULL);
    } else {
        ZETA_BinTreeNodeOperator_SetR(btn_opr, np, NULL);
    }

    ZETA_BinTreeNodeOperator_SetP(btn_opr, n, NULL);

    if (btn_opr->GetAccSize != NULL && btn_opr->SetAccSize != NULL) {
        AddDiffSize_(btn_opr, np,
                     ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, NULL) -
                         ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, n));
    }
}

static void EraseAll_B_(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                        void* callback_context,
                        void (*Callback)(void* callback_context, void* n)) {
    ZETA_DebugAssert(n != NULL);

    void* head = NULL;

    unsigned long long random_seed = Zeta_GetRandom();

    for (;;) {
        if (n == NULL) {
            if (head == NULL) { break; }
            n = head;
            head = ZETA_BinTreeNodeOperator_GetP(btn_opr, head);
        }

        void* nl = ZETA_BinTreeNodeOperator_GetL(btn_opr, n);
        void* nr = ZETA_BinTreeNodeOperator_GetR(btn_opr, n);

        if (Callback != NULL) { Callback(callback_context, n); }

        if (nl == NULL) {
            n = nr;
            continue;
        }

        if (nr == NULL) {
            n = nl;
            continue;
        }

        if (Zeta_SimpleRandomRotate(&random_seed) % 2 == 0) {
            ZETA_BinTreeNodeOperator_SetP(btn_opr, nl, head);
            head = nl;
            n = nr;
        } else {
            ZETA_BinTreeNodeOperator_SetP(btn_opr, nr, head);
            head = nr;
            n = nl;
        }
    }
}

static void EraseAll_A_(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                        void* callback_context,
                        void (*Callback)(void* callback_context, void* n)) {
    size_t buffer_capacity = ZETA_GetMaxOf(24, ZETA_ULLONG_WIDTH * 3 / 4);

    void* buffer[buffer_capacity];
    size_t buffer_i = 0;

    buffer[buffer_i++] = n;

    while (0 < buffer_i) {
        n = buffer[--buffer_i];

        void* nl = ZETA_BinTreeNodeOperator_GetL(btn_opr, n);
        void* nr = ZETA_BinTreeNodeOperator_GetR(btn_opr, n);

        if (Callback != NULL) { Callback(callback_context, n); }

        if (nl != NULL) {
            if (buffer_i == buffer_capacity) {
                EraseAll_B_(btn_opr, nl, callback_context, Callback);
            } else {
                buffer[buffer_i++] = nl;
            }
        }

        if (nr != NULL) {
            if (buffer_i == buffer_capacity) {
                EraseAll_B_(btn_opr, nr, callback_context, Callback);
            } else {
                buffer[buffer_i++] = nr;
            }
        }
    }
}

void Zeta_BinTree_EraseAll(Zeta_BinTreeNodeOperator const* btn_opr, void* root,
                           void* callback_context,
                           void (*Callback)(void* callback_context, void* n)) {
    ZETA_DebugAssert(btn_opr != NULL);

    ZETA_DebugAssert(ZETA_BinTreeNodeOperator_GetP(btn_opr, root) == NULL);

    EraseAll_A_(btn_opr, root, callback_context, Callback);
}

void Zeta_BinTree_Swap(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                       void* m) {
    ZETA_DebugAssert(btn_opr != NULL);

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);

    if (n == m) { return; }

    void* np = ZETA_BinTreeNodeOperator_GetP(btn_opr, n);
    void* mp = ZETA_BinTreeNodeOperator_GetP(btn_opr, m);

    if (np == m) {
        ZETA_Swap(n, m);
        ZETA_Swap(np, mp);
    }

    size_t n_acc_size;
    size_t m_acc_size;

    size_t n_size;
    size_t m_size;

    void* nl = ZETA_BinTreeNodeOperator_GetL(btn_opr, n);
    void* nr = ZETA_BinTreeNodeOperator_GetR(btn_opr, n);

    void* ml = ZETA_BinTreeNodeOperator_GetL(btn_opr, m);
    void* mr = ZETA_BinTreeNodeOperator_GetR(btn_opr, m);

    if (btn_opr->GetAccSize != NULL && btn_opr->SetAccSize != NULL) {
        n_acc_size = ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, n);
        m_acc_size = ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, m);

        n_size = n_acc_size - ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, nl) -
                 ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, nr);
        m_size = m_acc_size - ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, ml) -
                 ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, mr);
    }

    if (mp == n) {
        if (np != NULL) {
            if (ZETA_BinTreeNodeOperator_GetL(btn_opr, np) == n) {
                ZETA_BinTreeNodeOperator_SetL(btn_opr, np, m);
            } else {
                ZETA_BinTreeNodeOperator_SetR(btn_opr, np, m);
            }
        }

        ZETA_BinTreeNodeOperator_SetP(btn_opr, m, np);

        if (ZETA_BinTreeNodeOperator_GetL(btn_opr, n) == m) {
            ZETA_BinTreeNodeOperator_SetL(btn_opr, m, n);

            ZETA_BinTreeNodeOperator_SetR(btn_opr, m, nr);
            if (nr != NULL) { ZETA_BinTreeNodeOperator_SetP(btn_opr, nr, m); }
        } else {
            ZETA_BinTreeNodeOperator_SetL(btn_opr, m, nl);
            if (nl != NULL) { ZETA_BinTreeNodeOperator_SetP(btn_opr, nl, m); }

            ZETA_BinTreeNodeOperator_SetR(btn_opr, m, n);
        }

        ZETA_BinTreeNodeOperator_SetP(btn_opr, n, m);

        ZETA_BinTreeNodeOperator_SetL(btn_opr, n, ml);
        if (ml != NULL) { ZETA_BinTreeNodeOperator_SetP(btn_opr, ml, n); }

        ZETA_BinTreeNodeOperator_SetR(btn_opr, n, mr);
        if (mr != NULL) { ZETA_BinTreeNodeOperator_SetP(btn_opr, mr, n); }
    } else {
        if (np != NULL) {
            if (ZETA_BinTreeNodeOperator_GetL(btn_opr, np) == n) {
                ZETA_BinTreeNodeOperator_SetL(btn_opr, np, m);
            } else {
                ZETA_BinTreeNodeOperator_SetR(btn_opr, np, m);
            }
        }

        ZETA_BinTreeNodeOperator_SetP(btn_opr, m, np);

        ZETA_BinTreeNodeOperator_SetL(btn_opr, m, nl);
        if (nl != NULL) { ZETA_BinTreeNodeOperator_SetP(btn_opr, nl, m); }

        ZETA_BinTreeNodeOperator_SetR(btn_opr, m, nr);
        if (nr != NULL) { ZETA_BinTreeNodeOperator_SetP(btn_opr, nr, m); }

        if (mp != NULL) {
            if (ZETA_BinTreeNodeOperator_GetL(btn_opr, mp) == m) {
                ZETA_BinTreeNodeOperator_SetL(btn_opr, mp, n);
            } else {
                ZETA_BinTreeNodeOperator_SetR(btn_opr, mp, n);
            }
        }

        ZETA_BinTreeNodeOperator_SetP(btn_opr, n, mp);

        ZETA_BinTreeNodeOperator_SetL(btn_opr, n, ml);
        if (ml != NULL) { ZETA_BinTreeNodeOperator_SetP(btn_opr, ml, n); }

        ZETA_BinTreeNodeOperator_SetR(btn_opr, n, mr);
        if (mr != NULL) { ZETA_BinTreeNodeOperator_SetP(btn_opr, mr, n); }
    }

    if (btn_opr->GetAccSize != NULL && btn_opr->SetAccSize != NULL) {
        ZETA_BinTreeNodeOperator_SetAccSize(btn_opr, n, m_acc_size);
        ZETA_BinTreeNodeOperator_SetAccSize(btn_opr, m, n_acc_size);

        AddDiffSize_(btn_opr, n, n_size - m_size);
        AddDiffSize_(btn_opr, m, m_size - n_size);
    }
}

#define Rotate_(D, E)                                                        \
    ZETA_DebugAssert(btn_opr != NULL);                                       \
                                                                             \
    ZETA_DebugAssert(n != NULL);                                             \
                                                                             \
    void* ne = ZETA_BinTreeNodeOperator_Get##E(btn_opr, n);                  \
    ZETA_DebugAssert(ne != NULL);                                            \
                                                                             \
    void* ned = ZETA_BinTreeNodeOperator_Get##D(btn_opr, ne);                \
    void* np = ZETA_BinTreeNodeOperator_Get##P(btn_opr, n);                  \
                                                                             \
    if (np != NULL) {                                                        \
        if (ZETA_BinTreeNodeOperator_Get##D(btn_opr, np) == n) {             \
            ZETA_BinTreeNodeOperator_Set##D(btn_opr, np, ne);                \
        } else {                                                             \
            ZETA_BinTreeNodeOperator_Set##E(btn_opr, np, ne);                \
        }                                                                    \
    }                                                                        \
                                                                             \
    ZETA_BinTreeNodeOperator_SetP(btn_opr, ne, np);                          \
                                                                             \
    ZETA_BinTreeNodeOperator_Set##D(btn_opr, ne, n);                         \
    ZETA_BinTreeNodeOperator_SetP(btn_opr, n, ne);                           \
                                                                             \
    ZETA_BinTreeNodeOperator_Set##E(btn_opr, n, ned);                        \
    if (ned != NULL) { ZETA_BinTreeNodeOperator_SetP(btn_opr, ned, n); }     \
                                                                             \
    if (btn_opr->SetAccSize == NULL) { return; }                             \
                                                                             \
    size_t n_acc_size = ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, n);     \
    size_t ne_acc_size = ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, ne);   \
    size_t ned_acc_size = ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, ned); \
                                                                             \
    ZETA_BinTreeNodeOperator_SetAccSize(                                     \
        btn_opr, n, n_acc_size - ne_acc_size + ned_acc_size);                \
                                                                             \
    ZETA_BinTreeNodeOperator_SetAccSize(btn_opr, ne, n_acc_size);

void Zeta_BinTree_RotateL(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    Rotate_(L, R);
}

void Zeta_BinTree_RotateR(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    Rotate_(R, L);
}

#define StepP_(E)                                                             \
    ZETA_DebugAssert(btn_opr != NULL);                                        \
                                                                              \
    ZETA_DebugAssert(n != NULL);                                              \
                                                                              \
    for (;;) {                                                                \
        void* np = ZETA_BinTreeNodeOperator_GetP(btn_opr, n);                 \
        if (np == NULL) { return NULL; }                                      \
        if (ZETA_BinTreeNodeOperator_Get##E(btn_opr, np) == n) { return np; } \
        n = np;                                                               \
    }

void* Zeta_BinTree_StepPL(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    StepP_(R);
}

void* Zeta_BinTree_StepPR(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    StepP_(L);
}

#define Step_(D, E)                                                           \
    ZETA_DebugAssert(btn_opr != NULL);                                        \
                                                                              \
    ZETA_DebugAssert(n != NULL);                                              \
                                                                              \
    void* nd = ZETA_BinTreeNodeOperator_Get##D(btn_opr, n);                   \
                                                                              \
    if (nd != NULL) {                                                         \
        return Zeta_GetMostLink(btn_opr->context, btn_opr->Get##E, nd);       \
    }                                                                         \
                                                                              \
    for (;;) {                                                                \
        void* np = ZETA_BinTreeNodeOperator_GetP(btn_opr, n);                 \
        if (np == NULL) { return NULL; }                                      \
        if (ZETA_BinTreeNodeOperator_Get##E(btn_opr, np) == n) { return np; } \
        n = np;                                                               \
    }

void* Zeta_BinTree_StepL(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    Step_(L, R);
}

void* Zeta_BinTree_StepR(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    Step_(R, L);
}

#define Access_(D, E)                                                          \
    if (dst_n == NULL && dst_tail_idx == NULL) { return; }                     \
                                                                               \
    ZETA_DebugAssert(btn_opr != NULL);                                         \
                                                                               \
    size_t n_acc_size = ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, n);       \
                                                                               \
    if (n_acc_size <= idx) {                                                   \
        if (dst_n != NULL) { *dst_n = NULL; }                                  \
        if (dst_tail_idx != NULL) { *dst_tail_idx = idx - n_acc_size; }        \
        return;                                                                \
    }                                                                          \
                                                                               \
    while (n != NULL) {                                                        \
        void* nd = ZETA_BinTreeNodeOperator_Get##D(btn_opr, n);                \
        size_t nd_acc_size = ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, nd); \
                                                                               \
        if (idx < nd_acc_size) {                                               \
            n = nd;                                                            \
            n_acc_size = nd_acc_size;                                          \
            continue;                                                          \
        }                                                                      \
                                                                               \
        void* ne = ZETA_BinTreeNodeOperator_Get##E(btn_opr, n);                \
        size_t ne_acc_size = ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, ne); \
                                                                               \
        idx -= nd_acc_size;                                                    \
        size_t n_size = n_acc_size - nd_acc_size - ne_acc_size;                \
                                                                               \
        if (idx < n_size) { break; }                                           \
                                                                               \
        n = ne;                                                                \
        n_acc_size = ne_acc_size;                                              \
        idx -= n_size;                                                         \
    }                                                                          \
                                                                               \
    if (dst_n != NULL) { *dst_n = n; }                                         \
    if (dst_tail_idx != NULL) { *dst_tail_idx = idx; }

void Zeta_BinTree_AccessL(void** dst_n, size_t* dst_tail_idx,
                          Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                          size_t idx) {
    Access_(L, R);
}

void Zeta_BinTree_AccessR(void** dst_n, size_t* dst_tail_idx,
                          Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                          size_t idx) {
    Access_(R, L);
}

#define Advance_(D, E)                                                         \
    if (dst_n == NULL && dst_tail_idx == NULL) { return; }                     \
                                                                               \
    ZETA_DebugAssert(btn_opr != NULL);                                         \
                                                                               \
    while (n != NULL && 0 < step) {                                            \
        void* nd = ZETA_BinTreeNodeOperator_Get##D(btn_opr, n);                \
        void* ne = ZETA_BinTreeNodeOperator_Get##E(btn_opr, n);                \
                                                                               \
        size_t n_acc_size = ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, n);   \
        size_t nd_acc_size = ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, nd); \
        size_t ne_acc_size = ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, ne); \
                                                                               \
        size_t n_size = n_acc_size - nd_acc_size - ne_acc_size;                \
                                                                               \
        if (step < n_size) { break; }                                          \
                                                                               \
        step -= n_size;                                                        \
                                                                               \
        if (step < ne_acc_size) {                                              \
            Zeta_BinTree_Access##D(dst_n, dst_tail_idx, btn_opr, ne, step);    \
            return;                                                            \
        }                                                                      \
                                                                               \
        step -= ne_acc_size;                                                   \
                                                                               \
        for (;;) {                                                             \
            void* np = ZETA_BinTreeNodeOperator_GetP(btn_opr, n);              \
                                                                               \
            if (np == NULL ||                                                  \
                ZETA_BinTreeNodeOperator_Get##D(btn_opr, np) == n) {           \
                n = np;                                                        \
                break;                                                         \
            }                                                                  \
                                                                               \
            n = np;                                                            \
        }                                                                      \
    }                                                                          \
                                                                               \
    if (dst_n != NULL) { *dst_n = n; }                                         \
    if (dst_tail_idx != NULL) { *dst_tail_idx = step; }

void Zeta_BinTree_AdvanceL(void** dst_n, size_t* dst_tail_idx,
                           Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                           size_t step) {
    Advance_(R, L);
}

void Zeta_BinTree_AdvanceR(void** dst_n, size_t* dst_tail_idx,
                           Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                           size_t step) {
    Advance_(L, R);
}

void Zeta_BinTree_GetAccSize(size_t* dst_l_acc_size, size_t* dst_r_acc_size,
                             Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    ZETA_DebugAssert(btn_opr != NULL);

    ZETA_DebugAssert(n != NULL);

    if (dst_l_acc_size == NULL && dst_r_acc_size == NULL) { return; }

    size_t l_acc_size = ZETA_BinTreeNodeOperator_GetAccSize(
        btn_opr, ZETA_BinTreeNodeOperator_GetL(btn_opr, n));
    size_t r_acc_size = ZETA_BinTreeNodeOperator_GetAccSize(
        btn_opr, ZETA_BinTreeNodeOperator_GetR(btn_opr, n));

    size_t n_acc_size = ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, n);

    for (;;) {
        void* np = ZETA_BinTreeNodeOperator_GetP(btn_opr, n);
        if (np == NULL) { break; }

        size_t np_acc_size = ZETA_BinTreeNodeOperator_GetAccSize(btn_opr, np);
        size_t k = np_acc_size - n_acc_size;

        if (ZETA_BinTreeNodeOperator_GetL(btn_opr, np) == n) {
            r_acc_size += k;
        } else {
            l_acc_size += k;
        }

        n = np;
        n_acc_size = np_acc_size;
    }

    if (dst_l_acc_size != NULL) { *dst_l_acc_size = l_acc_size; }
    if (dst_r_acc_size != NULL) { *dst_r_acc_size = r_acc_size; }
}

static void Sanitize_(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    void* nl = ZETA_BinTreeNodeOperator_GetL(btn_opr, n);
    void* nr = ZETA_BinTreeNodeOperator_GetR(btn_opr, n);

    if (nl != NULL) {
        ZETA_DebugAssert(ZETA_BinTreeNodeOperator_GetP(btn_opr, nl) == n);
        Sanitize_(btn_opr, nl);
    }

    if (nr != NULL) {
        ZETA_DebugAssert(ZETA_BinTreeNodeOperator_GetP(btn_opr, nr) == n);
        Sanitize_(btn_opr, nr);
    }
}

void Zeta_BinTree_Sanitize(Zeta_BinTreeNodeOperator const* btn_opr,
                           void* root) {
    ZETA_DebugAssert(btn_opr != NULL);

    if (root == NULL) { return; }

    ZETA_DebugAssert(ZETA_BinTreeNodeOperator_GetP(btn_opr, root) == NULL);

    Sanitize_(btn_opr, root);
}
