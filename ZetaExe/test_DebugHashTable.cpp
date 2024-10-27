#include "../Zeta/debug_hash_table.h"

int main() {
    void* ht = DebugHashTable_Create();

    *DebugHashTable_Insert(ht, 0) = 5;
    *DebugHashTable_Insert(ht, 56) = 9;
    *DebugHashTable_Insert(ht, 6) = 13;

    DebugHashTable_Print(ht);

    return 0;
}
