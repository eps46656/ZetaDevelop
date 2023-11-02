#include "../Zeta/define.h"

#define WINDOWS

#if defined(WINDOWS)

#include <winbase.h>
#include <windows.h>

ZETA_DECL_STRUCT(Pipe) { HANDLE hl; };

void Pipe_Open(const char* name);

void Pipe_Open(const char* name) {
    HANDLE hl =
        CreateNamedPipeA(TEXT(name),          // name
                         PIPE_ACCESS_DUPLEX,  // open_mode: io
                         PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_NOWAIT,  //
                         1,               // max instance
                         4 * 1024,        // out buffer size
                         4 * 1024,        // in buffer size
                         1000 * 60 * 60,  // default timeout (ms)
                         NULL);
}

#elif defined(LINUX)

#endif
