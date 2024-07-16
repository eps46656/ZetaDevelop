#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_Pipe);

struct Zeta_Pipe {
    void* context;

    unsigned (*GetType)(void* context);

    unsigned (*GetFlags)(void* context);

    bool_t (*IsEmpty)(void* context);

    void (*Peek)(void* context, size_t cnt, void* dst);

    void (*Read)(void* context, size_t cnt, void* dst);

    void (*Write)(void* context, size_t cnt, void const* data);

    void (*Clear)(void* context);

    void (*Flush)(void* context);
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Zeta_PipeTransfer(Zeta_Pipe* dst, Zeta_Pipe* src);

ZETA_ExternC_End;
