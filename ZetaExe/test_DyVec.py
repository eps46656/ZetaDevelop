import os
import random
from ctypes import *
from DyVec import DyVec

File = os.path.abspath(__file__).replace("\\", "/")
Dir = os.path.dirname(File).replace("\\", "/")

la = list()
lb = DyVec()

def SetVal():
    idx = random.randint(0, len(la)-1)
    val = random.randint(-10000, 10000)

    la[idx] = val
    lb[idx] = val

def Insert():
    idx = random.randint(0, len(la))
    val = random.randint(-10000, 10000)

    la.insert(idx, val)
    lb.insert(idx, val)

def Erase():
    if len(la) == 0:
        return

    idx = random.randint(0, len(la)-1)

    la.pop(idx)
    lb.pop(idx)

def Check():
    global la, lb

    lb.check()

    assert len(la) == len(lb)

    for idx in range(len(la)):
        a = la[idx]
        b = lb[idx]
        assert a == b

def main():
    lb.insert(0, 0)
    lb.insert(1, 1)

    print(lb)

def main2():
    for i in range(random.randint(70, 80)):
        Insert()

    for _ in range(50):
        for _ in range(10):
            for i in range(random.randint(50, 60)):
                print(f"len(la) = {len(la)}")
                print(f"len(lb) = {len(lb)}")
                Insert()
            Check()

        for _ in range(10):
            for i in range(random.randint(50, 60)):
                SetVal()
            Check()

        for _ in range(10):
            for i in range(random.randint(20, 30)):
                Erase()
            Check()

        Check()

    print("ok")



if __name__ == "__main__":
    main()

