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
    '''
    For two uniform distributions U(a) and U(b), 
    return a combined distribution U(ab).
    '''
    return n_value * m_range + m_value, n_range * m_range

# Algorithm 4
def factorize(n_value, n_range, m_range):
    '''
    The inverse of combine(): For a distribution U(ab),
    return two distributions U(a) and U(b)
    '''
    assert n_range % m_range == 0
    return n_value%m_range, m_range, n_value//m_range, n_range//m_range

# Algorithm 5
def resize(a_value, a_range, b_range):
     '''
     For a distribution U(a), return a distribution
     of size U(b) or U(a-b)
     '''
     assert b_range <= a_range
     return (a_value, b_range) if a_value<b_range else (a_value-b_range, a_range-b_range)

def random_bit():
    return random.randint(0,1)


input_count = 0
output_count = 0

def fetch(a_value, a_range):
    '''
    Fetch one bit of entopy
    It's not using hardware entropy
    '''
    global input_count
    input_count = input_count+1
    return combine(a_value, a_range, random_bit(), 2)

def convert(a_value, a_range, b_range, min_range):
    first = True  # Work around do-while loops in Python
    while a_range<b_range or first:
        first = False
        while a_range<min_range or a_range < b_range:
            # Get one more bit of entropy
            a_value, a_range = fetch(a_value, a_range)
        a_value, a_range = resize(a_value, a_range, a_range%b_range)
    return factorize(a_value, a_range, b_range)


class U:
    '''
    An entropy store.

    This is a convenience wrapper around the primitive functions.
    '''
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
        r_value, r_range, a_value, a_range = convert(self.value, self.range, range, min_range)
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

# def shuffle(store, deck=52):
#    for i in range(1,deck+1):
#        store.get(i, 1<<24)

store = U()
# shuffle(store, 52)
cards = list(range(52))
# fisher_yates(cards, random)
fisher_yates(cards, store)

input_count = input_count - store.entropy()
print("Output count", output_count)
print("Input count", input_count)
print("Efficiency", output_count/input_count)

