# For exposition only.
# This file is not intended to be used as a random entropy source
# because it does not source its input from a random device.

import random
import math

# Algorithm 1

def fisher_yates(sequence, random):
    for i in range(1, len(sequence)):
        j = random.randint(0, i)
        sequence[i], sequence[j] = sequence[j], sequence[i]

# Algorithm 2
def inefficient_convert(range):
    while True:
        v, r = 0, 1
        while r < range:
            v = v*2 + random_bit()
            r = r*2
        if v<range:
            return v

# Algorithm 3
def combine(n_value, n_range, m_value, m_range):
    return n_value * m_range + m_value, n_range * m_range

# Algorithm 4
def factorize(n_value, n_range, m_range):
    assert n_range % m_range == 0
    return n_value%m_range, m_range, \
        n_value//m_range, n_range//m_range

# Algorithm 5
def resize(n_value, n_range, m_range):
     assert m_range <= n_range
     return (n_value, m_range) if n_value<m_range \
        else (n_value-m_range, n_range-m_range)

def random_bit():
    return random.randint(0,1)


input_count = 0
output_count = 0

def fetch(n_value, n_range):
    global input_count
    input_count = input_count+1
    return combine(n_value, n_range, random_bit(), 2)

total_loss = 0

def extract(n_value, n_range, m_range, min_range):
    first = True  # Work around do-while loops in Python
    if min_range < m_range:
        min_range = m_range
    p = (min_range - m_range + 1)/min_range
    global total_loss
    total_loss = total_loss + (-math.log2(p) - (1-p)*math.log2(1-p)/p)
    while n_range<m_range or first:
        first = False
        while n_range<min_range:
            # Get one more bit of entropy
            n_value, n_range = fetch(n_value, n_range)
        n_value, n_range = resize(n_value, n_range, n_range%m_range)
    return factorize(n_value, n_range, m_range)


class U:
    def __init__(self,value=0,range=1):
        self.value = value
        self.range = range
        self.entropy_out = 0
        # self.expected_loss = 0
    
    def reset(self):
        self.value = 0
        self.range = 1

    def fetch(self):
        v,r = fetch(self.value, self.range)
        return U(v,r)
    
    def factorize(self, b_range):
        a_value, a_range, b_value, b_range = factorize(self.value, self.range, b_range)
        return U(a_value, a_range), U(b_value, b_range)
    
    def resize(self, b_range):
        a_value, a_range = resize(self.value, self.range, b_range)
        return U(a_value, a_range)
    
    def get(self, range, min_range = 1<<32):
        '''
        Gets a random number.
        '''
        r_value, r_range, a_value, a_range = extract(self.value, self.range, range, min_range)
        self.value = a_value
        self.range = a_range
        global output_count
        output_count = output_count + math.log2(r_range)
        return r_value
    
    def randint(self, min, max):
        return min + self.get(max-min+1)

    def __str__(self):
        return f"U({self.range})"
    
    def entropy(self):
        return math.log2(self.range)

store = U()
cards = list(range(52))
fisher_yates(cards, store)

input_count = input_count - store.entropy()
print("Output count", output_count)
print("Input count", input_count)
print("Actual efficiency", output_count/input_count)
print("Total estimated loss", total_loss)
print("Total estimated input", output_count + total_loss)
print("Estimated efficiency", output_count/(output_count + total_loss))

exit(0)

n = 2**32
m = 6
p = (n-m+1)/n
print(p)
loss = -math.log2(p) - (1-p)*math.log2(1-p)/p
print("Loss", loss)
e = math.log2(m) / (math.log2(m) + loss)
print("e", e)