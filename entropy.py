
import random
import math
import os

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
def naive_convert(range:int, random) -> tuple[int,float,int]: 
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

def shannon_entropy(p: float) -> float:
    return -p*math.log2(p) - (1-p)*math.log2(1-p)

class U:
    def __init__(self,value=0,range=1):
        self.value = value
        self.range = range
        self.entropy_out = 0
    
    def destroy(self):
        value = self.value
        self.value = 0
        self.range = 1
        return value
    
    def __str__(self):
        return f"U({self.range})"
    
    def entropy(self):
        return math.log2(self.range)
    
class R:
    def __init__(self, value: bool, numerator: int, denominator: int):
        self._value = value
        self.numerator = numerator
        self.denominator = denominator

    def destroy(self):
        value = self._value
        self._value = False
        self.numerator = 1
        self.denominator = 1
        return value

    def entropy(self):
        return shannon_entropy(self.numerator/self.denominator)

# Algorithm 3
def multiply(a: U, b: U) -> U:
    a_range = a.range
    b_range = b.range
    return U(a.destroy()*b_range + b.destroy(), a_range * b_range)

# Algorithm 4
def divide(n_value, n_range, m_range):
    assert n_range % m_range == 0
    return n_value%m_range, m_range, \
        n_value//m_range, n_range//m_range

# Algorithm 5
def downsize(n_value, n_range, m_range):
     assert m_range <= n_range
     return (n_value, m_range, True) if n_value<m_range \
        else (n_value-m_range, n_range-m_range, False)

def divide2(a: U, b: int) -> tuple[U,U]:
    a_range = a.range
    assert a_range % b == 0
    a_value = a.destroy()
    return U(a_value%b, b), U(a_value//b, a_range//b)

def downsize2(a:U, m:int) -> tuple[U,R]:
    """
    Reduces entropy of size a to size m.
    """
    assert a.range >= m
    old_range = a.range
    new_value,new_range,r = downsize(a.value, a.range, m)
    a.destroy()
    return U(new_value,new_range), R(r, m, old_range)

def upsize(a:U, r:R) -> U:
    pass

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

    def entropy_consumed(self):
        return self.entropy_in

    def randint(self, min, max):
        range = max-min+1
        v, expected, actual = naive_convert(range, random)
        self.entropy_out += math.log2(range)
        self.expected_entropy_in += expected
        self.entropy_in += actual
        return min + v
    
def read(a:U) -> int:
    return a.destroy()
    
def read_bit(random) -> U:
    """
    Reads one bit of randomness from a random source.
    """
    return U(random.randint(0,1),2)

class HardwareEntropySource:
    def __init__(self):
        self.entropy_out = 0

    def fetch(self, min):
        self.entropy_out += 8
        return U(os.urandom(1), 256)

class EfficientEntropySource:
    def __init__(self, source, min_range=1<<31):
        self.source = source
        self.store = U(0,1)
        self.entropy_out = 0
        self.expected_entropy_in = 0
        self.min_range = min_range

    def entropy_consumed(self):
        return self.source.entropy_out - self.store.entropy()

    def fetch(self, n:int):
        first = True
        while first or self.store.range < n:
            first = False
            while self.store.range < self.min_range:
                self.store = multiply(self.store, read_bit(self.source))
            self.store, _ = downsize2(self.store, self.store.range%n)
        result, self.store = divide2(self.store, n)
        self.entropy_out += result.entropy()
        # The expected p
        p = (n-1)/self.min_range/2
        # We expect to lose the shannon-entropy of the downsize each time, and there are
        # 1/(1-p) iterations expected. This is an overestimate.
        self.expected_entropy_in += result.entropy() + shannon_entropy(p)/(1-p)
        return result

    def randint(self, min, max):
        return min + read(self.fetch(max-min+1))

def test_entropy_source(name, store):
    print("Results for store     ", name)
    cards = list(range(52))
    fisher_yates(cards, store)

    input_count = store.expected_entropy_in
    output_count = store.entropy_out

    print("Output entropy        ", store.entropy_out)
    print("Measured input entropy", store.entropy_consumed())

    print("Expected entropy in   ", store.expected_entropy_in)
    print("Expected efficiency   ", store.entropy_out/store.expected_entropy_in)
    print("Measured efficiency   ", store.entropy_out/store.entropy_consumed())
    print()

# Test a naive entropy source
test_entropy_source("Naive unbuffered", NaiveEntropySource())

# Test the efficient entropy source in a variety of configurations
test_entropy_source("Efficient 2**8 buffer", EfficientEntropySource(NaiveEntropySource(), 2**8))
test_entropy_source("Efficient 2**15 buffer", EfficientEntropySource(NaiveEntropySource(), 2**15))
test_entropy_source("Efficient 2**31 buffer", EfficientEntropySource(NaiveEntropySource(), 2**31))
test_entropy_source("Efficient 2**48 buffer", EfficientEntropySource(NaiveEntropySource(), 2**48))

