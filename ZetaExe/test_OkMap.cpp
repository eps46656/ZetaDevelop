#include "OkMap.h"

int main() {
    void* ok_map = OkMap_Create();

    *OkMap_Insert(ok_map, 0) = 5;
    *OkMap_Insert(ok_map, 56) = 9;
    *OkMap_Insert(ok_map, 6) = 13;

    OkMap_Print(ok_map);

    return 0;
}
