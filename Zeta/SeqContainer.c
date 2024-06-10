#include "SeqContainer.h"

void Zeta_SeqContainer_Init(Zeta_SeqContainer* seq_cntr) {
    seq_cntr->context = NULL;

    seq_cntr->GetWidth = NULL;

    seq_cntr->GetStride = NULL;

    seq_cntr->GetSize = NULL;

    seq_cntr->GetLBCursor = NULL;

    seq_cntr->GetRBCursor = NULL;

    seq_cntr->PeekL = NULL;

    seq_cntr->PeekR = NULL;

    seq_cntr->Access = NULL;

    seq_cntr->Refer = NULL;

    seq_cntr->Read = NULL;

    seq_cntr->Write = NULL;

    seq_cntr->PushL = NULL;

    seq_cntr->PushL = NULL;

    seq_cntr->Insert = NULL;

    seq_cntr->PopL = NULL;

    seq_cntr->PopR = NULL;

    seq_cntr->Erase = NULL;

    seq_cntr->EraseAll = NULL;

    seq_cntr->Cursor_IsEqual = NULL;

    seq_cntr->Cursor_Compare = NULL;

    seq_cntr->Cursor_GetDist = NULL;

    seq_cntr->Cursor_GetIdx = NULL;

    seq_cntr->Cursor_StepL = NULL;

    seq_cntr->Cursor_StepR = NULL;

    seq_cntr->Cursor_AdvanceL = NULL;

    seq_cntr->Cursor_AdvanceR = NULL;
}
