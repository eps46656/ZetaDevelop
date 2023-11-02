import os
import random
import ctypes
import time

File = os.path.abspath(__file__).replace("\\", "/")
Dir = os.path.dirname(File).replace("\\", "/")

void_p = ctypes.c_void_p
diff_t = ctypes.c_longlong
key_t = ctypes.c_ulonglong
val_t = ctypes.c_ulonglong

mlmap_so_file = f"{Dir}/mlmap.so"

mlmap_so = ctypes.CDLL(mlmap_so_file)

mlmap_so.MLMap_GetVal.argtypes = [void_p, void_p]
mlmap_so.MLMap_GetVal.restype = val_t

mlmap_so.MLMap_SetVal.argtypes = [void_p, void_p, val_t]
mlmap_so.MLMap_SetVal.restypes = None

mlmap_so.MLMap_Create.argtypes = []
mlmap_so.MLMap_Create.restype = void_p

mlmap_so.MLMap_GetSize.argtypes = [void_p]
mlmap_so.MLMap_GetSize.restype = val_t

mlmap_so.MLMap_IsNull.argtypes = [void_p]
mlmap_so.MLMap_IsNull.restype = ctypes.c_bool

mlmap_so.MLMap_GetVal.argtypes = [void_p]
mlmap_so.MLMap_GetVal.restype = val_t

mlmap_so.MLMap_SetVal.argtypes = [void_p, val_t]
mlmap_so.MLMap_SetVal.restype = None

mlmap_so.MLMap_Find.argtypes = [void_p, diff_t]
mlmap_so.MLMap_Find.restype = void_p

mlmap_so.MLMap_FindNext.argtypes = [void_p, key_t]
mlmap_so.MLMap_FindNext.restype = void_p

mlmap_so.MLMap_Insert.argtypes = [void_p, key_t, val_t]
mlmap_so.MLMap_Insert.restype = void_p

mlmap_so.MLMap_Erase.argtypes = [void_p, void_p]
mlmap_so.MLMap_Erase.restype = None

ctypes.pythonapi.Py_IncRef.argtypes = [ctypes.py_object]
ctypes.pythonapi.Py_IncRef.restype = None

ctypes.pythonapi.Py_DecRef.argtypes = [ctypes.py_object]
ctypes.pythonapi.Py_DecRef.restype = None

IncRef = ctypes.pythonapi.Py_IncRef
DecRef = ctypes.pythonapi.Py_DecRef

def id_to_obj(id):
    return ctypes.cast(id, ctypes.py_object).value

class MLMap:
    def __init__(self):
        # (hash_of(key), (key, val))
        self.mlmap_ptr = mlmap_so.MLMap_Create()

    def __len__(self):
        return mlmap_so.MLMap_GetSize(self.mlmap_ptr)

    def getn_(self, key):
        # p = (key, val)
        # (hash(key), id(p))

        h = hash(key)
        n = mlmap_so.MLMap_Find(self.mlmap_ptr, h)

        while n != None and key != id_to_obj(mlmap_so.MLMap_GetVal(n))[0]:
            n = mlmap_so.MLMap_FindNext(n, h)

        return n

    def __contains__(self, key):
        return self.getn_(key) != None

    def __getitem__(self, key):
        n = self.getn_(key)
        return None if n == None else id_to_obj(mlmap_so.MLMap_GetVal(n))[1]

    def __setitem__(self, key, val):
        # p = (key, val)
        # (hash(key), id(p))

        n = self.getn_(key)

        if n == None:
            p = (key, val)
            mlmap_so.MLMap_Insert(self.mlmap_ptr, hash(key), id(p))
            IncRef(p)
        else:
            old_p = id_to_obj(mlmap_so.MLMap_GetVal(n))
            assert old_p[0] == key
            new_p = (key, val)
            mlmap_so.MLMap_SetVal(n, id(new_p))
            IncRef(new_p)
            DecRef(old_p)

    def pop(self, key):
        n = self.getn_(key)

        if n == None:
            return None

        p = id_to_obj(mlmap_so.MLMap_GetVal(n))
        val = p[1]
        DecRef(p)
        mlmap_so.MLMap_Erase(self.mlmap_ptr, n)

        return val

    '''
    def __str__(self):
        s = ", ".join(str(self[i]) for i in range(len(self)))
        return f"[{s}]"
    '''

if __name__ == "__main__":
    mlmap = MLMap()

    mlmap[5] = 3

    print(mlmap[3])
    print(mlmap[5])
    print(mlmap[0])
