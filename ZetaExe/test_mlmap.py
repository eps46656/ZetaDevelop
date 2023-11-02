import os
import random
import ctypes
import time

from mlmap import MLMap

ma = {}
mb = MLMap()

def Insert():
    key = random.randint(-1000, 1000)
    val = random.randint(-100000000, 100000000)

    ma[key] = val
    mb[key] = val


def Erase():
    if len(ma) == 0:
        return

    key = random.randint(-1000, 1000)

    if key in ma:
        del ma[key]
        assert mb[key] != None
        mb.erase(key)
    else:
        assert mb[key] == None

def Compare():
    assert len(ma) == len(mb)

    for ma_key, ma_val in ma.items():
        mb_val = mb[ma_key]
        assert ma_val == mb_val

def main():
    for i in range(random.randint(1000, 2000)):
        Insert()
    Compare()

    print("a")

    for _ in range(100):
        for _ in range(random.randint(500, 600)):
            Insert()
            Compare()

        print("b")

        for _ in range(random.randint(500, 600)):
            Erase()
            Compare()

        print("c")

    Compare()

    print(f"len(ma) = {len(ma)}")
    print(f"len(mb) = {len(mb)}")

    print("ok\n")

if __name__ == "__main__":
    main()





