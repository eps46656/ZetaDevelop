#pragma once

#include "define.h"

ZETA_ExternC_Beg;

typedef struct Zeta_SegAlg_Seg Zeta_SegAlg_Seg;

struct Zeta_SegAlg_Seg {
    size_t offset;
    size_t size;
    size_t capacity;
    void* data;
};

void Zeta_SegAlg_Insert(size_t width, Zeta_SegAlg_Seg* l_seg,
                        Zeta_SegAlg_Seg* m_seg, Zeta_SegAlg_Seg* r_seg,
                        size_t idx);

void Zeta_SegAlg_Erase(size_t width, Zeta_SegAlg_Seg* l_seg,
                       Zeta_SegAlg_Seg* m_seg, Zeta_SegAlg_Seg* r_seg,
                       size_t idx);

ZETA_ExternC_End;
