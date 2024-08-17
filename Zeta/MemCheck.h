#pragma once

#include "AssocContainer.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_MemRecorder);

struct Zeta_MemRecorder;

Zeta_MemRecorder* Zeta_MemRecorder_Create();

void Zeta_MemRecorder_Destroy(Zeta_MemRecorder* mem_recorder);

size_t Zeta_MemRecorder_GetSize(Zeta_MemRecorder* mem_recorder);

size_t Zeta_MemRecorder_GetRecordSize(Zeta_MemRecorder* mem_recorder,
                                      void const* ptr);

void Zeta_MemRecorder_Record(Zeta_MemRecorder* mem_recorder, void const* ptr,
                             size_t size);

bool_t Zeta_MemRecorder_Unrecord(Zeta_MemRecorder* mem_recorder,
                                 void const* ptr);

void Zeta_MemCheck_MatchRecords(Zeta_MemRecorder const* src_mem_recorder,
                                Zeta_MemRecorder const* dst_mem_recorder);

ZETA_ExternC_End;
