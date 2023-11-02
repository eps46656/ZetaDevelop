import os
import random
import subprocess

File = os.path.abspath(__file__).replace("\\", "/")
Dir = os.path.dirname(File).replace("\\", "/")

l = []

p = subprocess.Popen(f"{Dir}\\test_gplist.exe",
                     stdin=subprocess.PIPE,
                     stdout=subprocess.PIPE,)

def PWrite(s):
    global p

    p.stdin.write(s.encode("utf-8"))
    p.stdin.flush()

def PRead():
    global p
    return p.stdout.readline().decode("utf-8")

def GetSize():
    global p
    PWrite(f"s ")
    return int(PRead())

def Access(idx):
    global p

    PWrite(f"a {idx} ")
    return int(PRead())

def Insert():
    global l, p

    val = random.randint(-1000, 1000)

    if random.randint(0, 1) == 0: # insert lt
        pos = random.randint(0, len(l))
        l.insert(pos, val)
        PWrite(f"il {pos} {val} ")
    else: # insert rt
        pos = random.randint(-1, len(l)-1)
        l.insert(pos+1, val)
        PWrite(f"ir {pos} {val} ")

    c = int(PRead())

def Erase():
    global l, p

    if len(l) == 0:
        return

    pos = random.randint(0, len(l)-1)

    l.pop(pos)
    PWrite(f"e {pos} ")

    c = int(PRead())

def End():
    PWrite(f"z ")

def Check():
    global l, p

    s = GetSize()

    assert len(l) == s

    gplist = []

    for pos in range(len(l)):
        val = Access(pos)
        gplist.append(val)

    # print(l)
    # print(gplist)

    assert l == gplist

for _ in range(10):
    Insert()

for _ in range(100):
    for _ in range(10):
        for i in range(random.randint(50, 60)):
            Insert()
        Check()

    for _ in range(10):
        for _ in range(random.randint(50, 60)):
            Erase()
        Check()

End()

print("ok")
