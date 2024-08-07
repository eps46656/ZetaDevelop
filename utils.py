from config import *

def ToPath(path):
    path = path.strip("\'\"")
    return f"\"{path}\""

def HighLightPrint(x):
    print(f"\033[93m{x}\033[0m")

def FilterNotNone(iter):
    return filter(lambda x: x is not None, iter)
