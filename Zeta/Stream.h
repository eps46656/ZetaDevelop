#pragma once

#include "define.h"

ZETA_extern_c_beg;

#define ZETA_Stream_type_byte 0
#define ZETA_Stream_type_unichar 1

typedef struct Zeta_Stream Zeta_Stream;

struct Zeta_Stream {
    void* context;

    int (*GetType)(void* context);

    int (*GetFlags)(void* context);

    bool_t (*IsEmpty)(void* context);

    const void* (*Peek)(void* context);

    void (*Read)(void* context);

    void (*Unread)(void* context, const void* data);

    void (*Write)(void* context, const void* data);
};

ZETA_extern_c_end;
