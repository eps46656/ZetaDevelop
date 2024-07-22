#include "DebugStrPipe.h"

#include <deque>

#include "Debugger.h"

struct Zeta_DebugStrPipe {
    std::deque<unsigned char> buffer;
};

void* Zeta_DebugStrPipe_Create() { return new Zeta_DebugStrPipe{}; }

void Zeta_DebugStrPipe_Destroy(void* debug_str_pipe_) {
    Zeta_DebugStrPipe* debug_str_pipe = (Zeta_DebugStrPipe*)debug_str_pipe_;
    ZETA_DebugAssert(debug_str_pipe != NULL);

    delete debug_str_pipe;
}

unsigned Zeta_DebugStrPipe_GetType(void* debug_str_pipe_) {
    Zeta_DebugStrPipe* debug_str_pipe = (Zeta_DebugStrPipe*)debug_str_pipe_;
    ZETA_DebugAssert(debug_str_pipe != NULL);

    return 0;
}

unsigned Zeta_DebugStrPipe_GetFlags(void* debug_str_pipe_) {
    Zeta_DebugStrPipe* debug_str_pipe = (Zeta_DebugStrPipe*)debug_str_pipe_;
    ZETA_DebugAssert(debug_str_pipe != NULL);

    return 0;
}

bool_t Zeta_DebugStrPipe_IsEmpty(void* debug_str_pipe_) {
    Zeta_DebugStrPipe* debug_str_pipe = (Zeta_DebugStrPipe*)debug_str_pipe_;
    ZETA_DebugAssert(debug_str_pipe != NULL);

    return debug_str_pipe->buffer.empty();
}

void Zeta_DebugStrPipe_Peek(void* debug_str_pipe_, size_t cnt, void* dst_) {
    Zeta_DebugStrPipe* debug_str_pipe = (Zeta_DebugStrPipe*)debug_str_pipe_;
    ZETA_DebugAssert(debug_str_pipe != NULL);

    unsigned char* dst = (unsigned char*)dst_;
    ZETA_DebugAssert(dst != NULL);

    ZETA_DebugAssert(cnt <= debug_str_pipe->buffer.size());
}

void Zeta_DebugStrPipe_Read(void* debug_str_pipe_, size_t cnt, void* dst_) {
    Zeta_DebugStrPipe* debug_str_pipe = (Zeta_DebugStrPipe*)debug_str_pipe_;
    ZETA_DebugAssert(debug_str_pipe != NULL);

    unsigned char* dst = (unsigned char*)dst_;
    ZETA_DebugAssert(dst != NULL);

    ZETA_DebugAssert(cnt <= debug_str_pipe->buffer.size());

    for (size_t i = 0; i < cnt; ++i) {
        dst[i] = debug_str_pipe->buffer.front();
        debug_str_pipe->buffer.pop_front();
    }
}

void Zeta_DebugStrPipe_Write(void* debug_str_pipe_, size_t cnt,
                             void const* src_) {
    Zeta_DebugStrPipe* debug_str_pipe = (Zeta_DebugStrPipe*)debug_str_pipe_;
    ZETA_DebugAssert(debug_str_pipe != NULL);

    unsigned char const* src = (unsigned char const*)src_;
    ZETA_DebugAssert(src != NULL);

    for (size_t i = 0; i < cnt; ++i) {
        debug_str_pipe->buffer.push_back(src[i]);
    }
}

void Zeta_DebugStrPipe_Clear(void* debug_str_pipe_) {
    Zeta_DebugStrPipe* debug_str_pipe = (Zeta_DebugStrPipe*)debug_str_pipe_;
    ZETA_DebugAssert(debug_str_pipe != NULL);

    debug_str_pipe->buffer.clear();
}

void Zeta_DebugStrPipe_Flush(void* debug_str_pipe_) {
    Zeta_DebugStrPipe* debug_str_pipe = (Zeta_DebugStrPipe*)debug_str_pipe_;
    ZETA_DebugAssert(debug_str_pipe != NULL);
}

void Zeta_DebugStrPipe_DeployPipe(void* debug_str_pipe_, Zeta_Pipe* pipe) {
    Zeta_DebugStrPipe* debug_str_pipe = (Zeta_DebugStrPipe*)debug_str_pipe_;
    ZETA_DebugAssert(debug_str_pipe != NULL);

    ZETA_DebugAssert(pipe != NULL);

    pipe->context = debug_str_pipe;

    pipe->GetType = Zeta_DebugStrPipe_GetType;

    pipe->GetFlags = Zeta_DebugStrPipe_GetFlags;

    pipe->IsEmpty = Zeta_DebugStrPipe_IsEmpty;

    pipe->Peek = Zeta_DebugStrPipe_Peek;

    pipe->Read = Zeta_DebugStrPipe_Read;

    pipe->Write = Zeta_DebugStrPipe_Write;

    pipe->Clear = Zeta_DebugStrPipe_Clear;

    pipe->Flush = Zeta_DebugStrPipe_Flush;
}
