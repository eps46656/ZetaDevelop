import itertools

'''

X array

n: The size of array x.
capacity: The capacity of each segment.
x[i]: The number of element in ith segment. 1 <= x[i] <= capacity. i = 0 ~ n-1
lb: The lower bound number

x[0] = capacity

For i = 1 ~ n - 1
    lb <= x[i-1] + x[i]

F(capacity, n) = (sum x) / (capacity * n)

Calculate
    min F(capacity, n)
    exp F(capacity, n)

Consider min F(capacity, n)
    D(n, j, i)
        For n-length x array.
        The last two numbers are i and j.

    D(n, i)           3 <= n
        = min for j in [1, capacity]
            if self.lb <= j + i
                D(n-1, j) + i

        x[n-3]   x[n-2]   x[n-1]
                          i

    D(1, l) = capacity

    x[-1]   x[0]            x[1]
    0       capacity        ?



    D(2, l) =
'''


class MinSizeEvaluator:
    def __init__(self, capacity, lb):
        self.capacity = capacity
        self.lb = lb

        self.cur_n = 2

        self.d_vals = {(2, i):self.capacity + i
                       for i in range(1, self.capacity+1)}

    def EvalDVal_(self, n, i):
        ret = float("inf")

        for j in range(1, self.capacity+1):
            if j + i < self.lb:
                continue

            ret = min(ret, self.d_vals[(n-1, j)] + i)

        return ret

    def GetValue(self, n):
        while self.cur_n < n:
            for i in range(1, self.capacity+1):
                self.d_vals[(self.cur_n + 1, i)] = \
                    self.EvalDVal_(self.cur_n + 1, i)

            self.cur_n += 1

        return self.d_vals[(n, self.capacity)]

class ExpSizeEvaluator:
    def __init__(self, capacity, lb):
        self.capacity = capacity
        self.lb = lb

        self.cur_n = 2

        self.d_vals = {(2, i): (self.capacity + i, 1.0)
                       for i in range(1, self.capacity+1)}

        self.NormCnt_(2)

    def NormCnt_(self, n):
        sum_cnt = 0.0

        for i in range(1, self.capacity+1):
            sum_cnt += self.d_vals[(n, i)][1]

        for i in range(1, self.capacity+1):
            val, cnt = self.d_vals[(n, i)]
            self.d_vals[(n, i)] = (val, cnt / sum_cnt)

    def EvalDVal_(self, n, i):
        sum_val = 0.0
        sum_cnt = 0.0

        for j in range(1, self.capacity+1):
            if j + i < self.lb:
                continue

            val, cnt = self.d_vals[(n-1, j)]

            if cnt == 0.0:
                continue

            sum_val += (val + i) * cnt
            sum_cnt += cnt

        return (None, 0.0) if sum_cnt == 0.0 else (sum_val / sum_cnt, sum_cnt)

    def GetValue(self, n):
        while self.cur_n < n:
            for i in range(1, self.capacity+1):
                self.d_vals[(self.cur_n + 1, i)] = \
                    self.EvalDVal_(self.cur_n + 1, i)

            self.NormCnt_(self.cur_n + 1)

            self.cur_n += 1

        return self.d_vals[(n, self.capacity)][0]

def main():
    C = 64
    N = 512

    exp_size_eval = ExpSizeEvaluator(C, C)
    min_size_eval = MinSizeEvaluator(C, C)

    print(exp_size_eval.GetValue(N) / (C * N))
    print(min_size_eval.GetValue(N) / (C * N))


if __name__ == "__main__":
    main()


'''

D(C, N, L)
    Capacity C
    Length N
    The last value L
    Need to concider the last connection

E(C, N, L)
    Capacity C
    Length N
    The last value L
    No need to concider the last connection

D(C, N, L)
    = sum {i = C - L + 1, C} E(C, N-1, i) / L

E(C, N, L)
    = (
        sum {i = 1 ~ C - L} D(C, N-1, i) +
        sum {i = C - L + 1, C} E(C, N-1, i)
      ) / C

'''
