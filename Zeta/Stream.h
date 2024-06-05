#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_Stream);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct Zeta_Stream {
    void* context;

    int (*GetType)(void* context);

    int (*GetFlags)(void* context);

    bool_t (*IsEmpty)(void* context);

    void const* (*Peek)(void* context);

    void (*Read)(void* context);

    void (*Unread)(void* context, void const* data);

    void (*Write)(void* context, void const* data);
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Zeta_StreamTransfer(Zeta_Stream* dst, Zeta_Stream* src);

ZETA_ExternC_End;
