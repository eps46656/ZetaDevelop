#include "utils.h"

#if !defined(TreeNode)
#error "TreeNode is not defined."
#endif

#pragma push_macro("Zeta_TreeNode_")
#pragma push_macro("AccSizeOnly")
#pragma push_macro("Attach_")
#pragma push_macro("Rotate_")
#pragma push_macro("StepP_")
#pragma push_macro("Step_")
#pragma push_macro("Access_")
#pragma push_macro("Advance_")

#define Zeta_TreeNode_(x) ZETA_Concat(Zeta_, TreeNode, _, x)

#if defined(AccSizeType)

#define AccSizeOnly(x) { x } ZETA_StaticAssert(TRUE)

#else

#define AccSizeOnly(x)

#endif

static size_t Zeta_BinTree_(TreeNode, Count_)(void* n) {
    size_t ret = 1;

    void* nl = Zeta_TreeNode_(GetL)(n);
    void* nr = Zeta_TreeNode_(GetR)(n);

    if (nl != NULL) { ret += Zeta_BinTree_(TreeNode, Count_)(nl); }
    if (nr != NULL) { ret += Zeta_BinTree_(TreeNode, Count_)(nr); }

    return ret;
}

size_t Zeta_BinTree_(TreeNode, Count)(void* n) {
    if (n == NULL) { return 0; }

    return Zeta_BinTree_(TreeNode, Count_)(n);
}

#if defined(AccSizeType)

static void Zeta_BinTree_(TreeNode, AddDiffSize_)(void* n, size_t diff_size) {
    if (diff_size == 0) { return; }

    for (; n != NULL; n = Zeta_TreeNode_(GetP)(n)) {
        Zeta_TreeNode_(SetAccSize)(n,
                                   Zeta_TreeNode_(GetAccSize)(n) + diff_size);
    }
}

size_t Zeta_BinTree_(TreeNode, GetSize)(void* n) {
    size_t n_acc_size = Zeta_TreeNode_(GetAccSize)(n);

    if (n == NULL) { return n_acc_size; }

    return n_acc_size - Zeta_TreeNode_(GetAccSize)(Zeta_TreeNode_(GetL)(n)) -
           Zeta_TreeNode_(GetAccSize)(Zeta_TreeNode_(GetR)(n));
}

void Zeta_BinTree_(TreeNode, SetSize)(void* n, size_t size) {
    ZETA_DebugAssert(n != NULL);

    Zeta_BinTree_(TreeNode, AddDiffSize_)(
        n, size - Zeta_BinTree_(TreeNode, GetSize(n)));
}

void Zeta_BinTree_(TreeNode, SetDiffSize)(void* n, size_t diff_size) {
    ZETA_DebugAssert(n != NULL);

    Zeta_BinTree_(TreeNode, AddDiffSize_)(n, diff_size);
}

#endif

