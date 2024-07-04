import itertools
import pulp
import math

'''

X array

n: The size of array x.
capacity: The capacity of each segment.
x[i]: The number of element in ith segment. 1 <= x[i] <= capacity. i = 0 ~ n-1
lb: The lower bound number

x[0] = capacity

For i = 1 ~ n - 2
    lb <= x[i-1] + x[i] + x[i+1]

F(capacity, n) = (sum x) / (capacity * n)

Calculate
    min F(capacity, n)
    exp F(capacity, n)

Consider min F(capacity, n)
    D(n, j, i)
        For n-length x array.
        The last two numbers are i and j.

    D(n, j, i)           3 <= n
        = min for k in [1, capacity]
            if self.lb <= i + j + k
                D(n-1, k, j) + i

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

        self.d_vals = {}

        for j, i in itertools.product(range(1, self.capacity+1),
                                      range(1, self.capacity+1)):
            if j == self.capacity:
                self.d_vals[(2, j, i)] = self.capacity + i
            else:
                self.d_vals[(2, j, i)] = float("inf")

    def EvalDVal_(self, n, j, i):
        ret = float("inf")

        for k in range(1, self.capacity+1):
            if k + j + i < self.lb:
                continue

            ret = min(ret, self.d_vals[(n-1, k, j)] + i)

        return ret

    def GetValue(self, n):
        while self.cur_n < n:
            for j, i in itertools.product(range(1, self.capacity+1),
                                          range(1, self.capacity+1)):
                self.d_vals[(self.cur_n + 1, j, i)] = \
                    self.EvalDVal_(self.cur_n + 1, j, i)

            self.cur_n += 1

        return min(self.d_vals[(n, j, self.capacity)]
                   for j in range(1, self.capacity+1))

class ExpSizeEvaluator:
    def __init__(self, capacity, lb):
        self.capacity = capacity
        self.lb = lb

        self.cur_n = 2

        self.d_vals = {}

        for j, i in itertools.product(range(1, self.capacity+1),
                                      range(1, self.capacity+1)):
            if j == self.capacity:
                self.d_vals[(2, j, i)] = (self.capacity + i, 1.0)
            else:
                self.d_vals[(2, j, i)] = (None, 0.0)

        self.NormCnt_(2)

    def NormCnt_(self, n):
        sum_cnt = 0.0

        for j, i in itertools.product(range(1, self.capacity+1),
                                      range(1, self.capacity+1)):
            sum_cnt += self.d_vals[(n, j, i)][1]

        for j, i in itertools.product(range(1, self.capacity+1),
                                      range(1, self.capacity+1)):
            val, cnt = self.d_vals[(n, j, i)]
            self.d_vals[(n, j, i)] = (val, cnt / sum_cnt)

    def EvalDVal_(self, n, j, i):
        sum_val = 0.0
        sum_cnt = 0.0

        for k in range(1, self.capacity+1):
            if k + j + i < self.lb:
                continue

            val, cnt = self.d_vals[(n-1, k, j)]

            if cnt == 0.0:
                continue

            sum_val += (val + i) * cnt
            sum_cnt += cnt

        return (None, 0.0) if sum_cnt == 0.0 else (sum_val / sum_cnt, sum_cnt)

    def GetValue(self, n):
        while self.cur_n < n:
            for j, i in itertools.product(range(1, self.capacity+1),
                                          range(1, self.capacity+1)):
                self.d_vals[(self.cur_n + 1, j, i)] = \
                    self.EvalDVal_(self.cur_n + 1, j, i)

            self.NormCnt_(self.cur_n + 1)

            self.cur_n += 1

        sum_val = 0.0
        sum_cnt = 0.0

        for j in range(1, self.capacity+1):
            val, cnt = self.d_vals[(n, j, self.capacity)]

            if cnt == 0.0:
                continue

            sum_val += val * cnt
            sum_cnt += cnt

        return None if sum_cnt == 0.0 else sum_val / sum_cnt

def main1():
    C = 64
    N = 512

    exp_size_eval = ExpSizeEvaluator(C, C * 2 + 1)
    min_size_eval = MinSizeEvaluator(C, C * 2 + 1)

    print(exp_size_eval.GetValue(N) / (C * N))
    print(min_size_eval.GetValue(N) / (C * N))

def main2():
    capacity = 35
    split_n = 4

    prob = pulp.LpProblem(name="insertion", sense=pulp.const.LpMinimize)

    ll = pulp.LpVariable(name="ll",
                         lowBound=1, upBound=capacity, cat="Integer")

    l = pulp.LpVariable(name="l",
                        lowBound=1, upBound=capacity, cat="Integer")

    m = pulp.LpVariable(name="m",
                        lowBound=1, upBound=capacity, cat="Integer")

    r = pulp.LpVariable(name="r",
                        lowBound=1, upBound=capacity, cat="Integer")

    rr = pulp.LpVariable(name="rr",
                         lowBound=1, upBound=capacity, cat="Integer")

    cnt = pulp.LpVariable(name="cnt",
                          lowBound=1, upBound=None, cat="Integer")

    prob.addConstraint(capacity * 2 + 1 <= ll + l + m)
    prob.addConstraint(capacity * 2 + 1 <= l + m + r)
    prob.addConstraint(capacity * 2 + 1 <= m + r + rr)

    prob.addConstraint(capacity * 3 + 1 <= l + m + r + cnt)
    prob.addConstraint(l + m + r + cnt <= capacity * split_n)

    new_l = math.floor((capacity * (split_n - 1) + 1) / split_n)
    new_m = math.floor((capacity * (split_n - 1) + 1) / split_n)

    prob.addConstraint(capacity * 2 + 1 <= ll + new_l + new_m)

    # prob.setObjective(ll + new_l + new_m - capacity * 2)

    prob += 0

    prob.solve()

def main3():
    for capacity in range(32, 32+4 + 1):
        k = math.floor((capacity * 5 + 1) / 6)
        assert capacity * 2 < k * 3

    print("ok")

def main4():
    capacity = 37

    prob = pulp.LpProblem(name="prob", sense=pulp.const.LpMinimize)

    l = pulp.LpVariable(name="l",
                        lowBound=1, upBound=capacity, cat="Integer")

    ml = pulp.LpVariable(name="ml",
                         lowBound=1, upBound=capacity, cat="Integer")

    mr = pulp.LpVariable(name="mr",
                         lowBound=1, upBound=capacity, cat="Integer")

    cnt = pulp.LpVariable(name="cnt",
                          lowBound=1, upBound=None, cat="Integer")

    prob.addConstraint(ml + mr <= capacity)
    prob.addConstraint(capacity * 2 + 1 <=  l + ml + cnt + mr)

    # prob.addConstraint(capacity <= l + ml + cnt)
    # prob.addConstraint(capacity <= l + ml + cnt)

    # prob.setObjective(l + ml + cnt)
    # capacity <= l + ml + cnt

    prob.setObjective(l + ml + cnt)

    prob.solve()

    '''

    mr always not intrude into l
    ml always not intrude into r

    '''

def main5():
    capacity = 37
    split_n = 3

    lb_avg_size = math.floor((capacity * (split_n - 1) + 1) / split_n)
    ub_avg_size = math.ceil((capacity * (split_n - 1) + 1) / split_n)

    print(f"lb_avg_size = {lb_avg_size}")
    print(f"ub_avg_size = {ub_avg_size}")

    prob = pulp.LpProblem(name="prob", sense=pulp.const.LpMinimize)

    l = pulp.LpVariable("l", 1, capacity, "Integer")
    new_l = pulp.LpVariable("new_l", lb_avg_size, ub_avg_size, "Integer")

    ml = pulp.LpVariable("ml", 1, capacity, "Integer")
    new_ml = pulp.LpVariable("new_ml", lb_avg_size, ub_avg_size, "Integer")

    mr = pulp.LpVariable("mr", 1, capacity, "Integer")
    cnt = pulp.LpVariable("cnt", 1, None, "Integer")

    # prob.addConstraint(ml <= mr)
    prob.addConstraint(ml + mr <= capacity)
    prob.addConstraint(capacity * 2 + 1 <=  l + ml + cnt + mr)

    # prob.addConstraint(capacity <= l + ml + cnt)
    # prob.addConstraint(capacity <= l + ml + cnt)

    # prob.setObjective(l + ml + cnt)
    # capacity <= l + ml + cnt

    prob.setObjective(new_l + new_ml - l - ml)
    # l + ml <= new_l + new_ml

    prob.solve()

def main6():
    capacity = 37
    split_n = 3

    lb_avg_size = math.floor((capacity * (split_n - 1) + 1) / split_n)
    ub_avg_size = math.ceil((capacity * (split_n - 1) + 1) / split_n)

    print(f"lb_avg_size = {lb_avg_size}")
    print(f"ub_avg_size = {ub_avg_size}")

    prob = pulp.LpProblem(name="prob", sense=pulp.const.LpMinimize)

    l = pulp.LpVariable("l", 1, capacity, "Integer")
    new_l = pulp.LpVariable("new_l", lb_avg_size, ub_avg_size, "Integer")

    m = pulp.LpVariable("ml", 1, capacity, "Integer")
    new_m = pulp.LpVariable("new_ml", lb_avg_size, ub_avg_size, "Integer")

    r = pulp.LpVariable("r", 1, capacity, "Integer")
    new_r = pulp.LpVariable("new_r", lb_avg_size, ub_avg_size, "Integer")

    ml = pulp.LpVariable("mr", 1, capacity, "Integer")
    mr = pulp.LpVariable("mr", 1, capacity, "Integer")

    cnt = pulp.LpVariable("cnt", 1, None, "Integer")

    # prob.addConstraint(ml <= mr)
    prob.addConstraint(ml + mr <= capacity)
    prob.addConstraint(capacity * 2 + 1 <= l + ml + cnt + mr)
    prob.addConstraint(capacity * 2 + 1 <= r + ml + cnt + mr)
    prob.addConstraint(l + ml + cnt + mr + r <= capacity * 3)

    # prob.addConstraint(capacity <= l + ml + cnt)
    # prob.addConstraint(capacity <= l + ml + cnt)

    # prob.setObjective(l + ml + cnt)
    # capacity <= l + ml + cnt

    prob.setObjective(new_l + new_ml - l - ml)
    # l + ml <= new_l + new_ml

    prob.solve()

if __name__ == "__main__":
    main5()


'''

ll  l   m   r   rr
ll  l'  m'  r'  rr

capacity * 2 < ll + l + m
capacity * 2 < l + m + r

capacity * 3 < (l + m + r + cnt) <= capacity * 4

floor(capacity * 3 / 4) <= l', m', mr', r' <= ceil(capacity * 3 / 4)

ll + l' + ml' = ll + floor(capacity * 3 / 4) * 2

capacity * 2 < ll + floor(capacity * 3 / 4) * 2
?

ll  l'  ml'  mr'  r'  rr

insert step:
    if l_n, m_n and r_n can accept cnt inserted element:
        direct insert cnt into l_n, m_n, r_n
    else:
        equally split l_n, m_n, r_n and cnt inserted element into minimal number segments.

'''
