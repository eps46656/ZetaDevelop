#include "logger.h"

#include "debugger.h"
#include "pipe.h"

void Zeta_Logger_LogUDec_(Zeta_Pipe* pipe, void* ptr_) {
    void* ptr = *((void**)ptr_);

    ZETA_CallMemberFunc(pipe, Write, 2, "0x");

    Zeta_Pipe_WriteUInt(                                    //
        pipe,                                               // pipe
        ZETA_PtrToAddr(ptr),                                // val
        FALSE,                                              // sign
        16,                                                 // base
        FALSE,                                              // lower_case
        FALSE,                                              // just_left
        ZETA_UnsafeCeilIntDiv(ZETA_WidthOf(uintptr_t), 4),  // just_width
        '0'                                                 // just_char
    );
}

void Zeta_Logger_LogBool_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteStr(
        pipe, (unsigned char const*)(*((bool_t const*)ptr) ? "True " : "False"),
        TRUE, 0, ' ');
}

void Zeta_Logger_LogChar_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteSInt(                //
        pipe,                           // pipe
        *((char const*)ptr),            // val
        FALSE,                          // sign
        10,                             // base
        FALSE,                          // lower_case
        FALSE,                          // just_left
        ZETA_LogVar_varval_just_width,  // just_width
        ' '                             // just_char
    );
}

void Zeta_Logger_LogUChar_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteUInt(                //
        pipe,                           // pipe
        *((unsigned char const*)ptr),   // val
        FALSE,                          // sign
        10,                             // base
        FALSE,                          // lower_case
        FALSE,                          // just_left
        ZETA_LogVar_varval_just_width,  // just_width
        ' '                             // just_char
    );
}

void Zeta_Logger_LogSChar_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteSInt(                //
        pipe,                           // pipe
        *((signed char const*)ptr),     // val
        FALSE,                          // sign
        10,                             // base
        FALSE,                          // lower_case
        FALSE,                          // just_left
        ZETA_LogVar_varval_just_width,  // just_width
        ' '                             // just_char
    );
}

void Zeta_Logger_LogUShort_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteUInt(                //
        pipe,                           // pipe
        *((unsigned short const*)ptr),  // val
        FALSE,                          // sign
        10,                             // base
        FALSE,                          // lower_case
        FALSE,                          // just_left
        ZETA_LogVar_varval_just_width,  // just_width
        ' '                             // just_char
    );
}

void Zeta_Logger_LogSShort_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteSInt(                //
        pipe,                           // pipe
        *((short const*)ptr),           // val
        FALSE,                          // sign
        10,                             // base
        FALSE,                          // lower_case
        FALSE,                          // just_left
        ZETA_LogVar_varval_just_width,  // just_width
        ' '                             // just_char
    );
}

void Zeta_Logger_LogUInt_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteUInt(                //
        pipe,                           // pipe
        *((unsigned const*)ptr),        // val
        FALSE,                          // sign
        10,                             // base
        FALSE,                          // lower_case
        FALSE,                          // just_left
        ZETA_LogVar_varval_just_width,  // just_width
        ' '                             // just_char
    );
}

void Zeta_Logger_LogSInt_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteSInt(                //
        pipe,                           // pipe
        *((int const*)ptr),             // val
        FALSE,                          // sign
        10,                             // base
        FALSE,                          // lower_case
        FALSE,                          // just_left
        ZETA_LogVar_varval_just_width,  // just_width
        ' '                             // just_char
    );
}

void Zeta_Logger_LogULong_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteUInt(                //
        pipe,                           // pipe
        *((unsigned long const*)ptr),   // val
        FALSE,                          // sign
        10,                             // base
        FALSE,                          // lower_case
        FALSE,                          // just_left
        ZETA_LogVar_varval_just_width,  // just_width
        ' '                             // just_char
    );
}

void Zeta_Logger_LogSLong_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteSInt(                //
        pipe,                           // pipe
        *((long const*)ptr),            // val
        FALSE,                          // sign
        10,                             // base
        FALSE,                          // lower_case
        FALSE,                          // just_left
        ZETA_LogVar_varval_just_width,  // just_width
        ' '                             // just_char
    );
}

void Zeta_Logger_LogULLong_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteUInt(                    //
        pipe,                               // pipe
        *((unsigned long long const*)ptr),  // val
        FALSE,                              // sign
        10,                                 // base
        FALSE,                              // lower_case
        FALSE,                              // just_left
        ZETA_LogVar_varval_just_width,      // just_width
        ' '                                 // just_char
    );
}

void Zeta_Logger_LogSLLong_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteSInt(                //
        pipe,                           // pipe
        *((long long const*)ptr),       // val
        FALSE,                          // sign
        10,                             // base
        FALSE,                          // lower_case
        FALSE,                          // just_left
        ZETA_LogVar_varval_just_width,  // just_width
        ' '                             // just_char
    );
}

void Zeta_Logger_LogPtr_(Zeta_Pipe* pipe, void const* ptr) {
    ZETA_CallMemberFunc(pipe, Write, 2, "0x");

    Zeta_Pipe_WriteUInt(                       //
        pipe,                                  // pipe
        ZETA_PtrToAddr(*((void* const*)ptr)),  // val
        FALSE,                                 // sign
        16,                                    // base
        FALSE,                                 // lower_case
        FALSE,                                 // just_left
        ZETA_LogVar_varval_just_width - 2,     // just_width
        '0'                                    // just_char
    );
}

void Zeta_Logger_LogStr_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteStr(pipe, *(unsigned char const**)ptr, TRUE, 0, ' ');
}