#define Attach_(D)                                                             \
                                                                               \
    ZETA_DebugAssert(n != NULL);                                               \
                                                                               \
    ZETA_DebugAssert(Zeta_TreeNode_(Get##D)(n) == NULL);                       \
    ZETA_DebugAssert(m == NULL || Zeta_TreeNode_(GetP)(m) == NULL);            \
                                                                               \
    if (m == NULL) { return; }                                                 \
                                                                               \
    Zeta_TreeNode_(Set##D)(n, m);                                              \
    Zeta_TreeNode_(SetP)(m, n);                                                \
                                                                               \
    AccSizeOnly({                                                              \
        Zeta_BinTree_(TreeNode, AddDiffSize_)(                                 \
            n,                                                                 \
            Zeta_TreeNode_(GetAccSize)(m) - Zeta_TreeNode_(GetAccSize)(NULL)); \
    });

void Zeta_BinTree_(TreeNode, AttatchL)(void* n, void* m) { Attach_(L); }

void Zeta_BinTree_(TreeNode, AttatchR)(void* n, void* m) { Attach_(R); }

void Zeta_BinTree_(TreeNode, Detach)(void* n) {
    ZETA_DebugAssert(n != NULL);

    void* np = Zeta_TreeNode_(GetP)(n);

    if (np == NULL) { return; }

    if (Zeta_TreeNode_(GetL)(np) == n) {
        Zeta_TreeNode_(SetL)(np, NULL);
    } else {
        Zeta_TreeNode_(SetR)(np, NULL);
    }

    Zeta_TreeNode_(SetP)(n, NULL);

    AccSizeOnly({
        Zeta_BinTree_(TreeNode, AddDiffSize_)(
            np,
            Zeta_TreeNode_(GetAccSize)(NULL) - Zeta_TreeNode_(GetAccSize)(n));
    });
}

static void Zeta_BinTree_(TreeNode, EraseAll_B_)(
    void* n, void (*Callback)(void* callback_context, void* n),
    void* callback_context) {
    ZETA_DebugAssert(n != NULL);

    void* head = NULL;

    unsigned long long random_seed = Zeta_GetRandom();

    for (;;) {
        if (n == NULL) {
            if (head == NULL) { break; }
            n = head;
            head = Zeta_TreeNode_(GetP)(head);
        }

        void* nl = Zeta_TreeNode_(GetL)(n);
        void* nr = Zeta_TreeNode_(GetR)(n);

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
            Zeta_TreeNode_(SetP)(nl, head);
            head = nl;
            n = nr;
        } else {
            Zeta_TreeNode_(SetP)(nr, head);
            head = nr;
            n = nl;
        }
    }
}

static void Zeta_BinTree_(TreeNode, EraseAll_A_)(
    void* n, void (*Callback)(void* callback_context, void* n),
    void* callback_context) {
    size_t buffer_capacity = ZETA_GetMaxOf(24, ZETA_ULLONG_WIDTH * 3 / 4);

    void* buffer[buffer_capacity];
    size_t buffer_i = 0;

    buffer[buffer_i++] = n;

    while (0 < buffer_i) {
        n = buffer[--buffer_i];

        void* nl = Zeta_TreeNode_(GetL)(n);
        void* nr = Zeta_TreeNode_(GetR)(n);

        if (Callback != NULL) { Callback(callback_context, n); }

        if (nl != NULL) {
            if (buffer_i == buffer_capacity) {
                Zeta_BinTree_(TreeNode, EraseAll_B_)(nl, Callback,
                                                     callback_context);
            } else {
                buffer[buffer_i++] = nl;
            }
        }

        if (nr != NULL) {
            if (buffer_i == buffer_capacity) {
                Zeta_BinTree_(TreeNode, EraseAll_B_)(nr, Callback,
                                                     callback_context);
            } else {
                buffer[buffer_i++] = nr;
            }
        }
    }
}

void Zeta_BinTree_(TreeNode,
                   ErassAll)(void* root,
                             void (*Callback)(void* callback_context, void* n),
                             void* callback_context) {
    ZETA_DebugAssert(Zeta_TreeNode_(GetP)(root) == NULL);

    Zeta_BinTree_(TreeNode, EraseAll_A_)(root, Callback, callback_context);
}

void Zeta_BinTree_(TreeNode, Swap)(void* n, void* m) {
    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);

    if (n == m) { return; }

    void* np = Zeta_TreeNode_(GetP)(n);
    void* mp = Zeta_TreeNode_(GetP)(m);

    if (np == m) {
        ZETA_Swap(n, m);
        ZETA_Swap(np, mp);
    }

    size_t n_acc_size;
    size_t m_acc_size;

    size_t n_size;
    size_t m_size;

    ZETA_Unused(n_acc_size);
    ZETA_Unused(m_acc_size);

    ZETA_Unused(n_size);
    ZETA_Unused(m_size);

    void* nl = Zeta_TreeNode_(GetL)(n);
    void* nr = Zeta_TreeNode_(GetR)(n);

    void* ml = Zeta_TreeNode_(GetL)(m);
    void* mr = Zeta_TreeNode_(GetR)(m);

    AccSizeOnly({
        n_acc_size = Zeta_TreeNode_(GetAccSize)(n);
        m_acc_size = Zeta_TreeNode_(GetAccSize)(m);

        n_size = n_acc_size - Zeta_TreeNode_(GetAccSize)(nl) -
                 Zeta_TreeNode_(GetAccSize)(nr);
        m_size = m_acc_size - Zeta_TreeNode_(GetAccSize)(ml) -
                 Zeta_TreeNode_(GetAccSize)(mr);
    });

    if (mp == n) {
        if (np != NULL) {
            if (Zeta_TreeNode_(GetL)(np) == n) {
                Zeta_TreeNode_(SetL)(np, m);
            } else {
                Zeta_TreeNode_(SetR)(np, m);
            }
        }

        Zeta_TreeNode_(SetP)(m, np);

        if (Zeta_TreeNode_(GetL)(n) == m) {
            Zeta_TreeNode_(SetL)(m, n);

            Zeta_TreeNode_(SetR)(m, nr);
            if (nr != NULL) { Zeta_TreeNode_(SetP)(nr, m); }
        } else {
            Zeta_TreeNode_(SetL)(m, nl);
            if (nl != NULL) { Zeta_TreeNode_(SetP)(nl, m); }

            Zeta_TreeNode_(SetR)(m, n);
        }

        Zeta_TreeNode_(SetP)(n, m);

        Zeta_TreeNode_(SetL)(n, ml);
        if (ml != NULL) { Zeta_TreeNode_(SetP)(ml, n); }

        Zeta_TreeNode_(SetR)(n, mr);
        if (mr != NULL) { Zeta_TreeNode_(SetP)(mr, n); }
    } else {
        if (np != NULL) {
            if (Zeta_TreeNode_(GetL)(np) == n) {
                Zeta_TreeNode_(SetL)(np, m);
            } else {
                Zeta_TreeNode_(SetR)(np, m);
            }
        }

        Zeta_TreeNode_(SetP)(m, np);

        Zeta_TreeNode_(SetL)(m, nl);
        if (nl != NULL) { Zeta_TreeNode_(SetP)(nl, m); }

        Zeta_TreeNode_(SetR)(m, nr);
        if (nr != NULL) { Zeta_TreeNode_(SetP)(nr, m); }

        if (mp != NULL) {
            if (Zeta_TreeNode_(GetL)(mp) == m) {
                Zeta_TreeNode_(SetL)(mp, n);
            } else {
                Zeta_TreeNode_(SetR)(mp, n);
            }
        }

        Zeta_TreeNode_(SetP)(n, mp);

        Zeta_TreeNode_(SetL)(n, ml);
        if (ml != NULL) { Zeta_TreeNode_(SetP)(ml, n); }

        Zeta_TreeNode_(SetR)(n, mr);
        if (mr != NULL) { Zeta_TreeNode_(SetP)(mr, n); }
    }

    AccSizeOnly({
        Zeta_TreeNode_(SetAccSize)(n, m_acc_size);
        Zeta_TreeNode_(SetAccSize)(m, n_acc_size);

        Zeta_BinTree_(TreeNode, AddDiffSize_)(n, n_size - m_size);
        Zeta_BinTree_(TreeNode, AddDiffSize_)(m, m_size - n_size);
    });
}

#define Rotate_(D, E)                                                        \
                                                                             \
    ZETA_DebugAssert(n != NULL);                                             \
                                                                             \
    void* ne = Zeta_TreeNode_(Get##E)(n);                                    \
    ZETA_DebugAssert(ne != NULL);                                            \
                                                                             \
    void* ned = Zeta_TreeNode_(Get##D)(ne);                                  \
    void* np = Zeta_TreeNode_(GetP)(n);                                      \
                                                                             \
    if (np != NULL) {                                                        \
        if (Zeta_TreeNode_(Get##D)(np) == n) {                               \
            Zeta_TreeNode_(Set##D)(np, ne);                                  \
        } else {                                                             \
            Zeta_TreeNode_(Set##E)(np, ne);                                  \
        }                                                                    \
    }                                                                        \
                                                                             \
    Zeta_TreeNode_(SetP)(ne, np);                                            \
                                                                             \
    Zeta_TreeNode_(Set##D)(ne, n);                                           \
    Zeta_TreeNode_(SetP)(n, ne);                                             \
                                                                             \
    Zeta_TreeNode_(Set##E)(n, ned);                                          \
    if (ned != NULL) { Zeta_TreeNode_(SetP)(ned, n); }                       \
                                                                             \
    AccSizeOnly({                                                            \
        size_t n_acc_size = Zeta_TreeNode_(GetAccSize)(n);                   \
        size_t ne_acc_size = Zeta_TreeNode_(GetAccSize)(ne);                 \
        size_t ned_acc_size = Zeta_TreeNode_(GetAccSize)(ned);               \
                                                                             \
        Zeta_TreeNode_(SetAccSize)(n,                                        \
                                   n_acc_size - ne_acc_size + ned_acc_size); \
                                                                             \
        Zeta_TreeNode_(SetAccSize)(ne, n_acc_size);                          \
    });

void Zeta_BinTree_(TreeNode, RotateL)(void* n) { Rotate_(L, R); }

void Zeta_BinTree_(TreeNode, RotateR)(void* n) { Rotate_(R, L); }

#define StepP_(E)                                           \
                                                            \
    ZETA_DebugAssert(n != NULL);                            \
                                                            \
    for (;;) {                                              \
        void* np = Zeta_TreeNode_(GetP)(n);                 \
        if (np == NULL) { return NULL; }                    \
        if (Zeta_TreeNode_(Get##E)(np) == n) { return np; } \
        n = np;                                             \
    }

void* Zeta_BinTree_(TreeNode, StepPL)(void* n) { StepP_(R); }

void* Zeta_BinTree_(TreeNode, StepPR)(void* n) { StepP_(L); }

#define Step_(D, E)                                                          \
                                                                             \
    ZETA_DebugAssert(n != NULL);                                             \
                                                                             \
    void* nd = Zeta_TreeNode_(Get##D)(n);                                    \
                                                                             \
    if (nd != NULL) { return Zeta_GetMostLink(nd, Zeta_TreeNode_(Get##E)); } \
                                                                             \
    for (;;) {                                                               \
        void* np = Zeta_TreeNode_(GetP)(n);                                  \
        if (np == NULL) { return NULL; }                                     \
        if (Zeta_TreeNode_(Get##E)(np) == n) { return np; }                  \
        n = np;                                                              \
    }

void* Zeta_BinTree_(TreeNode, StepL)(void* n) { Step_(L, R); }

void* Zeta_BinTree_(TreeNode, StepR)(void* n) { Step_(R, L); }

#if defined(AccSizeType)

#define Access_(D, E)                                                   \
    if (dst_n == NULL && dst_tail_idx == NULL) { return; }              \
                                                                        \
    size_t n_acc_size = Zeta_TreeNode_(GetAccSize)(n);                  \
                                                                        \
    if (n_acc_size <= idx) {                                            \
        if (dst_n != NULL) { *dst_n = NULL; }                           \
        if (dst_tail_idx != NULL) { *dst_tail_idx = idx - n_acc_size; } \
        return;                                                         \
    }                                                                   \
                                                                        \
    while (n != NULL) {                                                 \
        void* nd = Zeta_TreeNode_(Get##D)(n);                           \
        size_t nd_acc_size = Zeta_TreeNode_(GetAccSize)(nd);            \
                                                                        \
        if (idx < nd_acc_size) {                                        \
            n = nd;                                                     \
            n_acc_size = nd_acc_size;                                   \
            continue;                                                   \
        }                                                               \
                                                                        \
        void* ne = Zeta_TreeNode_(Get##E)(n);                           \
        size_t ne_acc_size = Zeta_TreeNode_(GetAccSize)(ne);            \
                                                                        \
        idx -= nd_acc_size;                                             \
        size_t n_size = n_acc_size - nd_acc_size - ne_acc_size;         \
                                                                        \
        if (idx < n_size) { break; }                                    \
                                                                        \
        n = ne;                                                         \
        n_acc_size = ne_acc_size;                                       \
        idx -= n_size;                                                  \
    }                                                                   \
                                                                        \
    if (dst_n != NULL) { *dst_n = n; }                                  \
    if (dst_tail_idx != NULL) { *dst_tail_idx = idx; }

void Zeta_BinTree_(TreeNode, AccessL)(void** dst_n, size_t* dst_tail_idx,
                                      void* n, size_t idx) {
    Access_(L, R);
}

void Zeta_BinTree_(TreeNode, AccessR)(void** dst_n, size_t* dst_tail_idx,
                                      void* n, size_t idx) {
    Access_(R, L);
}

#define Advance_(D, E)                                                         \
    if (dst_n == NULL && dst_tail_idx == NULL) { return; }                     \
                                                                               \
    while (n != NULL && 0 < step) {                                            \
        void* nd = Zeta_TreeNode_(Get##D)(n);                                  \
        void* ne = Zeta_TreeNode_(Get##E)(n);                                  \
                                                                               \
        size_t n_acc_size = Zeta_TreeNode_(GetAccSize)(n);                     \
        size_t nd_acc_size = Zeta_TreeNode_(GetAccSize)(nd);                   \
        size_t ne_acc_size = Zeta_TreeNode_(GetAccSize)(ne);                   \
                                                                               \
        size_t n_size = n_acc_size - nd_acc_size - ne_acc_size;                \
                                                                               \
        if (step < n_size) { break; }                                          \
                                                                               \
        step -= n_size;                                                        \
                                                                               \
        if (step < nd_acc_size) {                                              \
            Zeta_BinTree_(TreeNode, Access##E)(dst_n, dst_tail_idx, nd, step); \
            return;                                                            \
        }                                                                      \
                                                                               \
        step -= nd_acc_size;                                                   \
                                                                               \
        for (;;) {                                                             \
            void* np = Zeta_TreeNode_(GetP)(n);                                \
                                                                               \
            if (np == NULL || Zeta_TreeNode_(Get##E)(np) == n) {               \
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

void Zeta_BinTree_(TreeNode, AdvanceL)(void** dst_n, size_t* dst_tail_idx,
                                       void* n, size_t step) {
    Advance_(L, R);
}

void Zeta_BinTree_(TreeNode, AdvanceR)(void** dst_n, size_t* dst_tail_idx,
                                       void* n, size_t step) {
    Advance_(R, L);
}

void Zeta_BinTree_(TreeNode, GetAccSize)(size_t* dst_l_acc_size,
                                         size_t* dst_r_acc_size, void* n) {
    ZETA_DebugAssert(n != NULL);

    if (dst_l_acc_size == NULL && dst_r_acc_size == NULL) { return; }

    size_t l_acc_size = Zeta_TreeNode_(GetAccSize)(Zeta_TreeNode_(GetL)(n));
    size_t r_acc_size = Zeta_TreeNode_(GetAccSize)(Zeta_TreeNode_(GetR)(n));

    size_t n_acc_size = Zeta_TreeNode_(GetAccSize)(n);

    for (;;) {
        void* np = Zeta_TreeNode_(GetP)(n);
        if (np == NULL) { break; }

        size_t np_acc_size = Zeta_TreeNode_(GetAccSize)(np);
        size_t k = np_acc_size - n_acc_size;

        if (Zeta_TreeNode_(GetL)(np) == n) {
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

#endif

static void Zeta_BinTree_(TreeNode, Sanitize_)(void* n) {
    void* nl = Zeta_TreeNode_(GetL)(n);
    void* nr = Zeta_TreeNode_(GetR)(n);

    if (nl != NULL) {
        ZETA_DebugAssert(Zeta_TreeNode_(GetP)(nl) == n);
        Zeta_BinTree_(TreeNode, Sanitize_)(nl);
    }

    if (nr != NULL) {
        ZETA_DebugAssert(Zeta_TreeNode_(GetP)(nr) == n);
        Zeta_BinTree_(TreeNode, Sanitize_)(nr);
    }
}

void Zeta_BinTree_(TreeNode, Sanitize)(void* root) {
    if (root == NULL) { return; }

    ZETA_DebugAssert(Zeta_TreeNode_(GetP)(root) == NULL);

    Zeta_BinTree_(TreeNode, Sanitize_)(root);
}

#pragma pop_macro("Zeta_TreeNode_")
#pragma pop_macro("AccSizeOnly")
#pragma pop_macro("Attach_")
#pragma pop_macro("Rotate_")
#pragma pop_macro("StepP_")
#pragma pop_macro("Step_")
#pragma pop_macro("Access_")
#pragma pop_macro("Advance_")
