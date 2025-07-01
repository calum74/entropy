# Entropy Store

These are files to support the evaluation of the submitted paper.

To view calculations, go to [entropy.ipynb](entropy.ipynb).

To get a reusable C++ implementation of entropy store, go to [entropy_store.hpp](entropy_store.hpp).

You can also run some tests for example entropy.py in Python, and entropy.cpp in C++.

## C++ Reference

The implementation is in `entropy_store.hpp` which can be copied to your C++ project.

The file `sample.cpp` contains sample usage.

## Testing

The program [entropy.cpp](entropy.cpp) reads from the random device and generates output similar to this:

```
Here is a d6 roll: 4
Here is a d6 roll: 2
Here is a coin flip: 0
Fair coin:
  0: n=5002783 σ=1.76012
  1: n=4997217 σ=-1.76012
  Distribution entropy = 1
  Bits fetched: 10000000, 1 per sample, efficiency = 1
Fair coin as a uniform {0,1}:
  0: n=4997769 σ=-1.41101
  1: n=5002231 σ=1.41101
  Distribution entropy = 1
  Bits fetched: 10000000, 1 per sample, efficiency = 1
Fair coin as a 1:1 distribution:
  0: n=5001931 σ=1.22127
  1: n=4998069 σ=-1.22127
  Distribution entropy = 1
  Bits fetched: 10000000, 1 per sample, efficiency = 1
Fair d6:
  1: n=1668442 σ=1.50642
  2: n=1664916 σ=-1.48549
  3: n=1665381 σ=-1.09092
  4: n=1666968 σ=0.25569
  5: n=1666189 σ=-0.405314
  6: n=1668104 σ=1.21962
  Distribution entropy = 2.58496
  Bits fetched: 25849626, 2.58496 per sample, efficiency = 1
1:2 biassed coin:
  0: n=3335488 σ=1.44539
  1: n=6664512 σ=-1.44539
  Distribution entropy = 0.918296
  Bits fetched: 9185114, 0.918511 per sample, efficiency = 0.999765
49:2 biassed coin:
  0: n=9607860 σ=0.0274717
  1: n=392140 σ=-0.0274717
  Distribution entropy = 0.238685
  Bits fetched: 2386768, 0.238677 per sample, efficiency = 1.00003
1:999 biassed coin:
  0: n=10102 σ=1.02051
  1: n=9989898 σ=-1.02051
  Distribution entropy = 0.0114078
  Bits fetched: 115185, 0.0115185 per sample, efficiency = 0.990386
1:2:3:4 distribution:
  0: n=1000379 σ=0.399501
  1: n=1999521 σ=-0.378683
  2: n=3000995 σ=0.686615
  3: n=3999105 σ=-0.57772
  Distribution entropy = 1.84644
  Bits fetched: 18465086, 1.84651 per sample, efficiency = 0.999962
Fair coin from uniform input:
  0: n=4998405 σ=-1.00877
  1: n=5001595 σ=1.00877
  Distribution entropy = 1
  Bits fetched: 10000003, 1 per sample, efficiency = 1
Fair coin from 1:999 input (poor efficiency due to low N):
  0: n=5000288 σ=0.182147
  1: n=4999712 σ=-0.182147
  Distribution entropy = 1
  Bits fetched: 12548844, 1.25488 per sample, efficiency = 0.796886
Fair coin from 1:999 input (using 64-bit buffer):
  0: n=4999469 σ=-0.335834
  1: n=5000531 σ=0.335834
  Distribution entropy = 1
  Bits fetched: 10000005, 1 per sample, efficiency = 1
Fair coin from 1:99 input:
  0: n=4999471 σ=-0.334569
  1: n=5000529 σ=0.334569
  Distribution entropy = 1
  Bits fetched: 10005224, 1.00052 per sample, efficiency = 0.999478
Fair coin from 1:4 input:
  0: n=4996084 σ=-2.4767
  1: n=5003916 σ=2.4767
  Distribution entropy = 1
  Bits fetched: 10000001, 1 per sample, efficiency = 1
Fair coin from 4:3:2:1 input:
  0: n=4998576 σ=-0.900617
  1: n=5001424 σ=0.900617
  Distribution entropy = 1
  Bits fetched: 9999999, 1 per sample, efficiency = 1
4:1:5 distribution from 1:1 input:
  0: n=4000216 σ=0.139427
  1: n=1001655 σ=1.74452
  2: n=4998129 σ=-1.18332
  Distribution entropy = 1.36096
  Bits fetched: 13613554, 1.36136 per sample, efficiency = 0.999713
4:1:5 distribution from 4:3:2:1 input:
  0: n=4001257 σ=0.81139
  1: n=998944 σ=-1.11312
  2: n=4999799 σ=-0.127124
  Distribution entropy = 1.36096
  Bits fetched: 13607593, 1.36076 per sample, efficiency = 1.00015

All tests passed!
```
