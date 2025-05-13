# For exposition only.
# This file is not intended to be used as a random entropy source
# because it does not source its input from a random device.

import random
import math

# Algorithm 1

def fisher_yates(sequence: list[int], random):
    for i in range(1, len(sequence)):
        j = random.randint(0, i)
        sequence[i], sequence[j] = sequence[j], sequence[i]

def expected_naive_convert(range:int) -> float:
    """
    Expected number of bits fetched to convert a number
    in the range [0, range) to a number in the range [0, range).
    """
    r = 1 # The smallest power of 2 >= than range 
    b = 0 # The number of bits in r
    while r < range:
        r *= 2
        b += 1
    return r*b/range

# Algorithm 2
def naive_convert(range:int, random): # -> tuple(int,float,int): 
    actual_bits_fetched = 0
    expected_bits_fetched = expected_naive_convert(range)
    while True:
        v, r = 0, 1
        while r < range:
            actual_bits_fetched = actual_bits_fetched + 1
            v = v*2 + random.randint(0,1)
            r = r*2
        if v<range:
            return (v,expected_bits_fetched,actual_bits_fetched)

# Algorithm 3
def multiply(n_value, n_range, m_value, m_range):
    return n_value * m_range + m_value, n_range * m_range

# Algorithm 4
def divide(n_value, n_range, m_range):
    assert n_range % m_range == 0
    return n_value%m_range, m_range, \
        n_value//m_range, n_range//m_range

# Algorithm 5
def downsize(n_value, n_range, m_range):
     assert m_range <= n_range
     return (n_value, m_range) if n_value<m_range \
        else (n_value-m_range, n_range-m_range)

input_count = 0
output_count = 0

def random_bit():
    global input_count
    input_count = input_count+1
    return random.randint(0,1)

def fetch(n_value, n_range):
    return multiply(n_value, n_range, random_bit(), 2)

total_loss = 0

# fetch, multiply, downsize, divide (FMDD)
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
        n_value, n_range = downsize(n_value, n_range, n_range%m_range)
    return divide(n_value, n_range, m_range)


class U:
    def __init__(self,value=0,range=1):
        self.value = value
        self.range = range
        self.entropy_out = 0
    
    def reset(self):
        self.value = 0
        self.range = 1

    def fetch(self):
        v,r = fetch(self.value, self.range)
        return U(v,r)
    
    def factorize(self, b_range):
        a_value, a_range, b_value, b_range = divide(self.value, self.range, b_range)
        self.reset()
        return U(a_value, a_range), U(b_value, b_range)
    
    def downsize(self, b_range):
        a_value, a_range = downsize(self.value, self.range, b_range)
        self.reset()
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
    
def shannon_entropy(p: float) -> float:
    return -p*math.log2(p) - (1-p)*math.log2(1-p)

class R:
    def __init__(self, value: bool, numerator: int, denominator: int):
        self.value = value
        self.numerator = numerator
        self.denominator = denominator

    def reset(self):
        self.value = False
        self.numerator = 1
        self.denominator = 1

    def entropy(self):
        return shannon_entropy(self.numerator/self.denominator)

def multiply2(a: U, b: U) -> U:
    a_value, a_range = multiply(a.value, a.range, b.value, b.range)
    a.reset()
    b.reset()
    return U(a_value, a_range)

def divide2(a: U, b: int) -> tuple[U,U]:
    assert a.range % b == 0
    x_value, x_range, y_value, y_range = divide(a.value, a.range, b)
    a.reset()
    return U(x_value, x_range), U(y_value, y_range)

class NaiveEntropySource:
    """
    A naive source of entropy.
    """
    def __init__(self):
        self.entropy_out = 0
        self.entropy_in = 0
        self.expected_entropy_in = 0

    def entropy(self):
        # Nothing stored here
        return 0

    def randint(self, min, max):
        range = max-min+1
        v, expected, actual = naive_convert(range, random)
        self.entropy_out += math.log2(range)
        self.expected_entropy_in += expected
        self.entropy_in += actual
        return min + v


store = U()
store = NaiveEntropySource()
cards = list(range(52))
fisher_yates(cards, store)

input_count = store.expected_entropy_in
output_count = store.entropy_out
# input_count = input_count - store.entropy()

print("Output count", store.entropy_out)
print("Input count", store.entropy_in)
print("Expected entropy in", store.expected_entropy_in)
print("Expected efficiency", store.entropy_out/store.expected_entropy_in)
print("Actual efficiency", store.entropy_out/store.entropy_in)

#print("Actual efficiency", output_count/input_count)
#print("Total estimated loss", total_loss)
#print("Total estimated input", output_count + total_loss)
#print("Estimated efficiency", output_count/(output_count + total_loss))

n = 2**32
m = 6
p = (n-m)/n
print(p)
loss = -math.log2(p) - (1-p)*math.log2(1-p)/p
print("Loss", loss)
e = math.log2(m) / (math.log2(m) + loss)
print("e", e)

def shannon_entropy(p):
    return -p*math.log2(p) - (1-p)*math.log2(1-p)

print("Original", math.log2(5))
print("Total", shannon_entropy(1/5) + 2*shannon_entropy(2/5))

print("Expected(4)", expected_naive_convert(4))
print("Expected(5)", expected_naive_convert(5))
print("Expected(6)", expected_naive_convert(6))
print("Expected(8)", expected_naive_convert(8))