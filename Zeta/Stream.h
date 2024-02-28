#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef struct Zeta_Stream Zeta_Stream;

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

void Zeta_StreamTransfer(Zeta_Stream* dst, Zeta_Stream* src);

ZETA_extern_c_end;
