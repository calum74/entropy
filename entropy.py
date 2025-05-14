
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
     return (n_value, m_range, True) if n_value<m_range \
        else (n_value-m_range, n_range-m_range, False)


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

def downsize2(a:U, m:int) -> tuple[U,R]:
    """
    Reduces entropy of size a to size m.
    """
    assert a.range >= m
    old_range = a.range
    new_value,new_range,r = downsize(a.value, a.range, m)
    a.reset()
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
    v = a.value
    a.reset()
    return v
    
def read_bit(random) -> U:
    return U(random.randint(0,1),2)

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
                self.store = multiply2(self.store, read_bit(self.source))
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
test_entropy_source("Naive", NaiveEntropySource())
test_entropy_source("Efficient", EfficientEntropySource(NaiveEntropySource()))


