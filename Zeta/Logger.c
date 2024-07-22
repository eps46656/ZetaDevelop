#include "Logger.h"

#include "Pipe.h"

void Zeta_Logger_LogUDec_(Zeta_Pipe* pipe, void* ptr_) {
    void* ptr = *((void**)ptr_);

    pipe->Write(pipe->context, 2, "0x");

    Zeta_Pipe_WriteUInt(pipe, ZETA_PtrToAddr(ptr), FALSE, 16, ZETA_io_r_just,
                        ZETA_CeilIntDiv(ZETA_WidthOf(uintptr_t), 4), '0');
}

void Zeta_Logger_LogBool_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteStr(
        pipe, (unsigned char const*)(*((bool_t const*)ptr) ? "True " : "False"),
        ZETA_io_l_just, 0, ' ');
}

void Zeta_Logger_LogChar_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteSInt(pipe, *((char const*)ptr), FALSE, 10, ZETA_io_r_just,
                        ZETA_LogVar_varval_just_width, ' ');
}

void Zeta_Logger_LogUChar_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteUInt(pipe, *((unsigned char const*)ptr), FALSE, 10,
                        ZETA_io_r_just, ZETA_LogVar_varval_just_width, ' ');
}

void Zeta_Logger_LogSChar_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteSInt(pipe, *((signed char const*)ptr), FALSE, 10,
                        ZETA_io_r_just, ZETA_LogVar_varval_just_width, ' ');
}

void Zeta_Logger_LogUShort_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteUInt(pipe, *((unsigned short const*)ptr), FALSE, 10,
                        ZETA_io_r_just, ZETA_LogVar_varval_just_width, ' ');
}

void Zeta_Logger_LogSShort_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteSInt(pipe, *((short const*)ptr), FALSE, 10, ZETA_io_r_just,
                        ZETA_LogVar_varval_just_width, ' ');
}

void Zeta_Logger_LogUInt_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteUInt(pipe, *((unsigned const*)ptr), FALSE, 10,
                        ZETA_io_r_just, ZETA_LogVar_varval_just_width, ' ');
}

void Zeta_Logger_LogSInt_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteSInt(pipe, *((int const*)ptr), FALSE, 10, ZETA_io_r_just,
                        ZETA_LogVar_varval_just_width, ' ');
}

void Zeta_Logger_LogULong_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteUInt(pipe, *((unsigned long const*)ptr), FALSE, 10,
                        ZETA_io_r_just, ZETA_LogVar_varval_just_width, ' ');
}

void Zeta_Logger_LogSLong_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteSInt(pipe, *((long const*)ptr), FALSE, 10, ZETA_io_r_just,
                        ZETA_LogVar_varval_just_width, ' ');
}

void Zeta_Logger_LogULLong_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteUInt(pipe, *((unsigned long long const*)ptr), FALSE, 10,
                        ZETA_io_r_just, ZETA_LogVar_varval_just_width, ' ');
}

void Zeta_Logger_LogSLLong_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteSInt(pipe, *((long long const*)ptr), FALSE, 10,
                        ZETA_io_r_just, ZETA_LogVar_varval_just_width, ' ');
}

void Zeta_Logger_LogPtr_(Zeta_Pipe* pipe, void const* ptr) {
    pipe->Write(pipe->context, 2, "0x");

    Zeta_Pipe_WriteUInt(pipe, ZETA_PtrToAddr(*((void* const*)ptr)), FALSE, 16,
                        ZETA_io_r_just, ZETA_LogVar_varval_just_width - 2, '0');
}

void Zeta_Logger_LogStr_(Zeta_Pipe* pipe, void const* ptr) {
    Zeta_Pipe_WriteStr(pipe, *(unsigned char const**)ptr, ZETA_io_l_just, 0,
                       ' ');
}
