import random
import math
import os
from typing import Callable

####################################################
# Probability distributions used for entropy storage

class U:
    """
    Entropy storage using a uniform random distribution.
    Used for representing entropy >=1
    """
    def __init__(self, value:int=0, range:int=1):
        self._value = value
        self.range = range
        self.entropy_out = 0
    
    def read(self) -> int:
        """
        Reads the stored value, but destroys the stored entropy in the process.
        """
        value = self._value
        self._value = 0
        self.range = 1
        return value
    
    def __str__(self) -> str:
        return f"U({self.range})"
    
    def entropy(self) -> float:
        """
        Gets the amount of entropy stored in this distribution.
        """
        return math.log2(self.range)
    
class B:
    """
    Entropy storage using a binary/Bernoulli distribution with
    p = numerator/denominator. Used for representing entropy <=1
    """
    def __init__(self, value: bool=False, numerator: int=0, denominator: int=1):
        self._value = value
        self.numerator = numerator
        self.denominator = denominator

    def read(self) -> bool:
        """
        Reads the stored value, but destroys the stored entropy in the process. 
        """
        value = self._value
        self._value = False
        self.numerator = 0
        self.denominator = 1
        return value

    def entropy(self):
        """
        Gets the amount of entropy stored in this distribution.
        """
        return binary_entropy(self.numerator/self.denominator)

    def __str__(self):
        return f"B({self.numerator}/{self.denominator})"

#######################################
# Functions

def randint(entropy, min, max):
    """
    Returns a random integer in the range [min,max] using an entropy source.
    """
    return min + entropy.get(max-min+1).read()

def shuffle(sequence: list, entropy):
    """
    Shuffles a sequence using random numbers from an entropy source.
    Uses the Fisher-Yates algorithm.
    """
    for i in range(1, len(sequence)):
        j = randint(entropy, 0, i)
        sequence[i], sequence[j] = sequence[j], sequence[i]

def rejection_sampling(range:int, entropy) -> U:
    """
    Uses a selective discard algorithm to return
    a uniform distribution of the required range.
    """
    while True:
        v, r = 0, 1
        while r < range:
            v = v*2 + randint(entropy,0,1)
            r = r*2
        if v<range:
            return U(v,range)

def fast_dice_roller(n:int, entropy) -> U:
    """
    Implements Lumbroso's fast-dice-roller algorithm
    https://arxiv.org/pdf/1304.1916
    """
    v = 1
    c = 0
    while True:
        v = v*2
        c = c*2+randint(entropy,0,1)
        if v>=n:
            if c<n:
                return U(c,n)
            else:
                v = v-n
                c = c-n

def multiply(a: U, b: U) -> U:
    """
    Combines two uniform random variables into one.
    The original random variables are destroyed.
    """
    a_range = a.range
    b_range = b.range
    return U(a.read()*b_range + b.read(), a_range * b_range)

