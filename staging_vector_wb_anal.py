


import dataclasses
import numpy as np

from typeguard import typechecked

@dataclasses.dataclass
class Seg:
    is_ref: bool
    beg: int
    size: int

def CheckSegs(origin_size: int, segs: list[Seg]):
    assert 0 <= origin_size

    prv_is_ref = False
    prv_ref_end = -1

    for seg in segs:
        if not seg.is_ref:
            prv_is_ref = False
            continue

        assert 0 <= seg.beg
        assert 0 < seg.size
        assert seg.beg + seg.size <= origin_size

        assert prv_ref_end <= seg.beg

        if prv_is_ref:
            assert prv_ref_end < seg.beg

        prv_is_ref = True
        prv_ref_end = seg.beg + seg.size

@typechecked
def Solve3(arr: list[int],
           brr: list[int],
           inc_cost: int,
           dec_cost: int):
    N = len(arr)

    assert 1 <= N
    assert len(brr) == N - 1

    dp = np.zeros((N + 1, N + 1), dtype=int)

    for l in range(1, N + 1):
        for i in range(N - l + 1):
            end = i + l

            arr_sum = sum(arr[i:end])
            brr_sum = sum(brr[i:end-1])

            ans = 0

            if arr_sum < 0:
                ans = inc_cost * -arr_sum + brr_sum
            else:
                ans = dec_cost * arr_sum + brr_sum

            for j in range(i+1, end):
                ans = min(ans, dp[i][j] + dp[j][end])

            dp[i][end] = ans

    return dp[0][N]

'''

g(i, j)
= sum(brr[i:j-1]) + f(sum(arr[i:j])))


dp[0] = 0

dp[i] = min(
    dp[j] + g(j, i)     for j in [0, i-1)
)

'''

@typechecked
def Solve2(arr: list[int],
           brr: list[int],
           inc_cost: int,
           dec_cost: int):
    def f(x):
        return -x * inc_cost if x < 0 else x * dec_cost

    N = len(arr)

    assert 1 <= N
    assert len(brr) == N - 1

    acc_arr = np.zeros((N + 1,), dtype=int)
    acc_brr = np.zeros((N,), dtype=int)

    for i in range(1, N+1):
        acc_arr[i] = acc_arr[i-1] + arr[i-1]

    for i in range(1, N):
        acc_brr[i] = acc_brr[i-1] + brr[i-1]

    dp = np.zeros((N + 1,), dtype=int)

    for i in range(1, N + 1):
        ans = 2**32

        for j in range(i):
            ans = min(ans, dp[j] + acc_brr[i-1] - acc_brr[j] + f(acc_arr[i] - acc_arr[j]))

        dp[i] = ans

    return dp[N]

@typechecked
def CalculateBestWBCost(
    origin_size: int,
    segs: list[Seg],
    insert_cost: int,
    erase_cost: int,
    read_cost: int,
    write_cost: int):

    CheckSegs(origin_size, segs)

    stage_size = 0

    origin_arr: list[int] = list()
    stage_arr: list[int] = list()
    brr: list[int] = list()

    prv_ref_end = 0
    acc_data_size = 0

    for seg in segs:
        stage_size += seg.size

        if not seg.is_ref:
            acc_data_size += seg.size
            continue

        origin_arr.append(seg.beg - prv_ref_end)
        stage_arr.append(acc_data_size)
        brr.append((read_cost + write_cost) * seg.size)

        prv_ref_end = seg.beg + seg.size
        acc_data_size = 0

    origin_arr.append(origin_size - prv_ref_end)
    stage_arr.append(acc_data_size)

    arr: list[int] = [a - b for a, b in zip(origin_arr, stage_arr)]

    return Solve(arr, brr, insert_cost, erase_cost) + write_cost * acc_data_size

def main1():
    pass

if __name__ == "__main__":
    main1()
