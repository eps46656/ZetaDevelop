#pragma once

#include "pipe.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_DebugStrPipe);

void* Zeta_DebugStrPipe_Create();

void Zeta_DebugStrPipe_Destroy(void* debug_str_pipe);

unsigned Zeta_DebugStrPipe_GetType(void* debug_str_pipe);

unsigned Zeta_DebugStrPipe_GetFlags(void* debug_str_pipe);

bool_t Zeta_DebugStrPipe_IsEmpty(void* debug_str_pipe);

void Zeta_DebugStrPipe_Peek(void* debug_str_pipe, size_t cnt, void* dst);

void Zeta_DebugStrPipe_Read(void* debug_str_pipe, size_t cnt, void* dst);

void Zeta_DebugStrPipe_Write(void* debug_str_pipe, size_t cnt, void const* src);

void Zeta_DebugStrPipe_Clear(void* debug_str_pipe);

void Zeta_DebugStrPipe_Flush(void* debug_str_pipe);

void Zeta_DebugStrPipe_DeployPipe(void* debug_str_pipe, Zeta_Pipe* pipe);

ZETA_ExternC_End;
