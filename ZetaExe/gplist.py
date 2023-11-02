import os
import sys
import random
import ctypes
import time

File = os.path.abspath(__file__).replace("\\", "/")
Dir = os.path.dirname(File).replace("\\", "/")

void_p = ctypes.c_void_p
diff_t = ctypes.c_longlong
val_t = ctypes.c_ulonglong

gplist_so_file = f"{Dir}/gplist.so"

gplist_so = ctypes.CDLL(gplist_so_file)

gplist_so.GPList_GetVal.argtypes = [void_p, void_p]
gplist_so.GPList_GetVal.restype = val_t

gplist_so.GPList_SetVal.argtypes = [void_p, void_p, val_t]
gplist_so.GPList_SetVal.restypes = None

gplist_so.GPList_Create.argtypes = []
gplist_so.GPList_Create.restype = void_p

gplist_so.GPList_GetSize.argtypes = [void_p]
gplist_so.GPList_GetSize.restype = val_t

gplist_so.GPList_GetNode.argtypes = [void_p, diff_t]
gplist_so.GPList_GetNode.restype = void_p

gplist_so.GPList_InsertL.argtypes = [void_p, void_p]
gplist_so.GPList_InsertL.restype = void_p

gplist_so.GPList_Erase.argtypes = [void_p, void_p]
gplist_so.GPList_Erase.restype = None

ctypes.pythonapi.Py_IncRef.argtypes = [ctypes.py_object]
ctypes.pythonapi.Py_IncRef.restype = None

ctypes.pythonapi.Py_DecRef.argtypes = [ctypes.py_object]
ctypes.pythonapi.Py_DecRef.restype = None

def id_to_obj(id):
    return ctypes.cast(id, ctypes.py_object).value

IncRef = ctypes.pythonapi.Py_IncRef
DecRef = ctypes.pythonapi.Py_DecRef

class GPList:
    def __init__(self):
        self.gpl_ptr = gplist_so.GPList_Create()

    def __len__(self):
        return gplist_so.GPList_GetSize(self.gpl_ptr)

    def __getitem__(self, idx):
        n = gplist_so.GPList_GetNode(self.gpl_ptr, idx)
        item_id = gplist_so.GPList_GetVal(self.gpl_ptr, n)
        return id_to_obj(item_id)

    def __setitem__(self, idx, item):
        n = gplist_so.GPList_GetNode(self.gpl_ptr, idx)
        DecRef(id_to_obj(gplist_so.GPList_GetVal(self.gpl_ptr, n)))
        gplist_so.GPList_SetVal(self.gpl_ptr, n, id(item))
        IncRef(item)

    def insert(self, idx, item):
        pos = gplist_so.GPList_GetNode(self.gpl_ptr, idx)
        n = gplist_so.GPList_InsertL(self.gpl_ptr, pos)
        gplist_so.GPList_SetVal(self.gpl_ptr, n, id(item))
        IncRef(item)

    def pop(self, idx):
        n = gplist_so.GPList_GetNode(self.gpl_ptr, idx)
        DecRef(id_to_obj(gplist_so.GPList_GetVal(self.gpl_ptr, n)))
        gplist_so.GPList_Erase(self.gpl_ptr, n)

    def __str__(self):
        s = ", ".join(str(self[i]) for i in range(len(self)))
        return f"[{s}]"

l = GPList()
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
