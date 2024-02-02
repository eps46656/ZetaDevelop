import os
import sys
import random
import ctypes
import time

File = os.path.abspath(__file__).replace("\\", "/")
Dir = os.path.dirname(File).replace("\\", "/")

void_p = ctypes.c_void_p
size_t = ctypes.c_ulonglong
val_t = ctypes.c_ulonglong

dyvec_so_file = f"{Dir}/DyVec.so"

dyvec_so = ctypes.CDLL(dyvec_so_file)

dyvec_so.DyVec_Create.argtypes = []
dyvec_so.DyVec_Create.restype = void_p

dyvec_so.DyVec_GetSize.argtypes = [void_p]
dyvec_so.DyVec_GetSize.restype = val_t

dyvec_so.DyVec_GetVal.argtypes = [void_p, size_t]
dyvec_so.DyVec_GetVal.restype = val_t

dyvec_so.DyVec_SetVal.argtypes = [void_p, size_t, val_t]
dyvec_so.DyVec_SetVal.restypes = None

dyvec_so.DyVec_Insert.argtypes = [void_p, size_t, val_t]
dyvec_so.DyVec_Insert.restype = None

dyvec_so.DyVec_Erase.argtypes = [void_p, size_t]
dyvec_so.DyVec_Erase.restype = None

dyvec_so.DyVec_Check.argtypes = [void_p]
dyvec_so.DyVec_Check.restype = None

ctypes.pythonapi.Py_IncRef.argtypes = [ctypes.py_object]
ctypes.pythonapi.Py_IncRef.restype = None

ctypes.pythonapi.Py_DecRef.argtypes = [ctypes.py_object]
ctypes.pythonapi.Py_DecRef.restype = None

def id_to_obj(id):
    return ctypes.cast(id, ctypes.py_object).value

IncRef = ctypes.pythonapi.Py_IncRef
DecRef = ctypes.pythonapi.Py_DecRef

class DyVec:
    def __init__(self):
        self.dyvec_ptr = dyvec_so.DyVec_Create()

    def __len__(self):
        return dyvec_so.DyVec_GetSize(self.dyvec_ptr)

    def __getitem__(self, idx):
        item_id = dyvec_so.DyVec_GetVal(self.dyvec_ptr, idx)
        return id_to_obj(item_id)

    def __setitem__(self, idx, item):
        DecRef(id_to_obj(dyvec_so.DyVec_GetVal(self.dyvec_ptr, idx)))
        dyvec_so.DyVec_SetVal(self.dyvec_ptr, idx, id(item))
        IncRef(item)

    def insert(self, idx, item):
        dyvec_so.DyVec_Insert(self.dyvec_ptr, idx, id(item))
        IncRef(item)

    def pop(self, idx):
        DecRef(id_to_obj(dyvec_so.DyVec_GetVal(self.dyvec_ptr, idx)))
        dyvec_so.DyVec_Erase(self.dyvec_ptr, idx)

    def __str__(self):
        s = ", ".join(str(self[i]) for i in range(len(self)))
        return f"[{s}]"

    def check(self):
        dyvec_so.DyVec_Check(self.dyvec_ptr)

l = DyVec()
# l = list()

def GetSize():
    global l

    return len(l)

def GetValWithIdx(idx):
    global l

    return l[idx]

def SetValWithIdx(idx, val):
    global l

    l[idx] = val

def InsertWithIdx():
    global l

    pos = random.randint(0, len(l))
    # pos = len(l)
    val = random.randint(-1000, 1000)
    l.insert(pos, val)

def EraseWithIdx():
    global l

    if len(l) == 0:
        return

    pos = random.randint(0, len(l)-1)
    l.pop(pos)



if __name__ == "__main__":
    start_time = time.time()

    for _ in range(10):
        InsertWithIdx()

    print("ok")

    end_time = time.time()

    print(f" start_time = {start_time}")
    print(f"   end_time = {end_time}")
    print(f"elapse_time = {end_time - start_time}")
