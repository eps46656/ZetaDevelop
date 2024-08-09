#include <stdio.h>

#include "io.h"

ZETA_ExternC_Beg;

#define ZETA_LogPos_filename_just_width (48)
#define ZETA_LogPos_line_num_just_width (4)
#define ZETA_LogPos_funcname_just_width (16)
#define ZETA_LogVar_varname_just_width (16)
#define ZETA_LogVar_varval_just_width (24)

#define ZETA_LogPos_(pipe_tmp, filename_tmp, line_num_tmp, fucname_tmp, pipe,  \
                     filename, line_num, funcname, new_line)                   \
    {                                                                          \
        Zeta_Pipe* pipe_tmp = (pipe);                                          \
        unsigned char const filename_tmp[] = (filename);                       \
        size_t line_num_tmp = (line_num);                                      \
        unsigned char* const funcname_tmp = (unsigned char* const)(funcname);  \
                                                                               \
        Zeta_Pipe_WriteStr(pipe_tmp, filename_tmp, ZETA_io_r_just,             \
                           ZETA_LogPos_filename_just_width, ' ');              \
                                                                               \
        pipe_tmp->Write(pipe_tmp->context, 1, ":");                            \
                                                                               \
        Zeta_Pipe_WriteUInt(pipe_tmp, line_num_tmp, FALSE, 10, ZETA_io_l_just, \
                            ZETA_LogPos_line_num_just_width, ' ');             \
                                                                               \
        pipe_tmp->Write(pipe_tmp->context, 1, "\t");                           \
                                                                               \
        Zeta_Pipe_WriteStr(pipe_tmp, funcname_tmp, ZETA_io_l_just,             \
                           ZETA_LogPos_funcname_just_width, ' ');              \
                                                                               \
        if (new_line) { pipe_tmp->Write(pipe_tmp->context, 1, "\n"); }         \
    }                                                                          \
    ZETA_StaticAssert(TRUE)

#define ZETA_LogPos(pipe, filename, line_num, funcname, new_line)        \
    ZETA_LogPos_(ZETA_TmpName, ZETA_TmpName, ZETA_TmpName, ZETA_TmpName, \
                 (pipe), (filename), (line_num), (funcname), (new_line))

#define ZETA_LogCurPos(pipe) \
    ZETA_LogPos((pipe), __FILE__, __LINE__, __PRETTY_FUNCTION__, TRUE)

#define ZETA_LogVar_(pipe_tmp, varname_tmp, varval_tmp, pipe, var)             \
    {                                                                          \
        Zeta_Pipe* pipe_tmp = (pipe);                                          \
        unsigned char varname_tmp[] = ZETA_ToStr(var);                         \
        ZETA_AutoVar(varval_tmp, var);                                         \
                                                                               \
        ZETA_LogPos(pipe_tmp, __FILE__, __LINE__, __PRETTY_FUNCTION__, FALSE); \
                                                                               \
        pipe_tmp->Write(pipe_tmp->context, 1, "\t");                           \
                                                                               \
        Zeta_Pipe_WriteStr(pipe_tmp, varname_tmp, ZETA_io_r_just,              \
                           ZETA_LogVar_varname_just_width, ' ');               \
                                                                               \
        pipe_tmp->Write(pipe_tmp->context, 3, " = ");                          \
                                                                               \
        _Generic((var),                                                        \
            bool_t: Zeta_Logger_LogBool_(pipe_tmp, &varval_tmp),               \
                                                                               \
            char: Zeta_Logger_LogChar_(pipe_tmp, &varval_tmp),                 \
            unsigned char: Zeta_Logger_LogUChar_(pipe_tmp, &varval_tmp),       \
            signed char: Zeta_Logger_LogSChar_(pipe_tmp, &varval_tmp),         \
                                                                               \
            unsigned short: Zeta_Logger_LogUShort_(pipe_tmp, &varval_tmp),     \
            short: Zeta_Logger_LogSShort_(pipe_tmp, &varval_tmp),              \
                                                                               \
            unsigned: Zeta_Logger_LogUInt_(pipe_tmp, &varval_tmp),             \
            int: Zeta_Logger_LogSInt_(pipe_tmp, &varval_tmp),                  \
                                                                               \
            unsigned long: Zeta_Logger_LogULong_(pipe_tmp, &varval_tmp),       \
            long: Zeta_Logger_LogSLong_(pipe_tmp, &varval_tmp),                \
                                                                               \
            unsigned long long: Zeta_Logger_LogULLong_(pipe_tmp, &varval_tmp), \
            long long: Zeta_Logger_LogSLLong_(pipe_tmp, &varval_tmp),          \
                                                                               \
            void*: Zeta_Logger_LogPtr_(pipe_tmp, &varval_tmp),                 \
            void const*: Zeta_Logger_LogPtr_(pipe_tmp, &varval_tmp),           \
                                                                               \
            unsigned char*: Zeta_Logger_LogStr_(pipe_tmp, &varval_tmp),        \
            unsigned char const*: Zeta_Logger_LogStr_(pipe_tmp, &varval_tmp)); \
                                                                               \
        pipe_tmp->Write(pipe_tmp->context, 1, "\n");                           \
    }                                                                          \
    ZETA_StaticAssert(TRUE)

#define ZETA_LogVar(pipe, var) \
    ZETA_LogVar_(ZETA_TmpName, ZETA_TmpName, ZETA_TmpName, pipe, var)

void Zeta_Logger_LogBool_(Zeta_Pipe* pipe, void const* ptr);

void Zeta_Logger_LogChar_(Zeta_Pipe* pipe, void const* ptr);
void Zeta_Logger_LogUChar_(Zeta_Pipe* pipe, void const* ptr);
void Zeta_Logger_LogSChar_(Zeta_Pipe* pipe, void const* ptr);

void Zeta_Logger_LogUShort_(Zeta_Pipe* pipe, void const* ptr);
void Zeta_Logger_LogSShort_(Zeta_Pipe* pipe, void const* ptr);

void Zeta_Logger_LogUInt_(Zeta_Pipe* pipe, void const* ptr);
void Zeta_Logger_LogSInt_(Zeta_Pipe* pipe, void const* ptr);

void Zeta_Logger_LogULong_(Zeta_Pipe* pipe, void const* ptr);
void Zeta_Logger_LogSLong_(Zeta_Pipe* pipe, void const* ptr);

void Zeta_Logger_LogULLong_(Zeta_Pipe* pipe, void const* ptr);
void Zeta_Logger_LogSLLong_(Zeta_Pipe* pipe, void const* ptr);

void Zeta_Logger_LogPtr_(Zeta_Pipe* pipe, void const* ptr);

void Zeta_Logger_LogStr_(Zeta_Pipe* pipe, void const* ptr);

ZETA_ExternC_End;
