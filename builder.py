import os
import time

def NullFunc():
    pass

def GetABSPath(path):
    return os.path.abspath(path).replace("\\", "/")

def GetMTime(path):
    try:
        return os.path.getmtime(path)
    except:
        return 0

def SetMTime(path, mtime):
    try:
        os.utime(path, (os.path.getmtime(path), mtime))
    except:
        pass

def BFS(A, sources):
    # A = {
    #   "v 0": {"adj 00", "adj 01", "adj 02", ...},
    #   "v 1": {"adj 10", "adj 11", "adj 12", ...},
    #   "v 2": {"adj 20", "adj 21", "adj 22", ...},
    #   ...
    # }

    # sources = ["s_0", "s_1", "s_2", ...]

    ret = list()
    s = set()

    i = 0

    for source in sources:
        if source in s:
            continue

        ret.append(source)
        s.add(source)

        while i < len(ret):
            n = ret[i]
            i += 1

            assert n in A, f"unknown vertex: {n}"

            for adj in A[n]:
                if adj not in s:
                    ret.append(adj)
                    s.add(adj)

    return ret

def TPSort(A, sources):
    # A = {
    #   "v 0": {"adj 00", "adj 01", "adj 02", ...},
    #   "v 1": {"adj 10", "adj 11", "adj 12", ...},
    #   "v 2": {"adj 20", "adj 21", "adj 22", ...},
    #   ...
    # }
    #
    # return ["u 0", "u 1", "u 2", ... ]

    q = BFS(A, sources)

    indegs = {n: 0 for n in q}

    for n in q:
        for adj in A[n]:
            indegs[adj] += 1

    q.clear()

    for n, c in indegs.items():
        if c == 0:
            q.append(n)

    ret = []

    while 0 < len(q):
        n = q[-1]
        q.pop()
        ret.append(n)

        for adj in A[n]:
            indegs[adj] -= 1

            if indegs[adj] == 0:
                q.append(adj)

    assert len(indegs) <= len(ret), "cycle detected"

    return ret

class CmdExecutor:
    def __init__(self, cmd):
        self.cmd = " ".join(cmd)

    def __call__(self):
        print(f"executing: {self.cmd}")

        rc = os.system(self.cmd)

        assert rc == 0, f"failed, exit with {rc}"

        print(f"success")

class Builder:
    def __init__(self):
        self.deps = dict()
        # = {
        #       "unit 0": { "depended unit 00", "depended unit 01", ...},
        #       "unit 1": { "depended unit 10", "depended unit 01", ...},
        #       "unit 2": { "depended unit 20", "depended unit 01", ...},
        #       ...
        #   }

        self.callbacks = dict()
        # = {
        #       "unit 0": callback 0,
        #       "unit 1": callback 1,
        #       "unit 2": callback 2,
        #       ...
        #   }

    def add(self, unit, deps, callback):
        assert unit not in self.deps, f"duplicated unit: {unit}"

        self.deps[unit] = set(deps)
        self.callbacks[unit] = callback

    def units(self):
        return list(self.deps.keys())

    def build(self, target):
        assert target in self.deps, f"unknown unit {target}"

        v = TPSort(self.deps, [target])

        mtimes = { unit: GetMTime(unit) for unit in v }

        non_built = []
        built = []

        for unit in reversed(v):
            if len(self.deps[unit]) == 0:
                continue

            adj_mtime = max(mtimes[dep_unit] for dep_unit in self.deps[unit])

            if adj_mtime <= mtimes[unit]:
                non_built.append(unit)
                continue

            print(f"building {unit}")
            self.callbacks[unit]()

            assert os.path.exists(unit), \
                   f"unit {unit} does not exist after built"

            built.append(unit)

            now = time.time()
            SetMTime(unit, now)
            mtimes[unit] = now

        return non_built, built
