import os
import time

class EmptyClass:
    pass

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

def HighLightPrint(x):
    print(f"\033[93m{x}\033[0m")

def CheckAdjList(adj_list):
    # adj_list = {
    #   "v 0": {"adj 0 0", "adj 0 1", "adj 0 2", ...},
    #   "v 1": {"adj 1 0", "adj 1 1", "adj 1 2", ...},
    #   "v 2": {"adj 2 0", "adj 2 1", "adj 2 2", ...},
    #   ...
    # }

    vertices = {adj_list.keys()}

    for adjs in adj_list.values():
        assert adjs.issubset(vertices)

def BFS(adj_list, sources):
    # adj_list = {
    #   "v 0": {"adj 0 0", "adj 0 1", "adj 0 2", ...},
    #   "v 1": {"adj 1 0", "adj 1 1", "adj 1 2", ...},
    #   "v 2": {"adj 2 0", "adj 2 1", "adj 2 2", ...},
    #   ...
    # }

    # sources = ["s_0", "s_1", "s_2", ...]

    ret = list()
    discovered = set()

    i = 0

    for source in sources:
        if source in discovered:
            continue

        ret.append(source)
        discovered.add(source)

        while i < len(ret):
            n = ret[i]
            i += 1

            assert n in adj_list, f"unknown vertex: {n}"

            for adj in adj_list[n]:
                if adj not in discovered:
                    ret.append(adj)
                    discovered.add(adj)

    return ret

def TPSort(adj_list, sources):
    # adj_list = {
    #   "v 0": {"adj 0 0", "adj 0 1", "adj 0 2", ...},
    #   "v 1": {"adj 1 0", "adj 1 1", "adj 1 2", ...},
    #   "v 2": {"adj 2 0", "adj 2 1", "adj 2 2", ...},
    #   ...
    # }
    #
    # sources = [ "s 0", "s 1", "s 2", ... ]
    #
    # return [ "u 0", "u 1", "u 2", ... ]

    q = BFS(adj_list, sources)

    indegs = {n: 0 for n in q}

    for n in q:
        for adj in adj_list[n]:
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

        for adj in adj_list[n]:
            indegs[adj] -= 1

            if indegs[adj] == 0:
                q.append(adj)

    assert len(indegs) == len(ret), "cycle detected"

    return ret

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

    def Add(self, unit, deps, callback=None):
        assert unit not in self.deps, f"duplicated unit: {unit}"

        self.deps[unit] = set(deps)
        self.callbacks[unit] = callback

    def units(self):
        return list(self.deps.keys())

    def GetDepUnits(self, target):
        assert target in self.deps, f"unknown unit {target}"

        return list(reversed(TPSort(self.deps, [target])))

    def build(self, target, rebuild):
        dep_units = self.GetDepUnits(target)

        mtimes = { unit: GetMTime(unit) for unit in dep_units }

        not_built = []
        built = []

        ybeg = "\033[93m"
        yend = "\033[0m"

        for unit in dep_units:
            if len(self.deps[unit]) == 0:
                continue

            adj_mtime = max(mtimes[dep] for dep in self.deps[unit])

            if not rebuild and adj_mtime <= mtimes[unit]:
                not_built.append(unit)
                continue

            print(f"{ybeg}building:{yend} {unit}")

            callback = self.callbacks[unit]

            if callback is not None:
                rc = callback()
                assert rc == 0, f"error happened when building {unit}"

            assert os.path.exists(unit), \
                   f"unit {unit} does not exist after built"

            built.append(unit)

            now = time.time()
            SetMTime(unit, now)
            mtimes[unit] = now

        return not_built, built
