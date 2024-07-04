class ExpSizeEvaluator:
    def __init__(self, capacity, lb):
        self.capacity = capacity
        self.lb = lb

        self.d_vals = [
            [None for _ in range(self.capacity)],
            [None] + [self.capacity for _ in range(self.capacity)]
        ]

        self.e_vals = [
            [None for _ in range(self.capacity)],
            [None] + [self.capacity for _ in range(self.capacity)]
        ]

    def EvalDVal_(self, N, L):
        if L == 0:
            return None

        s = 0
        cnt = 0

        for i in range(1, self.capacity + 1):
            if self.lb <= i + L:
                s += self.e_vals[N-1][i]
                cnt += 1

        return L if cnt == 0 else (s / cnt + L)

    def EvalEVal_(self, N, L):
        if L == 0:
            return None

        s = 0

        for i in range(1, self.capacity + 1):
            if i + L < self.lb:
                s += self.d_vals[N-1][i]
            else:
                s += self.e_vals[N-1][i]

        return s / self.capacity + L

    def GetValue(self, N):
        while len(self.d_vals) <= N:
            cur_N = len(self.d_vals) - 1

            self.d_vals.append(
                [self.EvalDVal_(cur_N + 1, L) for L in range(self.capacity+1)])
            self.e_vals.append(
                [self.EvalEVal_(cur_N + 1, L) for L in range(self.capacity+1)])

        return self.d_vals[N][self.capacity]

class MinSizeEvaluator:
    def __init__(self, capacity, lb):
        self.capacity = capacity
        self.lb = lb

        self.d_vals = [
            [None for _ in range(self.capacity+1)],
            [None] + [i for i in range(self.capacity)],
        ]

        self.e_vals = [
            [None for _ in range(self.capacity+1)],
            [None] + [i for i in range(self.capacity)],
        ]

    def EvalDVal_(self, N, L):
        if L == 0:
            return None

        pre = float("inf")

        for i in range(1, self.capacity + 1):
            if self.lb <= i + L:
                pre = min(pre, self.e_vals[N-1][i])

        return pre + L

    def EvalEVal_(self, N, L):
        if L == 0:
            return None

        pre = float("inf")

        for i in range(1, self.capacity + 1):
            if i + L < self.lb:
                pre = min(pre, self.d_vals[N-1][i])
            else:
                pre = min(pre, self.e_vals[N-1][i])

        return pre + L

    def GetValue(self, N):
        while len(self.d_vals) <= N:
            cur_N = len(self.d_vals) - 1

            self.d_vals.append(
                [self.EvalDVal_(cur_N + 1, L) for L in range(self.capacity+1)])
            self.e_vals.append(
                [self.EvalEVal_(cur_N + 1, L) for L in range(self.capacity+1)])

        return self.d_vals[N][self.capacity]

def main():
    C = 64
    N = 1024 * 8

    exp_size_eval = ExpSizeEvaluator(C, C+1)
    min_size_eval = MinSizeEvaluator(C, C+1)

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
