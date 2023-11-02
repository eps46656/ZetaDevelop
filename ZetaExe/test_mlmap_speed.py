import os
import random
import ctypes
import time

from mlmap import MLMap

m = {}
# m = MLMap()

KEY_RANGE_MIN = -1000000000
KEY_RANGE_MAX =  1000000000

VAL_RANGE_MIN = -1000000000
VAL_RANGE_MAX =  1000000000

def Insert():
    key = random.randint(KEY_RANGE_MIN, KEY_RANGE_MAX)
    val = random.randint(VAL_RANGE_MIN, VAL_RANGE_MAX)

    m[key] = val

def Erase():
    if len(m) == 0:
        return

    key = random.randint(KEY_RANGE_MIN, KEY_RANGE_MAX)

    if key in m:
        m.pop(key)

def main():
    start_time = time.time()

    for i in range(1_000_000):
        Insert()

    end_time = time.time()

    print(f"len(m) = {len(m)}")

    print(f" start_time = {start_time}")
    print(f"   end_time = {end_time}")
    print(f"elapse_time = {end_time - start_time}")

if __name__ == "__main__":
    main()





