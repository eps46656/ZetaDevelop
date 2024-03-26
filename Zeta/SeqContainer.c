#include "SeqContainer.h"

void Zeta_SeqContainer_Init(Zeta_SeqContainer* seq_cntr) {
    seq_cntr->context = NULL;

    seq_cntr->GetWidth = NULL;
    seq_cntr->GetSize = NULL;

    seq_cntr->GetLBCursor = NULL;
    seq_cntr->GetRBCursor = NULL;

    seq_cntr->PeekL = NULL;
    seq_cntr->PeekR = NULL;

    seq_cntr->Access = NULL;

    seq_cntr->PushL = NULL;
    seq_cntr->PushL = NULL;

    seq_cntr->Insert = NULL;

    seq_cntr->PopL = NULL;
    seq_cntr->PopR = NULL;

    seq_cntr->Erase = NULL;
}
