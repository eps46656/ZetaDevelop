typedef unsigned long long key_t;
typedef unsigned long long val_t;

#if defined(__cplusplus)
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC void* OkMap_Create();

EXTERNC val_t* OkMap_Access(void* ok_map_, key_t key);

EXTERNC val_t* OkMap_Insert(void* ok_map_, key_t key);

EXTERNC int OkMap_Erase(void* ok_map_, key_t key);

EXTERNC void OkMap_Print(void* ok_map_);