def divide(a: U, b: int) -> tuple[U,U]:
    """
    Creates two uniform random variables from one random variable.
    The original random variable is destroyed.
    b must divide the range of a exactly.
    """
    a_range = a.range
    assert a_range % b == 0
    a_value = a.read()
    return U(a_value%b, b), U(a_value//b, a_range//b)

def downsize(a:U, m:int) -> tuple[U,B]:
    """
    Reduces entropy to size m.
    The original entropy is destroyed.
    """
    assert a.range >= m
    old_range = a.range
    old_value = a.read()
    if old_value < m:
        return U(old_value, m), B(True, m, old_range)
    else:
        return U(old_value-m, old_range-m), B(False, m, old_range)

def read_bit(entropy) -> U:
    """
    Reads one bit of randomness from a random source.
    """
    return entropy.get(2)

def entropy_convert(s:U, entropy:Callable[[],U], m:int, M:int) -> tuple[U,U]:
    """
    Extracts entropy U(m) from a uniform distribution of
    arbitrary size.

    Parameters:
    - u is the entropy input (of any range)
    - entropy is a function that returns entropy, of any range
    - m is the output range
    - M is the minimum size of our entropy before downsize
    Returns:
    - U(m)
    - The remaining entropy
    """
    assert m <= M
    while True:
        while u.range < M:
            s = multiply(s, entropy())
        s, r = downsize(s, s.range%m)
        if not r.read():
            return divide(s, m)

def optimized_convert(u:U, entropy, m:int, M:int) -> tuple[U,U]:
    """
    Same as entropy_convert() but optimizes the operations.
    It is less readable but retains the structure of the original algorithm.
    In this case, entropy must return 0 or 1.
    """
    assert m <= M
    u_range = u.range
    u_value = u.read()
    while True:
        while u_range < M:
            u_value = (u_value<<1) | randint(entropy, 0, 1)
            u_range = u_range<<1
        x, r = divmod(u_range, m)
        if u_value < u_range - r:
            u_value, m_value = divmod(u_value, m)
            return U(m_value, m), U(u_value, x)
        else:
            u_value -= r
            u_range = r

# Exposition only
# Write this out more fully to make it self-contained
def long_convert(u_value:int, u_range:int, fetch:Callable[[],int], m:int, M:int) \
        -> tuple[int,int,int]:
    while True:
        while u_range < M:
            u_value = (u_value<<1) | fetch()
            u_range = u_range<<1
        u2_range, r = divmod(u_range, m)
        if u_value >= r:
            u2_value, m_value = divmod(u_value-r, m)
            return m_value, u2_value, u2_range
        else:
            u_range = r

def long_convert_wrapper(u:U, entropy, m:int, M:int) -> tuple[U,U]:
    u_range = u.range
    u_value = u.read()
    m_value, u_value, u_range = long_convert(u_value, u_range, lambda: randint(entropy,0,1), m, M)
    return U(m_value, m), U(u_value, u_range)

######################################
# Entropy sources

class HardwareEntropySource:
    """
    A source of entropy in the form U(256),
    reading one byte of entropy each time.
    """
    def __init__(self):
        self.entropy_out = 0

    def get(self, range) -> U:
        assert range == 256
        self.entropy_out += 8
        return U(int.from_bytes(os.urandom(1)), 256)

class BinaryEntropySource:
    """
    A source of entropy in the form U(2).
    """
    def __init__(self, source = None):
        if source is None:
            source = HardwareEntropySource()
        self.source = source
        self.entropy_out = 0
        self.buffer = U(0,1)

    def get(self, range) -> U:
        assert range == 2
        if self.buffer.range<2:
            self.buffer = multiply(self.buffer, self.source.get(256))
        result, self.buffer = divide(self.buffer, 2)
        self.entropy_out += 1
        return result

class RejectionSamplingEntropySource:
    """
    A simple source of entropy, calculated
    using rejection sampling.
    """
    def __init__(self, source = None):
        if source is None:
            source = BinaryEntropySource()
        self.entropy_out = 0
        self.expected_entropy_in = 0
        self.binary_entropy = BinaryEntropySource()

    def entropy_consumed(self):
        return self.binary_entropy.entropy_out

    def get(self, n) -> U:
        self.entropy_out += math.log2(n)
        self.expected_entropy_in += expected_rejection_sampling(n)
        return rejection_sampling(n, self.binary_entropy)

class FastDiceRollerEntropySource:
    """
    Entropy source using FDR
    """
    def __init__(self, source = None):
        self.entropy_out = 0
        self.expected_entropy_in = 0
        if source is None:
            source = BinaryEntropySource()
        self.binary_entropy = source

    def entropy_consumed(self):
        return self.binary_entropy.entropy_out

    def get(self, n) -> U:
        self.entropy_out += math.log2(n)
        self.expected_entropy_in += expected_fast_dice_roller(n)
        return fast_dice_roller(n, self.binary_entropy)


class EfficientEntropySource:
    """
    An efficient entropy source calculated using
    fetch-multiply-downsize-divide.
    """
    def __init__(self, source, min_range=1<<31):
        self.source = source
        self.store = U(0,1)
        self.entropy_out = 0
        self.expected_entropy_in = 0
        self.entropy_put = 0
        self.min_range = min_range

    def entropy_consumed(self) -> float:
        return self.source.entropy_out + self.entropy_put - self.store.entropy()

    def put(self, e:U):
        self.entropy_put += e.entropy()
        self.store = multiply(self.store, e)

    def get_R(self, num:int, den:int) -> B:
        """
        Extract an entropy value <=1
        expressed as a probability num/den.
        """
        y, r = downsize(self.get(den), num)
        self.put(y)
        return r

    def put_R(self, r:B):
        """
        Put an B entropy value into the store
        """
        num = r.numerator
        den = r.denominator
        if r.read():
            self.put(U(self.get_with_min(num, den).read(), den))
        else:
            self.put(U(num+self.get_with_min(den-num, den).read(), den))

    def get(self, n:int) -> U:
        return self.get_with_min(n, self.min_range)

    def get_with_min(self, n:int, min_range:int) -> U:
        result, self.store = long_convert_wrapper(self.store, self.source, n, min_range)

        self.entropy_out += result.entropy()
        # The expected p (not the worst-case p)
        p = (n-1)/self.min_range/2  # Average-case
        # p = (n-1)/self.min_range    # Worst-case
        # We expect to lose the shannon-entropy of the downsize each time, and there are
        # 1/(1-p) iterations expected.
        self.expected_entropy_in += result.entropy() + binary_entropy(p)/(1-p)

        return result

######################################
# Tests

# TODO: Use this to measure things
class MeasuringStore:
    def __init__(self, store):
        self.store = store
        self.entropy = 0

    def get(self, n:int):
        self.entropy += math.log2(n)
        return self.store.get(n)

def binary_entropy(p: float) -> float:
    """
    Calculates the entropy contained in a binary/Bernoulli distribution
    where the probability of the event is p.
    """
    if p==0 or p==1:
        return 0
    return -p*math.log2(p) - (1-p)*math.log2(1-p)

def expected_rejection_sampling(range:int) -> float:
    r = 1 # The smallest power of 2 >= than range 
    b = 0 # The number of bits in r
    while r < range:
        r *= 2
        b += 1
    return r*b/range

def expected_fast_dice_roller(n:int) -> float:
    r = 1 # The smallest power of 2 >= n
    while r < n:
        r *= 2
    p = n/r
    # This is an underestimate because the number v can contain numbers greater than a power of 2
    # Entropy is between 
    # math.log2(n) + binary_entropy(p) 
    # and 
    # math.log2(n) + binary_entropy(p)/p 
    return math.log2(n) + binary_entropy(p) #/p 

def expected_efficiency(store):
    return store.entropy_out/store.expected_entropy_in

def measured_efficiency(store):
    return store.entropy_out/store.entropy_consumed()

def test_entropy_source(name, store):
    print("Results for store     ", name)
    cards = list(range(52))
    shuffle(cards, store)

    input_count = store.expected_entropy_in
    output_count = store.entropy_out

    print("Output entropy        ", store.entropy_out)
    print("Measured input entropy", store.entropy_consumed())

    print("Expected input entropy", store.expected_entropy_in)
    print("Expected efficiency   ", expected_efficiency(store))
    print("Measured efficiency   ", measured_efficiency(store))

def measure_entropy_source_efficiency(generator, iterations=100):
    total_in = 0
    total_out = 0
    for i in range(iterations):
        store = generator()
        cards = list(range(52))
        shuffle(cards, store)
        total_in += store.entropy_consumed()
        total_out += store.entropy_out
    return total_out/total_in

def measure_entropy_source(name:str, gen:Callable):
    print("Average efficiency for", name, "is", measure_entropy_source_efficiency(gen))

def test_efficiency_for_buffer(buffer:int):
    test_entropy_source(f"Efficient {buffer} buffer", EfficientEntropySource(RejectionSamplingEntropySource(), buffer))
    measure_entropy_source(f"Efficient {buffer} buffer", lambda: EfficientEntropySource(RejectionSamplingEntropySource(), buffer))
    print()

def run_benchmarks():
    # Test a naive entropy source
    test_entropy_source("Rejection sampling", RejectionSamplingEntropySource())
    measure_entropy_source("Rejection sampling", lambda: RejectionSamplingEntropySource())
    print()

    test_entropy_source("Fast dice roller", FastDiceRollerEntropySource())
    measure_entropy_source("Fast dice roller", lambda: FastDiceRollerEntropySource())
    print()

    test_efficiency_for_buffer(2**31)

def run_tests():
    s = EfficientEntropySource(RejectionSamplingEntropySource())
    u = s.get(10)
    s.put(u)
    print("Expected efficiency", expected_efficiency(s))
    print("Measured efficiency", measured_efficiency(s))

    s = EfficientEntropySource(BinaryEntropySource(HardwareEntropySource()))
    r = s.get_R(1,10)
    print("r has entropy", r.entropy())
    print("Store has entropy", s.store.entropy())
    s.put_R(r)
    print("Store has entropy", s.store.entropy())
    print("Expected efficiency", expected_efficiency(s))
    print("Measured efficiency", measured_efficiency(s))

def deck():
    return list("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")

def show_shuffle():
    s = EfficientEntropySource(RejectionSamplingEntropySource())
    d = deck()
    shuffle(d, s)
    print(''.join(d))

if __name__ == "__main__":
    run_benchmarks()
    run_tests()
    for i in range(10):
        show_shuffle()
