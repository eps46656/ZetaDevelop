#pragma once

#include "define.h"

typedef struct Zeta_SeqContainer Zeta_SeqContainer;

struct Zeta_SeqContainer {
    void* context;

    size_t (*GetWidth)(void* context);
    size_t (*GetSize)(void* context);

    void (*GetLBCursor)(void* context, void* dst_cursor);
    void (*GetRBCursor)(void* context, void* dst_cursor);

    void* (*PeekL)(void* context, void* dst_cursor);
    void* (*PeekR)(void* context, void* dst_cursor);

    void* (*Access)(void* context, void* dst_cursor, size_t idx);

    void* (*PushL)(void* context, void* dst_cursor);
    void* (*PushR)(void* context, void* dst_cursor);

    void* (*Insert)(void* context, void* pos_cursor);

    void (*PopL)(void* context);
    void (*PopR)(void* context);

    void* (*Erase)(void* context, void* pos_cursor);
    void (*EraseAll)(void* context, void* callback_context,
                     void (*Callback)(void* context, void* ele));
};

void Zeta_SeqContainer_Init(Zeta_SeqContainer* seq_cntr);
