import random
import math
import os
from typing import Callable

####################################################
# Building blocks for entropy conversion

def multiply(U_n:int, n:int, U_m:int, m:int) -> tuple[int,int]:
    '''
    Combines two uniform distributions into one.

    Inputs:
        U_n    uniformly distributed in 0..n-1
        n
        U_m    uniformly distributed in 0..m-1
        m
    Outputs:
        U_nm   uniformly distributed in 0..nm-1
        nm     n*m
    '''
    return U_m * n + U_n, n * m

def divide(U_nm:int, nm:int, m:int) -> tuple[int,int,int,int]:
    '''
    Divides a uniform distribution into two, if
    nm can be factorised.

    Inputs:
        U_nm    uniformly distributed in 0..nm-1
        nm
        m       a factor of nm
    Outputs:
        U_n     uniformly distributed in 0..n-1
        n       nm/m
    '''
    return U_nm%m, m, U_nm//m, nm//m

def downsample(U_nm:int, nm:int, n:int) -> tuple[int,int,bool]:
    '''
    Creates a smaller uniform distribution from a larger uniform distribution
    and returns the additional Bernoulli entropy.

    Inputs:
        U_nm    uniformly distributed in 0..nm-1
        nm
        n       the sample size
    Outputs:
        U_x     uniformly distributed in 0..x-1
        x       either n or nm-m
        B       the result of the Bernoulli trial n/(n+m)
    '''
    if U_nm < n:
        return U_nm, n, True
    else:
        return U_nm - n, nm - n, False

def upsample(U_x:int, x:int, nm:int, B:bool) -> tuple[int,int]:
    '''
    Creates a larger uniform distribution from a smaller
    distribution by reading Bernoulli entropy.

    Inputs:
        U_x     uniformly distributed in 0..x-1
        x       n if B else m
        nm      n+m
        B       The result of a Bernoulli trial n/(n+m)
    Outputs:
        U_nm
        nm      n+m
    '''
    if B:
        return U_x, nm
    else:
        return U_x + nm - x, nm

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
    
    def __repr__(self) -> str:
        return f"U({self.range})"


    def entropy(self) -> float:
        """
        Gets the amount of entropy stored in this distribution.
        """
        return math.log2(self.range)

    def multiply(self, u):
        u_range = u.range
        u_value = u.read()
        self_range = self.range
        self_value = self.read()
        U_nm, nm = multiply(self_value, self_range, u_value, u_range)
        return U(U_nm, nm)

    def divide(self, m:int):
        self_range = self.range
        assert self_range % m == 0
        self_value = self.read()
        U_n, n = divide(self_value, self_range, m)
        return U(U_n, n)

    def downsample(self, m:int):
        self_range = self.range
        self_value = self.read()
        U_x, x, b = downsample(self_value, self_range, m)
        return U(U_x, x), B(b, m, self_range)

    def upsample(self, nm:int, b):
        self_range = self.range
        self_value = self.read()
        U_nm = upsample(self_value, self_range, b)
        return U(U_nm, nm)

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

    def __repr__(self):
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

def multiplyU(a: U, b: U) -> U:
    """
    Combines two uniform random variables into one.
    The original random variables are destroyed.
    """
    a_range = a.range
    b_range = b.range
    return U(a.read()*b_range + b.read(), a_range * b_range)

def divideU(a: U, b: int) -> tuple[U,U]:
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
            s = multiplyU(s, entropy())
        s, r = downsize(s, s.range%m)
        if not r.read():
            return divideU(s, m)

def convert_entropy(s_value:int, s_range:int, fetch, out_range:int, M:int):
    while True:
        while s_range < M:
            s_value = (s_value<<1) | fetch()
            s_range = s_range<<1
        s_range, r = divmod(s_range, out_range)
        if s_value >= r:
            s_value, out_value = divmod(s_value-r, out_range)
            return out_value, s_value, s_range
        else:
            s_range = r

def convert_entropy_wrapper(u:U, entropy, m:int, M:int) -> tuple[U,U]:
    u_range = u.range
    u_value = u.read()
    m_value, u_value, u_range = convert_entropy(u_value, u_range, lambda: randint(entropy,0,1), m, M)
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
            self.buffer = multiplyU(self.buffer, self.source.get(256))
        result, self.buffer = divideU(self.buffer, 2)
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
        self.binary_entropy = source

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
        self.store = multiplyU(self.store, e)

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
        result, self.store = convert_entropy_wrapper(self.store, self.source, n, min_range)

        self.entropy_out += result.entropy()
        # The expected p (not the worst-case p)
        # p = (n-1)/self.min_range/2  # Average-case
        p = (n-1)/self.min_range    # Worst-case
        # We expect to lose the shannon-entropy of the downsize each time, and there are
        # 1/(1-p) iterations expected.
        self.expected_entropy_in += result.entropy() + binary_entropy(p)/(1-p)

        return result

def worst_case_convert(n:int,N:int=1<<31):
    # Technically, (n-1)/N
    # Make sure the paper uses this bound
    p = (n-1)/N
    return math.log2(n) + binary_entropy(p)/(1-p)

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

    def reset(self):
        self.entropy = 0

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

def expected_fast_dice_rollerXX(n:int) -> float:
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

# Measures the exact entropy consumption of the FDR to a number of iterations
def expected_fast_dice_roller(n:int) -> float:

    def calculate_fdr_exact(u, depth):
        count = 0
        while u<n:
            u *= 2
            count += 1
        if n==u or depth==1:
            return count
        p = n/u
        return count + (1-p) * calculate_fdr_exact(u-n, depth-1)

    return calculate_fdr_exact(1, 8)

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
    p = 50/(2**31)
    l = binary_entropy(p)/(1-p)
    out = math.log2(50)
    print(p)
    print(l)
    print(out/(out+l))
