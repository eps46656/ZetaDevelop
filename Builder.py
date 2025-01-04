import os
import pathlib
import time
import traceback
import typing

import termcolor
from typeguard import typechecked


@typechecked
def GetMTime(path: object):
    try:
        return os.path.getmtime(path)
    except:
        return 0.0


@typechecked
def SetMTime(path: object, mtime: float):
    try:
        os.utime(path, (os.path.getmtime(path), mtime))
    except:
        pass


@typechecked
def BFS(adj_list: dict[object, set[object]],
        sources: typing.Iterable[object]):
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

            assert n in adj_list, f"Unknown vertex {n} linked by {source}."

            for adj in adj_list[n]:
                if adj not in discovered:
                    ret.append(adj)
                    discovered.add(adj)

    return ret


@typechecked
def TPSort(adj_list: dict[object, set[object]],
           sources: typing.Iterable[object]):
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

    q = [n for n, c in indegs.items() if c == 0]

    ret = list()

    while 0 < len(q):
        n = q.pop()
        ret.append(n)

        for adj in adj_list[n]:
            indegs[adj] -= 1

            if indegs[adj] == 0:
                q.append(adj)

    assert len(indegs) == len(ret), "Cycle detected."

    return ret


@typechecked
class Builder:
    def __init__(self):
        self.deps: dict[pathlib.Path, set[pathlib.Path]] = dict()
        # = {
        #       "unit 0": { "depended unit 00", "depended unit 01", ...},
        #       "unit 1": { "depended unit 10", "depended unit 01", ...},
        #       "unit 2": { "depended unit 20", "depended unit 01", ...},
        #       ...
        #   }

        self.building_funcs: dict[pathlib.Path,
                                  typing.Callable[[], None]] = dict()
        # = {
        #       "unit 0": building_func 0,
        #       "unit 1": building_func 1,
        #       "unit 2": building_func 2,
        #       ...
        #   }

    def Add(self, unit: object, deps: typing.Optional[typing.Iterable[object]], building_func: typing.Optional[typing.Callable[[], None]] = None):
        unit = pathlib.Path(unit).resolve()

        ds = (pathlib.Path(dep).resolve()
              for dep in ([] if deps is None else deps) if dep is not None)

        if unit in self.deps:
            assert building_func is None, "Conflicting building function."
            self.deps[unit].update(ds)
        else:
            self.deps[unit] = set(ds)
            self.building_funcs[unit] = building_func

    def GetUnits(self):
        return self.deps.keys()

    def GetDepUnits(self, target_unit: object):
        target_unit = pathlib.Path(target_unit).resolve()

        assert target_unit in self.deps, f"Unknown unit {target_unit}."

        return list(reversed(TPSort(self.deps, [target_unit])))

    def Check(self):
        for unit, deps in self.deps.items():
            for dep in deps:
                assert dep in self.deps, \
                    f"Unknown unit {dep} depended by {unit}."

    def Build(self, target_unit: object, rebuild: bool):
        target_unit = pathlib.Path(target_unit).resolve()

        dep_units = self.GetDepUnits(target_unit)

        mtimes = {unit: GetMTime(unit) for unit in dep_units}

        skipped_build_units: set[pathlib.Path] = set()
        finished_build_units: set[pathlib.Path] = set()

        ready_build_units: set[pathlib.Path] = set()

        failed_build_units: set[pathlib.Path] = set()
        unready_build_units: set[pathlib.Path] = set()

        ready_unit_cnt = 0

        for unit in dep_units:
            if len(self.deps[unit]) == 0:
                skipped_build_units.add(unit)
                ready_build_units.add(unit)
                ready_unit_cnt += 1
                continue

            if not self.deps[unit].issubset(ready_build_units):
                unready_build_units.add(unit)
                continue

            if not rebuild and max(mtimes[dep] for dep in self.deps[unit]) <= mtimes[unit]:
                skipped_build_units.add(unit)
                ready_build_units.add(unit)
                ready_unit_cnt += 1
                continue

            print(termcolor.colored("Building", "yellow") +
                  f" ({round(ready_unit_cnt / len(dep_units) * 100):3}%): {unit}")

            building_func = self.building_funcs[unit]

            if building_func is not None:
                try:
                    building_func()
                except Exception as e:
                    failed_build_units.add(unit)
                    print(traceback.format_exc())
                    continue

            assert os.path.exists(unit), \
                f"Unit {unit} does not exist after building."

            finished_build_units.add(unit)
            ready_build_units.add(unit)

            now = time.time()
            SetMTime(unit, now)
            mtimes[unit] = now

            ready_unit_cnt += 1

        return len(dep_units) == len(ready_build_units), {
            "skipped": skipped_build_units,
            "finished": finished_build_units,
            "failed": failed_build_units,
            "unready": unready_build_units,
        }
