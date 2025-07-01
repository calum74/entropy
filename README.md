# Entropy Store

These are files to support the evaluation of the submitted paper.

To view calculations, go to [entropy.ipynb](entropy.ipynb).

To get a reusable C++ implementation of entropy store, go to [entropy_store.hpp](entropy_store.hpp).

You can also run some tests for example entropy.py in Python, and entropy.cpp in C++.

## C++ Reference

The implementation is in [entropy_store.hpp](entropy_store.hpp) which can be copied to your C++ project.

The file [sample.cpp](sample.cpp) contains instructions for usage.

## Testing

The program [entropy.cpp](entropy.cpp) reads from the random device and generates output similar to this:

```
Here is a d6 roll: 2
Here is a d6 roll: 6
Here is a coin flip: 1
Fair coin:
  0: n=480 σ=-1.3
  1: n=520 σ=1.3
  00: n=227 σ=-1.7
  01: n=253 σ=0.22
  10: n=253 σ=0.22
  11: n=267 σ=1.2
  Distribution entropy = 1
  Net output entropy = 1000
  Bits fetched = 1000
  Internal entropy delta = 0
  Net input entropy = 1000
  Net efficiency = 1
Fair coin as a uniform {0,1}:
  0: n=486 σ=-0.89
  1: n=514 σ=0.89
  00: n=225 σ=-1.8
  01: n=261 σ=0.8
  10: n=261 σ=0.8
  11: n=253 σ=0.22
  Distribution entropy = 1
  Net output entropy = 1000
  Bits fetched = 1000
  Internal entropy delta = 0
  Net input entropy = 1000
  Net efficiency = 1
Fair coin as a 1:1 distribution:
  0: n=506 σ=0.38
  1: n=494 σ=-0.38
  00: n=253 σ=0.22
  01: n=253 σ=0.22
  10: n=253 σ=0.22
  11: n=241 σ=-0.66
  Distribution entropy = 1
  Net output entropy = 1000
  Bits fetched = 1000
  Internal entropy delta = 0
  Net input entropy = 1000
  Net efficiency = 1
Fair d6:
  1: n=143 σ=-2
  2: n=175 σ=0.71
  3: n=177 σ=0.88
  4: n=177 σ=0.88
  5: n=155 σ=-0.99
  6: n=173 σ=0.54
  11: n=17 σ=-2.1
  12: n=29 σ=0.24
  13: n=27 σ=-0.15
  14: n=21 σ=-1.3
  15: n=23 σ=-0.92
  16: n=26 σ=-0.34
  21: n=22 σ=-1.1
  22: n=28 σ=0.043
  23: n=32 σ=0.81
  24: n=37 σ=1.8
  25: n=28 σ=0.043
  26: n=28 σ=0.043
  31: n=30 σ=0.43
  32: n=25 σ=-0.53
  33: n=32 σ=0.81
  34: n=36 σ=1.6
  35: n=18 σ=-1.9
  36: n=36 σ=1.6
  41: n=27 σ=-0.15
  42: n=27 σ=-0.15
  43: n=25 σ=-0.53
  44: n=34 σ=1.2
  45: n=30 σ=0.43
  46: n=34 σ=1.2
  51: n=20 σ=-1.5
  52: n=42 σ=2.7
  53: n=31 σ=0.62
  54: n=18 σ=-1.9
  55: n=26 σ=-0.34
  56: n=18 σ=-1.9
  61: n=27 σ=-0.15
  62: n=24 σ=-0.73
  63: n=30 σ=0.43
  64: n=31 σ=0.62
  65: n=30 σ=0.43
  66: n=31 σ=0.62
  Distribution entropy = 2.584962501
  Net output entropy = 2584.962501
  Bits fetched = 2585
  Internal entropy delta = 0.0374973437
  Net input entropy = 2584.962503
  Net efficiency = 0.9999999993
1:2 biassed coin:
  0: n=321 σ=-0.83
  1: n=679 σ=0.83
  00: n=106 σ=-0.51
  01: n=215 σ=-0.55
  10: n=215 σ=-0.55
  11: n=464 σ=1.2
  Distribution entropy = 0.9182958341
  Net output entropy = 918.2958341
  Bits fetched = 906
  Internal entropy delta = 0.03749834902
  Net input entropy = 905.9625017
  Net efficiency = 1.013613513
49:2 biassed coin:
  0: n=961 σ=0.035
  1: n=39 σ=-0.035
  00: n=923 σ=-0.013
  01: n=38 σ=0.054
  10: n=38 σ=0.054
  11: n=1 σ=-0.43
  Distribution entropy = 0.2386845114
  Net output entropy = 238.6845114
  Bits fetched = 238
  Internal entropy delta = 0.3107941533
  Net input entropy = 237.6892058
  Net efficiency = 1.004187424
1:999 biassed coin:
  0: n=2 σ=1
  1: n=998 σ=-1
  01: n=2 σ=1
  10: n=2 σ=1
  11: n=996 σ=-1.4
  Distribution entropy = 0.01140775774
  Net output entropy = 11.40775774
  Bits fetched = 22
  Internal entropy delta = 0.6274417292
  Net input entropy = 21.37255827
  Net efficiency = 0.5337572411
1:2:3:4 distribution:
  0: n=93 σ=-0.74
  1: n=192 σ=-0.63
  2: n=289 σ=-0.76
  3: n=426 σ=1.7
  00: n=6 σ=-1.3
  01: n=17 σ=-0.68
  02: n=27 σ=-0.56
  03: n=43 σ=0.48
  10: n=20 σ=-8e-16
  11: n=46 σ=0.97
  12: n=53 σ=-0.93
  13: n=73 σ=-0.82
  20: n=29 σ=-0.19
  21: n=61 σ=0.13
  22: n=94 σ=0.44
  23: n=105 σ=-1.5
  30: n=38 σ=-0.32
  31: n=68 σ=-1.4
  32: n=115 σ=-0.49
  33: n=205 σ=3.9
  Distribution entropy = 1.846439345
  Net output entropy = 1846.439345
  Bits fetched = 1820
  Internal entropy delta = 0.1260640339
  Net input entropy = 1819.873936
  Net efficiency = 1.01459739
Fair coin from uniform input:
  0: n=512 σ=0.76
  1: n=488 σ=-0.76
  00: n=263 σ=0.95
  01: n=249 σ=-0.073
  10: n=249 σ=-0.073
  11: n=239 σ=-0.8
  Distribution entropy = 1
  Net output entropy = 1000
  Bits fetched = 1001
  Internal entropy delta = 0.9999982329
  Net input entropy = 1000.000002
  Net efficiency = 0.9999999982
Fair coin from 1:999 input (using 64-bit buffer):
  0: n=509 σ=0.57
  1: n=491 σ=-0.57
  00: n=261 σ=0.8
  01: n=248 σ=-0.15
  10: n=248 σ=-0.15
  11: n=243 σ=-0.51
  Distribution entropy = 1
  Net output entropy = 1000
  Bits fetched = 1001
  Internal entropy delta = 0.9999999884
  Net input entropy = 1000
  Net efficiency = 1
Fair coin from 1:999 input (poor efficiency due to low buffer size):
  0: n=504 σ=0.25
  1: n=496 σ=-0.25
  00: n=252 σ=0.15
  01: n=252 σ=0.15
  10: n=252 σ=0.15
  11: n=244 σ=-0.44
  Distribution entropy = 1
  Net output entropy = 1000
  Bits fetched = 1453
  Internal entropy delta = 1.86403003
  Net input entropy = 1451.13597
  Net efficiency = 0.6891153005
Fair coin from 1:99 input:
  0: n=500 σ=0
  1: n=500 σ=0
  00: n=253 σ=0.22
  01: n=247 σ=-0.22
  10: n=247 σ=-0.22
  11: n=253 σ=0.22
  Distribution entropy = 1
  Net output entropy = 1000
  Bits fetched = 1004
  Internal entropy delta = 3.91359388
  Net input entropy = 1000.086406
  Net efficiency = 0.9999136013
Fair coin from 1:4 input:
  0: n=515 σ=0.95
  1: n=485 σ=-0.95
  00: n=269 σ=1.4
  01: n=246 σ=-0.29
  10: n=246 σ=-0.29
  11: n=239 σ=-0.8
  Distribution entropy = 1
  Net output entropy = 1000
  Bits fetched = 999
  Internal entropy delta = -1.000017451
  Net input entropy = 1000.000017
  Net efficiency = 0.9999999825
Fair coin from 4:3:2:1 input:
  0: n=474 σ=-1.6
  1: n=526 σ=1.6
  00: n=225 σ=-1.8
  01: n=249 σ=-0.073
  10: n=249 σ=-0.073
  11: n=277 σ=2
  Distribution entropy = 1
  Net output entropy = 1000
  Bits fetched = 999
  Internal entropy delta = -1.000011818
  Net input entropy = 1000.000012
  Net efficiency = 0.9999999882
4:1:5 distribution from 1:1 input:
  0: n=409 σ=0.58
  1: n=83 σ=-1.8
  2: n=508 σ=0.51
  00: n=181 σ=1.8
  01: n=32 σ=-1.3
  02: n=196 σ=-0.32
  10: n=32 σ=-1.3
  11: n=6 σ=-1.3
  12: n=45 σ=-0.73
  20: n=196 σ=-0.32
  21: n=45 σ=-0.73
  22: n=267 σ=1.2
  Distribution entropy = 1.360964047
  Net output entropy = 1360.964047
  Bits fetched = 1325
  Internal entropy delta = 0.6113754111
  Net input entropy = 1324.388625
  Net efficiency = 1.027616836
4:1:5 distribution from 4:3:2:1 input:
  0: n=385 σ=-0.97
  1: n=85 σ=-1.6
  2: n=530 σ=1.9
  00: n=152 σ=-0.69
  01: n=33 σ=-1.1
  02: n=200 σ=0
  10: n=31 σ=-1.5
  11: n=6 σ=-1.3
  12: n=48 σ=-0.29
  20: n=202 σ=0.16
  21: n=46 σ=-0.58
  22: n=282 σ=2.3
  Distribution entropy = 1.360964047
  Net output entropy = 1360.964047
  Bits fetched = 1322
  Internal entropy delta = 0.6937758294
  Net input entropy = 1321.306224
  Net efficiency = 1.030014105

All tests passed!
```

This tests a variety of distributions, and counts the number of outputs and the number of adjacent pairs against the expected number for that distribution. It then calculates how many standard deviations that number is compared to its expected value. Whilst large deviations are always possible, they are improbable and would indicate a bug. 