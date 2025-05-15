
import random
import math
import os

class U:
    """
    Entropy storage using a uniform random distribution.
    Used for representing entropy >=1
    """
    def __init__(self, value=0, range=1):
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
    """
    Entropy storage using a single bit and a probability p.
    Used for representing entropy <=1
    The value is a probability that U(1)<=numerator/denominator
    """
    def __init__(self, value: bool, numerator: int, denominator: int):
        self._value = value
        self.numerator = numerator
        self.denominator = denominator

    def destroy(self):
        value = self._value
        self._value = False
        self.numerator = 0
        self.denominator = 1
        return value

    def entropy(self):
        return shannon_entropy(self.numerator/self.denominator)

    def __str__(self):
        return f"R({self.numerator}/{self.denominator})"

def randint(entropy, min, max):
    """
    Returns a random integer in the range [min,max] using an entropy source.
    """
    return min + entropy.get(max-min+1).destroy()

def fisher_yates(sequence: list[int], entropy):
    for i in range(1, len(sequence)):
        j = randint(entropy, 0, i)
        sequence[i], sequence[j] = sequence[j], sequence[i]

def expected_simple_convert(range:int) -> float:
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

def simple_convert(range:int, entropy) -> U: 
    while True:
        v, r = 0, 1
        while r < range:
            v = v*2 + randint(entropy,0,1)
            r = r*2
        if v<range:
            return U(v,range)

def shannon_entropy(p: float) -> float:
    return -p*math.log2(p) - (1-p)*math.log2(1-p)

# Algorithm 3
def multiply(a: U, b: U) -> U:
    a_range = a.range
    b_range = b.range
    return U(a.destroy()*b_range + b.destroy(), a_range * b_range)

def divide(a: U, b: int) -> tuple[U,U]:
    a_range = a.range
    assert a_range % b == 0
    a_value = a.destroy()
    return U(a_value%b, b), U(a_value//b, a_range//b)

def downsize(a:U, m:int) -> tuple[U,R]:
    """
    Reduces entropy of size a to size m.
    """
    assert a.range >= m
    old_range = a.range
    old_value = a.destroy()
    if old_value < m:
        return U(old_value, m), R(True, m, old_range)
    else:
        return U(old_value-m, old_range-m), R(False, m, old_range)

def upsize(a:U, r:R) -> U:
    # TODO
    pass

class HardwareEntropySource:
    def __init__(self):
        self.entropy_out = 0

    def get(self, range):
        assert range == 256
        self.entropy_out += 8
        return U(int.from_bytes(os.urandom(1)), 256)

class BinaryEntropySource:
    def __init__(self, source):
        self.source = source
        self.entropy_out = 0
        self.buffer = U(0,1)

    def get(self, range):
        assert range == 2
        if self.buffer.range<2:
            self.buffer = multiply(self.buffer, self.source.get(256))
        result, self.buffer = divide(self.buffer, 2)
        self.entropy_out += 1
        return result    

class SimpleEntropySource:
    """
    A naive source of entropy.
    """
    def __init__(self):
        self.entropy_out = 0
        self.expected_entropy_in = 0
        self.binary_entropy = BinaryEntropySource(HardwareEntropySource())

    def entropy_consumed(self):
        return self.binary_entropy.entropy_out

    def get(self, n) -> U:
        self.entropy_out += math.log2(n)
        self.expected_entropy_in += expected_simple_convert(n)
        return simple_convert(n, self.binary_entropy)
        
def read_bit(entropy) -> U:
    """
    Reads one bit of randomness from a random source.
    """
    return entropy.get(2)

class EfficientEntropySource:
    def __init__(self, source, min_range=1<<31):
        self.source = source
        self.store = U(0,1)
        self.entropy_out = 0
        self.expected_entropy_in = 0
        self.min_range = min_range

    def entropy_consumed(self) -> float:
        return self.source.entropy_out - self.store.entropy()

    def get(self, n:int) -> U:
        first = True
        while first or self.store.range < n:
            first = False
            while self.store.range < self.min_range:
                self.store = multiply(self.store, read_bit(self.source))
            self.store, _ = downsize(self.store, self.store.range%n)
        result, self.store = divide(self.store, n)
        self.entropy_out += result.entropy()
        # The expected p
        p = (n-1)/self.min_range/2
        # We expect to lose the shannon-entropy of the downsize each time, and there are
        # 1/(1-p) iterations expected.
        self.expected_entropy_in += result.entropy() + shannon_entropy(p)/(1-p)
        return result


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

def run_benchmarks():
    # Test a naive entropy source
    test_entropy_source("Naive unbuffered", SimpleEntropySource())

    # Test the efficient entropy source in a variety of configurations
    test_entropy_source("Efficient 2**8 buffer", EfficientEntropySource(SimpleEntropySource(), 2**8))
    test_entropy_source("Efficient 2**15 buffer", EfficientEntropySource(SimpleEntropySource(), 2**15))
    test_entropy_source("Efficient 2**24 buffer", EfficientEntropySource(SimpleEntropySource(), 2**24))
    test_entropy_source("Efficient 2**31 buffer", EfficientEntropySource(SimpleEntropySource(), 2**31))
    test_entropy_source("Efficient 2**56 buffer", EfficientEntropySource(SimpleEntropySource(), 2**56))

run_benchmarks()