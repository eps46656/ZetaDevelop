import os
import sys
import random
import math
import ctypes
import time

File = os.path.abspath(__file__).replace("\\", "/")
Dir = os.path.dirname(File).replace("\\", "/")

ZetaExeDir = os.path.dirname(File).replace("\\", "/")
ZetaDevDir = f"{ZetaExeDir}/../"

sys.path.append(ZetaDevDir)

import build
from builder import *

class EMPTY_CLASS:
    pass

GLOBAL = EMPTY_CLASS

GLOBAL.void_p = ctypes.c_void_p
GLOBAL.lli = ctypes.c_longlong

def PySort(arr, beg, end):
    brr = arr[beg:end]

    beg_time = time.time()
    brr.sort()
    end_time = time.time()

    print(f"time a = {end_time - beg_time}")

    ret = arr[:]

    for arr_i, brr_val in zip(range(beg, end), brr):
        ret[arr_i] = brr_val

    return ret

def MySort(arr, beg, end):
    carr = (GLOBAL.lli * len(arr))(*arr)

    beg_time = time.time()
    GLOBAL.test_sort_so.CMySort(carr, len(arr), beg, end)
    end_time = time.time()

    print(f"time b = {end_time - beg_time}")

    return carr[:]

def main():
    test_sort_so_file = f"{Dir}/test_sort.so"

    builder = Builder()
    build.builder_add(builder)
    builder.build(GetABSPath(test_sort_so_file), False)

    GLOBAL.test_sort_so = ctypes.CDLL(test_sort_so_file)

    GLOBAL.test_sort_so.CMySort.argtypes = \
        [GLOBAL.void_p, GLOBAL.lli, GLOBAL.lli, GLOBAL.lli]
    GLOBAL.test_sort_so.CMySort.restype = None

    size = int(10**8)
    val_lb = int(-2**60)
    val_rb = int(2**60)

    arr = [random.randint(val_lb, val_rb) for _ in range(size)]

    beg = random.randint(0, size)
    end = random.randint(0, size)
    # beg = 0
    # end = size

    beg, end = min(beg, end), max(beg, end)

    print(f"beg = {beg}")
    print(f"end = {end}")

    py_brr = PySort(arr, beg, end)
    my_brr = MySort(arr, beg, end)

    # print(f"py_brr = {py_brr}")
    # print(f"my_brr = {my_brr}")
    print(f"compare = {py_brr == my_brr}")

if __name__ == "__main__":
    main()
